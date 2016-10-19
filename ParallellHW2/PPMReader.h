#pragma once

#include <vector>
#include <memory>
#include <string>

namespace ppm
{
	using uchar = unsigned char;
	using uint = unsigned int;

	struct PPMPixel
	{
		uchar red;
		uchar green;
		uchar blue;
	};

	struct PPMImage
	{
		uint width;
		uint height;
		uint color_max_val;
		std::vector<PPMPixel> data;
	};

	std::unique_ptr<PPMImage> read(std::string& path);
	bool write(std::unique_ptr<PPMImage> image, std::string& path);
}
