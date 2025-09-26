#include "Framebuffer.h"
#include "../ew/external/glad.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

Framebuffer::Framebuffer()
{
	fbo = 0;
	rbo = 0;
	textureColorBuffer = 0;
}

void Framebuffer::init(int width, int height)
{
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);

	glGenTextures(1, &textureColorBuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
}

void Framebuffer::deleteBuffer()
{
	glDeleteFramebuffers(1, &fbo);
}

bool Framebuffer::checkStatus()
{
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n");
		return false;
	}
	return true;
}

unsigned int Framebuffer::getFbo()
{
	return fbo;
}

unsigned int Framebuffer::getColorTexturebuffer()
{
	return textureColorBuffer;
}
