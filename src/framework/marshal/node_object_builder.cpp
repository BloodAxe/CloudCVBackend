#include "node_object_builder.hpp"
#include <string>
#include <iostream>
#include <sstream>

using namespace v8;

NodeObjectProperty::NodeObjectProperty(Local<Object> parent, const std::string& propertyName)
	: mParentObject(parent)
	, mPropertyName(propertyName)
{

}

NodeObjectProperty::~NodeObjectProperty()
{
	//m_parent.Dispose();
}

NodeObjectProperty NodeObjectProperty::operator[](const std::string& propertyName)
{
	NanScope();

	Local<Value> prop = mParentObject->Get(NanNew<String>(mPropertyName.c_str()));

	bool propertyExistsAndIsObject = prop->IsObject();

    Local<Object> target;

	if (!propertyExistsAndIsObject)
	{
        target = NanNew<Object>();
		mParentObject->Set(NanNew<String>(mPropertyName.c_str()), target);
	}
	else
	{
		target = Local<Object>::Cast(mParentObject->Get(NanNew<String>(mPropertyName.c_str())));
	}

	return NodeObjectProperty(target, propertyName);
}

NodeObjectProperty NodeObjectProperty::operator[](size_t propertyIdx)
{
    return this->operator[](lexical_cast(propertyIdx));
}

NodeObject::NodeObject(Local<Object> target)
	: mTargetObject(target)
{

}

NodeObjectProperty NodeObject::operator[](const std::string& propertyName)
{
    return NodeObjectProperty(mTargetObject, propertyName);
}

NodeObjectProperty NodeObject::operator[](size_t propertyIdx)
{
    return NodeObjectProperty(mTargetObject, lexical_cast(propertyIdx));
}
