// image_splitter.hpp
#pragma once
#include "image.hpp"
#include <vector>

class ImageSplitter {
public:
    // Constructor takes a reference to an Image
    ImageSplitter(const Image& img);

    // Extract sub-image from rectangle
    Image crop(Image img, size_t x, size_t y, size_t w, size_t h) const;
    Image split_vertically(Image img, size_t y) const;
    Image split_horizontally(Image img, size_t x) const;

    // Split into grid of rows x cols pieces
    std::vector<Image> splitGrid(Image img, size_t rows, size_t cols) const;

    //Find brightest/most white horizontal line
    size_t brightest_horrizontal(Image img, size_t min_y = NULL, size_t max_y= NULL);
    size_t brightest_vertical(Image img, size_t min_x = NULL, size_t max_x= NULL);
    //find brightest/most white vertical line

private:
    const Image& source;  // original image reference

   


    // use gradients of lines summed and compare the higehst ones will mark changes
};