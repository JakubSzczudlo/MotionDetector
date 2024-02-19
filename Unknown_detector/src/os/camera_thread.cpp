#include <camera.hpp>
#include <thread>
#include "os.hpp"
#include "queue.hpp"

void camera_thread(concurrent_queue<cv::Mat> &  photos_queue)
{
    auto cam = EmbeddedCamera(640,480);
    while (true)
    {
        photos_queue.push(cam.read_gray_image());
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}