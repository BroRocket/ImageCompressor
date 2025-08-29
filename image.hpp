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

public:
    Image(size_t h = 0, size_t w = 0, uint8_t c = 3, const std::string& sig = "HOWARDPNGBIN");

    void read(const std::string& filename);
    void write(const std::string& filename) const;

    uint8_t getPixel(size_t y, size_t x, uint8_t channel = 0) const;
    void setPixel(size_t y, size_t x, uint8_t r, uint8_t g = 0, uint8_t b = 0);

    void printGray() const;
    void printRGB() const;

    size_t getWidth() const;
    size_t getHeight() const;
    uint8_t getChannels() const;
};