#include "binding.hpp"
#include "analyze.hpp"
#include "../../node/node_helpers.hpp"

#include <node_buffer.h>

using namespace v8;
using namespace node;

struct ImageAnalyzeTask
{       
    ImageAnalyzeTask(Local<Value> imageBuffer, Local<Value> callback)
    {
        HandleScope scope;
        
        Local<Object> imageBufferObject = Local<Object>::New(imageBuffer->ToObject());
        m_callback  = Persistent<Function>::New(Handle<Function>::Cast(callback));

        char * bufferData   = Buffer::Data(imageBufferObject);
        size_t bufferLength = Buffer::Length(imageBufferObject);

        m_imageData = std::vector<char>(bufferData, bufferData + bufferLength);

        // This creates the work request struct.
        m_request       = new uv_work_t();
        m_request->data = this;  
    }

    virtual ~ImageAnalyzeTask()
    {
        m_callback.Dispose();
        delete m_request;
    }

    void StartTask()
    {
        // Schedule our work request with libuv. Here you can specify the functions
        // that should be executed in the threadpool and back in the main thread
        // after the threadpool function completed.
        int status = uv_queue_work( uv_default_loop(), 
                                    m_request, 
                                    &ImageAnalyzeTask::analyzeImageAsyncWork,
                                    (uv_after_work_cb)ImageAnalyzeTask::analyzeImageAsyncAfter);
        assert(status == 0);
    }

protected:

    // This function is executed in another thread at some point after it has been
    // scheduled. IT MUST NOT USE ANY V8 FUNCTIONALITY. Otherwise your extension
    // will crash randomly and you'll have a lot of fun debugging.
    // If you want to use parameters passed into the original call, you have to
    // convert them to PODs or some other fancy method.
    virtual void ExecuteNative()
    {
        cv::Mat input = cv::imdecode(m_imageData, 1);
        analyzeImage(input, m_result);
    }

    // This function is executed in the main V8/JavaScript thread. That means it's
    // safe to use V8 functions again. Don't forget the HandleScope!
    virtual void InvokeResultCallback()
    {
        HandleScope scope;

        Local<Object> obj = Object::New();

        ObjectBuilder resultBuilder(obj);

        bool isColor = m_result.source.channels() > 1;

        // Common information        
        resultBuilder.Set("source.aspectRatio",  m_result.aspectRatio);
        resultBuilder.Set("source.frameSize",    m_result.frameSize);
        resultBuilder.Set("source.channels",     m_result.source.channels());
        resultBuilder.Set("source.bitsPerPixel", m_result.source.depth());
        resultBuilder.Set("source.hasAlpha",     m_result.source.channels() == 4);        
        resultBuilder.Set("source.humanSize",    humanSize(m_result.source.depth() * m_result.frameSize.width * m_result.frameSize.height);
        resultBuilder.Set("source.isColor",      isColor);

        // Histogram
        if (isColor)
        {
            resultBuilder.Set("histogram.colorImage", toDataUri(m_result.colorHistogram));
        }

        resultBuilder.Set("histogram.grayImage",  toDataUri(m_result.grayHistogram));
        resultBuilder.Set("histogram.brightness", m_result.brightness);
        
        // Color analyze
        if (isColor)
        {
            resultBuilder.Set("colorAnalyze.uniqieColors",        m_result.uniqieColors);
            resultBuilder.Set("colorAnalyze.reducedColors",       m_result.reducedColors);
            resultBuilder.Set("colorAnalyze.dominantColors",      m_result.dominantColors);
            resultBuilder.Set("colorAnalyze.dominantColorsImage", m_result.dominantColorsImage);
            resultBuilder.Set("colorAnalyze.redDeviation",        m_result.redDeviation);
            resultBuilder.Set("colorAnalyze.greenDeviation",      m_result.greenDeviation);
            resultBuilder.Set("colorAnalyze.blueDeviation",       m_result.blueDeviation);
        }

        // Edges
        resultBuilder.Set("canny.image",                          m_result.cannyImage);
        resultBuilder.Set("canny.lowerThreshold",                 m_result.lowerThreshold);
        resultBuilder.Set("canny.upperThreshold",                 m_result.upperThreshold);
        resultBuilder.Set("canny.apertureSize",                   m_result.apertureSize);
        
        // Morphologic analyze
        resultBuilder.Set("morphology.houghImage",                m_result.houghImage);
        resultBuilder.Set("morphology.lines.count",               m_result.houghLines.size());
        resultBuilder.Set("morphology.circles.count",             m_result.houghCircles.size());

        // The rest
        resultBuilder.Set("processingTimeMs",                     m_result.processingTimeMs);

        const unsigned argc = 1;
        Local<Value> argv[argc] = { obj };

        // Wrap the callback function call in a TryCatch so that we can call
        // node's FatalException afterwards. This makes it possible to catch
        // the exception from JavaScript land using the
        // process.on('uncaughtException') event.
        TryCatch try_catch;
        m_callback->Call(Context::GetCurrent()->Global(), argc, argv);
        
        if (try_catch.HasCaught())
        {
            node::FatalException(try_catch);
        }
    }

private:

    void FinishTask()
    {
        InvokeResultCallback();
    }

    static void analyzeImageAsyncWork(uv_work_t* req)
    {
        ImageAnalyzeTask * task = static_cast<ImageAnalyzeTask*>(req->data);
        task->ExecuteNative();
    }

    static void analyzeImageAsyncAfter(uv_work_t* req)
    {
        ImageAnalyzeTask* task = static_cast<ImageAnalyzeTask*>(req->data);
        task->FinishTask();
        delete task;
    }

private:
    Persistent<Function>    m_callback;
    uv_work_t             * m_request;

    cloudcv::AnalyzeResult  m_result;
    std::vector<char>       m_imageData;
};




Handle<Value> analyzeImage(const Arguments& args)
{
    HandleScope scope;

    if (args.Length() != 2)
    {
        return ThrowException(Exception::TypeError(String::New("Invalid number of arguments")));        
    }

    if (!args[0]->IsObject())
    {
        return ThrowException(Exception::TypeError(String::New("First argument should be a Buffer")));        
    }

    if (!args[1]->IsFunction())
    {
        return ThrowException(Exception::TypeError(String::New("Second argument must be a callback function")));
    }

    // The task holds our custom status information for this asynchronous call,
    // like the callback function we want to call when returning to the main
    // thread and the status information.
    
    ImageAnalyzeTask* task = new ImageAnalyzeTask(args[0], args[1]);
    task->StartTask();

    return Undefined();
}