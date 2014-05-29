#include "CameraCalibrationBinding.hpp"

#include <framework/marshal/marshal.hpp>
#include <framework/Job.hpp>
#include <node/node_helpers.hpp>

using namespace v8;
using namespace node;

namespace cloudcv
{    
    enum PatternType {
        CHESSBOARD = 0,
        CIRCLES_GRID = 1,
        ASYMMETRIC_CIRCLES_GRID = 2
    };

    class DetectPatternTask : public Job
    {    
    public:   
        DetectPatternTask(NanCallback * callback, int width, int height, PatternType type, cv::Mat image, bool returnImage)
			: Job(callback)
            , m_pattern(type)
            , m_patternSize(width, height)
            , m_image(image)
            , m_patternfound(false)
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
            m_patternfound = false;

            switch(m_pattern)
            {
                case CHESSBOARD:
                {
                    const int flags = cv::CALIB_CB_FAST_CHECK | cv::CALIB_CB_NORMALIZE_IMAGE | cv::CALIB_CB_ADAPTIVE_THRESH;
                    m_patternfound = cv::findChessboardCorners(m_image, m_patternSize, m_corners2d, flags);

                    if (m_patternfound)
                    {
                        cv::Mat gray;
                        cv::cvtColor(m_image, gray, cv::COLOR_BGR2GRAY);

                        const cv::TermCriteria tc = cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1);
                        cv::cornerSubPix(gray, m_corners2d, cv::Size(11, 11), cv::Size(-1, -1), tc);
                    }

                };
                break;
                
                case CIRCLES_GRID:
                {
                    const int flags = cv::CALIB_CB_SYMMETRIC_GRID;
                    //cv::SimpleBlobDetector::Params params;
                    //params.filterByColor = true;
                    //params.blobColor = 0;
                    //params.filterByArea = false;
                    //params.filterByInertia = false;

                    //cv::Ptr<cv::FeatureDetector> blobDetector = new cv::SimpleBlobDetector(params);
                    m_patternfound = cv::findCirclesGrid(m_image, m_patternSize, m_corners2d, flags);
                };
                break;
                
                case ASYMMETRIC_CIRCLES_GRID:
                {
                    std::cout << "ASYMMETRIC_CIRCLES_GRID " << m_patternSize << std::endl;

                    cv::Mat gray;
                    cv::cvtColor(m_image, gray, cv::COLOR_BGR2GRAY);
                    //cv::imwrite("Gray.png", gray);

                    const int flags = cv::CALIB_CB_ASYMMETRIC_GRID;
                    //cv::SimpleBlobDetector::Params params;
                    //params.filterByColor = true;
                    //params.blobColor = 0;
                    //params.filterByArea = false;
                    //params.maxArea = 10e4;

                    //cv::Ptr<cv::FeatureDetector> blobDetector = new cv::SimpleBlobDetector(params);
                    m_patternfound = cv::findCirclesGrid(gray, m_patternSize, m_corners2d, flags );
                    //m_patternfound = cv::findCirclesGrid(m_image, m_patternSize, m_corners2d, flags, blobDetector);
                };
                break;
                
                default:
                    std::cerr << "Unsupported pattern type value " << m_pattern << std::endl;
                    break;
            };

            if (m_returnImage) 
            {
                cv::drawChessboardCorners(m_image, m_patternSize, cv::Mat(m_corners2d), m_patternfound);
            }
        }

		virtual Local<Value> CreateCallbackResult()
        {
			NanScope();
            Local<Object> res = NanNew<Object>(); //(Object::New());
			
			NodeObject resultWrapper(res);
			resultWrapper["corners"]      = m_corners2d;
            resultWrapper["patternFound"] = m_patternfound;
            if (m_returnImage) 
            {
                resultWrapper["image"]  = toDataUri(m_image, kImageTypeJpeg);                
            }

			NanReturnValue(res);
        }

    private:
        PatternType              m_pattern;
        cv::Size                 m_patternSize;
        bool                     m_returnImage;

        cv::Mat                  m_image;
		std::vector<cv::Point2f> m_corners2d;
        bool                     m_patternfound;
    };

    class ComputeIntrinsicParametersTask : public Job
    {
    public:
        typedef std::vector<std::vector<cv::Point3f> > VectorOfVectorOf3DPoints;
        typedef std::vector<std::vector<cv::Point2f> > VectorOfVectorOf2DPoints;
        typedef std::vector<cv::Mat>                   VectorOfMat;

        ComputeIntrinsicParametersTask(NanCallback * callback, cv::Size boardSize, cv::Size frameSize, PatternType type, const VectorOfVectorOf2DPoints& v)
            : Job(callback)
            , m_pattern(type)
            , m_imageSize(frameSize)
            , m_boardSize(boardSize)
            , m_corners2d(v)
            , m_calibrationSuccess(false)
        {

        }

    protected:

        virtual void Execute()
        {
            PatternType patternType = m_pattern;
            float squareSize        = 1;
            float aspectRatio       = 1;
            int flags               = 0;
            cv::Mat&   cameraMatrix = m_cameraMatrix; 
            cv::Mat&   distCoeffs   = m_distCoeffs;
            VectorOfMat rvecs; 
            VectorOfMat tvecs;
            std::vector<float> reprojErrs;

            m_calibrationSuccess = runCalibration(m_corners2d, m_imageSize, m_boardSize, patternType, squareSize, aspectRatio, flags, cameraMatrix, distCoeffs, rvecs, tvecs, reprojErrs, m_totalAvgErr);
        }

		virtual Local<Value> CreateCallbackResult()
		{
            NanScope();
            Local<Object> res = NanNew<Object>(); //Local(Object::New());
            
            NodeObject resultWrapper(res);
            resultWrapper["intrinsic"]  = m_cameraMatrix;
            resultWrapper["distCoeffs"] = m_distCoeffs;
            resultWrapper["rmsError"]   = m_totalAvgErr;

            NanReturnValue(res);			
        }

	private:

        static void calcChessboardCorners(cv::Size boardSize, float squareSize, std::vector<cv::Point3f>& corners, PatternType patternType)
        {
            corners.resize(0);

            switch(patternType)
            {
            case CHESSBOARD:
            case CIRCLES_GRID:
                for( int i = 0; i < boardSize.height; i++ )
                    for( int j = 0; j < boardSize.width; j++ )
                        corners.push_back(cv::Point3f(float(j*squareSize), float(i*squareSize), 0));
                break;

            case ASYMMETRIC_CIRCLES_GRID:
                for( int i = 0; i < boardSize.height; i++ )
                    for( int j = 0; j < boardSize.width; j++ )
                        corners.push_back(cv::Point3f(float((2*j + i % 2)*squareSize), float(i*squareSize), 0));
                break;

            default:
                break;
            }
        }


        static bool runCalibration(VectorOfVectorOf2DPoints imagePoints,
                    cv::Size imageSize, 
                    cv::Size boardSize, 
                    PatternType patternType,
                    float squareSize, 
                    float aspectRatio,
                    int flags, 
                    cv::Mat& cameraMatrix, 
                    cv::Mat& distCoeffs,
                    VectorOfMat& rvecs, 
                    VectorOfMat& tvecs,
                    std::vector<float>& reprojErrs,
                    double& totalAvgErr)
        {
            cameraMatrix = cv::Mat::eye(3, 3, CV_64F);

            if (flags & cv::CALIB_FIX_ASPECT_RATIO) 
            {
                cameraMatrix.at<double>(0,0) = aspectRatio;                
            }

            distCoeffs = cv::Mat::zeros(8, 1, CV_64F);

            VectorOfVectorOf3DPoints objectPoints(1);
            calcChessboardCorners(boardSize, squareSize, objectPoints[0], patternType);

            objectPoints.resize(imagePoints.size(), objectPoints[0]);

            double rms = cv::calibrateCamera(objectPoints, imagePoints, imageSize, cameraMatrix,
                            distCoeffs, rvecs, tvecs, flags | cv::CALIB_FIX_K4 | cv::CALIB_FIX_K5);

            std::cout << "RMS error reported by calibrateCamera: " << rms << std::endl;

            bool ok = cv::checkRange(cameraMatrix) && cv::checkRange(distCoeffs);

            totalAvgErr = computeReprojectionErrors(objectPoints, imagePoints, rvecs, tvecs, cameraMatrix, distCoeffs, reprojErrs);

            return ok;
        }

        static double computeReprojectionErrors(
            const VectorOfVectorOf3DPoints& objectPoints,
            const VectorOfVectorOf2DPoints& imagePoints,
            const VectorOfMat& rvecs, 
            const VectorOfMat& tvecs,
            const cv::Mat& cameraMatrix, 
            const cv::Mat& distCoeffs,
            std::vector<float>& perViewErrors
        )
        {
            std::vector<cv::Point2f> imagePoints2;
            int totalPoints = 0;
            double totalErr = 0, err;
            perViewErrors.resize(objectPoints.size());

            for (size_t i = 0; i < objectPoints.size(); i++ )
            {
                cv::projectPoints(cv::Mat(objectPoints[i]), rvecs[i], tvecs[i], cameraMatrix, distCoeffs, imagePoints2);
                err = cv::norm(cv::Mat(imagePoints[i]), cv::Mat(imagePoints2), CV_L2);
                int n = (int)objectPoints[i].size();
                perViewErrors[i] = (float)std::sqrt(err*err/n);
                totalErr += err*err;
                totalPoints += n;
            }

            return std::sqrt(totalErr/totalPoints);
        }

    private:
        PatternType              m_pattern;
        cv::Size                 m_imageSize;
        cv::Size                 m_boardSize;

        VectorOfVectorOf2DPoints m_corners2d;
        bool                     m_calibrationSuccess;
        cv::Mat                  m_cameraMatrix; 
        cv::Mat                  m_distCoeffs;
        double                   m_totalAvgErr;

    };

	NAN_METHOD(calibrationPatternDetect)
    {
        NanScope();

        if (args.Length() != 5)
        {
            return NanThrowError("Invalid number of arguments");  
        }

        if (!args[0]->IsObject())
        {
            return NanThrowTypeError("First argument should be a Buffer");      
        }

        // 0 - image
        // 1 - width
        // 2 - height
        // 3 - pattern
        // 4 - callback

		int w  = args[1]->Uint32Value();
		int h  = args[2]->Uint32Value();
        int pt = args[3]->Uint32Value();
        PatternType pattern;

        switch (pt)
        {
            case 0:
                pattern = CHESSBOARD;
                break;

            case 1:
                pattern = CIRCLES_GRID;
                break;

            case 2:
                pattern = ASYMMETRIC_CIRCLES_GRID;
                break;    

            default:
                return NanThrowError("Unsupported pattern type. Only 0 (CHESSBOARD), 1 (CIRCLES_GRID) or 2 (ASYMMETRIC_CIRCLES_GRID) are supported.");
        };

        if (!args[4]->IsFunction())
        {
            return NanThrowTypeError("Last argument must be a function.");
        }

        // The task holds our custom status information for this asynchronous call,
        // like the callback function we want to call when returning to the main
        // thread and the status information.

		NanCallback *callback = new NanCallback(args[4].As<Function>());

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
			NanAsyncQueueWorker(new DetectPatternTask(callback, w, h, pattern, inputImage, true));
		}

		NanReturnUndefined();
    }

	NAN_METHOD(calibrateCamera)
    {
        NanScope();

        if (args.Length() != 7)
        {
            return NanThrowError("Invalid number of arguments");  
        }

        if (!args[0]->IsObject())
        {
            return NanThrowTypeError("First argument should be a Buffer");      
        }

        // 0 - image
        // 1 - width
        // 2 - height
        // 3 - pattern
        // 4 - callback

        int w  = args[1]->Uint32Value();
        int h  = args[2]->Uint32Value();
        int pt = args[3]->Uint32Value();

        int frame_w  = args[4]->Uint32Value();
        int frame_h  = args[5]->Uint32Value();

        PatternType pattern;

        switch (pt)
        {
            case 0:
                pattern = CHESSBOARD;
                break;

            case 1:
                pattern = CIRCLES_GRID;
                break;

            case 2:
                pattern = ASYMMETRIC_CIRCLES_GRID;
                break;    

            default:
                return NanThrowError("Unsupported pattern type. Only 0 (CHESSBOARD), 1 (CIRCLES_GRID) or 2 (ASYMMETRIC_CIRCLES_GRID) are supported.");
        };

        if (!args[6]->IsFunction())
        {
            return NanThrowTypeError("Last argument must be a function.");
        }

        // The task holds our custom status information for this asynchronous call,
        // like the callback function we want to call when returning to the main
        // thread and the status information.

        NanCallback *callback = new NanCallback(args[6].As<Function>());

        ComputeIntrinsicParametersTask::VectorOfVectorOf2DPoints v;

        if (!MarshalToNative(args[0], v))
        {
            return NanThrowTypeError("Cannot convert first array.");            
        }

        // The task holds our custom status information for this asynchronous call,
        // like the callback function we want to call when returning to the main
        // thread and the status information.
        NanAsyncQueueWorker(new ComputeIntrinsicParametersTask(callback, cv::Size(w, h), cv::Size(frame_w, frame_h), pattern, v));

		NanReturnUndefined();
    }

}


