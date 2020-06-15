#ifndef _FRAME_BUFFER_H_
#define _FRAME_BUFFER_H_

#include <cstdint>
#include "geometry/geometry.h"

class FrameBuffer
{
public:
	enum class FrameFormatType { R8G8B8A8_32=4, R8G8B8_24=3, Bit8 = 1, };
private:
	uint8_t* data;
	int width;
	int height;
	FrameFormatType format;

public:
	FrameBuffer(int w, int h, FrameFormatType format = FrameFormatType::R8G8B8_24);
	~FrameBuffer();
	FrameBuffer(const FrameBuffer& rhl);
	FrameBuffer& operator=(const FrameBuffer& rhl);

	bool operator==(const FrameBuffer& rhs)const { return format == rhs.format && width == rhs.width && height == rhs.height; }
	bool is_size_equal(const FrameBuffer& rhs) const { return width == rhs.width && height == rhs.height; }
	uint8_t* operator[](int x);
	void clear(const uint8_t* dst);
	void clear(const Color3B& color);
	uint8_t* get_data()const;
	int size() const;
	void set_pixel(int x, int y, const uint8_t* dst);
	void set_pixel(int x, int y, const Color3B& color);
	int get_width()const { return width; }
	int get_height() const { return height; }
	int get_format()const { return static_cast<int>(format); }
};


#endif // _FRAME_BUFFER_H_

