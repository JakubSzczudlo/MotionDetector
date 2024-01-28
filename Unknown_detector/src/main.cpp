#include <iostream>
#include <stdio.h>
#include "pthread.h"
#include <vector>
#include <unistd.h>
#include <client.hpp>
#include <camera.hpp>
#include <detector.hpp>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <thread>

uint32_t test_detector(KnnBackgroundSubstractor & detector, cv::Mat & gray_frame, uint32_t counter)
{    
    std::vector<BoundingBox> finalDetections = detector.get_detections(gray_frame);

    // Draw bounding boxes on frame
    detector.draw_bboxes(gray_frame, finalDetections);

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
    return counter;
}


int main()
{
    std::string hello{"Hello from client"};
    auto cam = EmbeddedCamera(640,480);
    std::vector<cv::Mat> init_images;
    auto detector = KnnBackgroundSubstractor(400);

    for(int i = 0; i < 30; i++)
    {
        init_images.push_back(cam.read_gray_image());
    }
    detector.create_starting_background(init_images);
    auto socket_client = SimpleSocketClient(8080, "192.168.0.38");
    while(true)
    {
        uint32_t counter = 0;
        cv::Mat gray_frame = cam.read_gray_image();
        counter = test_detector(detector, gray_frame, counter);
        if (counter == 10)
        {
            socket_client.send_data(gray_frame);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    }
    return 0;
}