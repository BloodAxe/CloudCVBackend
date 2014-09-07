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

void MarshalToNative(V8Result inVal, int& outVal)
{
    outVal = inVal->Int32Value();
}

void MarshalToNative(V8Result inVal, unsigned int& outVal)
{
    outVal = inVal->Uint32Value();
}

void MarshalToNative(V8Result inVal, float& outVal)
{
    outVal = static_cast<float>(inVal->NumberValue());
}

void MarshalToNative(V8Result inVal, double& outVal)
{
    outVal = inVal->NumberValue();
}