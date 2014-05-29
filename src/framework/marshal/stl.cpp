#include "marshal.hpp"
using namespace v8;

V8Result MarshalFromNative(const std::string& value)
{
	HandleScope scope;
	return scope.Close(String::New(value.c_str(), value.size()));
}