#pragma once

#include <opencv2/opencv.hpp>

namespace cloudcv
{
    /**
     * Result of image analyze
     */
    struct AnalyzeResult
    {
        // Size of input image
        cv::Size size;

        // Aspect ratio of the input image
        cv::Size aspectRatio;

        cv::Mat histogram;
        cv::Mat laplaccian;
        cv::Mat canny;
        cv::Mat lines;

        float contrast;
        float brightness;

        float colorDeviation;
        float colorEntropy;

        float processingTimeMs;
    };

    void analyzeImage(cv::Mat src, AnalyzeResult& result);

}