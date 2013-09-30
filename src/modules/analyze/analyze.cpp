#include "analyze.hpp"
#include "../../node/node_helpers.hpp"

namespace cloudcv
{
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
        int hist_w = 512; int hist_h = 200;
        int bin_w = cvRound( (double) hist_w/histSize );

        histImage.create( hist_h, hist_w, CV_8UC4);
        histImage = Scalar::all(0);

        /// Normalize the result to [ 0, histImage.rows ]
        normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
        normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
        normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );

        /// Draw for each channel
        for( int i = 1; i < histSize; i++ )
        {
            line( histImage, Point( bin_w*(i-1), hist_h - cvRound(b_hist.at<float>(i-1)) ) ,
                             Point( bin_w*(i), hist_h - cvRound(b_hist.at<float>(i)) ),
                             Scalar( 255, 0, 0, 255), 2, CV_AA, 0  );
            line( histImage, Point( bin_w*(i-1), hist_h - cvRound(g_hist.at<float>(i-1)) ) ,
                             Point( bin_w*(i), hist_h - cvRound(g_hist.at<float>(i)) ),
                             Scalar( 0, 255, 0, 255), 2, CV_AA, 0  );
            line( histImage, Point( bin_w*(i-1), hist_h - cvRound(r_hist.at<float>(i-1)) ) ,
                             Point( bin_w*(i), hist_h - cvRound(r_hist.at<float>(i)) ),
                             Scalar( 0, 0, 255, 255), 2, CV_AA, 0  );
        }
  }

  cv::Size aspectRatio(const cv::Size& frameSize)
  {
      size_t d = gcd(frameSize.width, frameSize.height);
      return cv::Size(frameSize.width / d, frameSize.height/ d);
  }

  /**
   * Root mean square (RMS) contrast
   */
  float rmsContrast(cv::Mat_<unsigned char> grayscale)
  {
    cv::Mat I;
    cv::convertTo(I, CV_32F, 1.0f / 255.0f);
    
    cv::Mat normalize = (I - cv::mean(I).val[0]);
    
    float sum = cv::sum(normalize.mul(normalize)).val[0];
    float totalPixels = grayscale.rows * grayscale.cols;

    return sqrt(sum / totalPixels);
  }

  void extractDominantColors(cv::Mat_<cv::Vec3b> colorImg, std::vector<cv::Vec3b>& colors)
  {

  }

  Distribution distribution(cv::InputArray data)
  {
    cv::Scalar avg, dev;
    double min,max;

    cv::meanStdDev(data, avg, dev);
    cv::minMaxLoc(data, &min, &max);

    Distribution d;
    d.average           = avg.val[0];
    d.standardDeviation = dev.val[0];
    d.min     = min;
    d.max     = max;
    d.entropy = 0;
    return d;
  }

  void analyzeImage(cv::Mat src, AnalyzeResult& result)
  {
      ScopedTimer timer;

      result = AnalyzeResult();

      drawHistogram(src, result.histogram);

      result.source      = src;
      result.frameSize   = src.size();
      result.aspectRatio = aspectRatio(src.size());

      cv::Mat gray;

      if (src.channels() == 1)
      {
        gray = src;
      }
      else
      {
        cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);

        // Process color image here:

        std::map<size_t, size_t> fullColorsTable;
        std::map<size_t, size_t> quantizedColorsTable;

        cv::Mat_<cv::Vec3b> colorImg = src;
        std::vector<unsigned char> r,g,b;

        for (int row = 0; row < colorImg.rows; row++)
        {
          for (int col = 0; col < colorImg.cols; col++)
          {
              cv::Vec3b clr = colorImg(row, col);
              size_t hash1 = clr[0] + clr[1] << 8 + clr[2] << 16;
              size_t hash2 = ((clr[0]/2)*2) + ((clr[1]/2)*2) << 8 + ((clr[2]/2)*2) << 16;

              fullColorsTable[hash1]++;
              quantizedColorsTable[hash2]++;

              r.push_back(clr[0]);
              g.push_back(clr[0]);
              b.push_back(clr[0]);
          }
        }

        result.uniqieColors  = fullColorsTable.size();
        result.reducedColors = quantizedColorsTable.size();

        redDeviation   = distribution(r);
        greenDeviation = distribution(g);
        blueDeviation  = distribution(b);

        extractDominantColors(src, result.dominantColors, result.dominantColorsImage);
      }

      // Process the rest with grayscale image
      result.intensity  = distribution(gray);
      result.contrast   = rmsContrast(gray);

      // Canny
      {
        result.cannyLowerThreshold = 0.5 * result.brightness;
        result.cannyUpperThreshold = 1.5 * result.brightness;

        cv::Canny(gray, result.cannyImage, result.cannyLowerThreshold, result.cannyUpperThreshold);        
      }
      

      // Hough
      {
        std::vector<cv::Vec4i> lines;
        cv::HoughLinesP(result.cannyImage, result.houghLines, 2, 2 * CV_PI / 180, 100, 100, 5);

        cv::Mat linesImg(gray.size(), CV_8UC4);
        linesImg = cv::Scalar::all(0);

        for (size_t lineIdx= 0; lineIdx < lines.size(); lineIdx++)
        {
          cv::Vec4i v = lines[lineIdx];
          cv::line(linesImg, cv::Point(v[0], v[1]), cv::Point(v[2], v[3]), cv::Scalar(0,255,0,255));
        }

        result.houghImage = 0.5 * src + linesImg;        
      }
      
      result.processingTimeMs = timer.executionTimeMs();
  } 
}