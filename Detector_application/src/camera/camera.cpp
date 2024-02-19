#include "camera.hpp"
#include <iostream>

 EmbeddedCamera::EmbeddedCamera(uint16_t width, uint16_t height) : AbstractCamera(width, height)
 {
    camera_handler.open(CAMERA_ID);
    camera_handler.set(cv::CAP_PROP_FRAME_WIDTH, frame_width);
    camera_handler.set(cv::CAP_PROP_FRAME_HEIGHT, frame_height);
    if(!camera_handler.isOpened())
    {
        std::cerr << "ERROR! Unable to open camera\n";
        return;
    }
 }

cv::Mat EmbeddedCamera::read_gray_image(void)
{
    cv::Mat frame;
    cv::Mat grayed_frame;
    camera_handler >> frame;
    cv::cvtColor(frame, grayed_frame, cv::COLOR_BGR2GRAY);
    return grayed_frame;
}