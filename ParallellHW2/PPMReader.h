#pragma once

#include <vector>
#include <memory>
#include <string>

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

class PPMReader
{
private:
	const std::string path;
public:
	PPMReader(std::string path) : path(path) {}
	~PPMReader() {}
	std::unique_ptr<PPMImage> read();
};
