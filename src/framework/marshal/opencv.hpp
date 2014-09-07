#pragma once

#include <opencv2/opencv.hpp>
#include <v8.h>

typedef v8::Local<v8::Value> V8Result;

V8Result MarshalFromNative(const cv::Size& value);
V8Result MarshalFromNative(const cv::Point& value);
V8Result MarshalFromNative(const cv::Point2f& value);

/*
cv::Mat will be marshalled to JS as a wrapped object with methods to:
- get raw buffer
- get encoded (PNG, JPG, WEBP) image
- write to stream

enum OpenCVMatMarshalType {
    OpenCVMatMarshalTypeImage,
    OpenCVMatMarshalTypeMatrix
};

V8Result MarshalFromNative(const cv::Mat& value, OpenCVMatMarshalType type = OpenCVMatMarshalTypeMatrix);
*/
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

bool MarshalToNativeImage(V8Result buffer, cv::Mat& frame, int flags);


bool MarshalToNative(V8Result obj, cv::Point2f& value);
