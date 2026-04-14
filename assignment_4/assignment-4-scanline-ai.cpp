#include <iostream>
#include <vector>
#include <algorithm>
#include "CImg.h"
using namespace cimg_library;

int implicitLineEq(float x, float y, float x0, float y0, float x1, float y1)
{
    return (y1 - y0) * x - (x1 - x0) * y + x1 * y0 - x0 * y1;
}

// ─── Scanline structures ───────────────────────────────────────────────────────

struct Edge {
    float yMax;       // Upper y boundary of the edge
    float xAtYMin;    // x value at the lower y boundary
    float inverseSlope; // 1/m  (delta x per scanline step)
};

// Build the Edge Table from the polygon vertices
std::vector<std::vector<Edge>> buildEdgeTable(
    const std::vector<int>& xs,
    const std::vector<int>& ys,
    int imageHeight)
{
    std::vector<std::vector<Edge>> ET(imageHeight);
    int n = xs.size();

    for (int i = 0; i < n; i++)
    {
        int j = (i + 1) % n;

        int x0 = xs[i], y0 = ys[i];
        int x1 = xs[j], y1 = ys[j];

        // Skip horizontal edges — they don't contribute to scanline crossings
        if (y0 == y1) continue;

        // Ensure (x0,y0) is the lower point
        if (y0 > y1) {
            std::swap(x0, x1);
            std::swap(y0, y1);
        }

        Edge e;
        e.yMax        = y1;
        e.xAtYMin     = x0;
        e.inverseSlope = (float)(x1 - x0) / (float)(y1 - y0);

        if (y0 >= 0 && y0 < imageHeight)
            ET[y0].push_back(e);
    }
    return ET;
}

void scanlineFill(
    CImg<unsigned char>& img,
    const std::vector<int>& xs,
    const std::vector<int>& ys,
    const unsigned char* fillColor)
{
    int height = img.height();
    auto ET = buildEdgeTable(xs, ys, height);

    std::vector<Edge> AET; // Active Edge Table

    for (int y = 0; y < height; y++)
    {
        // 1. Move edges whose yMin == y from ET into AET
        for (auto& e : ET[y])
            AET.push_back(e);

        // 2. Remove edges from AET whose yMax <= current scanline
        AET.erase(
            std::remove_if(AET.begin(), AET.end(),
                [y](const Edge& e){ return e.yMax <= y; }),
            AET.end());

        // 3. Sort AET by current x intersection
        std::sort(AET.begin(), AET.end(),
            [](const Edge& a, const Edge& b){ return a.xAtYMin < b.xAtYMin; });

        // 4. Fill between pairs of intersections (even-odd rule)
        for (int k = 0; k + 1 < (int)AET.size(); k += 2)
        {
            int xStart = (int)std::ceil(AET[k].xAtYMin);
            int xEnd   = (int)std::floor(AET[k + 1].xAtYMin);

            for (int x = xStart; x <= xEnd; x++)
            {
                if (x >= 0 && x < img.width())
                {
                    img(x, y, 0, 0) = fillColor[0];
                    img(x, y, 0, 1) = fillColor[1];
                    img(x, y, 0, 2) = fillColor[2];
                }
            }
        }

        // 5. Advance each active edge's x by its inverse slope
        for (auto& e : AET)
            e.xAtYMin += e.inverseSlope;
    }
}

// ──────────────────────────────────────────────────────────────────────────────

int main()
{
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
            std::cout << "Mouse click on position: (" << x << "," << y << ")" << std::endl;
            raster.draw_circle(x, y, 5, color);
            window.display(raster);
        }
    }

    for (int i = 0; i < coords_x.size(); i++)
    {
        std::cout << "(" << coords_x[i] << "," << coords_y[i] << ")" << std::endl;
    }

    // ── Save the selected points image ───────────────────────────────────────
    raster.save("ai_points.jpg");
    // ─────────────────────────────────────────────────────────────────────────

    CImg<unsigned char> points = raster;

    for (int line = 0; line < (coords_x.size()); line++)
    {
        CImg<unsigned char> segmentRaster = points;
        int nextPoint = (line + 1) % coords_x.size();
        float x0, y0, x1, y1;

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

        std::cout << "coords: " << x0 << ", " << y0 << ", " << x1 << ", " << y1 << std::endl;
        float slope = (y1 - y0) / (x1 - x0);

        if (slope >= 0 && slope < 1)
        {
            std::cout << "CASE 1\n";
            float y = y0;
            for (float x = x0; x < x1; x++)
            {
                raster(x, y, 0, 0) = 255.0;
                segmentRaster(x, y, 0, 0) = 255.0;
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
                segmentRaster(x, y, 0, 0) = 255.0;
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
                segmentRaster(x, y, 0, 0) = 255.0;
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
                segmentRaster(x, y, 0, 0) = 255.0;
                if (implicitLineEq(x + 0.5, y - 1, x0, y0, x1, y1) > 0)
                    x++;
            }
        }

        std::string filename = "midpoint_line_" + std::to_string(nextPoint) + ".jpg";
        segmentRaster.save(filename.c_str());
    }

    // ── Save the closed outline after all midpoint edges are drawn ───────────
    raster.save("ai_outline.jpg");
    // ─────────────────────────────────────────────────────────────────────────

    // ── Scanline fill runs here, after all midpoint edges are drawn ──────────
    unsigned char fillColor[] = {255, 0, 255};
    scanlineFill(raster, coords_x, coords_y, fillColor);
    // ─────────────────────────────────────────────────────────────────────────

    // ── Save the final filled shape ──────────────────────────────────────────
    raster.save("finalAI.jpg");
    // ─────────────────────────────────────────────────────────────────────────

    CImgDisplay disp2(raster);
    while (!disp2.is_closed())
        disp2.wait();
}