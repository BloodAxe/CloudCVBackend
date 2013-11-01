#include "ImageUtils.hpp"

void getOptimalSizeImage(cv::Mat input, cv::Mat& result)
{
    const int maxWidth = 512;
    const int maxHeight = 512;

    if (input.rows > maxWidth || input.cols > maxHeight)
    {
        const int imgWidth = input.cols;
        const int imgHeight = input.rows;

        int width = input.cols;
        int height = input.rows;

        if (maxWidth && width > maxWidth)
        {
            width = maxWidth;
            height = (imgHeight * width / imgWidth);
        }

        if (maxHeight && height > maxHeight)
        {
            height = maxHeight;
            width = (imgWidth * height / imgHeight);
        }

        cv::resize(input, result, cv::Size(width, height));
    }
    else
    {
        input.copyTo(result);
    }
}