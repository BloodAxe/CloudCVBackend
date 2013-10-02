#pragma once

#include "modules/common/Numeric.hpp"
#include "modules/analyze/dominantColors.hpp"

#include <opencv2/opencv.hpp>

namespace cloudcv
{


	/**
	 * Result of image analyze
	 */
	struct AnalyzeResult
	{
		cv::Mat source;
		cv::Mat grayscale;

		cv::Size frameSize;   // Size of input image       
		cv::Size aspectRatio; // Aspect ratio of the input image

		// Histogram
		cv::Mat colorHistogram;
		cv::Mat grayHistogram;

		// Gray scale image
		Distribution intensity; // Distribution of the gray scale image intensity
		float   rmsContrast;    // RMS contrast measure

		// Color analyze
		size_t                     uniqieColors;
		size_t                     reducedColors;
		std::vector<DominantColor> dominantColors;
		cv::Mat                    dominantColorsImage;
		ColorDeviation             colorDeviation;
		

		// Edges
		cv::Mat cannyImage;
		int     cannyLowerThreshold;
		int     cannyUpperThreshold;
		int     apertureSize;

		// Morphologic analyze
		cv::Mat                houghImage;
		std::vector<cv::Vec4i> houghLines;
		std::vector<cv::Vec4i> houghCircles;

		double processingTimeMs;
	};

	std::ostream& operator<<(std::ostream& out, const AnalyzeResult& res);

	void analyzeImage(cv::Mat src, AnalyzeResult& result);

}