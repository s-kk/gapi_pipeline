//
// Created by 坂井一樹 on 3/9/21.
//

#include "ImageProcess.h"

// Copied from previous CV code
void ImagePreprocess(cv::Mat &src) {
    static cv::Mat kernel_erode = getStructuringElement(cv::MORPH_RECT, cv::Size(3, 5));
    erode(src, src, kernel_erode);

    static cv::Mat kernel_dilate = getStructuringElement(cv::MORPH_RECT, cv::Size(3, 5));
    dilate(src, src, kernel_dilate);

    static cv::Mat kernel_dilate2 = getStructuringElement(cv::MORPH_RECT, cv::Size(3, 5));
    dilate(src, src, kernel_dilate2);

    static cv::Mat kernel_erode2 = getStructuringElement(cv::MORPH_RECT, cv::Size(3, 5));
    erode(src, src, kernel_erode2);
}

// Copied from previous CV code
void ImageProcess(cv::Mat &src) {
    cv::Mat color_channel;
    cv::Mat src_bin_light, src_bin_dim;
    std::vector<cv::Mat> channels;

    cv::split(src, channels);
    int light_threshold;

    // two set of threshold parameters
    color_channel = channels[2];
    light_threshold = 200;
//  color_channel = channels[0];
//  light_threshold = 225;

    cv::threshold(color_channel, src_bin_light, light_threshold, 255, cv::THRESH_BINARY);
    ImagePreprocess(src_bin_light);

    cv::threshold(color_channel, src_bin_dim, 140, 255, cv::THRESH_BINARY);
    ImagePreprocess(src_bin_dim);

    // Show mat
//    cv::imshow("Threshold: light", src_bin_light);
//    cv::waitKey(1);
//    cv::imshow("Threshold: dim", src_bin_dim);
//    cv::waitKey(1);

    std::vector<std::vector<cv::Point>> light_contours_light, light_contours_dim;
    std::vector<cv::Vec4i> hierarchy_light, hierarchy_dim;
    cv::findContours(src_bin_light, light_contours_light, hierarchy_light, cv::RETR_CCOMP, cv::CHAIN_APPROX_NONE);
    cv::findContours(src_bin_dim, light_contours_dim, hierarchy_dim, cv::RETR_CCOMP, cv::CHAIN_APPROX_NONE);

}

void ImageProcessWithGapi() {
    static cv::Mat morph_kernel = getStructuringElement(cv::MORPH_RECT, cv::Size(3, 5));
    std::vector<std::vector<cv::Point>> light_contours_light, light_contours_dim;
    std::vector<cv::Vec4i> hierarchy_light, hierarchy_dim;

//  Parameters
//  int light_threshold;
//  color_channel = channels[2];
//  light_threshold = 200;
//  color_channel = channels[0];
//  light_threshold = 225;

    // Lambda expression
    cv::GComputation base_pipeline([]() {
        cv::GMat in;

        cv::GMat b, g, r;
        std::tie(b, g, r) = cv::gapi::split3(in);

        cv::GMat threshold_1 = cv::gapi::threshold(b, 200, 255, cv::THRESH_BINARY);
        cv::GMat threshold_2 = cv::gapi::threshold(b, 140, 255, cv::THRESH_BINARY);

        cv::GMat threshold_1_erode_1 = cv::gapi::erode(threshold_1, morph_kernel);
        cv::GMat threshold_1_dilate_1 = cv::gapi::dilate(threshold_1, morph_kernel);
        cv::GMat threshold_1_dilate_2 = cv::gapi::dilate(threshold_1, morph_kernel);
        cv::GMat threshold_1_erode_2 = cv::gapi::erode(threshold_1, morph_kernel);

        cv::GMat threshold_2_erode_1 = cv::gapi::erode(threshold_2, morph_kernel);
        cv::GMat threshold_2_dilate_1 = cv::gapi::dilate(threshold_2, morph_kernel);
        cv::GMat threshold_2_dilate_2 = cv::gapi::dilate(threshold_2, morph_kernel);
        cv::GMat threshold_2_erode_2 = cv::gapi::erode(threshold_2, morph_kernel);

        cv::GArray<cv::GArray<cv::Point>> contour_point_1, contour_point_2;
        cv::GArray<cv::Vec4i> hierarchy_1, hierarchy_2;

        std::tie(contour_point_1, hierarchy_1) =
                cv::gapi::findContoursH(threshold_1_erode_2, cv::RETR_CCOMP, cv::CHAIN_APPROX_NONE);
        std::tie(contour_point_2, hierarchy_2) =
                cv::gapi::findContoursH(threshold_2_erode_2, cv::RETR_CCOMP, cv::CHAIN_APPROX_NONE);

        // 6 Outputs
        // * threshold_1_erode_2    final erode mat for threshold parameter 1
        // * threshold_2_erode_2    final erode mat for threshold parameter 2
        // * contour_point_1        find contour result (a two-dimensional point set)
        // * hierarchy_1            find contour result (contour relationships)
        // * contour_point_2        find contour result (a two-dimensional point set)
        // * hierarchy_2            find contour result (contour relationships)
        // You can simplify these results when debugging
        return cv::GComputation(cv::GIn(in),
                                cv::GOut(threshold_1_erode_2, threshold_2_erode_2,
                                         contour_point_1, hierarchy_1,
                                         contour_point_2, hierarchy_2));

    });

    auto pipeline = base_pipeline.compileStreaming();
    auto in_src = cv::gapi::wip::make_src<cv::gapi::wip::GCaptureSource>("/path/to/your/source/video");
    pipeline.setSource(cv::gin(in_src));
    pipeline.start();
    cv::Mat thres_1, thres_2;
    bool no_show = true;

    auto out_vector = cv::gout(thres_1, thres_2,
                               light_contours_light, hierarchy_light,
                               light_contours_dim, hierarchy_dim);

    while (pipeline.running()) {
        if (no_show) {
            // Fatal bug occurs here
            if (!pipeline.pull(std::move(out_vector)))
                break;
        } else if (!pipeline.try_pull(std::move(out_vector))) {
            if (cv::waitKey(1) >= 0) break;
            else continue;
        }
        // Show mat
//        cv::imshow("thres_1", thres_1);
//        cv::waitKey(1);
//        cv::imshow("thres_2", thres_2);
//        cv::waitKey(1);
    }
}