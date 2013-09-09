#include "cloudcv.hpp"

namespace cloudcv
{
    struct ScopedTimer
    {
        inline ScopedTimer()
        {
            m_startTime = cv::getTickCount();
        }

        inline float executionTimeMs() const
        {
            return (cv::getTickCount() - m_startTime) * 1000. / cv::getTickFrequency();
        }

        int64_t m_startTime;
    };

  size_t gcd(size_t u, size_t v)
  {
      // simple cases (termination)
      if (u == v)
          return u;
   
      if (u == 0)
          return v;
   
      if (v == 0)
          return u;
   
      // look for factors of 2
      if (~u & 1) // u is even
      {
          if (v & 1) // v is odd
              return gcd(u >> 1, v);
          else // both u and v are even
              return gcd(u >> 1, v >> 1) << 1;
      }
   
      if (~v & 1) // u is odd, v is even
          return gcd(u, v >> 1);
   
      // reduce larger argument
      if (u > v)
          return gcd((u - v) >> 1, v);
   
      return gcd((v - u) >> 1, u);
  }

    void drawHistogram(cv::Mat src, cv::Mat& histImage)
    {
        using namespace cv;
        using namespace std;

        /// Separate the image in 3 places ( B, G and R )
          vector<Mat> bgr_planes;
          split( src, bgr_planes );

          /// Establish the number of bins
          int histSize = 256;

          /// Set the ranges ( for B,G,R) )
          float range[] = { 0, 256 } ;
          const float* histRange = { range };

          bool uniform = true; bool accumulate = false;

          Mat b_hist, g_hist, r_hist;

          /// Compute the histograms:
          calcHist( &bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate );
          calcHist( &bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate );
          calcHist( &bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate );

          // Draw the histograms for B, G and R
          int hist_w = 512; int hist_h = 400;
          int bin_w = cvRound( (double) hist_w/histSize );

          histImage.create( hist_h, hist_w, CV_8UC3);
          histImage = Scalar( 0,0,0) ;

          /// Normalize the result to [ 0, histImage.rows ]
          normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
          normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
          normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );

          /// Draw for each channel
          for( int i = 1; i < histSize; i++ )
          {
              line( histImage, Point( bin_w*(i-1), hist_h - cvRound(b_hist.at<float>(i-1)) ) ,
                               Point( bin_w*(i), hist_h - cvRound(b_hist.at<float>(i)) ),
                               Scalar( 255, 0, 0), 2, 8, 0  );
              line( histImage, Point( bin_w*(i-1), hist_h - cvRound(g_hist.at<float>(i-1)) ) ,
                               Point( bin_w*(i), hist_h - cvRound(g_hist.at<float>(i)) ),
                               Scalar( 0, 255, 0), 2, 8, 0  );
              line( histImage, Point( bin_w*(i-1), hist_h - cvRound(r_hist.at<float>(i-1)) ) ,
                               Point( bin_w*(i), hist_h - cvRound(r_hist.at<float>(i)) ),
                               Scalar( 0, 0, 255), 2, 8, 0  );
          }
    }

    void analyzeImage(cv::Mat src, AnalyzeResult& result)
    {
        ScopedTimer timer;

        result = AnalyzeResult();
        drawHistogram(src, result.histogram);

        size_t d = gcd(src.cols, src.rows);

        result.size        = src.size();
        result.aspectNum   = src.cols / d;
        result.aspectDenom = src.rows / d;

        cv::Mat gray;
        cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);

        result.brightness = cv::mean(gray).val[0];
        result.contrast = 0;

        result.colorDeviation = 0;
        result.colorEntropy   = 0;

        result.processingTimeMs = timer.executionTimeMs();
    }
    
}