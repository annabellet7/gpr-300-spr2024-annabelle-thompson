#pragma once

class Framebuffer
{
	unsigned int fbo;
	unsigned int rbo;
	unsigned int textureColorBuffer;

public:
	Framebuffer();

	void init(int width, int height);
	void deleteBuffer();
	bool checkStatus();
	
	unsigned int getFbo();
	unsigned int getColorTexturebuffer();
};