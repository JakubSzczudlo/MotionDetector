#ifndef OS_HPP
#define OS_HPP
#include "queue.hpp"
#include <opencv2/opencv.hpp>

void detector_thread(concurrent_queue<cv::Mat> & photos_queue, concurrent_queue<cv::Mat> & tcp_send_queue);
void client_thread(concurrent_queue<cv::Mat> & tcp_send_queue);
void camera_thread(concurrent_queue<cv::Mat> &  photos_queue);

#endif // OS_HPP