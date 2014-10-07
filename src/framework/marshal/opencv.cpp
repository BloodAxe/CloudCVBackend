#include "marshal.hpp"
#include "node_object_builder.hpp"
#include "framework/Image.hpp"

using namespace v8;

V8Result MarshalFromNative(const cv::Size& value)
{
	NanEscapableScope();
	Local<Object> structure = NanNew<Object>();
	structure->Set(NanNew<String>("width"),    MarshalFromNative(value.width));
	structure->Set(NanNew<String>("height"),   MarshalFromNative(value.height));
	return NanEscapeScope(structure);
}

V8Result MarshalFromNative(const cv::Rect& value)
{
	NanEscapableScope();
	Local<Object> structure = NanNew<Object>();
	structure->Set(NanNew<String>("x"),        MarshalFromNative(value.x));
	structure->Set(NanNew<String>("y"),        MarshalFromNative(value.y));
	structure->Set(NanNew<String>("width"),    MarshalFromNative(value.width));
	structure->Set(NanNew<String>("height"),   MarshalFromNative(value.height));
	return NanEscapeScope(structure);
}

V8Result MarshalFromNative(const cv::Point& value)
{
	NanEscapableScope();
	Local<Object> structure = NanNew<Object>();
	structure->Set(NanNew<String>("x"), MarshalFromNative(value.x));
	structure->Set(NanNew<String>("y"), MarshalFromNative(value.y));
	return NanEscapeScope(structure);
}

V8Result MarshalFromNative(const cv::Point2f& value)
{
	NanEscapableScope();
	Local<Object> structure = NanNew<Object>();
	structure->Set(NanNew<String>("x"), MarshalFromNative(value.x));
	structure->Set(NanNew<String>("y"), MarshalFromNative(value.y));
	return NanEscapeScope(structure);
}

V8Result MarshalFromNative(const cv::Matx33f& value)
{
    NanEscapableScope();
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

    return NanEscapeScope(result);
}

V8Result MarshalFromNative(const cv::Scalar& value)
{
    NanEscapableScope();
    Local<Array> result = NanNew<Array>(4);

	for (size_t i = 0; i < 4; i++) 
	{
		result->Set(i, MarshalFromNative(value.val[i]));
	}

	return NanEscapeScope(result);
}

V8Result MarshalFromNative(const cv::Mat& value)
{
    NanEscapableScope();
    return NanEscapeScope(cloudcv::ImageView::ViewForImage(value));
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
    static auto xKey = NanNew<String>("x");
    static auto yKey = NanNew<String>("y");

    if (!obj->IsObject())
        return false;

    auto object = obj.As<v8::Object>();

    if (object->HasOwnProperty(xKey) && object->HasOwnProperty(yKey)) {
        value.x = static_cast<float>(object->Get(xKey)->NumberValue());
        value.y = static_cast<float>(object->Get(yKey)->NumberValue());
        return true;
    }

    return false;
}

bool MarshalToNative(V8Result obj, cv::Size& value)
{
    static auto wKey = NanNew<String>("width");
    static auto hKey = NanNew<String>("height");

    if (!obj->IsObject())
        return false;

    auto object = obj.As<v8::Object>();

    if (object->HasOwnProperty(wKey) && object->HasOwnProperty(hKey)) {
        value.width = static_cast<float>(object->Get(wKey)->NumberValue());
        value.height = static_cast<float>(object->Get(hKey)->NumberValue());
        return true;
    }

    return false;
}

