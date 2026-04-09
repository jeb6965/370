#include <iostream>
#include <vector>
#include "CImg.h"
using namespace cimg_library;
int implicitLineEq(float x, float y, float x0, float y0, float x1, float y1)
{
    return (y1 - y0) * x - (x1 - x0) * y + x1 * y0 - x0 * y1;
}

int main(){
    CImg<unsigned char> raster(500, 500, 1, 3, 0);
    std::vector<int> coords_x;
    std::vector<int> coords_y;
    CImgDisplay window(raster, "Click Me!");
    unsigned char color[] = {0, 0, 0};
    while (!window.is_closed())
    {
        window.wait();
        if (window.button())
        {
            color[0] = rand() % 256;
            color[1] = rand() % 256;
            color[2] = rand() % 256;
            int x = window.mouse_x();
            int y = window.mouse_y();
            coords_x.push_back(x);
            coords_y.push_back(y);
            std::cout << "Mouse click on position: (" << x << "," << y << ")" <<
            std::endl;
            raster.draw_circle(x, y, 5, color);
            // canvas(x,y,0, 1) = 255;
            window.display(raster);
        }
    }
    for (int i = 0; i < coords_x.size(); i++)
    {
        std::cout << "(" << coords_x[i] << "," << coords_y[i] << ")" << std::endl;
    }
    
    for (int line = 0; line < (coords_x.size()); line++)
    {
        // Ensure closure of the shape when the final point is reached.
        int nextPoint = (line + 1) % coords_x.size();
        float x0, y0, x1, y1;
        // move left to right
        if (coords_x[line] < coords_x[nextPoint])
        {
            x0 = coords_x[line];
            x1 = coords_x[nextPoint];
            y0 = coords_y[line];
            y1 = coords_y[nextPoint];
        }
        else
        {
            x1 = coords_x[line];
            x0 = coords_x[nextPoint];
            y1 = coords_y[line];
            y0 = coords_y[nextPoint];
        }
        std::cout << "coords: " << x0 << ", " << y0 << ", " << x1 << ", " << y1 <<
        std::endl;
        float slope = (y1 - y0) / (x1 - x0);
        std::cout << "slope: " << slope << std::endl;
        if (slope >= 0 && slope < 1)
        {
            std::cout << "CASE 1\n";
            float y = y0;
            for (float x = x0; x < x1; x++)
            {
                raster(x, y, 0, 0) = 255.0;
                if (implicitLineEq(x + 1, y + 0.5, x0, y0, x1, y1) > 0)
                y++;
            }
        }

        if (slope >= -1 && slope < 0)
        {
            std::cout << "CASE 2\n";
            float y = y0;
            for (float x = x0; x < x1; x++)
            {
                raster(x, y, 0, 0) = 255.0;
                if (implicitLineEq(x + 1, y - 0.5, x0, y0, x1, y1) < 0)
                y--;
            }
        }

        if (slope > 1)
        {
            std::cout << "CASE 3\n";
            float x = x0;
            for (float y = y0; y < y1; y++)
            {
                raster(x, y, 0, 0) = 255.0;
                if (implicitLineEq(x + 0.5, y + 1, x0, y0, x1, y1) < 0)
                x++;
            }
        }

        if (slope < -1)
        {
            std::cout << "CASE 4\n";
            float x = x0;
            for (float y = y0; y > y1; y--)
            {
                raster(x, y, 0, 0) = 255.0;
                if (implicitLineEq(x + 0.5, y - 1, x0, y0, x1, y1) > 0)
                x++;
            }
        }

        std::string filename = "midpoint_line_" + std::to_string(nextPoint) + ".jpg";
        raster.save(filename.c_str());
    }
    // Display and save filtered image
    CImgDisplay disp2(raster);
    while (!disp2.is_closed())
    disp2.wait();
}
