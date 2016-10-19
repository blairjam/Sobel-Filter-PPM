#include <iostream>
#include "PPMReader.h"

int main()
{
	auto img = ppm::read(std::string("small.ppm"));

	std::cout << img->width << "\n" << img->height << "\n" << img->color_max_val << "\n\n";

	for (std::vector<ppm::PPMPixel>::iterator it = img->data.begin(); it != img->data.end(); ++it)
	{
		std::cout << static_cast<int>(it->red) << " " << static_cast<int>(it->green) << " " << static_cast<int>(it->blue) << "\n";
	}

	system("pause");
	return 0;
}