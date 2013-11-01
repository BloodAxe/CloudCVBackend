#include "faceRec.hpp"

int main(int argc, const char * argv[])
{
	cv::Mat i = cv::imread(argv[1]);

	FaceDetectionResult res;
	detectFace(i, res);

	std::cout << res << std::endl;


	cv::imshow("Result", res.imageWithFaces);
	cv::waitKey(-1);

	return 0;
}