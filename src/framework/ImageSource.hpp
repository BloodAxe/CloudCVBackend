#pragma once

#include <opencv2/opencv.hpp>
#include <memory>
#include <v8.h>

namespace cloudcv
{
    class ImageSource
    {
    public:
        virtual cv::Mat getImage() = 0;
        virtual ~ImageSource() {}

    protected:
        ImageSource() {}
    
    private:
        ImageSource(const ImageSource& src);
        ImageSource& operator=(const ImageSource& src);
    };

    typedef std::shared_ptr<ImageSource> ImageSourcePtr;

    ImageSourcePtr CreateImageSource(const std::string& filepath);
    ImageSourcePtr CreateImageSource(v8::Local<v8::Object> imageBuffer);
}