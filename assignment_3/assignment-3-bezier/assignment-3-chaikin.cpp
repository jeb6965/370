#include <iostream>
#include <vector>
#include "CImg.h"
using namespace cimg_library;
int main() {
	CImg<unsigned char> canvas(500, 500, 1, 3, 0);
	unsigned char c_color[] = { 255, 128, 64 };
	unsigned char l_color[] = {255, 255, 255};

	CImgDisplay window(canvas, "Click Me!");
	std::vector<int> coords_x;
	std::vector<int> coords_y;
	std::vector<int> cut_x;
	std::vector<int> cut_y;
	while (!window.is_closed()){
		window.wait();
		if (window.button()){
			int x = window.mouse_x();
			int y = window.mouse_y();
			if (x >= 0 && y >= 0 && x < canvas.width() && y < canvas.height()){
				std::cout << "Mouse click on position: (" << x << "," << y << ")" << std::endl;
				coords_x.push_back(x);
				coords_y.push_back(y);
				
				canvas(x, y, 0, 1) = 255;
				canvas.draw_circle(x, y, 5, c_color);
				
				window.display(canvas);
			}
		}

	for (int i = 0; i < coords_x.size(); i++){
		std::cout << "(" << coords_x[i] << "," << coords_y[i] << ")" << std::endl;
	}
	}
	if (coords_x.size() > 1){
		for (int i = 0; i < (int)coords_x.size() - 1; i++) { // Casting prevents wrapping.
    		canvas.draw_line(coords_x[i], coords_y[i], coords_x[i + 1], coords_y[i + 1], l_color);
		}
	}
	unsigned char colors[6][3] = {
        { 255,  50,  50 },
        {  50, 255,  50 },
        {  50, 150, 255 },
        { 255, 255,  50 },
        { 255, 100, 255 },
        {  50, 255, 200 }
    };
	canvas.save("control_points.jpg");
	for (int iteration = 0; iteration < 6; iteration++){
		cut_x.clear();
        cut_y.clear();
		for (int i = 0; i < (int)coords_x.size() - 1; i++) {
			int x0 = coords_x[i],     y0 = coords_y[i];
			int x1 = coords_x[i + 1], y1 = coords_y[i + 1];

			cut_x.push_back((int)(0.75f * x0 + 0.25f * x1));
			cut_y.push_back((int)(0.75f * y0 + 0.25f * y1));
			cut_x.push_back((int)(0.25f * x0 + 0.75f * x1));
			cut_y.push_back((int)(0.25f * y0 + 0.75f * y1));
		}
		coords_x.swap(cut_x);
        coords_y.swap(cut_y);
		for (int i = 0; i < (int)coords_x.size() - 1; i++) { // Casting prevents wrapping.
    		canvas.draw_line(coords_x[i], coords_y[i], coords_x[i + 1], coords_y[i + 1], colors[iteration]);
		}

		std::string filename = "chaikin_iteration_" + std::to_string(iteration + 1) + ".jpg";
		canvas.save(filename.c_str());
	}
	return 0;
}
