#include <iostream>
#include <thread>

#include <os.hpp>



int main()
{
    std::string hello{"Hello from client"};

    concurrent_queue<cv::Mat> photos_queue;
    concurrent_queue<cv::Mat> tcp_send_queue;

    std::thread thread_camera(camera_thread, std::ref(photos_queue));
    std::thread thread_detector(detector_thread, std::ref(photos_queue), std::ref(tcp_send_queue));
    std::thread thread_tcp(client_thread, std::ref(tcp_send_queue));

    // Wait for all threads to finish their execution
    thread_camera.join();
    thread_detector.join();
    thread_tcp.join();

    return 0;
}