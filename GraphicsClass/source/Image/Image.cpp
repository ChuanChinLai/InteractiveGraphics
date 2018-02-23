#include "Image.h"
#include <LodePNG\lodepng.h>

#include <iostream>
#include <cassert>

Lai::Image::Image(std::string name)
{
	bool res = Load(name);
	assert(res);
}

bool Lai::Image::Load(std::string name)
{
	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, name, LodePNGColorType::LCT_RGB);

	// If there's an error, display it.
	if (error != 0)
	{
		std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
		return false;
	}

	w = width;
	h = height;


	return true;
}
