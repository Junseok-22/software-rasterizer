#include "tgaimage.h"
#include <cstdlib>
#include <utility>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

constexpr TGAColor red = {0,0,255,255};

struct Vec3f {float x, y, z;};
struct Vec2i {int x, y;};

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

Vec3f barycentric(Vec2i a, Vec2i b, Vec2i c, Vec2i p) {
    Vec3f vx = {float(c.x - a.x), float(b.x - a.x), float(a.x - p.x)};
    Vec3f vy = {float(c.y - a.y), float(b.y - a.y), float(a.y - p.y)};
    Vec3f vu = {vx.y * vy.z - vx.z * vy.y, vx.z * vy.x - vx.x * vy.z, vx.x * vy.y - vx.y * vy.x};

    if (std::abs(vu.z) < 1) 
        return Vec3f{-1, 1, 1}; // triangle is degenerate, in this case return something with negative coordinates

    else 
        return Vec3f{1.0f - (vu.x + vu.y) / vu.z, vu.y / vu.z, vu.x / vu.z};
}

void triangle(Vec2i a, Vec2i b, Vec2i c, TGAImage &image, const TGAColor &color) {
    int minx = std::min({a.x, b.x, c.x});
    int maxx = std::max({a.x, b.x, c.x});
    int miny = std::min({a.y, b.y, c.y});
    int maxy = std::max({a.y, b.y, c.y});

    minx = std::max(0, minx);                    // don't start left of the image
    maxx = std::min(image.width() - 1, maxx);    // don't go past the right edge
    miny = std::max(0, miny);                    // don't start above the image
    maxy = std::min(image.height() - 1, maxy);   // don't go past the bottom

    for (int x = minx; x <= maxx; x++){
        for (int y = miny; y <= maxy; y++){
            Vec3f bc = barycentric(a, b, c, {x,y});
            if (bc.x < 0 || bc.y < 0 || bc.z < 0) continue; 
            image.set(x, y, color);
        }
    }
}   


int main() {
    constexpr int width = 800;
    constexpr int height = 800;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    std::vector<Vec3f> verts;
    std::vector<std::vector<int>> faces;

    std::ifstream in("obj/african_head.obj");
    std::string linetext;

     while (std::getline(in, linetext)) {
        std::istringstream iss(linetext);
        std::string type;
        iss >> type;
        if (type == "v") {
            Vec3f v;
            iss >> v.x >> v.y >> v.z;
            verts.push_back(v);
        } else if (type == "f") {
            std::vector<int> f;
            std::string group;
            while (iss >> group) {                
                std::istringstream gs(group);
                std::string idx;
                std::getline(gs, idx, '/');     
                f.push_back(std::stoi(idx) - 1); 
            }
            faces.push_back(f);
        }
    }
    
    for (int i = 0; i < faces.size(); i++) {
        std::vector<int> face = faces[i];
        
        Vec3f w0= verts[face[0]];
        Vec3f w1 = verts[face[1]];
        Vec3f w2 = verts[face[2]];

        Vec2i p0 = { int((w0.x + 1.0) * width  / 2.0), int((w0.y + 1.0) * height / 2.0) };
        Vec2i p1 = { int((w1.x + 1.0) * width  / 2.0), int((w1.y + 1.0) * height / 2.0) };
        Vec2i p2 = { int((w2.x + 1.0) * width  / 2.0), int((w2.y + 1.0) * height / 2.0) };

        triangle(p0, p1, p2, framebuffer, red);

    }

    framebuffer.write_tga_file("output.tga");
    return 0;
}