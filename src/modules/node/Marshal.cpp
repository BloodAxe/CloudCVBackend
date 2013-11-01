#include "Marshal.hpp"
#include "node/node_helpers.hpp"

using namespace v8;

Marshal::NativeResult Marshal::Native(int value)
{
	HandleScope scope;
	return scope.Close(Int32::New(value));
}

Marshal::NativeResult Marshal::Native(size_t value)
{
	HandleScope scope;
	return scope.Close(Uint32::New(value));
}

Marshal::NativeResult Marshal::Native(float value)
{ 
	HandleScope scope;
	return scope.Close(Number::New(value));
}

Marshal::NativeResult Marshal::Native(double value)
{
	HandleScope scope;
	return scope.Close(Number::New(value));
}

Marshal::NativeResult Marshal::Native(const char * value)
{ 
	HandleScope scope;
	return scope.Close(String::New(value));
}

Marshal::NativeResult Marshal::Native(const std::string& value)
{
	HandleScope scope;
	return scope.Close(String::New(value.c_str(), value.size()));
}

//////////////////////////////////////////////////////////////////////////
// OpenCV Types

Marshal::NativeResult Marshal::Native(const cv::Size& value)
{
	HandleScope scope;
	Local<Object> structure = Object::New();
	structure->Set(String::NewSymbol("width"),  Native(value.width));
	structure->Set(String::NewSymbol("height"), Native(value.height));
	return scope.Close(structure);
}

Marshal::NativeResult Marshal::Native(const cv::Rect& value)
{
    HandleScope scope;
    Local<Object> structure = Object::New();
    structure->Set(String::NewSymbol("x"),  Native(value.x));
    structure->Set(String::NewSymbol("y"),  Native(value.y));
    structure->Set(String::NewSymbol("width"),  Native(value.width));
    structure->Set(String::NewSymbol("height"), Native(value.height));
    return scope.Close(structure);
}

Marshal::NativeResult Marshal::Native(const cv::Point& value)
{
	HandleScope scope;
	Local<Object> structure = Object::New();
	structure->Set(String::NewSymbol("x"), Native(value.x));
	structure->Set(String::NewSymbol("y"), Native(value.y));
	return scope.Close(structure);	
}

Marshal::NativeResult Marshal::Native(const cv::Point2f& value)
{
	HandleScope scope;
	Local<Object> structure = Object::New();
	structure->Set(String::NewSymbol("x"), Native(value.x));
	structure->Set(String::NewSymbol("y"), Native(value.y));
	return scope.Close(structure);
}

Marshal::NativeResult Marshal::Native(const cv::Scalar& value)
{
	HandleScope scope;
	Handle<Array> result = Array::New(4);

	for (size_t i = 0; i < 4; i++)
		result->Set(i, Native(value.val[i]));

	return scope.Close(result);
}

Marshal::NativeResult Marshal::Native(const cv::Mat& value)
{
	HandleScope scope;
	return scope.Close(Native(toDataUri(value, kImageTypePng)));
	
}



//////////////////////////////////////////////////////////////////////////
// CloudCV Structures

Marshal::NativeResult Marshal::Native(const Distribution& value)
{
	HandleScope scope;
	Local<Object> structure = Object::New();
	structure->Set(String::NewSymbol("min"),               Native(value.min));
	structure->Set(String::NewSymbol("max"),               Native(value.max));
	structure->Set(String::NewSymbol("average"),           Native(value.average));
	structure->Set(String::NewSymbol("standardDeviation"), Native(value.standardDeviation));
	structure->Set(String::NewSymbol("entropy"),           Native(value.entropy));
	return scope.Close(structure);
}

Marshal::NativeResult Marshal::Native(const rgb8u_color_t& value)
{
	HandleScope scope;
	Local<Object> structure = Object::New();
	structure->Set(String::NewSymbol("min"),     Native(static_cast<int>(value.r)));
	structure->Set(String::NewSymbol("max"),     Native(static_cast<int>(value.g)));
	structure->Set(String::NewSymbol("average"), Native(static_cast<int>(value.b)));
	return scope.Close(structure);
}

Marshal::NativeResult Marshal::Native(const RGBDistribution& value)
{
	HandleScope scope;
	Local<Object> structure = Object::New();
	structure->Set(String::NewSymbol("r"), Native(value.r));
	structure->Set(String::NewSymbol("g"), Native(value.g));
	structure->Set(String::NewSymbol("b"), Native(value.b));
	return scope.Close(structure);
}

Marshal::NativeResult Marshal::Native(const AnalyzeResult& value)
{
	HandleScope scope;
	Local<Object> structure = Object::New();

	structure->Set(String::NewSymbol("common"),    Native(value.common));
	structure->Set(String::NewSymbol("grayscale"), Native(value.grayscale));
	structure->Set(String::NewSymbol("color"),     Native(value.color));
	structure->Set(String::NewSymbol("edges"),     Native(value.edges));
	structure->Set(String::NewSymbol("profiling"), Native(value.profiling));

	return scope.Close(structure);
}

Marshal::NativeResult Marshal::Native(const ImageInformation& value)
{
	HandleScope scope;
	Local<Object> structure = Object::New();

	structure->Set(String::NewSymbol("sourceImage"),	Native(value.sourceImage));
	structure->Set(String::NewSymbol("grayscaleImage"),	Native(value.grayscaleImage));
	structure->Set(String::NewSymbol("frameSize"),		Native(value.frameSize));
	structure->Set(String::NewSymbol("aspectRatio"), 	Native(value.aspectRatio));
	structure->Set(String::NewSymbol("hasColor"), 		Native(value.hasColor));

	return scope.Close(structure);
}

Marshal::NativeResult Marshal::Native(const DominantColor& value)
{
	HandleScope scope;
	Local<Object> structure = Object::New();

	structure->Set(String::NewSymbol("color"),				Native(value.color));
	structure->Set(String::NewSymbol("error"),				Native(value.error));
	structure->Set(String::NewSymbol("interclassVariance"), Native(value.interclassVariance));
	structure->Set(String::NewSymbol("totalPixels"),		Native(value.totalPixels));

	return scope.Close(structure);

}

Marshal::NativeResult Marshal::Native(const IntensityInformation& value)
{
	HandleScope scope;
	Local<Object> structure = Object::New();

	structure->Set(String::NewSymbol("intensity"),		Native(value.intensity));
	structure->Set(String::NewSymbol("rmsContrast"),	Native(value.rmsContrast));
	structure->Set(String::NewSymbol("histogramImage"), Native(value.histogramImage));

	return scope.Close(structure);
}

Marshal::NativeResult Marshal::Native(const ColorsInformation& value)
{
	HandleScope scope;
	Local<Object> structure = Object::New();

	structure->Set(String::NewSymbol("colorDeviation"),			Native(value.colorDeviation));
	structure->Set(String::NewSymbol("uniqieColors"),			Native(value.uniqieColors));
	structure->Set(String::NewSymbol("reducedColors"), 			Native(value.reducedColors));
	structure->Set(String::NewSymbol("dominantColors"),			Native(value.dominantColors));
	structure->Set(String::NewSymbol("dominantColorsImage"),	Native(value.dominantColorsImage));
	structure->Set(String::NewSymbol("histogramImage"), 		Native(value.histogramImage));

	return scope.Close(structure);
}

Marshal::NativeResult Marshal::Native(const MorphologicInformation& value)
{
	HandleScope scope;
	Local<Object> structure = Object::New();

	structure->Set(String::NewSymbol("cannyImage"),			 Native(value.cannyImage));
	structure->Set(String::NewSymbol("cannyLowerThreshold"), Native(value.cannyLowerThreshold));
	structure->Set(String::NewSymbol("cannyUpperThreshold"), Native(value.cannyUpperThreshold));

	return scope.Close(structure);
}

Marshal::NativeResult Marshal::Native(const FaceDetectionResult& value)
{
    HandleScope scope;
    Local<Object> structure = Object::New();

    structure->Set(String::NewSymbol("faces"),            Native(value.faces));
    structure->Set(String::NewSymbol("imageWithFaces"),   Native(value.imageWithFaces));
    structure->Set(String::NewSymbol("detectionTimeMs"),  Native(value.detectionTimeMs));

    return scope.Close(structure);
}
