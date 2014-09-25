#include "node_object_builder.hpp"
#include <string>
#include <iostream>
#include <sstream>
#include <framework/Logger.h>

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
