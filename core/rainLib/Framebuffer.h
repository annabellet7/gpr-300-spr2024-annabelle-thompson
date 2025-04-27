#pragma once

class Framebuffer
{
	unsigned int fbo;
	unsigned int rbo;
	unsigned int textureColorBuffer;

public:
	Framebuffer();

	void init(int width, int height);
	void checkStatus();
	
	unsigned int getFbo();
	unsigned int getColorTexturebuffer();
};