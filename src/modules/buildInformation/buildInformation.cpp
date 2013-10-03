#include "buildInformation.hpp"
#include "../../node/node_helpers.hpp"

using namespace v8;
using namespace node;

Handle<Value> buildInformation(const Arguments& args)
{
    HandleScope scope;

    std::string resultString = cv::getBuildInformation();

    return String::New(resultString.c_str());
}