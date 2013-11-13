#include "modules/analyze/binding.hpp"
#include "modules/analyze/analyze.hpp"
#include "modules/node/Marshal.hpp"

#include <node_buffer.h>

using namespace v8;
using namespace node;

struct ImageAnalyzeTask
{       
    ImageAnalyzeTask(Local<Value> imageBuffer, Local<Value> callback)
        : m_imageProcessed(false)
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
        // that should be executed in the thread pool and back in the main thread
        // after the thread pool function completed.
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
        if (!input.empty())
        {
            m_imageProcessed = true;
            buildFromImage(input, m_analyzeResult);
        }
    }



    // This function is executed in the main V8/JavaScript thread. That means it's
    // safe to use V8 functions again. Don't forget the HandleScope!
    virtual void InvokeResultCallback()
    {
        HandleScope scope;

        const unsigned argc = 1;
        Local<Value> argv[argc] = { m_imageProcessed ? Marshal::Native(m_analyzeResult) : Local<Value>::New(Undefined()) };

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
	
    bool                    m_imageProcessed;
	AnalyzeResult           m_analyzeResult;
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