#include "marshal.hpp"
#include "node_object_builder.hpp"
#include "framework/Image.hpp"

using namespace v8;

V8Result MarshalFromNative(const cv::Size& value)
{
	NanScope();
	Local<Object> structure = NanNew<Object>();
	structure->Set(NanNew<String>("width"),    MarshalFromNative(value.width));
	structure->Set(NanNew<String>("height"),   MarshalFromNative(value.height));
	return (structure);
}

V8Result MarshalFromNative(const cv::Rect& value)
{
	NanScope();
	Local<Object> structure = NanNew<Object>();
	structure->Set(NanNew<String>("x"),        MarshalFromNative(value.x));
	structure->Set(NanNew<String>("y"),        MarshalFromNative(value.y));
	structure->Set(NanNew<String>("width"),    MarshalFromNative(value.width));
	structure->Set(NanNew<String>("height"),   MarshalFromNative(value.height));
	return (structure);
}

V8Result MarshalFromNative(const cv::Point& value)
{
	NanScope();
	Local<Object> structure = NanNew<Object>();
	structure->Set(NanNew<String>("x"), MarshalFromNative(value.x));
	structure->Set(NanNew<String>("y"), MarshalFromNative(value.y));
	return (structure);
}

V8Result MarshalFromNative(const cv::Point2f& value)
{
	NanScope();
	Local<Object> structure = NanNew<Object>();
	structure->Set(NanNew<String>("x"), MarshalFromNative(value.x));
	structure->Set(NanNew<String>("y"), MarshalFromNative(value.y));
	return (structure);
}

V8Result MarshalFromNative(const cv::Matx33f& value)
{
    NanScope();
    Handle<Array> result = NanNew<Array>(9);
    
    result->Set(0, MarshalFromNative( value(0,0) ));
    result->Set(1, MarshalFromNative( value(0,1) ));
    result->Set(2, MarshalFromNative( value(0,2) ));

    result->Set(3, MarshalFromNative( value(1,0) ));
    result->Set(4, MarshalFromNative( value(1,1) ));
    result->Set(5, MarshalFromNative( value(1,2) ));

    result->Set(6, MarshalFromNative( value(2,0) ));
    result->Set(7, MarshalFromNative( value(2,1) ));
    result->Set(8, MarshalFromNative( value(2,2) ));

    return (result);
}

V8Result MarshalFromNative(const cv::Scalar& value)
{
    NanScope();
    Local<Array> result = NanNew<Array>(4);

	for (size_t i = 0; i < 4; i++) 
	{
		result->Set(i, MarshalFromNative(value.val[i]));
	}

	return (result);
}

V8Result MarshalFromNative(const cv::Mat& value)
{
    NanScope();
    return (cloudcv::ImageView::ViewForImage(value));
}

bool MarshalToNativeImage(V8Result imageBuffer, cv::Mat& frame, int flags)
{
	char * bufferData = node::Buffer::Data(imageBuffer);
	size_t bufferLength = node::Buffer::Length(imageBuffer);
	std::vector<char> imageData(bufferData, bufferData + bufferLength);

	frame = cv::imdecode(imageData, flags, &frame);

	return false == frame.empty();
}

bool MarshalToNative(V8Result obj, cv::Point2f& value)
{
    value.x = static_cast<float>(obj.As<v8::Object>()->Get(NanNew<String>("x"))->NumberValue());
    value.y = static_cast<float>(obj.As<v8::Object>()->Get(NanNew<String>("y"))->NumberValue());

    return true;
}


