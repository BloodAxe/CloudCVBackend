#include "Image.hpp"
#include "framework/marshal/marshal.hpp"
#include "framework/NanCheck.hpp"
#include "framework/Job.hpp"
#include "framework/marshal/node_object_builder.hpp"

using namespace v8;

#define SETUP_FUNCTION(TYP) \
    NanScope();      \
    TYP *self = ObjectWrap::Unwrap<TYP>(args.This());

namespace cloudcv
{
    enum EncodeOutputFormat {
        EncodeOutputFormatJpeg,
        EncodeOutputFormatWebp,
        EncodeOutputFormatPng
    };

    class EncodeImageTask : public Job
    {
    public:
        EncodeImageTask(cv::Mat image, NanCallback * callback, EncodeOutputFormat fmt, bool returnDataUri = false)
            : Job(callback)
            , mImage(image)
            , mFormat(fmt)
            , mReturnDataUri(returnDataUri)
        {
        }

        virtual ~EncodeImageTask()
        {
        }

    protected:

        virtual void ExecuteNativeCode()
        {
            std::string mimeType;

            switch (mFormat)
            {
            case EncodeOutputFormatPng:
                cv::imencode(".png", mImage, mEncodedData);
                mimeType = "image/png";
                break;

            case EncodeOutputFormatJpeg:
                cv::imencode(".jpeg", mImage, mEncodedData);
                mimeType = "image/jpeg";
                break;

            case EncodeOutputFormatWebp:
                cv::imencode(".webp", mImage, mEncodedData);
                mimeType = "image/webp";
                break;

            default:
                SetErrorMessage("Given format is not supported");
                return;
            };

            if (mReturnDataUri)
            {
                mDataUriString << "data:" << mimeType << ";base64,";
                Base64Encode(mEncodedData, mDataUriString);
            }
        }

        // This function is executed in the main V8/JavaScript thread. That means it's
        // safe to use V8 functions again. Don't forget the HandleScope!
        virtual Local<Value> CreateCallbackResult()
        {
            NanScope();
            if (mReturnDataUri)
                return (MarshalFromNative(mDataUriString.str()));
            else
                return (MarshalFromNative(mEncodedData));
        }

    private:

        static void Base64Encode(const std::vector<uint8_t>& data, std::ostringstream& encoded_data)
        {
            static size_t mod_table[] = { 0, 2, 1 };
            static char encoding_table[] = {
                'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                '4', '5', '6', '7', '8', '9', '+', '/'
            };

            size_t input_length = data.size();
            //size_t output_length = 4 * ((input_length + 2) / 3);
            //encoded_data.resize(output_length);

            for (size_t i = 0; i < input_length;)
            {
                uint32_t octet_a = i < input_length ? data[i++] : 0;
                uint32_t octet_b = i < input_length ? data[i++] : 0;
                uint32_t octet_c = i < input_length ? data[i++] : 0;

                uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

                encoded_data << encoding_table[(triple >> 3 * 6) & 0x3F]
                    << encoding_table[(triple >> 2 * 6) & 0x3F]
                    << encoding_table[(triple >> 1 * 6) & 0x3F]
                    << encoding_table[(triple >> 0 * 6) & 0x3F];
            }

            // Extra padding
            for (size_t i = 0; i < mod_table[input_length % 3]; i++)
            {
                encoded_data << '=';
            }
        }

    private:
        cv::Mat                 mImage;
        EncodeOutputFormat      mFormat;
        bool                    mReturnDataUri;
        std::vector<uint8_t>    mEncodedData;
        std::ostringstream      mDataUriString;
    };

    v8::Persistent<v8::Function> ImageView::constructor;

    ImageView::ImageView(const cv::Mat& image)
        : mImage(image)
    {

    }

    NAN_METHOD(ImageView::Width)
    {
        SETUP_FUNCTION(ImageView);
        int width = self->mImage.cols;
        NanReturnValue(NanNew<v8::Integer>(width));
    }

    NAN_METHOD(ImageView::Height)
    {
        SETUP_FUNCTION(ImageView);
        int height = self->mImage.rows;
        NanReturnValue(NanNew<v8::Integer>(height));
    }

    NAN_METHOD(ImageView::Channels)
    {
        SETUP_FUNCTION(ImageView);
        NanReturnValue(NanNew<v8::Integer>(self->mImage.channels()));
    }

    NAN_METHOD(ImageView::Type)
    {
        SETUP_FUNCTION(ImageView);
        NanReturnValue(NanNew<v8::Integer>(self->mImage.type()));
    }

    NAN_METHOD(ImageView::Stride)
    {
        SETUP_FUNCTION(ImageView);
        size_t stride = self->mImage.step[0];
        NanReturnValue(NanNew<v8::Integer>(stride));
    }

    NAN_METHOD(ImageView::AsJpegStream)
    {
        SETUP_FUNCTION(ImageView);
        try
        {
            Local<Function> imageCallback;

            if (NanCheck(args)
                .ArgumentsCount(1)
                .Argument(0).IsFunction().Bind(imageCallback))
            {
                NanCallback *callback = new NanCallback(imageCallback);
                NanAsyncQueueWorker(new EncodeImageTask(self->mImage, callback, EncodeOutputFormatJpeg));
                NanReturnValue(NanTrue());
            }

            NanReturnValue(NanFalse());
        }
        catch (ArgumentMismatchException& exc)
        {
            return NanThrowTypeError(exc.what());
        }
    }

    NAN_METHOD(ImageView::AsPngStream)
    {
        SETUP_FUNCTION(ImageView);
        try
        {
            Local<Function> imageCallback;

            if (NanCheck(args)
                .ArgumentsCount(1)
                .Argument(0).IsFunction().Bind(imageCallback))
            {
                NanCallback *callback = new NanCallback(imageCallback);
                NanAsyncQueueWorker(new EncodeImageTask(self->mImage, callback, EncodeOutputFormatPng));
                NanReturnValue(NanTrue());
            }

            NanReturnValue(NanFalse());
        }
        catch (ArgumentMismatchException& exc)
        {
            return NanThrowTypeError(exc.what());
        }
    }

    NAN_METHOD(ImageView::AsPngDataUri)
    {
        SETUP_FUNCTION(ImageView);
        try
        {
            Local<Function> imageCallback;

            if (NanCheck(args)
                .ArgumentsCount(1)
                .Argument(0).IsFunction().Bind(imageCallback))
            {
                NanCallback *callback = new NanCallback(imageCallback);
                NanAsyncQueueWorker(new EncodeImageTask(self->mImage, callback, EncodeOutputFormatPng, true));
                NanReturnValue(NanTrue());
            }

            NanReturnValue(NanFalse());
        }
        catch (ArgumentMismatchException& exc)
        {
            return NanThrowTypeError(exc.what());
        }
    }

    NAN_METHOD(ImageView::AsObject)
    {
        SETUP_FUNCTION(ImageView);
        Local<Object> res = NanNew<Object>();
        cv::Mat image = self->mImage;

        NodeObject resultWrapper(res);
        resultWrapper["size"] = image.size();
        resultWrapper["channels"] = image.channels();

        size_t length = image.total() * image.channels();

        switch (image.depth())
        {
        case CV_8U:
            resultWrapper["depth"] = "CV_8U";
            resultWrapper["data"] = std::vector<uint8_t>((uint8_t*)image.data, (uint8_t*)image.data + length);
            break;

        case CV_16U:
            resultWrapper["depth"] = "CV_16U";
            resultWrapper["data"] = std::vector<uint16_t>((uint16_t*)image.data, (uint16_t*)image.data + length);
            break;

        case CV_16S:
            resultWrapper["depth"] = "CV_16S";
            resultWrapper["data"] = std::vector<int16_t>((int16_t*)image.data, (int16_t*)image.data + length);
            break;

        case CV_32S:
            resultWrapper["depth"] = "CV_32S";
            resultWrapper["data"] = std::vector<int16_t>((int32_t*)image.data, (int32_t*)image.data + length);
            break;

        case CV_32F:
            resultWrapper["depth"] = "CV_32F";
            resultWrapper["data"] = std::vector<float>((float*)image.data, (float*)image.data + length);
            break;

        case CV_64F:
            resultWrapper["depth"] = "CV_64F";
            resultWrapper["data"] = std::vector<double>((double*)image.data, (double*)image.data + length);
            break;

        default:
            break;
        };

        NanReturnValue(res);
    }


    NAN_METHOD(ImageView::AsJpegDataUri)
    {
        SETUP_FUNCTION(ImageView);
        try
        {
            Local<Function> imageCallback;

            if (NanCheck(args)
                .ArgumentsCount(1)
                .Argument(0).IsFunction().Bind(imageCallback))
            {
                NanCallback *callback = new NanCallback(imageCallback);
                NanAsyncQueueWorker(new EncodeImageTask(self->mImage, callback, EncodeOutputFormatJpeg, true));
                NanReturnValue(NanTrue());
            }

            NanReturnValue(NanFalse());
        }
        catch (ArgumentMismatchException& exc)
        {
            return NanThrowTypeError(exc.what());
        }
    }

    void ImageView::Init(v8::Handle<v8::Object> exports)
    {
        NanScope();

        //Class
        v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(ImageView::New);
        tpl->InstanceTemplate()->SetInternalFieldCount(1);
        tpl->SetClassName(NanNew("ImageView"));

        NODE_SET_PROTOTYPE_METHOD(tpl, "width", ImageView::Width);
        NODE_SET_PROTOTYPE_METHOD(tpl, "height", ImageView::Height);
        NODE_SET_PROTOTYPE_METHOD(tpl, "channels", ImageView::Channels);
        NODE_SET_PROTOTYPE_METHOD(tpl, "type", ImageView::Type);
        NODE_SET_PROTOTYPE_METHOD(tpl, "stride", ImageView::Stride);

        NODE_SET_PROTOTYPE_METHOD(tpl, "asJpegStream", ImageView::AsJpegStream);
        NODE_SET_PROTOTYPE_METHOD(tpl, "asJpegDataUri", ImageView::AsJpegDataUri);
        NODE_SET_PROTOTYPE_METHOD(tpl, "asPngStream", ImageView::AsPngStream);
        NODE_SET_PROTOTYPE_METHOD(tpl, "asPngDataUri", ImageView::AsPngDataUri);
        NODE_SET_PROTOTYPE_METHOD(tpl, "asObject", ImageView::AsObject);

        constructor = Persistent<Function>::New(tpl->GetFunction());
        exports->Set(NanNew<String>("ImageView"), constructor);
        std::cout << "ImageView::Init finished" << std::endl;
    }

    Handle<Value> ImageView::New(_NAN_METHOD_ARGS_TYPE args)
    {
        NanScope();

        if (args.This()->InternalFieldCount() == 0)
            return v8::ThrowException(v8::Exception::TypeError(NanNew<String>("Cannot instantiate without new")));

        std::string filename;
        MarshalToNative(args[0], filename);

        cv::Mat im = cv::imread(filename.c_str());
        auto imageView = new ImageView(im);

        imageView->Wrap(args.Holder());
        NanReturnValue(args.Holder());
    }

    v8::Handle<v8::Value> ImageView::ViewForImage(cv::Mat image)
    {
        NanScope();
        
        // Insiped by SO: http://stackoverflow.com/questions/16600735/what-is-an-internal-field-count-and-what-is-setinternalfieldcount-used-for
        Local<Object> holder = constructor->NewInstance();

        ImageView * imageView = new ImageView(image);
        imageView->Wrap(holder);
        NanReturnValue(holder);
    }
}