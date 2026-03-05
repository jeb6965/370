// Jordan Boykin.
#include <iostream>
#include "CImg.h"
#include <cmath>
using namespace cimg_library;

int main(){
	// Create a blank image for each manipulation.
	CImg<unsigned char> image("mammothGrove_right.bmp");
	CImg<unsigned char> brightnessMult(image.width(), image.height(), 1, 3, 0);
	CImg<unsigned char> brightnessAdd(image.width(), image.height(), 1, 3, 0);
	CImg<unsigned char> rgbMult(image.width(), image.height(), 1, 3, 0);
	CImg<unsigned char> inversion(image.width(), image.height(), 1, 3, 0);
	CImg<unsigned char> contrastAdj(image.width(), image.height(), 1, 3, 0);
	// Factor and constant for brightness manipulation.
	double factorA = 2.0;
	int constantB = 15;

	// Copy each pixel change to the appropriate blank image using two for loops.
	// Covers brightness, RGB manipulation, inversion, and contrast.
	for (int x = 0; x < image.width(); x++){
		for(int y = 0; y < image.height(); y++){
			brightnessMult(x, y, 0, 0) = std::min(255.0, std::max(0.0, image(x, y, 0, 0) * factorA));
			brightnessMult(x, y, 0, 1) = std::min(255.0, std::max(0.0, image(x, y, 0, 1) * factorA));
			brightnessMult(x, y, 0, 2) = std::min(255.0, std::max(0.0, image(x, y, 0, 2) * factorA));

			brightnessAdd(x, y, 0, 0) = std::min(255, std::max(0, image(x, y, 0, 0) + constantB));
			brightnessAdd(x, y, 0, 1) = std::min(255, std::max(0, image(x, y, 0, 1) + constantB));
			brightnessAdd(x, y, 0, 2) = std::min(255, std::max(0, image(x, y, 0, 2) + constantB));

			rgbMult(x, y, 0, 0) = std::min(255.0, std::max(0.0, image(x, y, 0, 0) * 3.0));
			rgbMult(x, y, 0, 1) = std::min(255.0, std::max(0.0, image(x, y, 0, 1) * 1.5));
			rgbMult(x, y, 0, 2) = std::min(255.0, std::max(0.0, image(x, y, 0, 2) * 2.0));

			inversion(x, y, 0, 0) = 255 - image(x, y, 0, 0);
			inversion(x, y, 0, 1) = 255 - image(x, y, 0, 1);
			inversion(x, y, 0, 2) = 255 - image(x, y, 0, 2);

			contrastAdj(x, y, 0, 0) = std::max(0.0, std::min(255.0, (image(x, y, 0, 0) - 85) * 1.5 + 128));
			contrastAdj(x, y, 0, 1) = std::max(0.0, std::min(255.0, (image(x, y, 0, 1) - 85) * 1.5 + 128));
			contrastAdj(x, y, 0, 2) = std::max(0.0, std::min(255.0, (image(x, y, 0, 2) - 85) * 1.5 + 128));
		}
	}
	// Display all images in their own windows.
	CImgDisplay dispOrig(image);
	CImgDisplay disp1(brightnessMult);
	CImgDisplay disp2(brightnessAdd);
	CImgDisplay disp3(rgbMult);
	CImgDisplay disp4(inversion);
	CImgDisplay disp5(contrastAdj);
	// Keep display open.
	while (!disp5.is_closed())
		disp5.wait();
	// Save each image.
	brightnessMult.save("mammothGroveMult-2_final.bmp");
	brightnessAdd.save("mammothGroveAdd-15_final.bmp");
	rgbMult.save("mammothGroveRGB-3-1.5-2_final.bmp");
	inversion.save("mammothGroveInversion.bmp");
	contrastAdj.save("mammothGroveContrast-1.5_final.bmp");
	return 0;
}
