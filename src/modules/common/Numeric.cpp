#include "Numeric.hpp"


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


/**
 * Compute distribution of 1-D series
 */
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