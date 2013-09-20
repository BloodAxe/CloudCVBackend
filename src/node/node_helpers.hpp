#pragma once

#include <opencv2/opencv.hpp>
#include <node.h>
#include <node_buffer.h>
#include <v8.h>

#define kImageTypePng   "image/png"
#define kImageTypeJpeg  "image/png"

std::string toDataUri(cv::Mat& img, const char * imageMimeType = kImageTypePng);

struct ScopedTimer
{
    inline ScopedTimer()
    {
        m_startTime = cv::getTickCount();
    }
    
    inline float executionTimeMs() const
    {
        return (cv::getTickCount() - m_startTime) * 1000. / cv::getTickFrequency();
    }
    
    int64_t m_startTime;
};


class ObjectBuilder
{
public:
    typedef v8::Local<v8::Object> NodeObject;

    ObjectBuilder(NodeObject& obj);

    ObjectBuilder& Set(const char * name, int value);
    ObjectBuilder& Set(const char * name, size_t value);
    ObjectBuilder& Set(const char * name, float value);
    ObjectBuilder& Set(const char * name, const char * value);
    ObjectBuilder& Set(const char * name, const std::string& value);

    ObjectBuilder& Set(const char * name, const cv::Size& size);
    
private:
    NodeObject& m_object;
};
