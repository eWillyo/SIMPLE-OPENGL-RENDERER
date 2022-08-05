#pragma once

#include <GL/glew.h>
#include <GL/wglew.h>
#include <FreeImage.h>

extern int gl_max_texture_size;
extern int gl_max_texture_max_anisotropy_ext;

FIBITMAP* load_bitmap(char* path_to_image, int& width, int& height, int& bpp);
GLuint load_2D_texture(const char* path_to_image);
void unload_2D_texture(GLuint& texture);