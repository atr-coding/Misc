#include <fstream>
#include <string>
#include <exception>
#include <cmath>

class BMP
{
private:

    struct BMPFILEHEADER
    {
        unsigned char    type         [2] {'B', 'M'};
        unsigned char    size         [4] {0, 0, 0, 0};
        unsigned char    reserved1    [2] {0};
        unsigned char    reserved2    [2] {0};
        unsigned char    offBits      [4] {54, 0, 0, 0};
        unsigned char    info_size    [4] {40, 0, 0, 0};
        unsigned char    width        [4] {0, 0, 0, 0};
        unsigned char    height       [4] {0, 0, 0, 0};
        unsigned char    planes       [2] {0, 0};
        unsigned char    bpp          [2] {24, 0};
        unsigned char    compression  [4] {0, 0, 0, 0};
        unsigned char    sizeImage    [4] {0, 0, 0, 0};
        unsigned char    xPelsPerMeter[4] {0, 0, 0, 0};
        unsigned char    yPelsPerMeter[4] {0, 0, 0, 0};
        unsigned char    clrUsed      [4] {0, 0, 0, 0};
        unsigned char    clrImportant [4] {0, 0, 0, 0};

        BMPFILEHEADER() = default;

        BMPFILEHEADER(unsigned int _width, unsigned int _height, unsigned int _bpp)
        {

            unsigned int _size = _width * _height * (_bpp / 8);
            size[0] = (unsigned char)(_size);
            size[1] = (unsigned char)(_size >> 8);
            size[2] = (unsigned char)(_size >> 16);
            size[3] = (unsigned char)(_size >> 24);

            width[0] = (unsigned char)(_width);
            width[1] = (unsigned char)(_width >> 8);
            width[2] = (unsigned char)(_width >> 16);
            width[3] = (unsigned char)(_width >> 24);

            height[0] = (unsigned char)(_height);
            height[1] = (unsigned char)(_height >> 8);
            height[2] = (unsigned char)(_height >> 16);
            height[3] = (unsigned char)(_height >> 24);

            bpp[0] = _bpp;
        }
    };

    struct Pixel
    {
        unsigned char r {0};
        unsigned char g {0};
        unsigned char b {0};

        Pixel() = default;

        Pixel(unsigned char value)
        {
            r = value;
            g = value;
            b = value;
        }

        Pixel(unsigned char r, unsigned char g, unsigned char b)
        {
            r = r;
            g = g;
            b = b;
        }

        bool operator==(const Pixel& o) const
        {
            return (r == o.r && g == o.g && b == o.b);
        }
    };

public:

    BMP() = default;

    BMP(const std::string& _file_name, unsigned int _width, unsigned int _height, unsigned int _bpp = 24) : file_name(_file_name), width(_width), height(_height), bpp(_bpp)
    {
        if(_file_name == "") {
            throw std::invalid_argument("File name cannot be blank.");
        }

        if(_width == 0 || _height == 0) {
            throw std::invalid_argument("Width and Height cannot be equal to zero.");
        }

        if(_bpp != 8 && _bpp != 16 && _bpp != 24 && _bpp != 32) {
            throw std::invalid_argument("Bits per pixel must be 8, 16, 24, or 32.");
        }

        pixels = new unsigned char[width * height * (bpp / 8)]();
        header = BMPFILEHEADER(width, height, bpp);
    }

    bool setSize(unsigned int _width, unsigned int _height)
    {
        if(_width == width && _height == height) { return false; }
        header = BMPFILEHEADER(_width, _height, bpp);
        delete[] pixels;
        pixels = new unsigned char[_width * _height * (bpp / 8)]();
        width = _width;
        height = _height;
        return true;
    }

    bool write()
    {
        if(pixels == nullptr) {
            throw std::runtime_error("Pixel buffer must be set by calling setSize or the BMP constructor.");
        }

        std::ofstream file(file_name);
        if(file.is_open())
        {
            file.write((char*)(&header), header.offBits[0]);
            file.write((char*)pixels, width * height * (bpp / 8));
            file.close();
            return true;
        }
        return false;
    }

    unsigned char* getPixels()
    {
        return pixels;
    }

    void setPixel(unsigned int x, unsigned int y, unsigned char r, unsigned char g, unsigned char b)
    {
        if(pixels == nullptr) {
            throw std::runtime_error("Pixel buffer must be set by calling setSize or the BMP constructor.");
        }

        if(x >= 0 && y >= 0 && x < width && y < height) {
            pixels[(y * width + x) * 3 + 2] = r;
            pixels[(y * width + x) * 3 + 1] = g;
            pixels[(y * width + x) * 3 + 0] = b;
        }
    }

    Pixel getPixel(unsigned int x, unsigned int y)
    {
        if(pixels == nullptr) {
            throw std::runtime_error("Pixel buffer must be set by calling setSize or the BMP constructor.");
        }

        if(x >= 0 && y >= 0 && x < width && y < height && pixels != nullptr) {
            return Pixel(pixels[(y * width + x) * 3 + 2], pixels[(y * width + x) * 3 + 1], pixels[(y * width + x) * 3 + 0]);
        }
        return Pixel();
    }

    void fill(unsigned char value = 0)
    {
        if(pixels == nullptr) {
            throw std::runtime_error("Pixel buffer must be set by calling setSize or the BMP constructor.");
        }

        for(int i = 0; i < width * height * (bpp / 8); ++i) {
            pixels[i] = value;
        }
    }

    void circle(unsigned int x, unsigned int y, unsigned int radius, unsigned char r, unsigned char g, unsigned char b, bool fill)
    {
        if(pixels == nullptr) {
            throw std::runtime_error("Pixel buffer must be set by calling setSize or the BMP constructor.");
        }

        unsigned int new_x = 0, new_y = 0;
        for(int _y = y - radius; _y <= y + radius; ++_y) {
            for(int _x = x - radius; _x <= x + radius; ++_x) {
                if(_x >= 0 && _y >= 0 && _x < width && _y < height) {
                    float distance = std::sqrt((_x - x) * (_x - x) + (_y - y) * (_y - y));
                    if(fill) {
                        if(distance < radius) {
                            pixels[(_y * width + _x) * 3 + 2] = r;
                            pixels[(_y * width + _x) * 3 + 1] = g;
                            pixels[(_y * width + _x) * 3 + 0] = b;
                        }
                    } else {
                        if(std::abs(distance - (float)radius) < 0.5f) {
                            pixels[(_y * width + _x) * 3 + 2] = r;
                            pixels[(_y * width + _x) * 3 + 1] = g;
                            pixels[(_y * width + _x) * 3 + 0] = b;
                        }
                    }
                }
            }   
        }
    }

    void line(unsigned int x, unsigned int y, unsigned int x2, unsigned int y2, unsigned char r, unsigned char g, unsigned char b)
    {
        if(pixels == nullptr) {
            throw std::runtime_error("Pixel buffer must be set by calling setSize or the BMP constructor.");
        }

        float angle = std::atan2(y2 - y, x2 - x);
        float distance= std::sqrt((x2 - x) * (x2 - x) + (y2 - y) * (y2 - y));
        unsigned int _x = 0;
        unsigned int _y = 0;
        for(int i = 0; i < std::ceil(distance); ++i) {
            _x = cos(angle) * i + x;
            _y = sin(angle) * i + y;
            if(_x >= 0 && _y >= 0 && _x < width && _y < height) {
                pixels[(_y * width + _x) * 3 + 2] = r;
                pixels[(_y * width + _x) * 3 + 1] = g;
                pixels[(_y * width + _x) * 3 + 0] = b;
            }
        }
    }

    void lineAD(unsigned int x, unsigned int y, float angle, unsigned int distance, unsigned char r, unsigned char g, unsigned char b)
    {
        unsigned int _x = 0;
        unsigned int _y = 0;
        for(int i = 0; i < distance; ++i) {
            _x = cos(angle) * i + x;
            _y = sin(angle) * i + y;
            if(_x >= 0 && _y >= 0 && _x < width && _y < height) {
                pixels[(_y * width + _x) * 3 + 2] = r;
                pixels[(_y * width + _x) * 3 + 1] = g;
                pixels[(_y * width + _x) * 3 + 0] = b;
            }
        }
    }

    void square(unsigned int x, unsigned int y, unsigned int _width, unsigned int _height, unsigned char r, unsigned char g, unsigned b, bool fill)
    {
        if(pixels == nullptr) {
            throw std::runtime_error("Pixel buffer must be set by calling setSize or the BMP constructor.");
        }

        unsigned int new_x = 0, new_y = 0;
        for(int _y = y; _y <= y + _height; ++_y) {
            for(int _x = x; _x <= x + _width; ++_x) {
                if(_x >= 0 && _y >= 0 && _x < width && _y < height) {
                    if(fill) {
                        pixels[(_y * width + _x) * 3 + 2] = r;
                        pixels[(_y * width + _x) * 3 + 1] = g;
                        pixels[(_y * width + _x) * 3 + 0] = b;
                    } else {
                        if(_x == x || _x == x + _width || _y == y || _y == y + _height)
                        {
                            pixels[(_y * width + _x) * 3 + 2] = r;
                            pixels[(_y * width + _x) * 3 + 1] = g;
                            pixels[(_y * width + _x) * 3 + 0] = b;
                        }
                    }
                }
            }   
        }
    }

    const unsigned int dataSize() const
    {
        return width * height * (bpp / 8);
    }

    BMP(const BMP& other)
    {
        pixels = new unsigned char[other.dataSize()];
        memcpy(pixels, other.pixels, other.dataSize());
        width = other.width;
        height = other.height;
        bpp = other.bpp;
        file_name = other.file_name;
        header = other.header;
    }

    void operator=(const BMP& other)
    {
        pixels = new unsigned char[other.dataSize()];
        memcpy(pixels, other.pixels, other.dataSize());
        width = other.width;
        height = other.height;
        bpp = other.bpp;
        file_name = other.file_name;
        header = other.header;   
    }

    BMP(BMP&& other)
    {
        pixels = other.pixels;
        other.pixels = nullptr;
        width = other.width;
        height = other.height;
        bpp = other.bpp;
        file_name = other.file_name;
        header = other.header;
    }

    void operator=(BMP&& other)
    {
        pixels = other.pixels;
        other.pixels = nullptr;
        width = other.width;
        height = other.height;
        bpp = other.bpp;
        file_name = other.file_name;
        header = other.header;
    }

    ~BMP()
    {
        delete[] pixels;
    }

private:
    unsigned char* pixels   { nullptr };
    unsigned int width      { 100 };
    unsigned int height     { 100 };
    unsigned short bpp      { 24 };
    std::string file_name   { "_img.bmp" };
    BMPFILEHEADER header;
};