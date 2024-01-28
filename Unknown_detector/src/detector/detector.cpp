#include <detector.hpp>
#include <numeric>
#include <cstdint>
#include <stdexcept>



cv::Mat ImageProcessing::get_motion_mask(const cv::Mat& fg_mask) const
{
    // Threshold the foreground mask
    const cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(9, 9));
    cv::Mat after_thresh;
    cv::threshold(fg_mask, after_thresh, binaryzation_treshold, 255, cv::THRESH_BINARY);

    // Median blur
    cv::Mat after_blur;
    cv::medianBlur(after_thresh, after_blur, 3);

    // Morphological operations
    cv::Mat after_open_close;
    cv::morphologyEx(after_blur, after_open_close, cv::MORPH_OPEN, kernel, cv::Point(-1, -1), 1);
    cv::morphologyEx(after_open_close, after_open_close, cv::MORPH_CLOSE, kernel, cv::Point(-1, -1), 1);

    return after_open_close;
}

std::vector<cv::Rect> ImageProcessing::get_contour_detections(const cv::Mat& mask) const
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
        if (static_cast<uint32_t>(rect.area()) > min_area_threshold) // Area can't be lower than 0
        {
            // Store bounding box coordinates and area in detections
            boundRect.push_back(rect);
        }
    }
    return boundRect;
}

bool ImageProcessing::is_contained(const BoundingBox& box1, const BoundingBox& box2) 
{
    return ((box2.x1 >= box1.x1) && (box2.y1 >= box1.y1) && (box2.x2 <= box1.x2) && (box2.y2 <= box1.y2));
}

std::vector<uint32_t> ImageProcessing::remove_contained_bboxes(const std::vector<BoundingBox>& boxes) 
{
    std::vector<uint32_t> keep;
    for (size_t i = 0; i < boxes.size(); ++i) 
    {
        bool isContainedFlag = false;
        for (size_t j = 0; j < boxes.size(); ++j) 
        {
            if (i != j && is_contained(boxes[i], boxes[j])) 
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

std::vector<BoundingBox> ImageProcessing::non_max_suppression(const std::vector<BoundingBox>& boxes, const std::vector<double>& scores) 
{
    std::vector<uint32_t> order(boxes.size());
    std::iota(order.begin(), order.end(), 0);

    // Sort the boxes by score in descending order
    std::sort(order.begin(), order.end(), [&](uint32_t i, uint32_t j) { return scores[i] > scores[j]; });

    // Remove all contained bounding boxes and get ordered index
    std::vector<uint32_t> keep = remove_contained_bboxes(boxes);

    std::vector<BoundingBox> nmsResult;
    while (!order.empty()) 
    {
        uint32_t i = order[0];

        auto it = std::remove_if(order.begin(), order.end(), [&](uint32_t j) 
        {
            double intersection = std::max(0, std::min(boxes[i].x2, boxes[j].x2) - std::max(boxes[i].x1, boxes[j].x1)) *
                                  std::max(0, std::min(boxes[i].y2, boxes[j].y2) - std::max(boxes[i].y1, boxes[j].y1));
            double unionArea = (boxes[i].x2 - boxes[i].x1) * (boxes[i].y2 - boxes[i].y1) +
                               (boxes[j].x2 - boxes[j].x1) * (boxes[j].y2 - boxes[j].y1) - intersection;
            double iou = intersection / unionArea;

            // Remove boxes with IoU greater than the threshold
            return iou > nms_treshold;
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

    for (uint32_t idx : keep) 
    {
        nmsResult.push_back(boxes[idx]);
    }

    return nmsResult;
}

KnnBackgroundSubstractor::KnnBackgroundSubstractor(int history) : AbstractDetector(), history_lenght(history)
{
    back_substractor = cv::createBackgroundSubtractorKNN(history_lenght, false);
}

void KnnBackgroundSubstractor::create_starting_background(std::vector<cv::Mat> const & init_gray_frames)
{
    cv::Mat fg_mask;
    if(init_gray_frames.empty())
    {
        throw std::underflow_error("Not enough init frames");
    }
    for(auto const& gray_frame : init_gray_frames)
    {
        back_substractor->apply(gray_frame, fg_mask);
    }
}

std::vector<BoundingBox> KnnBackgroundSubstractor::get_detections(const cv::Mat& frame)
{
    const cv::Mat& kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(9, 9));

    // Update Background Model and get foreground mask
    cv::Mat fg_mask;
    back_substractor->apply(frame, fg_mask);

    // get clean motion mask
    cv::Mat motion_mask = image_processor.get_motion_mask(fg_mask);

    // get initially proposed detections from contours
    std::vector<cv::Rect> detectionsMat = image_processor.get_contour_detections(motion_mask);

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
    std::vector<BoundingBox> finalDetections = image_processor.non_max_suppression(bboxes, scores);

    return finalDetections;
}

void KnnBackgroundSubstractor::draw_bboxes(cv::Mat& frame, const std::vector<BoundingBox>& detections) const
{
    for (const BoundingBox& det : detections) 
    {
        cv::rectangle(frame, cv::Point(det.x1, det.y1), cv::Point(det.x2, det.y2), cv::Scalar(0, 255, 0), 3);
    }
}