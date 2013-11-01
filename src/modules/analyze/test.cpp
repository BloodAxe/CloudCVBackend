#include "analyze.hpp"

int main(int argc, const char * argv[])
{
	cv::Mat i = cv::imread(argv[1]);

	AnalyzeResult res;
	buildFromImage(i, res);

	std::cout << res << std::endl;
	return 0;
}