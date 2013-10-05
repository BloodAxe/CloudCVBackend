#include "Marshal.hpp"

using namespace v8;

Marshal::NativeResult Marshal::Native(int value)
{
	HandleScope scope;
	return scope.Close(Int32::New(value));
}

Marshal::NativeResult Marshal::Native(size_t value)
{
	HandleScope scope;
	return scope.Close(Uint32::New(value));
}

Marshal::NativeResult Marshal::Native(float value)
{ 
	HandleScope scope;
	return scope.Close(Number::New(value));
}

Marshal::NativeResult Marshal::Native(double value)
{
	HandleScope scope;
	return scope.Close(Number::New(value));
}

Marshal::NativeResult Marshal::Native(const char * value)
{ 
	HandleScope scope;
	return scope.Close(String::New(value));
}

Marshal::NativeResult Marshal::Native(const std::string& value)
{
	HandleScope scope;
	return scope.Close(String::New(value.c_str(), value.size()));
}

//////////////////////////////////////////////////////////////////////////
// OpenCV Types

Marshal::NativeResult Marshal::Native(const cv::Size& value)
{
	HandleScope scope;
	Local<Object> structure = Object::New();
	structure->Set(String::NewSymbol("width"),  Native(value.width));
	structure->Set(String::NewSymbol("height"), Native(value.height));
	return scope.Close(structure);
}

Marshal::NativeResult Marshal::Native(const cv::Point& value)
{
	HandleScope scope;
	Local<Object> structure = Object::New();
	structure->Set(String::NewSymbol("x"), Native(value.x));
	structure->Set(String::NewSymbol("y"), Native(value.y));
	return scope.Close(structure);	
}

Marshal::NativeResult Marshal::Native(const cv::Point2f& value)
{
	HandleScope scope;
	Local<Object> structure = Object::New();
	structure->Set(String::NewSymbol("x"), Native(value.x));
	structure->Set(String::NewSymbol("y"), Native(value.y));
	return scope.Close(structure);
}

Marshal::NativeResult Marshal::Native(const cv::Scalar& value)
{
	HandleScope scope;
	Handle<Array> result = Array::New(4);

	for (size_t i = 0; i < 4; i++)
		result->Set(i, Native(value.val[i]));

	return scope.Close(result);
}


//////////////////////////////////////////////////////////////////////////
// CloudCV Structures

Marshal::NativeResult Marshal::Native(const Distribution& value)
{
	HandleScope scope;
	Local<Object> structure = Object::New();
	structure->Set(String::NewSymbol("min"),               Native(value.min));
	structure->Set(String::NewSymbol("max"),               Native(value.max));
	structure->Set(String::NewSymbol("average"),           Native(value.average));
	structure->Set(String::NewSymbol("standardDeviation"), Native(value.standardDeviation));
	structure->Set(String::NewSymbol("entropy"),           Native(value.entropy));
	return scope.Close(structure);
}

Marshal::NativeResult Marshal::Native(const rgb8u_color_t& value)
{
	HandleScope scope;
	Local<Object> structure = Object::New();
	structure->Set(String::NewSymbol("min"),     Native(static_cast<int>(value.r)));
	structure->Set(String::NewSymbol("max"),     Native(static_cast<int>(value.g)));
	structure->Set(String::NewSymbol("average"), Native(static_cast<int>(value.b)));
	return scope.Close(structure);
}


