#include "node_object_builder.hpp"
#include <string>
#include <iostream>
#include <sstream>

using namespace v8;

NodeObjectProperty::NodeObjectProperty(Persistent<Object> parent, const std::string& propertyName)
	: m_parent(parent)
	, m_propertyName(propertyName)
{

}

NodeObjectProperty::~NodeObjectProperty()
{
	m_parent.Dispose();
}

NodeObjectProperty NodeObjectProperty::operator[](const std::string& propertyName)
{
	NanScope();

	Local<Value> prop = m_parent->Get(NanNew(m_propertyName.c_str()));

	bool propertyExistsAndIsObject = prop->IsObject();

	Persistent<Object> target;

	if (!propertyExistsAndIsObject)
	{
		target = Persistent<Object>::New(Object::New());
		m_parent->Set(NanNew(m_propertyName.c_str()), target);
	}
	else
	{
		target = Persistent<Object>::New( Local<Object>::Cast(m_parent->Get(NanNew(m_propertyName.c_str()))));
	}

	return NodeObjectProperty(target, propertyName);
}

NodeObjectProperty NodeObjectProperty::operator[](size_t propertyIdx)
{
    return this->operator[](lexical_cast(propertyIdx));
}

NodeObject::NodeObject(Local<Object>& target)
	: m_target(target)
{

}

NodeObjectProperty NodeObject::operator[](const std::string& propertyName)
{
    NanScope();
    return NodeObjectProperty(Persistent<Object>::New(m_target), propertyName);
}

NodeObjectProperty NodeObject::operator[](size_t propertyIdx)
{
    NanScope();
    return NodeObjectProperty(Persistent<Object>::New(m_target), lexical_cast(propertyIdx));
}
