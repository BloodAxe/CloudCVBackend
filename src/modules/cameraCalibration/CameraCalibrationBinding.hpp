#pragma once

#include <node.h>
#include <v8.h>
#include <nan.h>

namespace cloudcv 
{
    NAN_METHOD(calibrationPatternDetect);

    /**
     * function calibrateCamera(points[][], callback(error, result))); 
     */
    NAN_METHOD(calibrateCamera);
}