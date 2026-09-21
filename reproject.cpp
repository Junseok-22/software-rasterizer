#include "tgaimage.h"
#include <vector>
#include <cmath>
#include <iostream>
#include <utility>

struct Vec3f { float x, y, z; };
struct Vec2i { int x, y; };

constexpr TGAColor red = {0,0,255,255};

void line(int x0, int y0, int x1, int y1, TGAImage &image, const TGAColor &color) {
    bool steep = false;

    if (abs(y1 - y0) > abs(x1 - x0)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }

    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    for (float t = 0.0; t < 1.0; t += 0.01) {
        int x = x0 + (x1 - x0) * t;
        int y = y0 + (y1 - y0) * t;
        
        if (steep) 
            image.set(y, x, color);
        
        else 
            image.set(x, y, color);
    }
}

int main() {
    // Camera intrinsics (KITTI-like)
    float fx = 721.5f, fy = 721.5f;
    float cx = 609.5f, cy = 172.8f;

    int img_w = 1242, img_h = 375;
    TGAImage image(img_w, img_h, TGAImage::RGB);

    // 8 corners of a cuboid ahead of the camera (meters, camera coords)
    // x in {-1, 1}, y in {-1, 1}, z in {8, 10}
    std::vector<Vec3f> corners = {
        {-1, -1,  8},  // 0
        { 1, -1,  8},  // 1
        { 1,  1,  8},  // 2
        {-1,  1,  8},  // 3  (front face: 0-1-2-3)
        {-1, -1, 10},  // 4
        { 1, -1, 10},  // 5
        { 1,  1, 10},  // 6
        {-1,  1, 10},  // 7  (back face: 4-5-6-7)
    };

    // 12 edges as index pairs: 4 front, 4 back, 4 connecting
    std::vector<std::pair<int,int>> edges = {
        {0,1},{1,2},{2,3},{3,0},   // front face
        {4,5},{5,6},{6,7},{7,4},   // back face
        {0,4},{1,5},{2,6},{3,7},   // front-to-back connectors
    };

    std::vector<Vec2i> proj;

    for (auto &c : corners) {
    if (c.z <= 0) continue;
    float u = fx * (c.x / c.z) + cx;
    float v = fy * (c.y / c.z) + cy;
    proj.push_back({ int(u), int(v) });
    std::cout << u << ", " << v << "\n";
    
    }

    
    for (auto &e : edges) {
    line(proj[e.first].x, proj[e.first].y,
         proj[e.second].x, proj[e.second].y,
         image, red);
    }

    image.write_tga_file("reproject.tga");
    return 0;
}