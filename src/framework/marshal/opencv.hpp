#pragma once

#include <opencv2/opencv.hpp>
#include <v8.h>

typedef v8::Local<v8::Value> V8Result;

V8Result MarshalFromNative(const cv::Size& value);
V8Result MarshalFromNative(const cv::Point& value);
V8Result MarshalFromNative(const cv::Point2f& value);
V8Result MarshalFromNative(const cv::Rect& value);
V8Result MarshalFromNative(const cv::Scalar& value);
V8Result MarshalFromNative(const cv::Matx33f& value);
V8Result MarshalFromNative(const cv::Mat& value);

template<typename _Tp, int cn>
V8Result MarshalFromNative(const cv::Vec<_Tp, cn>& value)
{
	using namespace v8;
    NanEscapableScope();

	Local<Array> result = NanNew<Array>(cn);

	for (size_t i = 0; i < cn; i++)
	{
		result->Set(i, MarshalFromNative(value.val[i]));
	}

	return NanEscapeScope(result);
}

bool MarshalToNativeImage(V8Result buffer, cv::Mat& frame, int flags);


bool MarshalToNative(V8Result obj, cv::Point2f& value);
bool MarshalToNative(V8Result obj, cv::Size& value);
