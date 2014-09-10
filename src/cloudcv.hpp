#pragma once

#include <v8.h>
#include <nan.h>

namespace cloudcv
{
    NAN_METHOD(version);
    NAN_METHOD(buildInformation);
    NAN_METHOD(analyzeImage);
    NAN_METHOD(calibrationPatternDetect);
    NAN_METHOD(calibrateCamera);
}