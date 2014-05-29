#include "Job.hpp"

namespace cloudcv {

	Job::Job(NanCallback *callback)
		: NanAsyncWorker(callback)
    {
    }

    Job::~Job()
    {
    }

	void Job::HandleOKCallback()
	{
		NanScope();

		Local<Value> argv[] = {
			NanNull(),
			CreateCallbackResult()
		};

		callback->Call(2, argv);
	}
}