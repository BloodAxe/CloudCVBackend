#pragma once 

#include <opencv2/opencv.hpp>
#include <stdint.h>

struct ScopedTimer
{
    inline ScopedTimer()
    {
        m_startTime = cv::getTickCount();
    }
    
    inline double executionTimeMs() const
    {
        return (cv::getTickCount() - m_startTime) * 1000. / cv::getTickFrequency();
    }
    
    int64_t m_startTime;
};
