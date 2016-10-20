#include "ppmbin.h"

#include <fstream>
#include <iostream>

#define PPM_MAGIC_NUM_LEN 2
#define PPM_BIN_MN "P6"
#define PPM_HEADER_VAL_LEN 3
#define PPM_COLOR_MAX_VAL 255
#define PPM_PIXEL_COMPS 3

#define ERR_PATH_EMPTY "No file name given."
#define ERR_OPENING_FILE "File could not be opened."
#define ERR_INVALID_FORM "Invalid image format. Must be a P6 PPM image."
#define ERR_COLOR_COMP "The color component must be 255."

#define GS_RED_WEIGHT 0.21F
#define GS_GREEN_WEIGHT 0.72F
#define GS_BLUE_WEIGHT 0.07F

void print_error(std::string msg)
{
    std::cerr << "[ERROR] " << msg << std::endl;
}

std::shared_ptr<PPMImage> read(std::string path)
{
    // Check that path isn't empty.
    if (path.empty())
    {
        print_error(ERR_PATH_EMPTY);
        return nullptr;
    }

    // Attempt to open file in binary read mode.
    std::ifstream file(path, std::ifstream::binary);
    if (!file)
    {
        print_error(ERR_OPENING_FILE);
        return nullptr;
    }

    // Check that the file is a P6 (binary) type.
    std::string magic_num;
    file >> magic_num;
    if (magic_num != PPM_BIN_MN)
    {
        print_error(ERR_INVALID_FORM);
        return nullptr;
    }

    // Extract the next three values from the file, excluding any comments.
    // These values are the width, height, and size.
    uint header_values[PPM_HEADER_VAL_LEN];
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

    if (header_values[2] != PPM_COLOR_MAX_VAL)
    {
        print_error(ERR_COLOR_COMP);
        return nullptr;
    }

    // Ignore empty space after color compontent size.
    file.ignore(1, '\n');

    uint pixels = header_values[0] * header_values[1];
    uint pixel_bytes = pixels * PPM_PIXEL_COMPS;

    // Create buffer and read remaining data.
    auto pixel_buf = std::make_unique<char[]>(pixel_bytes);
    file.read(pixel_buf.get(), pixel_bytes);

    // Create an image to return.
    auto image = std::make_shared<PPMImage>();
    image->width = header_values[0];
    image->height = header_values[1];
    image->color_max_val = header_values[2];
    image->data.reserve(pixels);

    for (int i = 0; i < pixel_bytes; i += 3)
    {
        image->data.emplace_back(PPMPixel{ static_cast<uchar>(pixel_buf[i]),
                                 static_cast<uchar>(pixel_buf[i + 1]),
                                 static_cast<uchar>(pixel_buf[i + 2]) });
    }

    // Clean up pixel buffer, we don't need this extra memory sitting around.
    pixel_buf.reset();

    file.close();

    return image;
}

bool write(std::shared_ptr<PPMImage> image, std::string path)
{
    // Check that file path isn't empty.
    if (path.empty())
    {
        print_error(ERR_PATH_EMPTY);
        return false;
    }

    // Attempt to open file in binary write mode.
    std::ofstream file(path, std::ofstream::binary);
    if (!file)
    {
        print_error(ERR_OPENING_FILE);
        return false;
    }

    // Write the magic number (P6), the width, the height, and the size to the file.
    file << PPM_BIN_MN << ' ' << image->width << ' ' << image->height << ' ' << image->color_max_val << '\n';

    uint pixel_bytes = image->width * image->height * PPM_PIXEL_COMPS;

    // Fill a buffer with the stored pixel information.
    //auto pixel_buf = std::make_unique<char[]>(pixel_bytes);
	std::vector<char> pixel_buf(pixel_bytes, 0);
    int current_byte = 0;
    for (std::vector<PPMPixel>::iterator it = image->data.begin(); it != image->data.end(); ++it)
    {
        pixel_buf[current_byte] = it->red;
        pixel_buf[current_byte + 1] = it->green;
        pixel_buf[current_byte + 2] = it->blue;

        current_byte += PPM_PIXEL_COMPS;
    }

    // Perform a final write to the file. This is all of the pixel data in bytes.
    file.write(pixel_buf.data(), pixel_bytes);

    file.close();

    return true;
}

void to_grayscale(std::shared_ptr<PPMImage> image)
{
    for (std::vector<PPMPixel>::iterator it = image->data.begin(); it != image->data.end(); ++it)
    {
        // Calculate the grayscale using the luminosity method, a weighted average.
        uchar gray = (it->red * GS_RED_WEIGHT) + (it->green * GS_GREEN_WEIGHT) + (it->blue * GS_BLUE_WEIGHT);

        // Assign all color components to the gray value.
        it->red = gray;
        it->green = gray;
        it->blue = gray;
    }
}
