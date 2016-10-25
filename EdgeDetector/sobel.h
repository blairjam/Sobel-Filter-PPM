#pragma once

#include "ppmbin.h"

std::shared_ptr<PPMImage> apply_filter_serial(std::shared_ptr<PPMImage> image);
std::shared_ptr<PPMImage> apply_filter_parallel(std::shared_ptr<PPMImage> image);
