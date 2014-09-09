#include "analyze.hpp"
#include "dominantColors.hpp"
#include "modules/common/ScopedTimer.hpp"
#include "modules/common/ImageUtils.hpp"

namespace cloudcv
{

    void drawHistogram(cv::Mat src, cv::Mat& histImage)
    {
        using namespace cv;
        using namespace std;

        /// Separate the image in 3 places ( B, G and R )
        vector<Mat> bgr_planes;
        split(src, bgr_planes);

        /// Establish the number of bins
        int histSize = 256;

        /// Set the ranges ( for B,G,R) )
        float range[] = { 0, 256 };
        const float* histRange = { range };

        bool uniform = true; bool accumulate = false;

        if (src.channels() == 1)
        {
            Mat b_hist;

            /// Compute the histograms:
            calcHist(&src, 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);

            // Draw the histograms for B, G and R
            int hist_w = 512; int hist_h = 200;
            int bin_w = cvRound((double)hist_w / histSize);

            histImage.create(hist_h, hist_w, CV_8UC4);
            histImage = Scalar::all(0);

            /// Normalize the result to [ 0, histImage.rows ]
            normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());


            /// Draw for each channel
            for (int i = 1; i < histSize; i++)
            {
                line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
                    Point(bin_w*(i), hist_h - cvRound(b_hist.at<float>(i))),
                    Scalar(127, 127, 127, 255), 2, CV_AA, 0);

            }
        }
        else if (src.channels() == 3)
        {
            Mat b_hist, g_hist, r_hist;

            /// Compute the histograms:
            calcHist(&bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);
            calcHist(&bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
            calcHist(&bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);

            // Draw the histograms for B, G and R
            int hist_w = 512; int hist_h = 200;
            int bin_w = cvRound((double)hist_w / histSize);

            histImage.create(hist_h, hist_w, CV_8UC4);
            histImage = Scalar::all(0);

            /// Normalize the result to [ 0, histImage.rows ]
            normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
            normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
            normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());

            /// Draw for each channel
            for (int i = 1; i < histSize; i++)
            {
                line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
                    Point(bin_w*(i), hist_h - cvRound(b_hist.at<float>(i))),
                    Scalar(255, 0, 0, 255), 2, CV_AA, 0);
                line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(g_hist.at<float>(i - 1))),
                    Point(bin_w*(i), hist_h - cvRound(g_hist.at<float>(i))),
                    Scalar(0, 255, 0, 255), 2, CV_AA, 0);
                line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(r_hist.at<float>(i - 1))),
                    Point(bin_w*(i), hist_h - cvRound(r_hist.at<float>(i))),
                    Scalar(0, 0, 255, 255), 2, CV_AA, 0);
            }
        }

    }

    cv::Size ComputeImageAspectRatio(const cv::Size& frameSize)
    {
        int d = gcd(frameSize.width, frameSize.height);
        return cv::Size(frameSize.width / d, frameSize.height / d);
    }

    /**
    * Root mean square (RMS) contrast
    */
    double ComputeRmsContrast(cv::Mat_<unsigned char> grayscale)
    {
        cv::Mat I;
        grayscale.convertTo(I, CV_32F, 1.0f / 255.0f);

        cv::Mat normalized = (I - cv::mean(I).val[0]);

        double sum = cv::sum(normalized.mul(normalized)).val[0];
        double totalPixels = grayscale.rows * grayscale.cols;

        return sqrt(sum / totalPixels);
    }

    void AnalyzeImage(cv::Mat input, AnalyzeResult& value)
    {
        value = AnalyzeResult();

        DominantColorsExtractor colorsExtractor;
        colorsExtractor.process(input);

        value.frameSize = input.size();
        value.aspectRatio = ComputeImageAspectRatio(input.size());

        value.intensity = distribution(input);
        value.rmsContrast = ComputeRmsContrast(input);
        
        value.quantizedColors = colorsExtractor.getImage();
        value.dominantColors = colorsExtractor.mainColors;

        value.uniqieColors = colorsExtractor.getUniqueColors();
        value.reducedColors = colorsExtractor.getRedicedColors();
        value.colorDeviation = colorsExtractor.getColorDeviation();
    }

}