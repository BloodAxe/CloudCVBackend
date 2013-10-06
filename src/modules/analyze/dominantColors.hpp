#pragma once

#include "modules/common/Numeric.hpp"

#include <opencv2/opencv.hpp>
#include <set>

struct Color
{
	size_t hash;
	size_t count;

	Color() : hash(0), count(0) {}
	Color(int h, int c) : hash(h), count(c) {}

	int distanceTo(const Color& other) const;
	int distanceTo(const cv::Vec3b& other) const;

	bool operator< (const Color& other) const 
	{
		return hash < other.hash;
	}
};

struct DominantColor
{
	cv::Vec3b color;
	size_t    totalPixels;

	float     interclassVariance;
	float     error;
};



class DominantColorsExtractor
{
public:
	void process(const cv::Mat_<cv::Vec3b>& bgrImage);

	std::vector<DominantColor> mainColors;

	cv::Mat getImage() const;

	RGBDistribution getColorDeviation() const;

	size_t getUniqueColors() const;
	size_t getRedicedColors() const;

protected:
	typedef std::set<size_t> ColorsSet;

	bool findLargestColorSet(int similarityTolerance, int minPixelsInSet, const std::set<Color>& input, std::set<Color>& colorsSet) const;
	DominantColor computeFinalColor(const std::set<Color>& colorsSet) const;

private:
	std::vector<unsigned char> rVec, gVec, bVec;
	std::map<size_t, size_t> fullColorsTable;
	std::map<size_t, size_t> quantizedColorsTable;

};

