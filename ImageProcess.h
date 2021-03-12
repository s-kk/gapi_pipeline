//
// Created by 坂井一樹 on 3/9/21.
//

#ifndef REMOTE_TEST_IMAGEPROCESS_H
#define REMOTE_TEST_IMAGEPROCESS_H

#include <opencv2/opencv.hpp>
#include <opencv2/gapi.hpp>
#include <opencv2/gapi/core.hpp>
#include <opencv2/gapi/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/gapi/video.hpp>
#include <opencv2/gapi/streaming/cap.hpp>

void ImagePreprocess(cv::Mat &);

void ImageProcess(cv::Mat &);

void ImageProcessWithGapi();

#endif //REMOTE_TEST_IMAGEPROCESS_H
