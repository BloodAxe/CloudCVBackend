#include <node.h>
#include <v8.h>

#include "framework/Image.hpp"
#include "cloudcv.hpp"

using namespace v8;
using namespace node;
using namespace cloudcv;

void RegisterModule(Handle<Object> target)
{
    NODE_SET_METHOD(target, "version",                  version);
    NODE_SET_METHOD(target, "buildInformation",         buildInformation);
    NODE_SET_METHOD(target, "analyzeImage",             analyzeImage);
    NODE_SET_METHOD(target, "calibrationPatternDetect", calibrationPatternDetect);
    NODE_SET_METHOD(target, "calibrateCamera",          calibrateCamera);
    NODE_SET_METHOD(target, "loadImage",                loadImage);

    ImageView::Init(target);
}

NODE_MODULE(cloudcv, RegisterModule);
