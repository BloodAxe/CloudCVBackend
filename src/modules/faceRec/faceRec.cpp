#include "faceRec.hpp"
#include "modules/common/ScopedTimer.hpp"
#include "modules/common/ImageUtils.hpp"

cv::Ptr<cv::CascadeClassifier> faceCascade;
cv::Ptr<cv::CascadeClassifier> nestedCascade;

const static cv::Scalar colors[] =  {	CV_RGB(0,0,255),
    CV_RGB(0,128,255),
    CV_RGB(0,255,255),
    CV_RGB(0,255,0),
    CV_RGB(255,128,0),
    CV_RGB(255,255,0),
    CV_RGB(255,0,0),
    CV_RGB(255,0,255)	};

bool initCascades(std::string cascadeName, std::string nestedCascadeName)
{
    if (!faceCascade)
    {
        faceCascade = new cv::CascadeClassifier();

        if (!faceCascade->load(cascadeName))
        {
            std::cerr << "ERROR: Could not load classifier cascade" << std::endl;
            faceCascade = NULL;
        }
    }

    if (!nestedCascade)
    {
        nestedCascade = new cv::CascadeClassifier();
        if (!nestedCascade->load( nestedCascadeName ) )
        {
            nestedCascade = NULL;
            std::cerr << "WARNING: Could not load classifier cascade for nested objects" << std::endl;
        }
    }

    return !faceCascade.empty();
}

void detectFace(cv::Mat img, FaceDetectionResult& result)
{
    result = FaceDetectionResult();
 
    ScopedTimer timer;
    getOptimalSizeImage(img, result.imageWithFaces);


    int detectedFacesCounter = 0;

    cv::Mat gray;
    cv::cvtColor(result.imageWithFaces, gray, cv::COLOR_BGR2GRAY);
    cv::equalizeHist( gray, gray );

    faceCascade->detectMultiScale( gray, result.faces,
        1.1, 2, 0
        //|CV_HAAR_FIND_BIGGEST_OBJECT
        //|CV_HAAR_DO_ROUGH_SEARCH
        |CV_HAAR_SCALE_IMAGE
        ,
        cv::Size(30, 30) );

    result.detectionTimeMs = timer.executionTimeMs();

    for (auto r = result.faces.begin(); r != result.faces.end(); r++, detectedFacesCounter++ )
    {
        cv::Mat smallImgROI;
        std::vector<cv::Rect> nestedObjects;
        cv::Point center;
        cv::Scalar color = colors[detectedFacesCounter%8];
        int radius;

        double aspect_ratio = (double)r->width/r->height;

        if (0.75 < aspect_ratio && aspect_ratio < 1.3)
        {
            center.x = cvRound((r->x + r->width*0.5));
            center.y = cvRound((r->y + r->height*0.5));
            radius   = cvRound((r->width + r->height)*0.25);
            cv::circle(result.imageWithFaces, center, radius, color, 3, 8, 0 );
        }
        else
        {
            cv::rectangle(result.imageWithFaces,  cvPoint(cvRound(r->x), 
                                cvRound(r->y)),
                                cvPoint(cvRound((r->x + r->width-1)), 
                                cvRound((r->y + r->height-1))),
                                color, 3, 8, 0);
        }

        if (nestedCascade.empty())
            continue;

        smallImgROI = gray(*r);
        nestedCascade->detectMultiScale( smallImgROI, nestedObjects,
            1.1, 2, 0
            //|CV_HAAR_FIND_BIGGEST_OBJECT
            //|CV_HAAR_DO_ROUGH_SEARCH
            //|CV_HAAR_DO_CANNY_PRUNING
            |CV_HAAR_SCALE_IMAGE
            ,
            cv::Size(30, 30) );

        for (auto nr = nestedObjects.begin(); nr != nestedObjects.end(); nr++ )
        {
            center.x = cvRound((r->x + nr->x + nr->width*0.5));
            center.y = cvRound((r->y + nr->y + nr->height*0.5));
            radius = cvRound((nr->width + nr->height)*0.25);
            circle(result.imageWithFaces, center, radius, color, 3, 8, 0 );
        }
    }
}

std::ostream& operator<<(std::ostream& str, const FaceDetectionResult& res)
{
    str << "Detection time (ms) : " << res.detectionTimeMs << std::endl;
    return str;
}
