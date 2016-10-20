#include <iostream>
#include <memory>
#include "ppmbin.h"
#include "sobel.h"

int main()
{   
	std::string name = "lady";
	std::string type = ".ppm";

    /* READ */
    std::cout << "Reading image." << std::endl;

    auto image = read(name + type);
    if (image == nullptr)
    {
        return 1;
    }

    std::cout << "Image read successful." << std::endl;

    /* GRAY */

    std::cout << "Converting to grayscale." << std::endl;

    to_grayscale(image);

    std::cout << "Grayscale conversion complete." << std::endl;

    /* WRITE */

    std::cout << "Writing grayscale image to file." << std::endl;

    write(image, name + "_gray" + type);

    std::cout << "Image write successful." << std::endl;

    /* FILTER */

    std::cout << "Applying edge detection filter." << std::endl;

    auto filtered = apply_filter(image);

    std::cout << "Filter application successful." << std::endl;

    /* WRITE */

    std::cout << "Writing image to file." << std::endl;

    write(filtered, name + "_filter" + type);

    std::cout << "Image write successful." << std::endl;

    system("pause");
    return 0;
}