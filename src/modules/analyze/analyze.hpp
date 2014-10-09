#pragma once

#include "modules/common/Numeric.hpp"
#include "modules/analyze/dominantColors.hpp"

#include <opencv2/opencv.hpp>
#include <array>

namespace cloudcv
{

    /**
    * Result of image analyze
    */
    struct AnalyzeResult
    {
        cv::Size                    frameSize;		// Size of input image       
        cv::Size                    aspectRatio;	// Aspect ratio of the input image

        Distribution                intensity;		// Distribution of the gray scale image intensity
        float                       rmsContrast;    // RMS contrast measure
        std::array<cv::Scalar, 255> histogram;
        int                         uniqieColors;
        int                         reducedColors;
        std::vector<DominantColor> dominantColors;
        RGBDistribution            colorDeviation;
    };

    std::ostream& operator<<(std::ostream& out, const AnalyzeResult& res);

    void AnalyzeImage(cv::Mat input, AnalyzeResult& value);

}