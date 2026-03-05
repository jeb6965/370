#include <iostream>
#include "CImg.h"
using namespace std;
using namespace cimg_library;
int main(){
	//read in an image file that's in the same folder as your code
	CImg<unsigned char> image1("baboon.bmp");
	//get image dimensions
	cout << "image width: " << image1.width() << endl; //how many columns are the image
	cout << "image height: " << image1.height() << endl; //how many rows are in the image
	//create an empty image
	//specify the width, height, and number of color channels (3 for RGB, 1 for grayscale)
	CImg<unsigned char> image2(image1.width(), image1.height(), 1, 3, 0);
	//for (start, stop, step size)
	for (int x = 0; x < image1.width(); x++){
		for (int y = 0; y < image1.height(); y++){
			/*
			//to access the red value at pixel location (x,y):
			image1(x, y, 0, 0)
			//to access the green value at pixel location (x,y):
			image1(x, y, 0, 1)
			//to access the blue value at pixel location (x,y):
			image1(x, y, 0, 2)
			*/
			//copy color from first image into second
			image2(x, y, 0, 0) = image1(x, y, 0, 0);
			image2(x, y, 0, 1) = image1(x, y, 0, 1);
			image2(x, y, 0, 2) = image1(x, y, 0, 2);
		}
	}
	//display an image
	CImgDisplay disp(image2);
	while (!disp.is_closed()) //can skip {} for 1 line blocks
		disp.wait();
	return 0;
}
