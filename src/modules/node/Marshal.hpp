#pragma once

#include "modules/common/Numeric.hpp"
#include "modules/common/Color.hpp"

#include <node_buffer.h>
#include <node.h>
#include <v8.h>
#include <string>
#include <opencv2/opencv.hpp>


class Marshal
{
public:	
	typedef v8::Handle<v8::Value> NativeResult;

	//////////////////////////////////////////////////////////////////////////
	// Built-in and std:: types

	NativeResult Native(int value);
	NativeResult Native(size_t value);
	NativeResult Native(float value);
	NativeResult Native(double value);
	NativeResult Native(const char * value);
	NativeResult Native(const std::string& value);

	//////////////////////////////////////////////////////////////////////////
	// OpenCV types
	
	NativeResult Native(const cv::Size& value);
	NativeResult Native(const cv::Point& value);
	NativeResult Native(const cv::Point2f& value);
	NativeResult Native(const cv::Mat& value);
	
	template<typename _Tp, int cn> 
	NativeResult Native(const cv::Vec<_Tp, cn>& value)
	{
		using namespace v8;
		HandleScope scope;

		Handle<Array> result = Array::New(cn);

		for (size_t i = 0; i < values.size(); i++)
		{
			result->Set(i, Native(value.val[i]));
		}

		return scope.Close(result);
	}

	NativeResult Native(const cv::Scalar& value);

	//////////////////////////////////////////////////////////////////////////
	// CloudCV types

	NativeResult Native(const Distribution& value);
	NativeResult Native(const rgb8u_color_t& value);

	template <typename T>
	NativeResult Native(const std::vector<T>& values)
	{
		using namespace v8;
		HandleScope scope;

		Handle<Array> result = Array::New(values.size());

		for (size_t i = 0; i < values.size(); i++)
			(*result)[i] = Native(values[i]);

		return scope.Close(result);
	}



};