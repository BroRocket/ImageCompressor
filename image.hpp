// image.hpp
#pragma once
#include <vector>
#include <string>
#include <cstdint>

class Image {
private:
    std::vector<uint8_t> raster;
    size_t width;
    size_t height;
    uint8_t channels;
    std::string signature;
    int compression;     // 0 = none, 1 = compressed (you can define later)

public:
    Image(size_t h = 0, size_t w = 0, uint8_t c = 1, const std::string& sig = "HOWARDPNGBIN");

    void read(const std::string& filename);
    void write(const std::string& filename) const;

    uint8_t getPixel(size_t y, size_t x, uint8_t color_index = 0) const;
    void setPixel(size_t y, size_t x, uint8_t r, uint8_t g = 0, uint8_t b = 0);

    void printGray() const;
    void printRGB() const;

    int sumRow(size_t y) const;
    int sumColumn(size_t x) const;

    size_t getWidth() const;
    size_t getHeight() const;
    uint8_t getChannels() const;
    int getCompression() const;
    void setRaster(std::vector<uint8_t>&& newRaster) {
        raster = std::move(newRaster);
    }
};