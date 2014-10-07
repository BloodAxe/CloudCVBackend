#include <cloudcv.hpp>

#include <framework/marshal/marshal.hpp>
#include <framework/marshal/node_object_builder.hpp>
#include <framework/Job.hpp>
#include <framework/NanCheck.hpp>

#include "CameraCalibrationAlgorithm.hpp"

using namespace v8;
using namespace node;

namespace cloudcv
{

    class DetectPatternTask : public Job
    {
    public:
        DetectPatternTask(Local<Object> imageBuffer, cv::Size patternSize, PatternType type, NanCallback * callback)
            : Job(callback)
            , m_algorithm(patternSize, type)
            , m_patternfound(false)
        {
            NanAssignPersistent(mImageBuffer, imageBuffer);

            mImageData = Buffer::Data(imageBuffer);
            mImageDataLen = Buffer::Length(imageBuffer);
        }

        virtual ~DetectPatternTask()
        {
            NanDisposePersistent(mImageBuffer);
        }

    protected:

        void ExecuteNativeCode()
        {
            m_patternfound = false;

            cv::Mat frame = cv::imdecode(cv::_InputArray(mImageData, mImageDataLen), cv::IMREAD_GRAYSCALE);
            if (frame.empty())
            {
                SetErrorMessage("Cannot decode input image");
                return;
            }

            m_patternfound = m_algorithm.detectCorners(frame, m_corners2d);
            
        }

        virtual Local<Value> CreateCallbackResult()
        {
            NanEscapableScope();
            Local<Object> res = NanNew<Object>(); //(Object::New());

            NodeObject resultWrapper(res);
            resultWrapper["patternFound"] = m_patternfound;

            if (m_patternfound)
            {
                resultWrapper["corners"] = m_corners2d;
            }

            return NanEscapeScope(res);
        }

    private:
        CameraCalibrationAlgorithm                      m_algorithm;
        CameraCalibrationAlgorithm::VectorOf2DPoints    m_corners2d;

        Persistent<Object>	 	 mImageBuffer;
        char                   * mImageData;
        int                      mImageDataLen;

        bool                     m_patternfound;
        bool                     m_returnImage;
    };

    class ComputeIntrinsicParametersTask : public Job
    {
    public:
        typedef CameraCalibrationAlgorithm::VectorOfVectorOf3DPoints VectorOfVectorOf3DPoints;
        typedef CameraCalibrationAlgorithm::VectorOfVectorOf2DPoints VectorOfVectorOf2DPoints;
        typedef CameraCalibrationAlgorithm::VectorOfMat              VectorOfMat;

        ComputeIntrinsicParametersTask(
            const std::vector<std::string>& files,
            cv::Size boardSize, 
            PatternType type, 
            NanCallback * callback
        )
            : Job(callback)
            , m_algorithm(boardSize, type)
            , m_imageFiles(files)
        {

        }

        ComputeIntrinsicParametersTask(
            const VectorOfVectorOf2DPoints& v,
            cv::Size imageSize,
            cv::Size boardSize, 
            PatternType type, 
            NanCallback * callback
        )
            : Job(callback)
            , m_algorithm(boardSize, type)
            , m_imageSize(imageSize)
            , m_gridCorners(v)
        {

        }

    protected:

        virtual void ExecuteNativeCode()
        {
            if (!m_imageFiles.empty()) {
                m_calibrationSuccess = m_algorithm.calibrateCamera(m_imageFiles, m_cameraMatrix, m_distCoeffs);                
            }
            else if (!m_gridCorners.empty()) {
                m_calibrationSuccess = m_algorithm.calibrateCamera(m_gridCorners, m_imageSize, m_cameraMatrix, m_distCoeffs);                
            }
            else {
                SetErrorMessage("Neither image files nor grid corners were passed");
            }

        }

        virtual Local<Value> CreateCallbackResult()
        {
            NanEscapableScope();
            Local<Object> res = NanNew<Object>(); //Local(Object::New());

            NodeObject resultWrapper(res);
            resultWrapper["intrinsic"]  = m_cameraMatrix;
            resultWrapper["distCoeffs"] = m_distCoeffs;

            return NanEscapeScope(res);
        }

    private:
        CameraCalibrationAlgorithm m_algorithm;
        std::vector<std::string>   m_imageFiles;
        cv::Size                   m_imageSize;
        VectorOfVectorOf2DPoints   m_gridCorners;
        cv::Mat                    m_cameraMatrix;
        cv::Mat                    m_distCoeffs;
        bool                       m_calibrationSuccess;
    };

    NAN_METHOD(calibrationPatternDetect)
    {
        NanEscapableScope();

        Local<Object>	imageBuffer;
        Local<Function> callback;
        cv::Size        patternSize;
        PatternType     pattern;

        try
        {
            if (NanCheck(args).ArgumentsCount(5)
                .Argument(0).IsBuffer().Bind(imageBuffer)
                .Argument(1).Bind(patternSize.width)
                .Argument(2).Bind(patternSize.height)
                .Argument(3).StringEnum<PatternType>({ 
                    { "CHESSBOARD",     PatternType::CHESSBOARD }, 
                    { "CIRCLES_GRID",   PatternType::CIRCLES_GRID }, 
                    { "ACIRCLES_GRID",  PatternType::ACIRCLES_GRID } }).Bind(pattern)
                .Argument(4).IsFunction().Bind(callback))
            {
                NanCallback *nanCallback = new NanCallback(callback);
                NanAsyncQueueWorker(new DetectPatternTask(imageBuffer, patternSize, pattern, nanCallback));
                NanReturnValue(NanTrue());
            }

            NanReturnValue(NanFalse());
        }
        catch (ArgumentMismatchException exc)
        {
            return NanThrowTypeError(exc.what());
        }
    }

    NAN_METHOD(calibrateCamera)
    {
        NanEscapableScope();

        std::vector<std::string>                imageFiles;
        std::vector< std::vector<cv::Point2f> > imageCorners;
        Local<Function> callback;
        cv::Size        patternSize;
        cv::Size        imageSize;
        PatternType     pattern;

        try
        {
            if (NanCheck(args).ArgumentsCount(4)
                .Argument(0).IsArray().Bind(imageFiles)
                .Argument(1).Bind(patternSize)
                .Argument(2).StringEnum<PatternType>({ 
                    { "CHESSBOARD",     PatternType::CHESSBOARD }, 
                    { "CIRCLES_GRID",   PatternType::CIRCLES_GRID }, 
                    { "ACIRCLES_GRID",  PatternType::ACIRCLES_GRID } }).Bind(pattern)
                .Argument(3).IsFunction().Bind(callback))
            {
                NanCallback *nanCallback = new NanCallback(callback);
                NanAsyncQueueWorker(new ComputeIntrinsicParametersTask(imageFiles, patternSize, pattern, nanCallback));
                NanReturnValue(NanTrue());
            } else  if (NanCheck(args).ArgumentsCount(6)
                .Argument(0).IsArray().Bind(imageCorners)
                .Argument(1).Bind(imageSize)
                .Argument(2).Bind(patternSize)
                .Argument(3).StringEnum<PatternType>({ 
                    { "CHESSBOARD",     PatternType::CHESSBOARD }, 
                    { "CIRCLES_GRID",   PatternType::CIRCLES_GRID }, 
                    { "ACIRCLES_GRID",  PatternType::ACIRCLES_GRID } }).Bind(pattern)
                .Argument(4).IsFunction().Bind(callback))
            {
                NanCallback *nanCallback = new NanCallback(callback);
                NanAsyncQueueWorker(new ComputeIntrinsicParametersTask(imageCorners, imageSize, patternSize, pattern, nanCallback));
                NanReturnValue(NanTrue());
            }

            NanReturnValue(NanFalse());
        }
        catch (ArgumentMismatchException exc)
        {
            return NanThrowTypeError(exc.what());
        }
    }
}