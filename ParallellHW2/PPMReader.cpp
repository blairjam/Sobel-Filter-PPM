#include "PPMReader.h"

#include <fstream>
#include <iostream>

#define PPM_MAGIC_NUM_LEN 2
#define PPM_BIN_MN "P6"
#define PPM_HEADER_VAL_LEN 3
#define PPM_COLOR_MAX_VAL 255
#define PPM_PIXEL_COMPS 3

std::unique_ptr<PPMImage> PPMReader::read()
{
	if (path == "")
	{
		return nullptr;
	}

	std::ifstream file(path, std::ifstream::binary);

	if (!file)
	{
		std::cerr << "File could not be opened." << std::endl;
		return nullptr;
	}

	// Check that the file is a P6 (binary) type.
	std::string magic_num;
	file >> magic_num;
	if (magic_num != PPM_BIN_MN)
	{
		std::cerr << "Invalid image format. Must be a P6 PPM image." << std::endl;
		return nullptr;
	}

	// Extract the next three values from the file, excluding any comments.
	// These values are the width, height, and size.
	std::string header_values[PPM_HEADER_VAL_LEN];
	int current_val = 0;
	while (!file.eof())
	{
		if (file.peek() == '#')
		{
			file.ignore(INT_MAX, '\n');
		}
		else
		{
			file >> header_values[current_val];

			if (current_val++ >= PPM_HEADER_VAL_LEN - 1)
			{
				break;
			}
		}
	}

	auto image = std::make_unique<PPMImage>();
	image->width = std::stoul(header_values[0]);
	image->height = std::stoul(header_values[1]);
	image->color_max_val = std::stoul(header_values[2]);

	if (image->color_max_val != PPM_COLOR_MAX_VAL)
	{
		std::cerr << "The color component value must be 255." << std::endl;
		return nullptr;
	}

	std::cout << "Width: " << image->width << std::endl
		<< "Height: " << image->height << std::endl
		<< "Color Component: " << image->color_max_val << std::endl << std::endl;

	image->data.reserve(image->width * image->height);

	file.ignore(1, '\n');

	auto pixel_buf = std::make_unique<char[]>(PPM_PIXEL_COMPS);
	while (!file.eof())
	{
		file.read(pixel_buf.get(), PPM_PIXEL_COMPS);
		image->data.push_back({ static_cast<uchar>(pixel_buf[0]), 
							    static_cast<uchar>(pixel_buf[1]), 
							    static_cast<uchar>(pixel_buf[2]) });
	}

	int i = 1;
	for (std::vector<PPMPixel>::iterator it = image->data.begin(); it != image->data.end(); ++it)
	{
		std::cout << "Pixel: " << i << std::endl
			<< "\tR: " << static_cast<int>(it->red) << " G: " << static_cast<int>(it->green) << " B: " << static_cast<int>(it->blue) << std::endl;

		i++;
	}

	/*std::cout << "Magic Num: " << magic_num << std::endl
		<< "Width: " << header_values[0] << std::endl
		<< "Height: " << header_values[1] << std::endl
		<< "Size: " << header_values[2] << std::endl;*/

	file.close();

	return image;
}
