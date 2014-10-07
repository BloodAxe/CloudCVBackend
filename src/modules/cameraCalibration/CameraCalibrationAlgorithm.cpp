#include "CameraCalibrationAlgorithm.hpp"

namespace cloudcv {


    CameraCalibrationAlgorithm::CameraCalibrationAlgorithm(cv::Size patternSize, PatternType type)
        : m_patternSize(patternSize)
        , m_pattern(type)
    {
    }

    bool CameraCalibrationAlgorithm::detectCorners(const cv::Mat& frame, VectorOf2DPoints& corners2d) const
    {
        corners2d.clear();
        bool m_patternfound = false;

        switch (m_pattern)
        {
        case CHESSBOARD:
        {
           const int flags = cv::CALIB_CB_FAST_CHECK | cv::CALIB_CB_NORMALIZE_IMAGE | cv::CALIB_CB_ADAPTIVE_THRESH;
           m_patternfound = cv::findChessboardCorners(frame, m_patternSize, corners2d, flags);

           if (m_patternfound)
           {
               const cv::TermCriteria tc = cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1);
               cv::cornerSubPix(frame, corners2d, cv::Size(11, 11), cv::Size(-1, -1), tc);
           }
        };  break;

        case CIRCLES_GRID:
        {
             const int flags = cv::CALIB_CB_SYMMETRIC_GRID;
             //cv::SimpleBlobDetector::Params params;
             //params.filterByColor = true;
             //params.blobColor = 0;
             //params.filterByArea = false;
             //params.filterByInertia = false;

             //cv::Ptr<cv::FeatureDetector> blobDetector = new cv::SimpleBlobDetector(params);
             m_patternfound = cv::findCirclesGrid(frame, m_patternSize, corners2d, flags);
        };  break;

        case ACIRCLES_GRID:
        {
            const int flags = cv::CALIB_CB_ASYMMETRIC_GRID;
            //cv::SimpleBlobDetector::Params params;
            //params.filterByColor = true;
            //params.blobColor = 0;
            //params.filterByArea = false;
            //params.maxArea = 10e4;

            //cv::Ptr<cv::FeatureDetector> blobDetector = new cv::SimpleBlobDetector(params);
            m_patternfound = cv::findCirclesGrid(frame, m_patternSize, corners2d, flags);
            //m_patternfound = cv::findCirclesGrid(m_image, m_patternSize, m_corners2d, flags, blobDetector);
        };  break;

        default:
            std::cerr << "Unsupported pattern type value " << m_pattern << std::endl;
            break;
        };

        return m_patternfound;
    }

    bool CameraCalibrationAlgorithm::calibrateCamera(
        const std::vector<std::string>& imageFiles,
        cv::Mat& cameraMatrix,
        cv::Mat& distCoeffs
    ) const
    {
        VectorOfMat images(imageFiles.size());

        for (size_t i = 0; i < images.size(); i++)
        {
            images[i] = cv::imread(imageFiles[i], cv::IMREAD_GRAYSCALE);
            if (images[i].empty())
            {
                return false;                
            }
        }

        return calibrateCamera(images, cameraMatrix, distCoeffs);
    }


    bool CameraCalibrationAlgorithm::calibrateCamera(
        const VectorOfMat& images,
        cv::Mat& cameraMatrix,
        cv::Mat& distCoeffs
        ) const 
    {
        VectorOfVectorOf2DPoints corners(images.size());
        bool allDetected = true;
        cv::Size imageSize = images[0].size();

        for (size_t i = 0; i < images.size(); i++) {
            allDetected &= (imageSize == images[i].size()) && detectCorners(images[i], corners[i]);                
        }

        if (!allDetected)
            return false;

        return calibrateCamera(corners, imageSize, cameraMatrix, distCoeffs);
    }

    bool CameraCalibrationAlgorithm::calibrateCamera(
        const VectorOfVectorOf2DPoints& gridCorners,
        const cv::Size imageSize,
        cv::Mat& cameraMatrix,
        cv::Mat& distCoeffs
    ) const
    {
        VectorOfMat rvecs;
        VectorOfMat tvecs;
        std::vector<float> reprojErrs;
        double totalAvgErr;

        return runCalibration(gridCorners, 
                              imageSize, 
                              m_patternSize, 
                              m_pattern, 
                              1,
                              0, 
                              0, 
                              cameraMatrix, 
                              distCoeffs, 
                              rvecs,
                              tvecs,
                              reprojErrs,
                              totalAvgErr);
    }

    bool CameraCalibrationAlgorithm::runCalibration
    (
        VectorOfVectorOf2DPoints imagePoints,
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
        double& totalAvgErr
    )
    {
        cameraMatrix = cv::Mat::eye(3, 3, CV_64F);

        if (flags & cv::CALIB_FIX_ASPECT_RATIO)
        {
            cameraMatrix.at<double>(0, 0) = aspectRatio;
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

    void CameraCalibrationAlgorithm::calcChessboardCorners
    (
        cv::Size boardSize, 
        float squareSize, 
        VectorOf3DPoints& corners, 
        PatternType patternType
    )
    {
        corners.resize(0);

        switch (patternType)
        {
        case CHESSBOARD:
        case CIRCLES_GRID:
            for (int i = 0; i < boardSize.height; i++)
            for (int j = 0; j < boardSize.width; j++)
                corners.push_back(cv::Point3f(float(j*squareSize), float(i*squareSize), 0));
            break;

        case ACIRCLES_GRID:
            for (int i = 0; i < boardSize.height; i++)
            for (int j = 0; j < boardSize.width; j++)
                corners.push_back(cv::Point3f(float((2 * j + i % 2)*squareSize), float(i*squareSize), 0));
            break;

        default:
            break;
        }
    }

    double CameraCalibrationAlgorithm::computeReprojectionErrors(
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

        for (size_t i = 0; i < objectPoints.size(); i++)
        {
            cv::projectPoints(cv::Mat(objectPoints[i]), rvecs[i], tvecs[i], cameraMatrix, distCoeffs, imagePoints2);
            err = cv::norm(cv::Mat(imagePoints[i]), cv::Mat(imagePoints2), CV_L2);
            int n = (int)objectPoints[i].size();
            perViewErrors[i] = (float)std::sqrt(err*err / n);
            totalErr += err*err;
            totalPoints += n;
        }

        return std::sqrt(totalErr / totalPoints);
    }

}

