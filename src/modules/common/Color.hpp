#pragma once

#include <stdint.h>

namespace cloudcv 
{

template <typename ValueType>
struct rgb_color_t
{
	typedef ValueType value_t;

	value_t r;	
	value_t g;	
	value_t b;	
};

typedef rgb_color_t<uint8_t> rgb8u_color_t;
typedef rgb_color_t<uint8_t> rgb32f_color_t;

}