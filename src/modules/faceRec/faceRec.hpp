#pragma once

#include "modules/common/Numeric.hpp"

#include <opencv2/opencv.hpp>
#include <vector>

struct FaceDetectionResult
{
    cv::Mat               imageWithFaces;
	std::vector<cv::Rect> faces;
	float                 detectionTimeMs;
};

void detectFace(cv::Mat input, FaceDetectionResult& result);
bool initCascades(std::string cascadeName, std::string nestedCascadeName);

std::ostream& operator<<(std::ostream& str, const FaceDetectionResult& res);
