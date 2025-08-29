#include "image.hpp"


int main() {
    Image img(0, 0, 1, "HOWARDPNGBIN");  // specify the signature
    img.read("RECEIPT.bin");
    img.printGray();

    return 0;
}

int old_main() {
    Image img(2, 2, 3);
    img.setPixel(0,0,255,0,0); // red
    img.setPixel(0,1,0,255,0); // green
    img.setPixel(1,0,0,0,255); // blue
    img.setPixel(1,1,255,255,0); // yellow

    img.write("test.bin");

    Image img2;
    img2.read("test.bin");
    img2.printRGB();

    return 0;
}