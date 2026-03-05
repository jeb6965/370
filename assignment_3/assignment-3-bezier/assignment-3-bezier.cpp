#include <iostream>
#include <vector>
#include "CImg.h"
using namespace cimg_library;
int main() {
	CImg<unsigned char> canvas(500, 500, 1, 3, 0);
	unsigned char op_color[] = {255, 128, 64};
	unsigned char np_color[] = {255, 255, 255};

	CImgDisplay window(canvas, "Click Me!");
	std::vector<int> coords_x;
	std::vector<int> coords_y;
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
				canvas.draw_circle(x, y, 5, op_color);
				
				window.display(canvas);
			}
		}

	for (int i = 0; i < coords_x.size(); i++){
		std::cout << "(" << coords_x[i] << "," << coords_y[i] << ")" << std::endl;
	}
	}
	if (coords_x.size() > 1){
		for (int i = 0; i < (int)coords_x.size() - 1; i++) { // Casting prevents wrapping.
    		canvas.draw_line(coords_x[i], coords_y[i], coords_x[i + 1], coords_y[i + 1], op_color);
		}
	}
	canvas.save("control_points.jpg");

	for (float step = 0.0f; step < 1.01f; step += 0.01f){
		std::cout << "Step: "<< step << std::endl;
		std::vector<int> old_points_x;
		std::vector<int> old_points_y;

		for(int i = 0; i < (int)coords_x.size(); i++){
			old_points_x.push_back(coords_x[i]);
			old_points_y.push_back(coords_y[i]);
		}
		std::vector<int> new_points_x;
		std::vector<int> new_points_y;
		for(int level = 0; level < (int)coords_x.size() - 1; level++){
			new_points_x.clear();
			new_points_y.clear();
			for(int i = 0; i < (int)old_points_x.size() - 1; i++){
				float left_old_point_x  = old_points_x[i];
                float left_old_point_y  = old_points_y[i];
                float right_old_point_x = old_points_x[i + 1];
                float right_old_point_y = old_points_y[i + 1];

				float new_x = (1 - step) * left_old_point_x + step * right_old_point_x;
                float new_y = (1 - step) * left_old_point_y + step * right_old_point_y;
				new_points_x.push_back(new_x);
                new_points_y.push_back(new_y);
			}
			old_points_x.swap(new_points_x);
			old_points_y.swap(new_points_y);

		}
		canvas.draw_circle(old_points_x[0], old_points_y[0], 2, np_color);
	}
	CImgDisplay display_window(canvas, "Bezier Curve");
    	while (!display_window.is_closed()) {
        	display_window.wait();
		}
	return 0;
}
