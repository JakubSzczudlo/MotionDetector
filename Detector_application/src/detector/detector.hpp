#ifndef DETECTOR_HPP
#define DETECTOR_HPP

#include <vector>
#include <opencv2/opencv.hpp>

struct BoundingBox 
{
    int x1;
    int y1;
    int x2;
    int y2;
    int area;
};

class ImageProcessing
{
    private:
        const uint32_t min_area_threshold = 100;
        const uint32_t binaryzation_treshold = 0;
        const double nms_treshold = 0.1;
        bool is_contained(const BoundingBox& box1, const BoundingBox& box2);
    public:
        ImageProcessing() = default;
        ~ImageProcessing() = default;

        cv::Mat get_motion_mask(const cv::Mat& fg_mask) const;
        std::vector<cv::Rect> get_contour_detections(const cv::Mat& mask) const;
        std::vector<uint32_t> remove_contained_bboxes(const std::vector<BoundingBox>& boxes);
        std::vector<BoundingBox> non_max_suppression(const std::vector<BoundingBox>& boxes, const std::vector<double>& scores);
};

class AbstractDetector
{
    public:
        virtual std::vector<BoundingBox> get_detections(const cv::Mat& frame) = 0;
        AbstractDetector() = default;
        virtual ~AbstractDetector() = default;
};
class KnnBackgroundSubstractor : private AbstractDetector
{
    private:
        const uint8_t frame_times_minimal = 10;
        const uint16_t minimal_detected_object_area = 100*100;
        const int history_lenght;
        cv::Ptr<cv::BackgroundSubtractor> back_substractor;
        ImageProcessing image_processor = ImageProcessing();
    public:
        explicit KnnBackgroundSubstractor(int history);
        ~KnnBackgroundSubstractor() override = default;
        void create_starting_background(std::vector<cv::Mat> const &  init_gray_frames);
        bool isObjectDetected(cv::Mat & gray_frame);
        std::vector<BoundingBox> get_detections(const cv::Mat& frame) override;
        void draw_bboxes(cv::Mat& frame, const std::vector<BoundingBox>& detections) const;
};




#endif // DETECTOR_HPP