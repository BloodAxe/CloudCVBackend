#pragma once 

#include <v8.h>
#include <node_buffer.h>
#include <nan.h>

#include <framework/marshal/marshal.hpp>

using namespace v8;
using namespace node;

namespace cloudcv {

    class Job : public NanAsyncWorker
    {
    public:
        virtual ~Job();
		explicit Job(NanCallback *callback);

        void Execute();

        virtual void HandleOKCallback();

    protected:
        virtual void ExecuteNativeCode() = 0;

		virtual Local<Value> CreateCallbackResult() = 0;


    private:
    };
}
