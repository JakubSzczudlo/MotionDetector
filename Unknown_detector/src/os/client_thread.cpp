#include <stdio.h>
#include "os.hpp"
#include "client.hpp"
#include <thread>

void client_thread(concurrent_queue<cv::Mat> & tcp_send_queue)
{
    auto socket_client = SimpleSocketClient(8080, "192.168.0.38");
    cv::Mat image_to_send;
    auto last_send_time = std::chrono::steady_clock::now();
    while (true)
    {
        tcp_send_queue.wait_and_pop(image_to_send);
        auto current_time = std::chrono::steady_clock::now();
        double elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(current_time - last_send_time).count();
        if(elapsed_seconds > 0.5)
        {
            last_send_time = std::chrono::steady_clock::now();
            socket_client.send_data(image_to_send);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}