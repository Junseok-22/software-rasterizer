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
    // KITTI intrinsics 
    float fx = 707.0493f, fy = 707.0493f;
    float cx = 604.0814f, cy = 180.5066f;

    // Load KITTI image
    TGAImage image;
    image.read_tga_file("kitti/000000.tga");
    image.flip_vertically();

    // Label: Pedestrian, dims h=1.89 w=0.48 l=1.20, loc (1.84,1.47,8.41), yaw~0
    float h = 1.89f, w = 0.48f, l = 1.20f;
    float lx = 1.84f, ly = 1.47f, lz = 8.41f;

    //bounds
    float x0 = lx - l/2, x1 = lx + l/2;
    float z0 = lz - w/2, z1 = lz + w/2;
    float yb = ly;        
    float yt = ly - h;     

    std::vector<Vec3f> corners = {
        {x0, yb, z0}, 
        {x1, yb, z0},  
        {x1, yt, z0},  
        {x0, yt, z0},  
        {x0, yb, z1},  
        {x1, yb, z1},  
        {x1, yt, z1},  
        {x0, yt, z1},  

    };

    std::vector<std::pair<int,int>> edges = {
        {0,1},{1,2},{2,3},{3,0},
        {4,5},{5,6},{6,7},{7,4},
        {0,4},{1,5},{2,6},{3,7},
    };

    std::vector<Vec2i> proj;

    for (auto &c : corners) {
        if (c.z <= 0) continue;
        float u = fx * (c.x / c.z) + cx;
        float v = fy * (c.y / c.z) + cy;
        proj.push_back({ int(u), image.height() - 1 - int(v) });
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