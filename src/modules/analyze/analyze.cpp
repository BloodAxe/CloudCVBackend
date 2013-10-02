#include "analyze.hpp"
#include "dominantColors.hpp"
#include "modules/common/ScopedTimer.hpp"

namespace cloudcv
{
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

	void analyzeImage(cv::Mat src, AnalyzeResult& result)
	{
		ScopedTimer timer;

		result = AnalyzeResult();

		result.source      = src;
		result.frameSize   = src.size();
		result.aspectRatio = aspectRatio(src.size());

		if (src.channels() == 1)
		{
			result.grayscale = src;
		}
		else
		{
			cv::cvtColor(src, result.grayscale, cv::COLOR_BGR2GRAY);

			// Process color image here:
			cv::Mat_<cv::Vec3b> colorImg = src;
			
			DominantColorsExtractor colorsExtractor;
			colorsExtractor.process(colorImg);

			result.dominantColors      = colorsExtractor.mainColors;
			result.dominantColorsImage = colorsExtractor.getImage();

			result.uniqieColors        = colorsExtractor.getUniqueColors();
			result.reducedColors       = colorsExtractor.getRedicedColors();
			result.colorDeviation      = colorsExtractor.getColorDeviation();

			drawHistogram(src, result.colorHistogram);
		}

		// Process the rest with grayscale image
		result.intensity   = distribution(result.grayscale);
		result.rmsContrast = rmsContrast(result.grayscale);
		
		// Draw grayscale histogram
		drawHistogram(result.grayscale, result.grayHistogram);


		// Canny
		{
			result.cannyLowerThreshold = static_cast<int>(0.5 * result.intensity.average);
			result.cannyUpperThreshold = static_cast<int>(1.5 * result.intensity.average);
			result.apertureSize = 3;

			cv::Canny(result.grayscale, result.cannyImage, result.cannyLowerThreshold, result.cannyUpperThreshold, result.apertureSize);        
		}


		// Hough
		{
			cv::Mat houghImage = result.cannyImage.clone();

			cv::HoughLinesP(houghImage, result.houghLines, 2, 2 * CV_PI / 180, 50, 50, 5);

			cv::Mat linesImg(src.size(), src.type());
			linesImg = cv::Scalar::all(0);

			for (size_t lineIdx= 0; lineIdx < result.houghLines.size(); lineIdx++)
			{
				cv::Vec4i v = result.houghLines[lineIdx];
				cv::line(linesImg, cv::Point(v[0], v[1]), cv::Point(v[2], v[3]), cv::Scalar(0,255,0,255), CV_AA);
			}

			result.houghImage = 0.5 * src + linesImg;        
		}

		result.processingTimeMs = timer.executionTimeMs();
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

	std::ostream& operator<<(std::ostream& str, const ColorDeviation& res)
	{
		str << "R:"<< res.red << std::endl;
		str << "G:"<< res.green << std::endl;
		str << "B:"<< res.blue << std::endl;
		
		return str;
	}

	std::ostream& operator<<(std::ostream& str, const DominantColor& res)
	{
		str << "Color:" << res.color       << std::endl;
		str << "Area :" << res.totalPixels << std::endl;
		str << "Error:" << res.error       << std::endl;
		return str;
	}

	std::ostream& operator<<(std::ostream& str, const AnalyzeResult& res)
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
	}

}