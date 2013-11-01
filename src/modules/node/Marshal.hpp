#pragma once

#include "modules/common/Numeric.hpp"
#include "modules/common/Color.hpp"
#include "modules/analyze/analyze.hpp"
#include "modules/faceRec/faceRec.hpp"

#include <node_buffer.h>
#include <node.h>
#include <v8.h>
#include <string>
#include <opencv2/opencv.hpp>


class Marshal
{
public:	
	typedef v8::Local<v8::Value> NativeResult;

	//////////////////////////////////////////////////////////////////////////
	// Built-in and std:: types

	static NativeResult Native(int value);
	static NativeResult Native(size_t value);
	static NativeResult Native(float value);
	static NativeResult Native(double value);
	static NativeResult Native(const char * value);
	static NativeResult Native(const std::string& value);
	
	template <typename T>
	static NativeResult Native(const std::vector<T>& values)
	{
		using namespace v8;
		HandleScope scope;

		Handle<Array> result = Array::New(values.size());

		for (size_t i = 0; i < values.size(); i++)
			result->Set(i, Native(values[i]));

		return scope.Close(result);
	}

	template<typename _Val_type> 
	static NativeResult Native(const std::map<std::string, _Val_type>& values)
	{
		using namespace v8;

		HandleScope scope;
		Local<Object> structure = Object::New();

		typedef typename std::map<std::string, _Val_type>::const_iterator const_iterator;
		for (const_iterator it = values.begin(); it != values.end(); ++it)
		{
			structure->Set(Native(it->first), Native(it->second));
		}

		return scope.Close(structure);
	}

	//////////////////////////////////////////////////////////////////////////
	// OpenCV types
	
	static NativeResult Native(const cv::Size& value);
	static NativeResult Native(const cv::Point& value);
	static NativeResult Native(const cv::Point2f& value);
	static NativeResult Native(const cv::Mat& value);
    static NativeResult Native(const cv::Rect& value);

	template<typename _Tp, int cn> 
	static NativeResult Native(const cv::Vec<_Tp, cn>& value)
	{
		using namespace v8;
		HandleScope scope;

		Handle<Array> result = Array::New(cn);

		for (size_t i = 0; i < cn; i++)
		{
			result->Set(i, Native(value.val[i]));
		}

		return scope.Close(result);
	}

	static NativeResult Native(const cv::Scalar& value);

	//////////////////////////////////////////////////////////////////////////
	// CloudCV types

	static NativeResult Native(const Distribution& value);
	static NativeResult Native(const RGBDistribution& value);
	static NativeResult Native(const rgb8u_color_t& value);


	static NativeResult Native(const AnalyzeResult& value);
	static NativeResult Native(const ImageInformation& value);
	static NativeResult Native(const IntensityInformation& value);
	static NativeResult Native(const ColorsInformation& value);
	static NativeResult Native(const MorphologicInformation& value);
	static NativeResult Native(const DominantColor& value);

    static NativeResult Native(const FaceDetectionResult& value);

};