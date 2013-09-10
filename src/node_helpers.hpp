#pragma once

#include <opencv2/opencv.hpp>

namespace cloudcv
{
    std::string base64encode(cv::Mat& input);
    
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
}