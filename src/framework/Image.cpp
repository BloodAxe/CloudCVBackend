#include "Image.hpp"
#include "framework/marshal/stl.hpp"

using namespace v8;

#define SETUP_FUNCTION(TYP) \
    NanScope();      \
    TYP *self = ObjectWrap::Unwrap<TYP>(args.This());

namespace cloudcv
{
    v8::Persistent<v8::Function> ImageView::constructor;

    ImageView::ImageView(const cv::Mat& image)
        : mImage(image)
    {

    }

    Handle<Value> ImageView::Width(const Arguments& args)
    {
        SETUP_FUNCTION(ImageView);
        int width = self->mImage.cols;
        NanReturnValue(NanNew<v8::Integer>(width));
    }

    Handle<Value> ImageView::Height(const Arguments& args)
    {
        SETUP_FUNCTION(ImageView);
        int height = self->mImage.rows;
        NanReturnValue(NanNew<v8::Integer>(height));
    }

    void ImageView::Init(v8::Handle<v8::Object> exports)
    {
        NanScope();

        //Class
        v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(ImageView::New);
        tpl->InstanceTemplate()->SetInternalFieldCount(1);
        tpl->SetClassName(NanNew("ImageView"));
        
        NODE_SET_PROTOTYPE_METHOD(tpl, "width",  ImageView::Width);
        NODE_SET_PROTOTYPE_METHOD(tpl, "height", ImageView::Height);

        constructor = Persistent<Function>::New(tpl->GetFunction());
        exports->Set(String::New("ImageView"), constructor);
        std::cout << "ImageView::Init finished" << std::endl;
    }

    Handle<Value> ImageView::New(const Arguments &args)
    {
        NanScope();

        if (args.This()->InternalFieldCount() == 0)
            return v8::ThrowException(v8::Exception::TypeError(v8::String::New("Cannot instantiate without new")));

        std::string filename;
        MarshalToNative(args[0], filename);

        std::cout << filename << std::endl;

        cv::Mat im = cv::imread(filename.c_str());
        auto imageView = new ImageView(im);

        imageView->Wrap(args.Holder());
        return scope.Close(args.Holder());
    }
}