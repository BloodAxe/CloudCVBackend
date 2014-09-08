#pragma once

#include <v8.h>
#include <nan.h>
#include <iostream>
#include <sstream>

typedef v8::Local<v8::Value> V8Result;

template <typename T>
std::string lexical_cast(const T& value)
{
    std::ostringstream sSteam;
    sSteam << value;
    return sSteam.str();
}

#include <framework/marshal/primitives.hpp>
#include <framework/marshal/opencv.hpp>
#include <framework/marshal/stl.hpp>
