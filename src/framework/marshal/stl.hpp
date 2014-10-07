#pragma once

#include <string>
#include <vector>
#include <map>
#include <array>
#include <v8.h>

typedef v8::Local<v8::Value> V8Result;

V8Result MarshalFromNative(const std::string& value);


template <typename T>
V8Result MarshalFromNative(const std::vector<T>& values)
{
	using namespace v8;
	NanEscapableScope();

    uint32_t length = static_cast<uint32_t>(values.size());
    Local<Array> result = NanNew<Array>(length);

    for (uint32_t i = 0; i < length; i++)
	{
		const T& item = values[i];
		result->Set(i, MarshalFromNative(item));
	}

	return NanEscapeScope(result);
}

template<typename _Val_type>
V8Result MarshalFromNative(const std::map<std::string, _Val_type>& values)
{
	using namespace v8;

	NanEscapableScope();
	Local<Object> structure = NanNew<Object>();

	typedef typename std::map<std::string, _Val_type>::const_iterator const_iterator;
	for (const_iterator it = values.begin(); it != values.end(); ++it)
	{
		structure->Set(MarshalFromNative(it->first), MarshalFromNative(it->second));
	}

	return NanEscapeScope(structure);
}

template<typename _Tp, size_t _Size>
V8Result MarshalFromNative(const std::array<_Tp, _Size>& values)
{
    using namespace v8;
    NanEscapableScope();

    uint32_t length = static_cast<uint32_t>(values.size());
    Local<Array> result = NanNew<Array>(length);

    for (uint32_t i = 0; i < length; i++)
    {
        const _Tp& item = values[i];
        result->Set(i, MarshalFromNative(item));
    }

    return NanEscapeScope(result);
}

//////////////////////////////////////////////////////////////////////////

bool MarshalToNative(V8Result inVal, std::string& outVal);

template<typename _Tp>
bool MarshalToNative(V8Result obj, std::vector<_Tp>& value)
{
    if (obj->IsArray())
    {
        uint32_t len = obj.As<v8::Array>()->Length();
        value.resize(len);

        bool converted = true;
        
        for (uint32_t i = 0; i < len && converted; i++)
        {
            V8Result item = obj.As<v8::Array>()->Get(i);
            converted &= MarshalToNative(item, value[i]);
        }

        return converted;
    }

    return false;
}
