#include <node.h>
#include <node_buffer.h>
#include <v8.h>
#include <opencv2/opencv.hpp>
#include <string>
#include "src/cloudcv.hpp"

using namespace v8;
using namespace node;

// This function returns a JavaScript number that is either 0 or 1.
Handle<Value> buildInformation(const Arguments& args)
{
    // At thep top of every function that uses anything about v8, include a
    // definition like this. It ensures that any v8 handles you create in that
    // function are properly cleaned up. If you see memory rising in your
    // application, chances are that a scope isn't properly cleaned up.
    HandleScope scope;

    // When returning a value from a function, make sure to wrap it in
    // scope.Close(). This ensures that the handle stays valid after the current
    // scope (declared with the previous statement) is cleaned up.
    return scope.Close
    (
        // Creating a new JavaScript integer is as simple as passing a C int
        // (technically a int32_t) to this function.
        String::New(cv::getBuildInformation().c_str())
    );
}

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
    cv::Mat input = cv::imdecode(buffer, CV_LOAD_IMAGE_COLOR);

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

        obj->Set(String::NewSymbol("aspectNum"), Int32::New(task->result.aspectNum));
        obj->Set(String::NewSymbol("aspectDenom"), Int32::New(task->result.aspectDenom));

        obj->Set(String::NewSymbol("contrast"), Int32::New(task->result.contrast));
        obj->Set(String::NewSymbol("brightness"), Int32::New(task->result.brightness));

        obj->Set(String::NewSymbol("colorDeviation"), Int32::New(task->result.colorDeviation));
        obj->Set(String::NewSymbol("histogram"), String::New(base64encode(task->result.histogram));

        obj->Set(String::NewSymbol("processingTimeMs"), Int32::New(task->result.processingTimeMs));

        // In case the operation succeeded, convention is to pass null as the
        // first argument before the result arguments.
        // In case you produced more complex data, this is the place to convert
        // your plain C++ data structures into JavaScript/V8 data structures.
        const unsigned argc = 1;
        Local<Value> argv[argc] = 
        {
            obj
        };

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



void RegisterModule(Handle<Object> target)
{
    // target is the module object you see when require()ing the .node file.
    target->Set(String::NewSymbol("buildInformation"), FunctionTemplate::New(buildInformation)->GetFunction());

    target->Set(String::NewSymbol("analyze"), FunctionTemplate::New(analyzeImage)->GetFunction());
}

NODE_MODULE(cloudcv, RegisterModule);
