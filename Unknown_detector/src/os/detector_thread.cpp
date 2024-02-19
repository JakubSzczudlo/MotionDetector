#include <detector.hpp>
#include "os.hpp"
#include "queue.hpp"
#include <thread>

void detector_thread(concurrent_queue<cv::Mat> & photos_queue, concurrent_queue<cv::Mat> & tcp_send_queue)
{
    auto detector = KnnBackgroundSubstractor(400);
    
    cv::Mat imageForProcess;
    while (true)
    {
        photos_queue.wait_and_pop(imageForProcess);
        bool is_detected = detector.isObjectDetected(imageForProcess);
        if(is_detected)
        {
            tcp_send_queue.push(imageForProcess);
        }
    }
}