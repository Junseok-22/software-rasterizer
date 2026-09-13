#include "tgaimage.h"
#include <cstdlib>
#include <utility>

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
    constexpr int width = 100;
    constexpr int height = 100;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    line(13, 20, 80, 40, framebuffer, red);
    line(20, 13, 40, 80, framebuffer, red);
    line(80, 40, 13, 20, framebuffer, red);
    
    framebuffer.write_tga_file("output.tga");
    return 0;
}