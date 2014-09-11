#pragma once

#include <v8.h>
#include <nan.h>

#include <framework/marshal/marshal.hpp>

typedef v8::Local<v8::Value> V8Result;

class NodeObject;
class NodeObjectProperty;

class NodeObjectProperty
{
public:
	NodeObjectProperty(v8::Local<v8::Object> parent, const std::string& propertyName);
	~NodeObjectProperty();

	template <typename Val>
	NodeObjectProperty& operator=(const Val& val);

	NodeObjectProperty operator[](const std::string& propertyName);
    NodeObjectProperty operator[](size_t propertyIdx);

private:
    v8::Local<v8::Object> mParentObject;
	std::string           mPropertyName;
};

class NodeObject
{
public:
	NodeObject(v8::Local<v8::Object> target);

	NodeObjectProperty operator[](const std::string& propertyName);
    NodeObjectProperty operator[](size_t propertyIdx);

private:
	v8::Local<v8::Object> mTargetObject;
};

template <typename Val>
NodeObjectProperty& NodeObjectProperty::operator=(const Val& val)
{
	mParentObject->Set(NanNew(mPropertyName.c_str()), MarshalFromNative(val));
	return *this;
}