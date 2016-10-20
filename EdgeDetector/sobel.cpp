#include "sobel.h"

#include <vector>
#include <cmath>

#define KERNAL_LEN 9
#define PPM_COLOR_MAX_VAL 255

const char kernal_x[] = { -1,  0,  1,
                          -2,  0,  2,
                          -1,  0,  1 };
const char kernal_y[] = { -1, -2, -1,
                           0,  0,  0,
                           1,  2,  1 };

std::shared_ptr<PPMImage> apply_filter(std::shared_ptr<PPMImage> image)
{
    int width = image->width;
    std::vector<int> offsets = { -(width + 1), -width, -(width - 1), -1, 0, 1, (width - 1), width, (width + 1) };

    PPMImage filtered;
    filtered.width = image->width - 2;
    filtered.height = image->height - 2;
    filtered.color_max_val = PPM_COLOR_MAX_VAL;
    filtered.data.reserve((image->width - 2) * (image->height - 2));

    for (unsigned int i = image->width + 1; i < image->data.size() - image->width - 1; i++)
    {
        int pixel_x = 0;
        for (int j = 0; j < KERNAL_LEN; j++)
        {
            pixel_x += kernal_x[j] * image->data[i + offsets[j]].red;
        }

        int pixel_y = 0;
        for (int j = 0; j < KERNAL_LEN; j++)
        {
            pixel_y += kernal_y[j] * image->data[i + offsets[j]].red;
        }

        double value = std::trunc(std::sqrt((pixel_x * pixel_x) + (pixel_y * pixel_y)));
        uchar byte_val = static_cast<uchar>(value > 255 ? 255 : value);
        filtered.data.push_back(PPMPixel{byte_val, byte_val, byte_val});
    }

    return std::make_shared<PPMImage>(filtered);
}