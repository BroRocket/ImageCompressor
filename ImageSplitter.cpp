#include "Image.hpp"
#include "ImageSplitter.hpp"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <numeric>

ImageSplitter::ImageSplitter(const Image& img)
    : source(img) {};

//Accessors

//

Image crop(const Image& img, size_t x1, size_t x2, size_t y1, size_t y2) {
    
    size_t height = img.getHeight();
    size_t width = img.getWidth();

    if (x1 < 0 && x2 >= width && x2 < x1) {
         throw std::runtime_error("X values x1: " + std::to_string(x1) + " and x2: "  + std::to_string(x1) + " are not acceptable based on image width: " +  std::to_string(width) + " or x2 < x1");
    };

    if (y1 < 0 && y2 >= height && y2 < y1) {
         throw std::runtime_error("X values x1: " + std::to_string(x1) + " and x2: "  + std::to_string(x1) + " are not acceptable based on image height:" +  std::to_string(height) + " or y2 < y1");
    };


    Image imgNew(y2 - y1 + 1, x2 - x1 + 1, img.getChannels()); // check logic
    std::vector<uint8_t> rasterNew(imgNew.getHeight()*imgNew.getWidth()*imgNew.getChannels());

    // finsihs logic by adding actual raster splitting

};

std::array<Image, 2> ImageSplitter::split_vertically(const Image& img, size_t x_split) const{

    size_t height = img.getHeight();
    size_t width = img.getWidth();

    if (x_split >= width){
        throw std::runtime_error("Cannot split photo at location greater than or equal to the width");
    }

    size_t widthLeft = x_split;
    size_t widthRight = width - x_split;

    Image imgLeft(height, widthLeft, img.getChannels());
    Image imgRight(height, widthRight, img.getChannels());

    std::vector<uint8_t> rasterLeft(height*widthLeft*img.getChannels());
    std::vector<uint8_t> rasterRight(height*widthRight*img.getChannels());
    

    if (img.getChannels() == 1){
        for (size_t x = 0; x < width; x++){
            for (size_t y = 0; y < height; y++){
                if (x < x_split){
                    rasterLeft[img.getChannels() * (y * widthLeft + x)] = img.getPixel(y, x);
                } else {
                    rasterRight[img.getChannels() * (y * widthRight + (x - x_split))] = img.getPixel(y, x);
                };
            };
        };
    } else if (img.getChannels() == 3) {
        for (size_t x = 0; x < width; x++){
            for (size_t y = 0; y < height; y++){
                if (x < x_split){
                    rasterLeft[img.getChannels() * (y * widthLeft + x)] = img.getPixel(y, x, 0);
                    rasterLeft[img.getChannels() * (y * widthLeft + x) + 1] = img.getPixel(y, x, 1);
                    rasterLeft[img.getChannels() * (y * widthLeft + x) + 2] = img.getPixel(y, x, 2);
                } else {
                    rasterRight[img.getChannels() * (y * widthRight + (x - x_split))] = img.getPixel(y, x, 0);
                    rasterRight[img.getChannels() * (y * widthRight + (x - x_split)) + 1] = img.getPixel(y, x, 1);
                    rasterRight[img.getChannels() * (y * widthRight + (x - x_split)) + 2] = img.getPixel(y, x, 2);
                };
            };
        };
    }   else {
        throw std::runtime_error("Cannot split image with" + std::to_string(img.getChannels()) + " channels");
    };

    // get rasters and set them to each image
    imgLeft.setRaster(std::move(rasterLeft));
    imgRight.setRaster(std::move(rasterRight));

    return {imgLeft, imgRight};

};

size_t ImageSplitter::column_projection(const Image& img) {
    size_t width = img.getWidth();
    size_t height = img.getHeight();

    // Step 1: Compute column sums (inverted for "ink" intensity)
    std::vector<int> colSums(width, 0);
    for (size_t x = 0; x < width; x++) {
        colSums[x] = (255 * img.getHeight()) - img.sumColumn(x);
    }

    // Step 2: Smooth with moving average (optional)
    std::vector<int> smooth(width, 0);
    int window = 5;
    for (int x = 0; x < width; ++x) {
        int start = std::max(0, x - window);
        int end = std::min(static_cast<int>(width - 1), x + window);
        smooth[x] = std::accumulate(colSums.begin() + start,
                                    colSums.begin() + end + 1, 0);
    }

    // Step 3: Find minimum point around middle of receipt
    int mid = width / 2;
    int bestSplit = mid;
    int minVal = smooth[mid];
    for (int x = mid - width/4; x <= mid + width/4; ++x) {
        if (x >= 0 && x < width && smooth[x] < minVal) {
            minVal = smooth[x];
            bestSplit = x;
        };
    };
    
    return static_cast<size_t>(bestSplit);
};

size_t ImageSplitter::brightest_horrizontal(const Image& img, size_t min_y, size_t max_y) {
    size_t top = 0;
    size_t bottom = img.getWidth();

    if (min_y != 0) {
        top = min_y;
    }
    if (max_y != 0) {
        bottom = max_y;
    }

    int maxSum = 0;
    int loc = 0;
    int sum;
    for (size_t y = top; y < bottom; y++) {
        sum = img.sumRow(y);
        if (sum > maxSum){
            maxSum = sum;
            loc = y;
        }
    }
    return loc;
};

size_t ImageSplitter::brightest_vertical(const Image& img, size_t min_x, size_t max_x) {

    size_t left = 0;
    size_t right = img.getWidth();

    if (min_x != 0) {
        left = min_x;
    }
    if (max_x != 0) {
        right = max_x;
    }

    int maxSum = 0;
    int loc = 0;
    int sum;
    for (size_t x = left; x < right; x++) {
        sum = img.sumColumn(x);
        if (sum > maxSum){
            maxSum = sum;
            loc = x;
        }
    }
    return loc;
};