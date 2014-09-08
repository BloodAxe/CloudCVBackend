#pragma once

#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <nan.h>


namespace cloudcv
{
	class ImageView : node::ObjectWrap
    {
    public:
        explicit ImageView(const cv::Mat& image);
        ~ImageView() {}
    
        static void Init(v8::Handle<v8::Object> exports);
        static v8::Handle<v8::Value> New(const v8::Arguments &args);

        static v8::Persistent<v8::Function> constructor;
    private:

        static NAN_METHOD(SaveAsJpeg);
        static NAN_METHOD(SaveAsPng);
        static NAN_METHOD(GetBuffer);

        static NAN_METHOD(Width);
        static NAN_METHOD(Height);
        static NAN_METHOD(Channels);
        static NAN_METHOD(Type);
        static NAN_METHOD(Stride);

    private:
        cv::Mat mImage;    
    };

}

