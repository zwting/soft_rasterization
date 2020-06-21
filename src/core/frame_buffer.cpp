#include "frame_buffer.h"
#include <cstring>
#include <cassert>

FrameBuffer::FrameBuffer(int w, int h, FrameFormatType format)
{
    width = w;
    height = h;
    this->format = format;

    const int total_len = w * h * static_cast<int>(format);
    this->data = new uint8_t[total_len];
    std::memset(this->data, 0, total_len);
}

FrameBuffer::~FrameBuffer()
{
    delete[] this->data;
    this->data = nullptr;
}

FrameBuffer::FrameBuffer(const FrameBuffer& rhl)
{
    this->width = rhl.width;
    this->height = rhl.height;
    this->format = rhl.format;
    this->data = new uint8_t[rhl.size()];
    std::memcpy(data, rhl.data, size());
}

FrameBuffer& FrameBuffer::operator=(const FrameBuffer& rhl)
{
    assert(width == rhl.width && height == rhl.height && format == rhl.format);
    std::memcpy(data, rhl.data, size());
    return (*this);
}

uint8_t* FrameBuffer::operator[](int x)
{
    assert(x >= 0 && x < width);
    return data + x * static_cast<int>(format);
}

void FrameBuffer::clear(const uint8_t* dst)
{
    assert(dst != nullptr);
    uint8_t r, g, b;
    r = dst[2];
    g = dst[1];
    b = dst[0];

    if (r == g && g == b && r == 0)
    {
        std::memset(data, 0, size());
    }
    else
    {
        int step = static_cast<int>(format);
        for (int i = 0; i < size(); i += step)
        {
            std::memcpy(data + i, dst, step);
        }
    }
}

void FrameBuffer::clear(const Color3B& color)
{
    this->clear(color.raw);
}

uint8_t* FrameBuffer::get_data() const
{
    return data;
}

int FrameBuffer::size() const
{
    return width * height * static_cast<int>(format);
}

void FrameBuffer::set_pixel(int x, int y, const uint8_t* dst)
{
    if(x < 0 || x > width || y < 0 || y > height)
        return;
    int ele_size = static_cast<int>(format);
    int idx = (width * y + x) * ele_size;
    std::memcpy(this->data + idx, dst, ele_size);
}

void FrameBuffer::set_pixel(int x, int y, const Color3B& color)
{
    this->set_pixel(x, y, color.raw);
}

