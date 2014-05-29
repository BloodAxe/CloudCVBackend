#pragma once

#include <opencv2/opencv.hpp>
#include <node.h>
#include <node_buffer.h>
#include <v8.h>

#define kImageTypePng   "image/png"
#define kImageTypeJpeg  "image/jpeg"

std::string toDataUri(const cv::Mat& img, const char * imageMimeType = kImageTypePng);

