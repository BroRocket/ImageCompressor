// image_splitter.hpp
#pragma once
#include "image.hpp"
#include <vector>
#include <array>

class ImageSplitter {
public:
    // Constructor takes a reference to an Image
    ImageSplitter(const Image& img);

    // Extract sub-image from rectangle
    //Image crop(Image img, size_t x, size_t y, size_t w, size_t h) const;
    std::array<Image, 2> split_vertically(const Image& img, size_t x_split) const;
    std::array<Image, 2> split_horizontally(Image img, size_t y) const;
    Image crop(const Image& img, size_t x1, size_t x2, size_t y1, size_t y2);

    // Split into grid of rows x cols pieces
    //std::vector<Image> splitGrid(Image img, size_t rows, size_t cols) const;

    //Find brightest/most white horizontal line
    size_t brightest_horrizontal(const Image& img, size_t min_y = 0, size_t max_y= 0);
    size_t brightest_vertical(const Image& img, size_t min_x = 0, size_t max_x = 0);
    //find brightest/most white vertical line

    //find point to split at
    size_t column_projection(const Image& img);

private:
    const Image& source;  // original image reference

   


    // use gradients of lines summed and compare the higehst ones will mark changes
};