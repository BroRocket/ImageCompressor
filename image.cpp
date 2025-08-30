#include "Image.hpp"
#include <iostream>
#include <fstream>
#include <stdexcept>

// Constructor
Image::Image(size_t h, size_t w, uint8_t c, const std::string& sig)
    : height(h), width(w), channels(c), signature(sig), raster(h * w * c, 0) {}

// Accessors
size_t Image::getWidth() const { return width; }
size_t Image::getHeight() const { return height; }
uint8_t Image::getChannels() const { return channels; }
int Image::getCompression() const { return compression; }

// Read from .bin file
void Image::read(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in) throw std::runtime_error("Cannot open file for reading: " + filename);

    // 12 byte signature
    // 1 byte mode
    // 2 byte heihgt
    // 2 byte width
    // 1 byte compression flag

    std::string file_sig(signature.size(), '\0');
    in.read(&file_sig[0], signature.size());
    if (file_sig != signature)
        throw std::runtime_error("Invalid file signature: expected " + signature + ", got " + file_sig);

    in.read(reinterpret_cast<char*>(&channels), 1);
    if (channels != 1 && channels != 3) 
        throw std::runtime_error("Unsupported number of channels: " + std::to_string(channels));
    // in wrong order and needs to be set right in the numebering. Important

    uint8_t h_bytes[2], w_bytes[2];
    in.read(reinterpret_cast<char*>(h_bytes), 2);
    in.read(reinterpret_cast<char*>(w_bytes), 2);

    height = (h_bytes[0] << 8) | h_bytes[1];
    width  = (w_bytes[0] << 8) | w_bytes[1];

    in.read(reinterpret_cast<char*>(&compression), 1);

    raster.resize(height * width * channels);
    in.read(reinterpret_cast<char*>(raster.data()), raster.size());

    if (!in) throw std::runtime_error("Error reading pixel data from file");
    in.close();
}

// Write to .bin file
void Image::write(const std::string& filename) const {
    std::ofstream out(filename, std::ios::binary);
    if (!out) throw std::runtime_error("Cannot open file for writing: " + filename);

    out.write(signature.c_str(), signature.size());

    // Write mode (channels)
    out.write(reinterpret_cast<const char*>(&channels), 1);

    uint8_t h_bytes[2] = { static_cast<uint8_t>((height >> 8) & 0xFF),
                           static_cast<uint8_t>(height & 0xFF) };
    uint8_t w_bytes[2] = { static_cast<uint8_t>((width >> 8) & 0xFF),
                           static_cast<uint8_t>(width & 0xFF) };

    out.write(reinterpret_cast<char*>(h_bytes), 2);
    out.write(reinterpret_cast<char*>(w_bytes), 2);
    out.write(reinterpret_cast<const char*>(&compression), 1);
    out.write(reinterpret_cast<const char*>(raster.data()), raster.size());

    if (!out) throw std::runtime_error("Error writing pixel data to file");
    out.close();
}

// getPixel
uint8_t Image::getPixel(size_t y, size_t x, uint8_t color_index = 0) const {
    if (y >= height || x >= width || color_index >= channels) 
        throw std::out_of_range("Pixel index out of range");
    return raster[channels * (y * width + x) + color_index];
}

// setPixel
void Image::setPixel(size_t y, size_t x, uint8_t r, uint8_t g, uint8_t b) {
    if (y >= height || x >= width) throw std::out_of_range("Pixel index out of range");
    if (channels == 1) {
        raster[y * width + x] = r; // grayscale
    } else if (channels == 3) {
        size_t idx = 3 * (y * width + x);
        raster[idx + 0] = r;
        raster[idx + 1] = g;
        raster[idx + 2] = b;
    }
}

// printGray
void Image::printGray() const {
    if (channels != 1) throw std::runtime_error("printGray only works for grayscale images");
    for (size_t i = 0; i < 50; ++i) {
        for (size_t j = 0; j < width; ++j) {
            std::cout << static_cast<int>(raster[i * width + j]) << "|";// << "\t";
        }
        std::cout << "\n";
    }
}

// printRGB
void Image::printRGB() const {
    if (channels != 3) throw std::runtime_error("printRGB only works for RGB images");
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
            std::cout   << "(" 
                        << static_cast<int>(raster[3* (i * width + j)]) << ", "
                        << static_cast<int>(raster[3* (i * width + j) + 1]) << ", "
                        << static_cast<int>(raster[3* (i * width + j) + 2]) << ")\t";
        }
        std::cout << "\n";
    }
}

int Image::sumRow(size_t y) const {
    int sum = 0;
    if (channels == 3) {
        for (int x = 0; x < width; x++) {
            sum += getPixel(y, x, 0) + getPixel(y, x, 1); + getPixel(y, x, 2); //Could use the greyscale algrotihm
        }
    } else if (channels == 1) {
        for (int x = 0; x < width; x++) {
            sum += getPixel(y, x, 0);
        }
    } else {
        throw std::runtime_error("Cannot sum column for the image storgae mode: " + channels);
    }
    return sum;
};
    
int Image::sumColumn(size_t x) const {
    int sum = 0;
    if (channels == 3) {
        for (int y = 0; y < height; y++) {
            sum += getPixel(y, x, 0) + getPixel(y, x, 1); + getPixel(y, x, 2); //Could use the greyscale algrotihm
        }
    } else if (channels == 1) {
        for (int y = 0; y < height; y++) {
            sum += getPixel(y, x, 0);
        }
    } else {
        throw std::runtime_error("Cannot sum column for the image storgae mode: " + channels);
    }
    return sum;
};