#pragma once

#include <opencv2/opencv.hpp>
#include <node.h>
#include <node_buffer.h>
#include <v8.h>

#define kImageTypePng   "image/png"
#define kImageTypeJpeg  "image/png"

std::string toDataUri(const cv::Mat& img, const char * imageMimeType = kImageTypePng);


/**
 * Prints a human-friendly size like 2G, 153Mb 54K, 1M, etc..
 */
std::string humanSize(size_t sizeInBytes);

class ObjectBuilder
{
public:
    typedef v8::Local<v8::Object> NodeObject;

    ObjectBuilder(NodeObject& obj);

    ObjectBuilder& Set(const char * name, int value);
    ObjectBuilder& Set(const char * name, size_t value);
    ObjectBuilder& Set(const char * name, float value);
    ObjectBuilder& Set(const char * name, double value);
    ObjectBuilder& Set(const char * name, const char * value);
    ObjectBuilder& Set(const char * name, const std::string& value);

    ObjectBuilder& Set(const char * name, const cv::Size& size);
    
private:
    NodeObject& m_object;
};
