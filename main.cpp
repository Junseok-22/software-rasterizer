#include "tgaimage.h"

constexpr TGAColor red = {0,0,255,255};

int main() {
    constexpr int width = 100;
    constexpr int height = 100;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    int x1 = 52;
    int y1 = 41;

    framebuffer.set(x1, y1, red);

    framebuffer.write_tga_file("output.tga");
    return 0;
}