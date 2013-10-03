#include <node.h>
#include <node_buffer.h>
#include <v8.h>

#include "src/modules/buildInformation/buildInformation.hpp"
#include "src/modules/analyze/binding.hpp"

using namespace v8;
using namespace node;

void RegisterModule(Handle<Object> target)
{
    target->Set(String::NewSymbol("buildInformation"),
                FunctionTemplate::New(buildInformation)->GetFunction());

    target->Set(String::NewSymbol("analyze"),
                FunctionTemplate::New(analyzeImage)->GetFunction());
}

NODE_MODULE(cloudcv, RegisterModule);
