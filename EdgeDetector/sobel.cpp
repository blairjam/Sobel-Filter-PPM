#include "sobel.h"

#include <vector>
#include <cmath>
#include <thread>

#define KERNAL_LEN 6
#define PPM_COLOR_MAX_VAL 255
#define MAX_THREADS 4

const char kernal_x[] = { -1, 1, -2, 2, -1, 1 };
const char kernal_y[] = { -1, -2, -1, 1, 2, 1 };

std::vector<int> get_offsets(int width)
{
	std::vector<int> offsets = { /* X offsets */ -(width + 1), -(width - 1), -1, 1, (width - 1), (width + 1),
		/* Y offsets */ -(width + 1), -width, -(width - 1), (width - 1), width, (width + 1) };
	return offsets;
}

bool is_edge_pixel(int width, int height, int pixel)
{
	if ((pixel < width) || // First row
		(pixel >= width * height - width && pixel < width * height) || // Last row
		(pixel % width == width - 1) || // First column
		(pixel % width == 0)) // Last column
	{
		return true;
	}
	else
	{
		return false;
	}
}

void apply_filter(std::shared_ptr<PPMImage> image, int pix_begin, int pix_end, std::vector<PPMPixel> *return_pixels)
{
	auto offsets = get_offsets(image->width);

	for (uint i = pix_begin; i < pix_end; i++)
	{
		if (is_edge_pixel(image->width, image->height, i))
		{
			continue;
		}

		int pixel_x = 0;
		int pixel_y = 0;

		for (int j = 0; j < KERNAL_LEN; j++)
		{
			pixel_x += kernal_x[j] * image->data[i + offsets[j]].red;
			pixel_y += kernal_y[j] * image->data[i + offsets[j + KERNAL_LEN]].red;
		}

		double mag = std::trunc(std::sqrt((pixel_x * pixel_x) + (pixel_y * pixel_y)));
		if (mag > 255)
		{
			mag = 255;
		}

		uchar byte_val = static_cast<uchar>(mag);

		return_pixels->push_back(PPMPixel{ byte_val, byte_val, byte_val });
	}
}

std::shared_ptr<PPMImage> apply_filter_serial(std::shared_ptr<PPMImage> image)
{
	auto offsets = get_offsets(image->width);

	auto filtered_image = std::make_shared<PPMImage>(PPMImage{ image->width - 2, image->height - 2, PPM_COLOR_MAX_VAL, std::vector<PPMPixel>() });
	filtered_image->data.reserve((image->width - 2) * (image->height - 2));

	std::vector<PPMPixel> pixels;
	apply_filter(image, 0, image->data.size(), &pixels);

	filtered_image->data.insert(filtered_image->data.end(), pixels.begin(), pixels.end());

	return filtered_image;
}

std::shared_ptr<PPMImage> apply_filter_parallel(std::shared_ptr<PPMImage> image)
{
	auto offsets = get_offsets(image->width);

	auto filtered_image = std::make_shared<PPMImage>(PPMImage{ image->width - 2, image->height - 2, PPM_COLOR_MAX_VAL, std::vector<PPMPixel>() });
	filtered_image->data.reserve((image->width - 2) * (image->height - 2));

	std::vector<std::vector<PPMPixel>> pixel_container(4, std::vector<PPMPixel>());
	std::vector<std::thread> threads;

	int chunk = static_cast<int>(std::ceil(static_cast<double>(image->height) / static_cast<double>(MAX_THREADS)));
	int begin = 0;
	int end;
	for (int i = 0; i < MAX_THREADS; i++)
	{
		end = begin + (image->width * chunk);
		if (end > image->width * image->height)
		{
			end = image->width * image->height;
		}

		threads.emplace_back(std::thread(apply_filter, image, begin, end, &(pixel_container[i])));

		begin = end;
	}

	for (int i = 0; i < MAX_THREADS; i++)
	{
		threads[i].join();
	}

	for (int i = 0; i < MAX_THREADS; i++)
	{
		filtered_image->data.insert(filtered_image->data.end(), pixel_container[i].begin(), pixel_container[i].end());
	}

	return filtered_image;
}
