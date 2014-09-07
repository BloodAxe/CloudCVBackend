#include "Job.hpp"
#include <stdexcept>

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
            std::cout << "Native code ended" << std::endl;
        }
        catch (std::runtime_error& e)
        {
            std::cout << e.what() << std::endl;
            SetErrorMessage(e.what());
        }
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