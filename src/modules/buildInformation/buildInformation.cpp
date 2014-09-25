#include <cloudcv.hpp>
#include <framework/marshal/marshal.hpp>

namespace cloudcv 
{
    NAN_METHOD(version)
    {
        NanScope();
        std::string versionString = lexical_cast(CV_MAJOR_VERSION) + "." + lexical_cast(CV_MINOR_VERSION);
        NanReturnValue(MarshalFromNative(versionString));
    }

	NAN_METHOD(buildInformation)
    {
		NanEscapableScope();
		NanReturnValue(MarshalFromNative(cv::getBuildInformation()));
    }
}