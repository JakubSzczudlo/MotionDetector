#ifndef CAMERA_HPP
#define CAMERA_HPP
#include <opencv2/opencv.hpp>


class AbstractCamera
{
    private:
        uint16_t frame_width;
        uint16_t frame_height;
    public:
        virtual cv::Mat read_gray_image(void) = 0;
        AbstractCamera(uint16_t width, uint16_t height): frame_width(width), frame_height(height) {};
        virtual ~AbstractCamera() = default;
};

class EmbeddedCamera : AbstractCamera
{
    private:
        const uint8_t CAMERA_ID = 0;
        cv::VideoCapture camera_handler;
    public:
        cv::Mat read_gray_image(void) override;
        EmbeddedCamera(uint16_t width, uint16_t height);
        ~EmbeddedCamera() override {camera_handler.release();};
};





#endif // CAMERA_HPP