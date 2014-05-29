#include "buildInformation.hpp"
#include <framework/marshal/marshal.hpp>

namespace cloudcv 
{
	NAN_METHOD(buildInformation)
    {
		NanScope();
		NanReturnValue(MarshalFromNative(cv::getBuildInformation()));
    }
}