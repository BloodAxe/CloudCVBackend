#include <node.h>
#include <node_buffer.h>
#include <v8.h>

#include "src/modules/buildInformation/buildInformation.hpp"
#include "src/modules/analyze/binding.hpp"
#include "src/modules/faceRec/faceRecBinding.hpp"
#include "src/modules/markerDetection/MarkerDetectionBinding.hpp"

using namespace v8;
using namespace node;

void RegisterModule(Handle<Object> target)
{
    target->Set(String::NewSymbol("buildInformation"),
                FunctionTemplate::New(buildInformation)->GetFunction());

    target->Set(String::NewSymbol("analyze"),
                FunctionTemplate::New(analyzeImage)->GetFunction());

    target->Set(String::NewSymbol("detectMarkers"),
                FunctionTemplate::New(detectMarkers)->GetFunction());

    if (initializeFaceDetector())
    {
        target->Set(String::NewSymbol("detectFaces"),
            FunctionTemplate::New(detectFaces)->GetFunction());
    }
}

NODE_MODULE(cloudcv, RegisterModule);
