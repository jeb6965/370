// windows: g++ -o assignment-2-demo.exe assignment-2-demo.cpp -O2 -lgdi32
// mac: g++ assignment-2-demo.cpp -o assignment-2-demo -std=c++11 -lX11

#include <iostream>
#include "CImg.h"

using namespace cimg_library;

int main(){  

	//create an empty 439x514
	CImg<unsigned char> image(439, 514, 1, 3, 0);
	

	//set colors at corners
	//top left: 
	image(0,0,0,0) = 150; //<- set red channel (x, y, 0, rgb channel)
	image(0,0,0,1) = 250; //<- set green channel
	image(0,0,0,2) = 10; //<- set blue channel
	
	//bottom right:
	image(image.width()-1,image.height()-1,0,0) = 10; //<- set red channel 
	image(image.width()-1,image.height()-1,0,1) = 100; //<- set green channel
	image(image.width()-1,image.height()-1,0,2) = 250; //<- set blue channel
	
	//TODO: Complete the color values at the other two corners
	//bottom left:
	image(0,image.height()-1,0,0) = 190; //<- set red channel 
	image(0,image.height()-1,0,1) = 25; //<- set green channel
	image(0,image.height()-1,0,2) = 60; //<- set blue channel	
	
	//top right:
	image(image.width()-1,0,0,0) = 5;   //<- set red channel 
	image(image.width()-1,0,0,1) = 180; //<- set green channel
	image(image.width()-1,0,0,2) = 40; //<- set blue channel

	
	//AVOID int division for accurate interpolation
	//image.width()*1.0 <-- use something like this when you divide declare i/j as floats
	
		//TODO: interpolation formula per channel for edges (top/bottom)
	for (float i = 0; i < image.width(); i++){
	        float t = i / ((image.width() * 1.0) - 1.0); 
	//Top edge interpolation.	
			image(i,0,0,0) = (1 - t) * 150 + (t * 5);//red color based on lineraly interpolating left corner 
						 //and right corner;
			image(i,0,0,1) = (1 - t) * 250 + (t * 180);//green color based on lineraly interpolating left corner 
						 //and right corner;
			image(i,0,0,2) = (1 - t) * 10 + (t * 40); //blue color based on lineraly interpolating left corner 
						 //and right corner;
	//Bottom edge interpolation.
			image(i,image.height()-1,0,0) = (1 - t) * 190 + (t * 10);//red color based on lineraly interpolating left corner 
						 //and right corner;
			image(i,image.height()-1,0,1) = (1 - t) * 25 + (t * 100);//green color based on lineraly interpolating left corner 
						 //and right corner;
			image(i,image.height()-1,0,2) = (1 - t) * 60 + (t * 250); //blue color based on lineraly interpolating left corner 
						 //and right corner;
	}
	image.save("top-bottom.jpg");
	
	
	//TODO: interpolate down the columns
	for (int i = 0; i < image.width(); i++){
		//iterate over all the columns in the image
		for (int j = 1; j < image.height() - 1; j++){ //TODO: change to start at second row and end at second to last to last row
			float t = j / ((image.height() * 1.0) - 1.0);
			//Get RGB values at column ends.
			float topRed = image(i, 0, 0, 0);
			float topGreen = image(i, 0, 0, 1);
			float topBlue = image(i, 0, 0, 2);

			float bottomRed = image(i, image.height()-1, 0, 0);
			float bottomGreen = image(i, image.height()-1, 0, 1);
			float bottomBlue = image(i, image.height()-1, 0, 2);

			image(i,j,0,0) = (1 - t) * topRed + (t * bottomRed);//red color based on lineraly interpolating top to bottom.
			image(i,j,0,1) = (1 - t) * topGreen + (t * bottomGreen);//green color based on lineraly interpolating top to bottom. 
			image(i,j,0,2) = (1 - t) * topBlue + (t * bottomBlue); //blue color based on lineraly interpolating top to bottom.
		}
	}	
	
	
	//display an image	
	/*CImgDisplay disp(image);
	while (!disp.is_closed()){
		disp.wait();
	}*/
	
	image.save("gradient-image.jpg");
	
	//Second image.
	//create an empty 439x514
	CImg<unsigned char> image2(439, 514, 1, 3, 0);
	

	//set colors at corners
	//top left: 
	image2(0,0,0,0) = 255; //<- set red channel (x, y, 0, rgb channel)
	image2(0,0,0,1) = 2; //<- set green channel
	image2(0,0,0,2) = 150; //<- set blue channel
	
	//bottom right:
	image2(image2.width()-1,image2.height()-1,0,0) = 100; //<- set red channel 
	image2(image2.width()-1,image2.height()-1,0,1) = 200; //<- set green channel
	image2(image2.width()-1,image2.height()-1,0,2) = 40; //<- set blue channel
	
	//TODO: Complete the color values at the other two corners
	//bottom left:
	image2(0,image2.height()-1,0,0) = 1; //<- set red channel 
	image2(0,image2.height()-1,0,1) = 60; //<- set green channel
	image2(0,image2.height()-1,0,2) = 150; //<- set blue channel	
	
	//top right:
	image2(image2.width()-1,0,0,0) = 90;   //<- set red channel 
	image2(image2.width()-1,0,0,1) = 200; //<- set green channel
	image2(image2.width()-1,0,0,2) = 200; //<- set blue channel

	
	//AVOID int division for accurate interpolation
	//image.width()*1.0 <-- use something like this when you divide declare i/j as floats
	
		//TODO: interpolation formula per channel for edges (top/bottom)
	for (float i = 0; i < image2.width(); i++){
	        float t = i / ((image2.width() * 1.0) - 1.0); 
	//Top edge interpolation.	
			image2(i,0,0,0) = (1 - t) * 255 + (t * 90);//red color based on lineraly interpolating left corner 
						 //and right corner;
			image2(i,0,0,1) = (1 - t) * 2 + (t * 200);//green color based on lineraly interpolating left corner 
						 //and right corner;
			image2(i,0,0,2) = (1 - t) * 150 + (t * 200); //blue color based on lineraly interpolating left corner 
						 //and right corner;
	//Bottom edge interpolation.
			image2(i,image2.height()-1,0,0) = (1 - t) * 1 + (t * 100);//red color based on lineraly interpolating left corner 
						 //and right corner;
			image2(i,image2.height()-1,0,1) = (1 - t) * 60 + (t * 200);//green color based on lineraly interpolating left corner 
						 //and right corner;
			image2(i,image2.height()-1,0,2) = (1 - t) * 150 + (t * 40); //blue color based on lineraly interpolating left corner 
						 //and right corner;
	}
	image2.save("top-bottom-2.jpg");
	
	
	//TODO: interpolate down the columns
	for (int i = 0; i < image2.width(); i++){
		//iterate over all the columns in the image
		for (int j = 1; j < image2.height() - 1; j++){ //TODO: change to start at second row and end at second to last to last row
			float t = j / ((image2.height() * 1.0) - 1.0);
			//Get RGB values at column ends.
			float topRed = image2(i, 0, 0, 0);
			float topGreen = image2(i, 0, 0, 1);
			float topBlue = image2(i, 0, 0, 2);

			float bottomRed = image2(i, image2.height()-1, 0, 0);
			float bottomGreen = image2(i, image2.height()-1, 0, 1);
			float bottomBlue = image2(i, image2.height()-1, 0, 2);

			image2(i,j,0,0) = (1 - t) * topRed + (t * bottomRed);//red color based on lineraly interpolating top to bottom.
			image2(i,j,0,1) = (1 - t) * topGreen + (t * bottomGreen);//green color based on lineraly interpolating top to bottom. 
			image2(i,j,0,2) = (1 - t) * topBlue + (t * bottomBlue); //blue color based on lineraly interpolating top to bottom.
		}
	}
	image2.save("gradient-image_2.jpg");
	return 0;
}








	

