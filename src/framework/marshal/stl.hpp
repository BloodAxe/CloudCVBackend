#pragma once

#include <string>
#include <vector>
#include <map>

#include <v8.h>

typedef v8::Local<v8::Value> V8Result;

V8Result MarshalFromNative(const std::string& value);


template <typename T>
V8Result MarshalFromNative(const std::vector<T>& values)
{
	using namespace v8;
	HandleScope scope;

	Handle<Array> result = Array::New(values.size());

	for (size_t i = 0; i < values.size(); i++)
	{
		const T& item = values[i];
		result->Set(i, MarshalFromNative(item));
	}

	return scope.Close(result);
}

template<typename _Val_type>
V8Result MarshalFromNative(const std::map<std::string, _Val_type>& values)
{
	using namespace v8;

	HandleScope scope;
	Local<Object> structure = Object::New();

	typedef typename std::map<std::string, _Val_type>::const_iterator const_iterator;
	for (const_iterator it = values.begin(); it != values.end(); ++it)
	{
		structure->Set(MarshalFromNative(it->first), MarshalFromNative(it->second));
	}

	return scope.Close(structure);
}