#include <opencv2/opencv.hpp>
#include <numeric>

struct BoundingBox 
{
    int x1;
    int y1;
    int x2;
    int y2;
    int area;
};

cv::Ptr<cv::BackgroundSubtractor> create_substractor(void) 
{
    //KNN substractor
    cv::Ptr<cv::BackgroundSubtractor> backSub;
    backSub = cv::createBackgroundSubtractorKNN(200, false);
    return backSub;
}

void update_background(cv::VideoCapture& cam, cv::Ptr<cv::BackgroundSubtractor> const &  backSub, int frame_count)
{
    cv::Mat frame;
    cv::Mat fg_mask;
    cv::Mat gray_frame;
    for(int i=0; i < frame_count; i++)
    {
        cam >> frame;
        cv::cvtColor(frame, gray_frame, cv::COLOR_BGR2GRAY);
        backSub->apply(gray_frame, fg_mask);
    }
}

cv::VideoCapture get_cam(int cam_number)
{
    cv::VideoCapture cam;
    cam.open(cam_number);
    cam.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cam.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    return cam;
}

cv::Mat get_motion_mask(const cv::Mat& fg_mask, int min_thresh = 0) 
{
    // Threshold the foreground mask
    const cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(9, 9));
    cv::Mat after_thresh;
    cv::threshold(fg_mask, after_thresh, min_thresh, 255, cv::THRESH_BINARY);

    // Median blur
    cv::Mat after_blur;
    cv::medianBlur(after_thresh, after_blur, 3);

    // Morphological operations
    cv::Mat after_open_close;
    cv::morphologyEx(after_blur, after_open_close, cv::MORPH_OPEN, kernel, cv::Point(-1, -1), 1);
    cv::morphologyEx(after_open_close, after_open_close, cv::MORPH_CLOSE, kernel, cv::Point(-1, -1), 1);

    return after_open_close;
}

std::vector<cv::Rect> get_contour_detections(const cv::Mat& mask, int thresh = 400) 
{
    // Find contours in the mask
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(mask, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_TC89_L1);

    std::vector<cv::Rect> boundRect;

    for (std::vector<cv::Point> countour : contours) 
    {
        // Find bounding rectangle for each contour
        cv::Rect rect = cv::boundingRect(countour);

        // Check if area exceeds the threshold
        if (rect.area() > thresh) 
        {
            // Store bounding box coordinates and area in detections
            boundRect.push_back(rect);
        }
    }
    return boundRect;
}

bool isContained(const BoundingBox& box1, const BoundingBox& box2) 
{
    return ((box2.x1 >= box1.x1) && (box2.y1 >= box1.y1) && (box2.x2 <= box1.x2) && (box2.y2 <= box1.y2));
}

std::vector<int> remove_contained_bboxes(const std::vector<BoundingBox>& boxes) 
{
    std::vector<int> keep;
    for (int i = 0; i < boxes.size(); ++i) 
    {
        bool isContainedFlag = false;
        for (int j = 0; j < boxes.size(); ++j) 
        {
            if (i != j && isContained(boxes[i], boxes[j])) 
            {
                isContainedFlag = true;
                break;
            }
        }
        if (!isContainedFlag) 
        {
            keep.push_back(i);
        }
    }
    return keep;
}

std::vector<BoundingBox> non_max_suppression(const std::vector<BoundingBox>& boxes, const std::vector<double>& scores, double threshold = 0.1) {
    std::vector<int> order(boxes.size());
    std::iota(order.begin(), order.end(), 0);

    // Sort the boxes by score in descending order
    std::sort(order.begin(), order.end(), [&](int i, int j) { return scores[i] > scores[j]; });

    // Remove all contained bounding boxes and get ordered index
    std::vector<int> keep = remove_contained_bboxes(boxes);

    std::vector<BoundingBox> nmsResult;
    while (!order.empty()) 
    {
        int i = order[0];

        auto it = std::remove_if(order.begin(), order.end(), [&](int j) 
        {
            double intersection = std::max(0, std::min(boxes[i].x2, boxes[j].x2) - std::max(boxes[i].x1, boxes[j].x1)) *
                                  std::max(0, std::min(boxes[i].y2, boxes[j].y2) - std::max(boxes[i].y1, boxes[j].y1));
            double unionArea = (boxes[i].x2 - boxes[i].x1) * (boxes[i].y2 - boxes[i].y1) +
                               (boxes[j].x2 - boxes[j].x1) * (boxes[j].y2 - boxes[j].y1) - intersection;
            double iou = intersection / unionArea;

            // Remove boxes with IoU greater than the threshold
            return iou > threshold;
        });
        if (it != order.end())
        {
            order.erase(it, order.end());
        }
        else
        {
            keep.push_back(i);
        }
    }

    for (int idx : keep) 
    {
        nmsResult.push_back(boxes[idx]);
    }

    return nmsResult;
}

std::vector<BoundingBox> get_detections(cv::Ptr<cv::BackgroundSubtractor> backSub, const cv::Mat& frame, int bbox_thresh = 100, double nms_thresh = 0.1) {
    const cv::Mat& kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(9, 9));
    // Update Background Model and get foreground mask
    cv::Mat fg_mask;
    backSub->apply(frame, fg_mask);

    // get clean motion mask
    cv::Mat motion_mask = get_motion_mask(fg_mask, 0);

    // get initially proposed detections from contours
    std::vector<cv::Rect> detectionsMat = get_contour_detections(motion_mask, bbox_thresh);

    // separate bboxes and scores
    std::vector<BoundingBox> bboxes;
    std::vector<double> scores;
    for (cv::Rect given_rectangle : detectionsMat) 
    {
        BoundingBox box;
        box.x1 = given_rectangle.x;
        box.y1 = given_rectangle.y;
        box.x2 = given_rectangle.x + given_rectangle.width;
        box.y2 = given_rectangle.y + given_rectangle.height;
        box.area = given_rectangle.area();

        bboxes.push_back(box);
        scores.push_back(box.area);
    }

    // perform Non-Maximal Suppression on initial detections
    std::vector<BoundingBox> finalDetections = non_max_suppression(bboxes, scores, nms_thresh);

    return finalDetections;
}

void draw_bboxes(cv::Mat& frame, const std::vector<BoundingBox>& detections) {
    for (const BoundingBox& det : detections) 
    {
        cv::rectangle(frame, cv::Point(det.x1, det.y1), cv::Point(det.x2, det.y2), cv::Scalar(0, 255, 0), 3);
    }
}



int main() 
{
    cv::Ptr<cv::BackgroundSubtractor> backSub = create_substractor();
    cv::VideoCapture cam = get_cam(0);
    cv::Mat frame;
    cv::Mat gray_frame;
    cv::Mat fg_mask;

    update_background(cam, backSub, 30);
    while (true) 
    {
        cam >> frame;
        cv::cvtColor(frame, gray_frame, cv::COLOR_BGR2GRAY);
        
        std::vector<BoundingBox> finalDetections = get_detections(backSub, gray_frame);

        // Draw bounding boxes on frame
        draw_bboxes(gray_frame, finalDetections);
        int counter = 0;

        for(BoundingBox detection : finalDetections)
        {
            if(detection.area > 200)
            {
                counter += 1;
            }
            else if(counter > 0)
            {
                counter -= 1;
            }
        }
        if (counter == 10)
        {
            cv::imshow("camera", gray_frame);
        }
        

        char k = cv::waitKey(10);
        if (k == 27) { // Press 'ESC' to exit
            break;
        }
    }

    return 0;
}