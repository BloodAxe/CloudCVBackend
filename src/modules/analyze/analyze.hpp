#pragma once

#include <opencv2/opencv.hpp>

namespace cloudcv
{
    struct Distribution
    {
        float min;
        float max;
        float average;
        float standardDeviation;
        float entropy;
    };

    /**
     * Result of image analyze
     */
    struct AnalyzeResult
    {
        cv::Mat source;

        cv::Size frameSize;   // Size of input image       
        cv::Size aspectRatio; // Aspect ratio of the input image

        // Histogram
        cv::Mat colorHistogram;
        cv::Mat grayImage;

        Distribution intensity;
        
        float   rmsContrast;

        // Color analyze
        size_t                 uniqieColors;
        size_t                 reducedColors;
        std::vector<cv::Vec3b> dominantColors;
        cv::Mat                dominantColorsImage;

        Distribution   redDeviation;
        Distribution   greenDeviation;
        Distribution   blueDeviation;

        // Edges
        cv::Mat cannyImage;
        int     cannyLowerThreshold;
        int     cannyUpperThreshold;
        int     apertureSize;

        // Morphologic analyze
        cv::Mat                houghImage;
        std::vector<cv::Vec4i> houghLines;
        std::vector<cv::Vec4i> houghCircles;

        float processingTimeMs;
    };

    void analyzeImage(cv::Mat src, AnalyzeResult& result);

}