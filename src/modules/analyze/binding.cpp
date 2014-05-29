#include "modules/analyze/binding.hpp"
#include "modules/analyze/analyze.hpp"

#include <framework/marshal/marshal.hpp>
#include "framework/Job.hpp"

#include <vector>

using namespace v8;
using namespace node;

typedef v8::Local<v8::Value> V8Result;

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





namespace cloudcv
{
	class ImageAnalyzeTask : public Job
	{
	public:

		ImageAnalyzeTask(NanCallback * callback, const cv::Mat& inputImage)
			: Job(callback)
			, m_source(inputImage)
		{
		}

	protected:

		// This function is executed in another thread at some point after it has been
		// scheduled. IT MUST NOT USE ANY V8 FUNCTIONALITY. Otherwise your extension
		// will crash randomly and you'll have a lot of fun debugging.
		// If you want to use parameters passed into the original call, you have to
		// convert them to PODs or some other fancy method.
		virtual void Execute()
		{
			buildFromImage(m_source, m_analyzeResult);
		}

		// This function is executed in the main V8/JavaScript thread. That means it's
		// safe to use V8 functions again. Don't forget the HandleScope!
		virtual Local<Value> CreateCallbackResult()
		{
			NanScope();
			Local<Object> res = NanNew<Object>();

			NodeObject resultWrapper(res);

			resultWrapper["common"]["frameSize"]          = m_analyzeResult.common.frameSize;
			resultWrapper["common"]["aspectRatio"]        = m_analyzeResult.common.aspectRatio;
			resultWrapper["common"]["hasColor"]           = m_analyzeResult.common.hasColor;
			resultWrapper["common"]["sourceImage"]        = m_analyzeResult.common.sourceImage;
			resultWrapper["common"]["grayscaleImage"]     = m_analyzeResult.common.grayscaleImage;

			resultWrapper["grayscale"]["intensity"]       = m_analyzeResult.grayscale.intensity;
			resultWrapper["grayscale"]["rmsContrast"]     = m_analyzeResult.grayscale.rmsContrast;
			resultWrapper["grayscale"]["histogramImage"]  = m_analyzeResult.grayscale.histogramImage;

			resultWrapper["color"]["colorDeviation"]      = m_analyzeResult.color.colorDeviation;
			resultWrapper["color"]["uniqieColors"]        = m_analyzeResult.color.uniqieColors;
			resultWrapper["color"]["reducedColors"]       = m_analyzeResult.color.reducedColors;
			resultWrapper["color"]["dominantColors"]      = m_analyzeResult.color.dominantColors;
			resultWrapper["color"]["dominantColorsImage"] = m_analyzeResult.color.dominantColorsImage;
			resultWrapper["color"]["histogramImage"]      = m_analyzeResult.color.histogramImage;

			resultWrapper["edges"]["cannyImage"]          = m_analyzeResult.edges.cannyImage;
			resultWrapper["edges"]["cannyLowerThreshold"] = m_analyzeResult.edges.cannyLowerThreshold;
			resultWrapper["edges"]["cannyUpperThreshold"] = m_analyzeResult.edges.cannyUpperThreshold;

			resultWrapper["profiling"] = m_analyzeResult.profiling;

			NanReturnValue(res);
		}

	private:
		cv::Mat                 m_source;
		AnalyzeResult           m_analyzeResult;
	};



	NAN_METHOD(analyzeImage)
	{
		NanScope();

		if (args.Length() != 2)
		{
			return NanThrowError("Invalid number of arguments");
		}

		if (!args[0]->IsObject()) 
		{
			return NanThrowTypeError("First argument should be a Buffer");
		}

		if (!args[1]->IsFunction())
		{
			return NanThrowTypeError("Second argument must be a callback function");
		}

		NanCallback *callback = new NanCallback(args[1].As<Function>());

		cv::Mat inputImage;

		if (!MarshalToNativeImage(args[0], inputImage, 1))
		{
			v8::Local<v8::Value> argv[] = {
				v8::Exception::Error(String::New("Cannot open image"))
			};
			callback->Call(1, argv);
		}
		else
		{
			NanAsyncQueueWorker(new ImageAnalyzeTask(callback, inputImage));
		}

		NanReturnUndefined();
	}


}