#include "marshal.hpp"
#include "framework/Logger.h"
using namespace v8;

V8Result MarshalFromNative(const std::string& value)
{
	NanEscapableScope();
	return NanEscapeScope(NanNew<String>(value.c_str()));
}

bool MarshalToNative(V8Result inVal, std::string& outVal)
{
    TRACE_FUNCTION;
    
    if (inVal->IsString()) {
        NanAsciiString cStr(inVal);
        outVal = std::string(*cStr, cStr.Size());
        return true;
    }

    return false;
}