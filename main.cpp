#include <iostream>
#include <opencv2/opencv.hpp>
#include <spdlog/spdlog.h>

#include "ImageProcess.h"

int main() {
//    cv::VideoCapture cam("/path/to/your/source/video");
//    cv::VideoCapture cam(0);
    cv::Mat src = cv::imread("/path/to/your/source/video");
    cv::TickMeter meter;
    double time = 0.0;
    int frame_count = 0;
    
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hier;


    // Trad
//    meter.start();
//    while (1) {
//        cam >> src;
//        frame_count++;
//        if (src.empty()) {
//            break;
//        } else {
//            ImageProcess(src);
//        }
//    }
//    meter.stop();

    // G-API
    for (int i = 0; i < 10; i++) {
        meter.start();
        ImageProcessWithGapi();
        meter.stop();
        spdlog::info("Round: {0}", i);
        spdlog::info("Time taken: {0}", meter.getTimeSec());
        time = time + meter.getTimeSec();
        meter.reset();
    }

    return 0;
}
