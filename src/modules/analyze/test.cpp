#include "analyze.hpp"

int main(int argc, const char * argv[])
{
	cv::Mat i = cv::imread(argv[1]);

	cloudcv::AnalyzeResult res;
	cloudcv::analyzeImage(i, res);

	std::cout << res << std::endl;
	return 0;
}