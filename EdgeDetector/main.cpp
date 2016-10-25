#include <iostream>
#include <memory>
#include "ppmbin.h"
#include "sobel.h"
#include "Timer.h"

int main()
{   
	std::string dir = "./images/";
	std::string name = "lady";
	std::string type = ".ppm";

    /* READ */
	std::cout << "Reading image... ";
    auto image = read(dir + name + type);
    if (image == nullptr)
    {
		system("pause");
        return 1;
    }
    std::cout << "Done." << std::endl;

    /* GRAY */
    std::cout << "Converting to grayscale... ";
    to_grayscale(image);
    std::cout << "Done." << std::endl;

	/* SERIAL FILTER */
	std::cout << "\nApplying edge detection serially 5 times..." << std::endl;

	std::vector<std::shared_ptr<PPMImage>> serial_images;
	std::vector<double> serial_times;

	for (int i = 0; i < 5; i++)
	{
		Timer timer;

		timer.start();
		auto img = apply_filter_serial(image);
		timer.stop();

		serial_images.push_back(img);
		serial_times.push_back(timer.get_elapsed_time());
	}

	write(serial_images[0], dir + name + "_serial" + type);

	double serial_tot = 0;
	for (int i = 0; i < 5; i++)
	{
		serial_tot += serial_times[i];
	}

	double serial_avg = serial_tot / 5.0;

	std::cout << "Done. Average runtime for edge detection: " << serial_avg << " seconds." << std::endl;

	/* PARALLEL FILTER */
	std::cout << "\nApplying edge detection in parallel 5 times..." << std::endl;

	std::vector<std::shared_ptr<PPMImage>> para_images;
	std::vector<double> para_times;

	for (int i = 0; i < 5; i++)
	{
		Timer timer;

		timer.start();
		auto img = apply_filter_parallel(image);
		timer.stop();

		para_images.push_back(img);
		para_times.push_back(timer.get_elapsed_time());
	}

	write(para_images[0], dir + name + "_para" + type);

	double para_tot = 0;
	for (int i = 0; i < 5; i++)
	{
		para_tot += para_times[i];
	}

	double para_avg = para_tot / 5.0;

	std::cout << "Done. Average runtime for edge detection: " << para_avg << " seconds." << std::endl;


	std::cout << "\nSpeedup of edge detection using 4 threads: " << (serial_avg / para_avg) << std::endl;

    system("pause");
    return 0;
}