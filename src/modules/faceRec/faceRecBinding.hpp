#pragma once

#include <node.h>
#include <v8.h>

v8::Handle<v8::Value> detectFaces(const v8::Arguments& args);

bool initializeFaceDetector();