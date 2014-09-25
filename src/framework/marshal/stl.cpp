#include "marshal.hpp"
using namespace v8;

V8Result MarshalFromNative(const std::string& value)
{
	NanEscapableScope();
	return NanEscapeScope(NanNew<String>(value.c_str()));
}

void MarshalToNative(V8Result inVal, std::string& outVal)
{
    NanAsciiString cStr = NanAsciiString(inVal);

    outVal = std::string(*cStr, cStr.Size());
}