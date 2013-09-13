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
        m_imageData = Persistent<Object>::New(imageBuffer->ToObject());
        m_callback  = Persistent<Function>::New(Handle<Function>::Cast(callback));

        bufferData   = Buffer::Data(m_imageData);;
        bufferLength = Buffer::Length(m_imageData);
    }

    ~ImageAnalyzeTask()
    {
        m_callback.Dispose();
        m_imageData.Dispose();
    }

    char*         bufferData;
    size_t        bufferLength;

    cloudcv::AnalyzeResult result;
    Persistent<Function> m_callback;

private:
    Persistent<Object>   m_imageData;
};


// This function is executed in another thread at some point after it has been
// scheduled. IT MUST NOT USE ANY V8 FUNCTIONALITY. Otherwise your extension
// will crash randomly and you'll have a lot of fun debugging.
// If you want to use parameters passed into the original call, you have to
// convert them to PODs or some other fancy method.
void analyzeImageAsyncWork(uv_work_t* req)
{
    ImageAnalyzeTask* task = static_cast<ImageAnalyzeTask*>(req->data);

    std::vector<char> buffer(task->bufferData, task->bufferData + task->bufferLength);
    cv::Mat input = cv::imdecode(buffer, 1);

    analyzeImage(input, task->result);
}

// This function is executed in the main V8/JavaScript thread. That means it's
// safe to use V8 functions again. Don't forget the HandleScope!
void analyzeImageAsyncAfter(uv_work_t* req)
{
    HandleScope scope;
    ImageAnalyzeTask* task = static_cast<ImageAnalyzeTask*>(req->data);

    {
        Local<Object> obj = Object::New();

        ObjectBuilder resultBuilder(obj);

        resultBuilder.Set("aspectNum",         task->result.aspectNum);
        resultBuilder.Set("aspectDenom",       task->result.aspectDenom);
        resultBuilder.Set("contrast",          task->result.contrast);
        resultBuilder.Set("brightness",        task->result.brightness);
        resultBuilder.Set("colorDeviation",    task->result.colorDeviation);
        resultBuilder.Set("histogram",         toDataUri(task->result.histogram));
        resultBuilder.Set("processingTimeMs",  task->result.processingTimeMs);

        const unsigned argc = 1;
        Local<Value> argv[argc] = { obj };

        // Wrap the callback function call in a TryCatch so that we can call
        // node's FatalException afterwards. This makes it possible to catch
        // the exception from JavaScript land using the
        // process.on('uncaughtException') event.
        TryCatch try_catch;
        task->m_callback->Call(Context::GetCurrent()->Global(), argc, argv);
        
        if (try_catch.HasCaught())
        {
            node::FatalException(try_catch);
        }
    }

    // We also created the task and the work_req struct with new, so we have to
    // manually delete both.
    delete task;
    delete req;
}

Handle<Value> analyzeImage(const Arguments& args)
{
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

    // This creates the work request struct.
    uv_work_t *req = new uv_work_t();
    req->data = task;  

    // Schedule our work request with libuv. Here you can specify the functions
    // that should be executed in the threadpool and back in the main thread
    // after the threadpool function completed.
    int status = uv_queue_work( uv_default_loop(), 
                                req, 
                                analyzeImageAsyncWork,
                                (uv_after_work_cb)analyzeImageAsyncAfter);
    assert(status == 0);

    return Undefined();
}