#include <iostream>
#include <vector>
#include "CImg.h"
using namespace cimg_library;
int main() {
	CImg<unsigned char> canvas(500, 500, 1, 3, 0);
	unsigned char color[] = { 255, 128, 64 };

	CImgDisplay window(canvas, "Click Me!");
	std::vector<int> coords_x;
	std::vector<int> coords_y;
	
	while (!window.is_closed()){
		window.wait();
		if (window.button()){
			int x = window.mouse_x();
			int y = window.mouse_y();
			std::cout << "Mouse click on position: (" << x << "," << y << ")" << std::endl;
			coords_x.push_back(x);
			coords_y.push_back(y);
			
			canvas(x, y, 0, 1) = 255;
			canvas.draw_circle(x, y, 5, color);
			
			window.display(canvas);
		}
	}
	for (int i = 0; i < coords_x.size(); i++){
		std::cout << "(" << coords_x[i] << "," << coords_y[i] << ")" << std::endl;
	}
return 0;
}
