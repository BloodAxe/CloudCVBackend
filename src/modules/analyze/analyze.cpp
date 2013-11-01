#include "analyze.hpp"
#include "dominantColors.hpp"
#include "modules/common/ScopedTimer.hpp"
#include "modules/common/ImageUtils.hpp"

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

    if (src.channels() == 1)
    {
        Mat b_hist;

        /// Compute the histograms:
        calcHist( &src, 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate );

        // Draw the histograms for B, G and R
        int hist_w = 512; int hist_h = 200;
        int bin_w = cvRound( (double) hist_w/histSize );

        histImage.create( hist_h, hist_w, CV_8UC4);
        histImage = Scalar::all(0);

        /// Normalize the result to [ 0, histImage.rows ]
        normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );


        /// Draw for each channel
        for( int i = 1; i < histSize; i++ )
        {
            line( histImage, Point( bin_w*(i-1), hist_h - cvRound(b_hist.at<float>(i-1)) ) ,
                Point( bin_w*(i), hist_h - cvRound(b_hist.at<float>(i)) ),
                Scalar( 127, 127, 127, 255), 2, CV_AA, 0  );

        }
    }
    else if (src.channels() == 3)
    {
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

}

cv::Size aspectRatio(const cv::Size& frameSize)
{
    size_t d = gcd(frameSize.width, frameSize.height);
    return cv::Size(frameSize.width / d, frameSize.height/ d);
}

/**
* Root mean square (RMS) contrast
*/
double rmsContrast(cv::Mat_<unsigned char> grayscale)
{
    cv::Mat I;
    grayscale.convertTo(I, CV_32F, 1.0f / 255.0f);

    cv::Mat normalized = (I - cv::mean(I).val[0]);

    double sum = cv::sum(normalized.mul(normalized)).val[0];
    double totalPixels = grayscale.rows * grayscale.cols;

    return sqrt(sum / totalPixels);
}

void buildFromImage(cv::Mat input, ImageInformation& value)
{
    value.sourceImage = input;

    if (input.channels() == 1)
    {
        value.grayscaleImage = input;
    }
    else
    {
        cv::cvtColor(input, value.grayscaleImage, cv::COLOR_BGR2GRAY);
    }

    value.hasColor = input.channels() == 3 || input.channels() == 4;

    value.frameSize   = input.size();
    value.aspectRatio = aspectRatio(input.size());
}

void buildFromImage(cv::Mat input, IntensityInformation& value)
{
    // Process the rest with grayscale image
    value.intensity   = distribution(input);
    value.rmsContrast = rmsContrast(input);

    // Draw grayscale histogram
    drawHistogram(input, value.histogramImage);
}

void buildFromImage(cv::Mat input, ColorsInformation& value)
{
    DominantColorsExtractor colorsExtractor;
    colorsExtractor.process(input);

    value.dominantColors      = colorsExtractor.mainColors;
    value.dominantColorsImage = colorsExtractor.getImage();

    value.uniqieColors        = colorsExtractor.getUniqueColors();
    value.reducedColors       = colorsExtractor.getRedicedColors();
    value.colorDeviation      = colorsExtractor.getColorDeviation();

    drawHistogram(input, value.histogramImage);
}

void buildFromImage(cv::Mat input, MorphologicInformation& value, int cannyLower, int cannyUpper)
{
    value.cannyLowerThreshold = cannyLower; //static_cast<int>(0.5 * result.intensity.average);
    value.cannyUpperThreshold = cannyUpper; //static_cast<int>(1.5 * result.intensity.average);

    cv::Canny(input, value.cannyImage, cannyLower, cannyUpper, 3);   
}



void buildFromImage(cv::Mat input, AnalyzeResult& value)
{		
    cv::Mat normalizedImage;
    getOptimalSizeImage(input, normalizedImage);
    input = normalizedImage;


    value = AnalyzeResult();

    buildFromImage(input, value.common);

    buildFromImage(value.common.grayscaleImage, value.grayscale);
    buildFromImage(value.common.grayscaleImage, value.edges, 0.5 * value.grayscale.intensity.average, 1.5 * value.grayscale.intensity.average);

    if (value.common.hasColor)
    {
        buildFromImage(input, value.color);
    }
}

std::ostream& operator<<(std::ostream& str, const Distribution& res)
{
    str << "Min:"				<< res.min << std::endl;
    str << "Max:"				<< res.max << std::endl;
    str << "Average:"			<< res.average << std::endl;
    str << "StandardDeviation:" << res.standardDeviation << std::endl;
    str << "Entropy:"			<< res.entropy << std::endl;
    return str;
}

std::ostream& operator<<(std::ostream& str, const RGBDistribution& res)
{
    str << "R:"<< res.r << std::endl;
    str << "G:"<< res.g << std::endl;
    str << "B:"<< res.b << std::endl;

    return str;
}

std::ostream& operator<<(std::ostream& str, const DominantColor& res)
{
    str << "Color:" << res.color       << std::endl;
    str << "Area :" << res.totalPixels << std::endl;
    str << "Error:" << res.error       << std::endl;
    return str;
}

/*std::ostream& operator<<(std::ostream& str, const AnalyzeResult& res)
{
str << "Frame Size  :" << res.frameSize   << std::endl;
str << "Aspect Ratio:" << res.aspectRatio << std::endl;

std::cout << "Gray Analyze" << std::endl;

str << "Intensity:" << std::endl << res.intensity << std::endl;
str << "RMS Contrast:" << res.rmsContrast << std::endl;

std::cout << "Color Analyze" << std::endl;
str << "Dominant Colors: " << res.dominantColors.size() << std::endl;
for (size_t i=0; i < res.dominantColors.size(); i++)
{
str << "Color " << i << std::endl;
str << res.dominantColors[i] << std::endl;
}
str << "Color Variation:" << std::endl;
str << res.colorDeviation << std::endl;

std::cout << "Morphology analyze" << std::endl;
str << res.cannyLowerThreshold << std::endl;
str << res.cannyUpperThreshold << std::endl;

str << "Processing time (ms):" << res.processingTimeMs;

return str;
}*/
