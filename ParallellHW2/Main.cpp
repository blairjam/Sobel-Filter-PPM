#include "PPMReader.h"

int main()
{
	PPMReader reader("small.ppm");
	reader.read();
	system("pause");
	return 0;
}