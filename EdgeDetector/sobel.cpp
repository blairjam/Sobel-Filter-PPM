#include "sobel.h"

#include <vector>
#include <cmath>
#include <thread>

#define KERNAL_LEN 6
#define PPM_COLOR_MAX_VAL 255
#define MAX_THREADS 4

const char kernal_x[] = { -1, 1, -2, 2, -1, 1 };
const char kernal_y[] = { -1, -2, -1, 1, 2, 1 };

// Generate a vector of pixel locations representing the square around a pixel.
// Both X and Y offsets are given here.
std::vector<int> get_offsets(int width)
{
	std::vector<int> offsets = { /* X offsets */ -(width + 1), -(width - 1), -1, 1, (width - 1), (width + 1),
								 /* Y offsets */ -(width + 1), -width, -(width - 1), (width - 1), width, (width + 1) };
	return offsets;
}

// Determine if a pixel falls on the edge of an image.
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

// Apply a sobel filter to the specified range of pixels, packing the supplied vector with the results.
void apply_filter(std::shared_ptr<PPMImage> image, int pix_begin, int pix_end, std::vector<PPMPixel> *return_pixels)
{
	auto offsets = get_offsets(image->width);

	// Iterate over pixel range and find the values based on the sobel filters.
	for (uint i = pix_begin; i < pix_end; i++)
	{
		// Skip all edge pixels.
		if (is_edge_pixel(image->width, image->height, i))
		{
			continue;
		}

		int pixel_x = 0;
		int pixel_y = 0;

		// Calculate values here. This is the kernal value times the offset pixel value.
		for (int j = 0; j < KERNAL_LEN; j++)
		{
			pixel_x += kernal_x[j] * image->data[i + offsets[j]].red;
			pixel_y += kernal_y[j] * image->data[i + offsets[j + KERNAL_LEN]].red;
		}

		// Get the magnitude of the x and y values. Clamped to 255.
		double mag = std::trunc(std::sqrt((pixel_x * pixel_x) + (pixel_y * pixel_y)));
		if (mag > 255)
		{
			mag = 255;
		}

		// Cast to unsigned char and pack vector.
		uchar byte_val = static_cast<uchar>(mag);

		return_pixels->push_back(PPMPixel{ byte_val, byte_val, byte_val });
	}
}

// Applies a sobel filter serially.
std::shared_ptr<PPMImage> apply_filter_serial(std::shared_ptr<PPMImage> image)
{
	// Create an image pointer to store the created image. Reserve space for the generated pixels.
	auto filtered_image = std::make_shared<PPMImage>(PPMImage{ image->width - 2, image->height - 2, PPM_COLOR_MAX_VAL, std::vector<PPMPixel>() });
	filtered_image->data.reserve((image->width - 2) * (image->height - 2));

	// Generate the pixels here. Store them in the pixels vector.
	std::vector<PPMPixel> pixels;
	apply_filter(image, 0, image->data.size(), &pixels);

	// Insert the pixels vector into the data of the image.
	filtered_image->data.insert(filtered_image->data.end(), pixels.begin(), pixels.end());

	return filtered_image;
}

std::shared_ptr<PPMImage> apply_filter_parallel(std::shared_ptr<PPMImage> image)
{
	// Create an image pointer to store the created image. Reserve space for the generated pixels.
	auto filtered_image = std::make_shared<PPMImage>(PPMImage{ image->width - 2, image->height - 2, PPM_COLOR_MAX_VAL, std::vector<PPMPixel>() });
	filtered_image->data.reserve((image->width - 2) * (image->height - 2));

	// Create a 2d vector of PPMPixel(s) to store the generated pixels. Initalize it with empty PPMPixel vectors.
	std::vector<std::vector<PPMPixel>> pixel_container(MAX_THREADS, std::vector<PPMPixel>());

	std::vector<std::thread> threads;

	// Define a chunk of each image as 1 / MAX_THREADS rows.
	int chunk = static_cast<int>(std::ceil(static_cast<double>(image->height) / static_cast<double>(MAX_THREADS)));
	int begin = 0;
	int end;
	for (int i = 0; i < MAX_THREADS; i++)
	{
		// Calculate and end pixel as the width offest from the beginning pixel multiplied by the chunk value. Clamp to the size of the image.
		end = begin + (image->width * chunk);
		if (end > image->width * image->height)
		{
			end = image->width * image->height;
		}

		// Create a new thread and start filtering over the given range.
		threads.emplace_back(std::thread(apply_filter, image, begin, end, &(pixel_container[i])));

		// Set the beginning to the end for the next iteration.
		begin = end;
	}

	// Join all the threads.
	for (int i = 0; i < MAX_THREADS; i++)
	{
		threads[i].join();
	}

	// Append the generated pixels to the data of the image.
	for (int i = 0; i < MAX_THREADS; i++)
	{
		filtered_image->data.insert(filtered_image->data.end(), pixel_container[i].begin(), pixel_container[i].end());
	}

	return filtered_image;
}
