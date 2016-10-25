#include "sobel.h"

#include <vector>
#include <cmath>

#define KERNAL_LEN 6
#define PPM_COLOR_MAX_VAL 255

const char kernal_x[] = { -1, 1, -2, 2, -1, 1 };
const char kernal_y[] = { -1, -2, -1, 1, 2, 1 };

std::vector<int> get_offsets(int width)
{
	std::vector<int> offsets = { /* X offsets */ -(width + 1), -(width - 1), -1, 1, (width - 1), (width + 1),
								 /* Y offsets */ -(width + 1), -width, -(width - 1), (width - 1), width, (width + 1) };

	return offsets;
}

std::vector<PPMPixel> apply_filter(std::shared_ptr<PPMImage> image, int pix_begin, int pix_end)
{
	auto offsets = get_offsets(image->width);

	std::vector<PPMPixel> pixels;

	for (uint i = pix_begin; i < pix_end; i += (i + 2) % image->width == 0 ? 3 : 1)
	{
		int pixel_x = 0;
		int pixel_y = 0;

		for (int j = 0; j < KERNAL_LEN; j++)
		{
			pixel_x += kernal_x[j] * image->data[i + offsets[j]].red;
			pixel_y += kernal_y[j] * image->data[i + offsets[j + KERNAL_LEN]].red;

			int mag = std::trunc(std::sqrt((pixel_x * pixel_x) + (pixel_y * pixel_y)));
			if (mag > 255)
			{
				mag = 255;
			}

			uchar byte_val = static_cast<uchar>(mag);

			pixels.emplace_back(PPMPixel{ byte_val, byte_val, byte_val });
		}
	}

	return pixels;
}

std::shared_ptr<PPMImage> apply_filter_serial(std::shared_ptr<PPMImage> image)
{
	int width = image->width;
	int height = image->height;
	auto offsets = get_offsets(width);

	auto filtered_image = std::make_shared<PPMImage>(PPMImage{});
	filtered_image->width = width - 2;
	filtered_image->height = height - 2;
	filtered_image->color_max_val = PPM_COLOR_MAX_VAL;
	filtered_image->data.reserve((width - 2) * (height - 2));
	
	for (unsigned int i = width + 1; i < image->data.size() - width - 1; i += (i + 2) % width == 0 ? 3 : 1)
	{
		int pixel_x = 0;
		int pixel_y = 0;

		for (int j = 0; j < KERNAL_LEN; j++)
		{
			pixel_x += kernal_x[j] * image->data[i + offsets[j]].red;
			pixel_y += kernal_y[j] * image->data[i + offsets[j + KERNAL_LEN]].red;
		}

		int mag = std::trunc(std::sqrt((pixel_x * pixel_x) + (pixel_y * pixel_y)));
		if (mag > 255)
		{
			mag = 255;
		}

		uchar byte_val = static_cast<uchar>(mag);

		filtered_image->data.emplace_back(PPMPixel{ byte_val, byte_val, byte_val });
	}

	return filtered_image;
}

std::shared_ptr<PPMImage> apply_filter_parallel(std::shared_ptr<PPMImage> image)
{
	
}
