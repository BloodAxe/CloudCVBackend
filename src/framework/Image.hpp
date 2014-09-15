#pragma once

#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <nan.h>


namespace cloudcv
{
    /**
     * @brief A wrapper around cv::Mat type that is returned in callbacks to Node.js.
     * @details This class provide basic methods to get image dimension and type, but 
     *          it's goal to allow 'export' binary data to Jpg/Png/Webp/Json formats.
     */
	class ImageView : public node::ObjectWrap
    {
    public:
        explicit ImageView(const cv::Mat& image);
        ~ImageView() {}
    
        static void Init(v8::Handle<v8::Object> exports);
        static v8::Handle<v8::Value> New(_NAN_METHOD_ARGS_TYPE args);
        static v8::Handle<v8::Value> ViewForImage(cv::Mat image);

        static v8::Persistent<v8::Function> constructor;
    private:

        static NAN_METHOD(AsJpegStream);
        static NAN_METHOD(AsJpegDataUri);

        static NAN_METHOD(AsPngStream);
        static NAN_METHOD(AsPngDataUri);

        static NAN_METHOD(AsObject);

        static NAN_METHOD(Width);
        static NAN_METHOD(Height);
        static NAN_METHOD(Channels);
        static NAN_METHOD(Type);
        static NAN_METHOD(Stride);

    private:
        cv::Mat mImage;    
    };

}

