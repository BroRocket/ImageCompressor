#include "Image.hpp"
#include "ImageSplitter.hpp"
#include <iostream>
#include <fstream>
#include <stdexcept>

ImageSplitter::ImageSplitter(const Image& img)
    : source(img) {};

//Accessors

//
size_t ImageSplitter::brightest_horrizontal(Image img, size_t min_y = NULL, size_t max_y= NULL) {
    size_t top = 0;
    size_t bottom = img.getWidth();

    if (min_y) {
        top = min_y;
    }
    if (max_y) {
        bottom = max_y;
    }

    int max = 0;
    int loc;
    int sum;
    for (int y = top; y < bottom; y++) {
        sum = img.sumRow(y);
        if (sum > max){
            max = sum;
            loc = y;
        }
    }
    return loc;
};

size_t ImageSplitter::brightest_vertical(Image img, size_t min_x = NULL, size_t max_x= NULL) {

    size_t left = 0;
    size_t right = img.getWidth();

    if (min_x) {
        left = min_x;
    }
    if (max_x) {
        right = max_x;
    }

    int max = 0;
    int loc;
    int sum;
    for (int x = left; x < right; x++) {
        sum = img.sumColumn(x);
        if (sum > max){
            max = sum;
            loc = x;
        }
    }
    return loc;
};