#include <iostream>

class FrameBuffer {

private:
	unsigned int frame_buffer;
	unsigned int color_buffer;

public:
	FrameBuffer(int screen_w, int screen_h);

	unsigned int get_buffer();

	void bind();

	void unbind();
};