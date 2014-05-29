#include "marshal.hpp"

using namespace v8;

V8Result MarshalFromNative(int value)
{
	HandleScope scope;
	return scope.Close(Int32::New(value));
}

V8Result MarshalFromNative(unsigned int value)
{
	HandleScope scope;
	return scope.Close(Uint32::New(value));
}

V8Result MarshalFromNative(float value)
{
	HandleScope scope;
	return scope.Close(Number::New(value));
}

V8Result MarshalFromNative(double value)
{
	HandleScope scope;
	return scope.Close(Number::New(value));
}

V8Result MarshalFromNative(const char * value)
{
	HandleScope scope;
	return scope.Close(String::New(value));
}