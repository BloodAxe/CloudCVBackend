#pragma once

#include <opencv2/opencv.hpp>
#include <v8.h>

typedef v8::Local<v8::Value> V8Result;

V8Result MarshalFromNative(const cv::Size& value);
V8Result MarshalFromNative(const cv::Point& value);
V8Result MarshalFromNative(const cv::Point2f& value);

enum OpenCVMatMarshalType {
    OpenCVMatMarshalTypeImage,
    OpenCVMatMarshalTypeMatrix
};

V8Result MarshalFromNative(const cv::Mat& value, OpenCVMatMarshalType type = OpenCVMatMarshalTypeMatrix);
V8Result MarshalFromNative(const cv::Rect& value);
V8Result MarshalFromNative(const cv::Scalar& value);
V8Result MarshalFromNative(const cv::Matx33f& value);

template<typename _Tp, int cn>
V8Result MarshalFromNative(const cv::Vec<_Tp, cn>& value)
{
	using namespace v8;
	HandleScope scope;

	Handle<Array> result = Array::New(cn);

	for (size_t i = 0; i < cn; i++)
	{
		result->Set(i, MarshalFromNative(value.val[i]));
	}

	return scope.Close(result);
}

bool MarshalToNativeImage(v8::Local<v8::Value> buffer, cv::Mat& frame, int flags);


bool MarshalToNative(V8Result obj, cv::Point2f& value);


template<typename _Tp>
bool MarshalToNative(V8Result obj, std::vector<_Tp>& value)
{
    bool converted = true;

    value.resize(obj.As<v8::Array>()->Length());

    for (size_t i = 0; i < value.size(); i++)
    {
        converted &= MarshalToNative(obj.As<v8::Object>()->Get(i), value[i]);
    }

    return converted;
}
