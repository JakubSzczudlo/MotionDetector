



// int main() 
// {
//     cv::Ptr<cv::BackgroundSubtractor> backSub = create_substractor();
//     cv::VideoCapture cam = get_cam(0);
//     cv::Mat frame;
//     cv::Mat gray_frame;
//     cv::Mat fg_mask;

//     update_background(cam, backSub, 30);
//     while (true) 
//     {
//         cam >> frame;
//         cv::cvtColor(frame, gray_frame, cv::COLOR_BGR2GRAY);
        
//         std::vector<BoundingBox> finalDetections = get_detections(backSub, gray_frame);

//         // Draw bounding boxes on frame
//         draw_bboxes(gray_frame, finalDetections);
//         int counter = 0;

//         for(BoundingBox detection : finalDetections)
//         {
//             if(detection.area > 200)
//             {
//                 counter += 1;
//             }
//             else if(counter > 0)
//             {
//                 counter -= 1;
//             }
//         }
//         if (counter == 10)
//         {
//             cv::imshow("camera", gray_frame);
//         }
        

//         char k = cv::waitKey(10);
//         if (k == 27) { // Press 'ESC' to exit
//             break;
//         }
//     }

//     return 0;
// }