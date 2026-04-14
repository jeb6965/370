#include <iostream>
#include <vector>
#include <algorithm>
#include "CImg.h"
using namespace cimg_library;

int implicitLineEq(float x, float y, float x0, float y0, float x1, float y1)
{
    return (y1 - y0) * x - (x1 - x0) * y + x1 * y0 - x0 * y1;
}

void barycentricFill(
    CImg<unsigned char>& img,
    int x0, int y0, unsigned char* c0,
    int x1, int y1, unsigned char* c1,
    int x2, int y2, unsigned char* c2)
{
    int minX = std::max(0, std::min({x0, x1, x2}));
    int maxX = std::min(img.width()-1,  std::max({x0, x1, x2}));
    int minY = std::max(0, std::min({y0, y1, y2}));
    int maxY = std::min(img.height()-1, std::max({y0, y1, y2}));

    float area = implicitLineEq(x2, y2, x0, y0, x1, y1);

    if (area == 0) return; // degenerate triangle, skip

    for (int y = minY; y <= maxY; y++)
    {
        for (int x = minX; x <= maxX; x++)
        {
            float w0 = implicitLineEq(x, y, x1, y1, x2, y2) / area;
            float w1 = implicitLineEq(x, y, x2, y2, x0, y0) / area;
            float w2 = implicitLineEq(x, y, x0, y0, x1, y1) / area;

            if (w0 >= 0 && w1 >= 0 && w2 >= 0)
            {
                img(x, y, 0, 0) = (unsigned char)(w0*c0[0] + w1*c1[0] + w2*c2[0]);
                img(x, y, 0, 1) = (unsigned char)(w0*c0[1] + w1*c1[1] + w2*c2[1]);
                img(x, y, 0, 2) = (unsigned char)(w0*c0[2] + w1*c1[2] + w2*c2[2]);
            }
        }
    }
}

struct Edge {
    float yMax;
    float xAtYMin;
    float inverseSlope;
};

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

        if (y0 == y1) continue;

        if (y0 > y1) {
            std::swap(x0, x1);
            std::swap(y0, y1);
        }

        Edge e;
        e.yMax         = y1;
        e.xAtYMin      = x0;
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
    std::vector<Edge> AET;

    for (int y = 0; y < height; y++)
    {
        for (auto& e : ET[y])
            AET.push_back(e);

        AET.erase(
            std::remove_if(AET.begin(), AET.end(),
                [y](const Edge& e){ return e.yMax <= y; }),
            AET.end());

        std::sort(AET.begin(), AET.end(),
            [](const Edge& a, const Edge& b){ return a.xAtYMin < b.xAtYMin; });

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

        for (auto& e : AET)
            e.xAtYMin += e.inverseSlope;
    }
}

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
        std::cout << "(" << coords_x[i] << "," << coords_y[i] << ")" << std::endl;

    // ── Save the selected points image ───────────────────────────────────────
    raster.save("final_points.jpg");
    // ─────────────────────────────────────────────────────────────────────────

    CImg<unsigned char> points = raster;

    for (int line = 0; line < (coords_x.size()); line++)
    {
        CImg<unsigned char> segmentRaster = points;
        int nextPoint = (line + 1) % coords_x.size();
        float x0, y0, x1, y1;

        if (coords_x[line] < coords_x[nextPoint])
        {
            x0 = coords_x[line]; x1 = coords_x[nextPoint];
            y0 = coords_y[line]; y1 = coords_y[nextPoint];
        }
        else
        {
            x1 = coords_x[line]; x0 = coords_x[nextPoint];
            y1 = coords_y[line]; y0 = coords_y[nextPoint];
        }

        float slope = (y1 - y0) / (x1 - x0);

        if (slope >= 0 && slope < 1)
        {
            float y = y0;
            for (float x = x0; x < x1; x++)
            {
                raster(x, y, 0, 0) = 255.0;
                segmentRaster(x, y, 0, 0) = 255.0;
                if (implicitLineEq(x + 1, y + 0.5, x0, y0, x1, y1) > 0) y++;
            }
        }
        if (slope >= -1 && slope < 0)
        {
            float y = y0;
            for (float x = x0; x < x1; x++)
            {
                raster(x, y, 0, 0) = 255.0;
                segmentRaster(x, y, 0, 0) = 255.0;
                if (implicitLineEq(x + 1, y - 0.5, x0, y0, x1, y1) < 0) y--;
            }
        }
        if (slope > 1)
        {
            float x = x0;
            for (float y = y0; y < y1; y++)
            {
                raster(x, y, 0, 0) = 255.0;
                segmentRaster(x, y, 0, 0) = 255.0;
                if (implicitLineEq(x + 0.5, y + 1, x0, y0, x1, y1) < 0) x++;
            }
        }
        if (slope < -1)
        {
            float x = x0;
            for (float y = y0; y > y1; y--)
            {
                raster(x, y, 0, 0) = 255.0;
                segmentRaster(x, y, 0, 0) = 255.0;
                if (implicitLineEq(x + 0.5, y - 1, x0, y0, x1, y1) > 0) x++;
            }
        }

        std::string filename = "midpoint_line_" + std::to_string(nextPoint) + ".jpg";
        segmentRaster.save(filename.c_str());
    }

    // ── Save the closed outline after all midpoint edges are drawn ───────────
    raster.save("final_outline.jpg");
    // ─────────────────────────────────────────────────────────────────────────

    // ── Run barycentric fill if at least 3 points were clicked ───────────────
    if (coords_x.size() >= 3)
    {
        unsigned char c0[] = {255, 0,   0  };  // vertex 0 — red
        unsigned char c1[] = {0,   255, 0  };  // vertex 1 — green
        unsigned char c2[] = {0,   0,   255};  // vertex 2 — blue

        barycentricFill(raster,
            coords_x[0], coords_y[0], c0,
            coords_x[1], coords_y[1], c1,
            coords_x[2], coords_y[2], c2);
    }
    // ─────────────────────────────────────────────────────────────────────────

    // ── Save the final filled shape ──────────────────────────────────────────
    raster.save("finalAIB.jpg");
    // ─────────────────────────────────────────────────────────────────────────

    CImgDisplay disp2(raster);
    while (!disp2.is_closed())
        disp2.wait();
}