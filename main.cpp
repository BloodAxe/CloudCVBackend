#include <node.h>
#include <v8.h>

#include "src/modules/analyze/binding.hpp"
#include "src/modules/buildInformation/buildInformation.hpp"
#include "src/modules/cameraCalibration/CameraCalibrationBinding.hpp"

using namespace v8;
using namespace node;
using namespace cloudcv;

void RegisterModule(Handle<Object> target)
{
    target->Set(String::NewSymbol("buildInformation"),
                FunctionTemplate::New(buildInformation)->GetFunction());

    target->Set(String::NewSymbol("analyze"),
                FunctionTemplate::New(analyzeImage)->GetFunction());

    target->Set(String::NewSymbol("calibrationPatternDetect"),
                FunctionTemplate::New(calibrationPatternDetect)->GetFunction());

    target->Set(String::NewSymbol("calibrateCamera"),
                FunctionTemplate::New(calibrateCamera)->GetFunction());
}

NODE_MODULE(cloudcv, RegisterModule);
