#include "modules/analyze/binding.hpp"
#include "modules/analyze/analyze.hpp"

#include <framework/marshal/marshal.hpp>
#include <framework/NanCheck.hpp>
#include "framework/Job.hpp"

#include <vector>

using namespace v8;
using namespace node;
using namespace cloudcv;

namespace cloudcv
{
    V8Result MarshalFromNative(const Distribution& d);
    V8Result MarshalFromNative(const DominantColor& d);
    V8Result MarshalFromNative(const RGBDistribution& d);
    V8Result MarshalFromNative(const AnalyzeResult& res);
}

#include <framework/marshal/node_object_builder.hpp>

namespace cloudcv
{

    class ImageAnalyzeTask : public Job
    {
    public:

        ImageAnalyzeTask(Local<Object> imageBuffer, NanCallback * callback)
            : Job(callback)
        {
            mImageBuffer = Persistent<Object>::New(imageBuffer);

            mImageData = Buffer::Data(imageBuffer);
            mImageDataLen = Buffer::Length(imageBuffer);
        }

        virtual ~ImageAnalyzeTask()
        {
            if (!mImageBuffer.IsEmpty())
            {
                mImageBuffer.Dispose();
                mImageBuffer.Clear();
            }
        }

    protected:

        // This function is executed in another thread at some point after it has been
        // scheduled. IT MUST NOT USE ANY V8 FUNCTIONALITY. Otherwise your extension
        // will crash randomly and you'll have a lot of fun debugging.
        // If you want to use parameters passed into the original call, you have to
        // convert them to PODs or some other fancy method.
        virtual void ExecuteNativeCode()
        {
            m_source = cv::imdecode(cv::_InputArray(mImageData, mImageDataLen), cv::IMREAD_COLOR);

            if (m_source.empty())
            {
                SetErrorMessage("Cannot decode image");
                return;
            }

            buildFromImage(m_source, m_analyzeResult);
        }

        // This function is executed in the main V8/JavaScript thread. That means it's
        // safe to use V8 functions again. Don't forget the HandleScope!
        virtual Local<Value> CreateCallbackResult()
        {
            NanScope();
            NanReturnValue(MarshalFromNative(m_analyzeResult));
        }

    private:

        Persistent<Object>	 	 mImageBuffer;
        char                   * mImageData;
        int                      mImageDataLen;

        cv::Mat                 m_source;
        AnalyzeResult           m_analyzeResult;
    };

    NAN_METHOD(analyzeImage)
    {
        NanScope();

        try
        {
            Local<Object> imageBuffer;
            Local<Function> imageCallback;

            if (NanCheck(args)
                .ArgumentsCount(2)
                .Argument(0).IsBuffer().Bind(imageBuffer)
                .Argument(1).IsFunction().Bind(imageCallback))
            {
                NanCallback *callback = new NanCallback(imageCallback);
                NanAsyncQueueWorker(new ImageAnalyzeTask(imageBuffer, callback));
                NanReturnValue(NanTrue());
            }

            NanReturnValue(NanFalse());
        }
        catch (ArgumentMismatchException exc)
        {
            return NanThrowTypeError(exc.what());
        }
    }

    V8Result MarshalFromNative(const Distribution& d)
    {
        NanScope();
        Local<Object> structure = NanNew<Object>();
        NodeObject resultWrapper(structure);

        resultWrapper["average"] = d.average;
        resultWrapper["entropy"] = d.entropy;
        resultWrapper["max"] = d.max;
        resultWrapper["min"] = d.min;
        resultWrapper["standardDeviation"] = d.standardDeviation;

        NanReturnValue(structure);
    }

    V8Result MarshalFromNative(const DominantColor& d)
    {
        NanScope();
        Local<Object> structure = NanNew<Object>();
        NodeObject resultWrapper(structure);

        resultWrapper["average"] = d.color;
        resultWrapper["entropy"] = d.error;
        resultWrapper["max"] = d.interclassVariance;
        resultWrapper["min"] = d.totalPixels;

        NanReturnValue(structure);
    }

    V8Result MarshalFromNative(const RGBDistribution& d)
    {
        NanScope();
        Local<Object> structure = NanNew<Object>();
        NodeObject resultWrapper(structure);

        resultWrapper["b"] = d.b;
        resultWrapper["g"] = d.g;
        resultWrapper["r"] = d.r;

        NanReturnValue(structure);
    }

    V8Result MarshalFromNative(const AnalyzeResult& res)
    {
        NanScope();
        Local<Object> structure = NanNew<Object>();
        NodeObject resultWrapper(structure);

        resultWrapper["aspectRatio"] = res.aspectRatio;
        resultWrapper["colorDeviation"] = res.colorDeviation;
        resultWrapper["dominantColors"] = res.dominantColors;
        resultWrapper["frameSize"] = res.frameSize;
        resultWrapper["histogram"] = res.histogram;
        resultWrapper["intensity"] = res.intensity;
        resultWrapper["reducedColors"] = res.reducedColors;

        resultWrapper["rmsContrast"] = res.rmsContrast;
        resultWrapper["uniqieColors"] = res.uniqieColors;

        NanReturnValue(structure);
    }
}