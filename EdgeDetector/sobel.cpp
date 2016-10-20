#include "sobel.h"

#include <vector>
#include <cmath>

#define KERNAL_LEN 6
#define PPM_COLOR_MAX_VAL 255

const char kernal_x[] = { -1, 1, -2, 2, -1, 1 };
const char kernal_y[] = { -1, -2, -1, 1, 2, 1 };

std::shared_ptr<PPMImage> apply_filter(std::shared_ptr<PPMImage> image)
{
	int width = image->width;
	int offsets_x[] = { -(width + 1), -(width - 1), -1, 1, (width - 1), (width + 1) };
	int offsets_y[] = { -(width + 1), -width, -(width - 1), (width - 1), width, (width + 1) };

	auto filtered_image = std::make_shared<PPMImage>(PPMImage{});
	filtered_image->width = image->width - 2;
	filtered_image->height = image->height - 2;
	filtered_image->color_max_val = PPM_COLOR_MAX_VAL;
	filtered_image->data.reserve((image->width - 2) * (image->height - 2));

	
	for (unsigned int i = image->width + 1; i < image->data.size() - image->width - 1; i += (i + 2) % image->width == 0 ? 3 : 1)
	{
		int pixel_x = 0;
		int pixel_y = 0;

		for (int j = 0; j < KERNAL_LEN; j++)
		{
			pixel_x += kernal_x[j] * image->data[i + offsets_x[j]].red;
			pixel_y += kernal_y[j] * image->data[i + offsets_y[j]].red;
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