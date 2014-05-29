#pragma once

#include <string>
#include <vector>
#include <map>

#include <v8.h>

typedef v8::Local<v8::Value> V8Result;


V8Result MarshalFromNative(int value);
V8Result MarshalFromNative(unsigned int value);
V8Result MarshalFromNative(float value);
V8Result MarshalFromNative(double value);
V8Result MarshalFromNative(const char * value);