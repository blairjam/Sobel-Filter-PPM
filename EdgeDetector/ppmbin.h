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

std::shared_ptr<PPMImage> read(std::string path);
bool write(std::shared_ptr<PPMImage> image, std::string path);
void to_grayscale(std::shared_ptr<PPMImage> image);
