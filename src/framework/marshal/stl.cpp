#include "marshal.hpp"
using namespace v8;

V8Result MarshalFromNative(const std::string& value)
{
	HandleScope scope;
	return scope.Close(String::New(value.c_str(), value.size()));
}

void MarshalToNative(V8Result inVal, std::string& outVal)
{
    v8::String::AsciiValue string(inVal);
    outVal = std::string(string.operator*(), string.length());
}