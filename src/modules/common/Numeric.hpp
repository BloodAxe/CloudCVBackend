#pragma once

#include <opencv2/opencv.hpp>
#include <stdint.h>

size_t gcd(size_t u, size_t v);

struct Distribution
{
	float min;
	float max;
	float average;
	float standardDeviation;
	float entropy;
};

struct RGBDistribution
{
	Distribution r;
	Distribution g;
	Distribution b;
};
	
Distribution distribution(cv::InputArray data);
