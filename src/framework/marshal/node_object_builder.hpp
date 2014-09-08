#pragma once

#include <v8.h>
#include <nan.h>

#include <framework/marshal/marshal.hpp>

typedef v8::Local<v8::Value> V8Result;

class NodeObject;

class NodeObjectProperty
{
public:
	NodeObjectProperty(v8::Persistent<v8::Object> parent, const std::string& propertyName);
	~NodeObjectProperty();

	template <typename Val>
	NodeObjectProperty& operator=(const Val& val);

	NodeObjectProperty operator[](const std::string& propertyName);
    NodeObjectProperty operator[](size_t propertyIdx);

private:
	v8::Persistent<v8::Object> m_parent;
	std::string                m_propertyName;
};

class NodeObject
{
public:
	NodeObject(v8::Local<v8::Object>& target);

	NodeObjectProperty operator[](const std::string& propertyName);
    NodeObjectProperty operator[](size_t propertyIdx);

private:
	v8::Local<v8::Object>& m_target;
};

template <typename Val>
NodeObjectProperty& NodeObjectProperty::operator=(const Val& val)
{
	m_parent->Set(NanNew(m_propertyName.c_str()), MarshalFromNative(val));
	return *this;
}