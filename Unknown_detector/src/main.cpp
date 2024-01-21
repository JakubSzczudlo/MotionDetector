#include <iostream>
#include <stdio.h>
#include "pthread.h"
#include "semaphore.h"
#include <unistd.h>
#include <client.hpp>
#include <camera.hpp>
#include <opencv2/opencv.hpp>

// sem_t produced_reader;
// sem_t produced_analyzer;
// pthread_mutex_t mutex_reader_analyzer = PTHREAD_MUTEX_INITIALIZER;
// pthread_mutex_t mutex_analyzer_printer = PTHREAD_MUTEX_INITIALIZER;

int main()
{
    std::string hello{"Hello from client"};
    auto socket_client = SimpleSocketClient(8080, "192.168.0.38");

    auto cam = EmbeddedCamera(640,480);
    cv::Mat img = cam.read_gray_image();
    socket_client.send_data(img);

    return 0;
}