#include "Job.hpp"
#include <stdexcept>
#include <iostream>

namespace cloudcv {

	Job::Job(NanCallback *callback)
		: NanAsyncWorker(callback)
    {
    }

    Job::~Job()
    {
    }

    void Job::Execute()
    {
        try
        {
            ExecuteNativeCode();
        }
        catch (std::runtime_error& e)
        {
            SetErrorMessage(e.what());
        }
    }

	void Job::HandleOKCallback()
	{
		NanEscapableScope();

		v8::Local<v8::Value> argv[] = {
			NanNull(),
			CreateCallbackResult()
		};

		callback->Call(2, argv);
	}
}