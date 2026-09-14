#include "tgaimage.h"
#include <cstdlib>
#include <utility>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

constexpr TGAColor red = {0,0,255,255};

struct Vec3f {float x, y, z;};

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
        for (int j = 0; j < face.size(); j++) {
            Vec3f v0 = verts[face[j]];
            Vec3f v1 = verts[face[(j + 1) % 3]];

            int x0 = (v0.x + 1.0) * width  / 2.0;
            int y0 = (v0.y + 1.0) * height / 2.0;
            int x1 = (v1.x + 1.0) * width  / 2.0;
            int y1 = (v1.y + 1.0) * height / 2.0;

            line(x0, y0, x1, y1, framebuffer, red);
        }
    }

    framebuffer.write_tga_file("output.tga");
    return 0;
}