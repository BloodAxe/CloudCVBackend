#pragma once

#include "modules/common/Numeric.hpp"
#include "modules/analyze/dominantColors.hpp"

#include <opencv2/opencv.hpp>

struct ImageInformation
{
	cv::Mat  sourceImage;       // Source image (can be resized)
	cv::Mat  grayscaleImage;	// Grayscale image
	cv::Size frameSize;			// Size of input image       
	cv::Size aspectRatio;		// Aspect ratio of the input image

	bool hasColor;
};

struct IntensityInformation
{
	cv::Mat_<uint8_t> image;
	cv::Mat      histogramImage;
	Distribution intensity;		// Distribution of the gray scale image intensity
	float        rmsContrast;   // RMS contrast measure
};

struct ColorsInformation
{
	cv::Mat					   histogramImage;
	cv::Mat                    dominantColorsImage;

	size_t                     uniqieColors;
	size_t                     reducedColors;
	std::vector<DominantColor> dominantColors;
	RGBDistribution            colorDeviation;
};

struct MorphologicInformation
{
	cv::Mat cannyImage;

	int     cannyLowerThreshold;
	int     cannyUpperThreshold;
};

/**
* Result of image analyze
*/
struct AnalyzeResult
{
	ImageInformation             common;
	IntensityInformation         grayscale;
	ColorsInformation            color;
	MorphologicInformation       edges;

	std::map<std::string, float> profiling; 
};

std::ostream& operator<<(std::ostream& out, const AnalyzeResult& res);

void buildFromImage(cv::Mat input, ImageInformation& value);
void buildFromImage(cv::Mat input, IntensityInformation& value);
void buildFromImage(cv::Mat input, ColorsInformation& value);
void buildFromImage(cv::Mat input, MorphologicInformation& value, int cannyLower, int cannyUpper);
void buildFromImage(cv::Mat input, AnalyzeResult& value);
