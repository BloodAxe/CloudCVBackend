#pragma once

#include <v8.h>
#include <node.h>
#include <nan.h>

namespace cloudcv 
{
    NAN_METHOD(version);
	NAN_METHOD(buildInformation);
}