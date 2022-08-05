#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <GL/glew.h>
#include <GL/wglew.h>
#include <map>

#include <memory>

#define INDEX_BUFFER				0  
#define POS_VB						1
//#define NORMAL_VB					2
#define TEXCOORD_VB					2
//#define BONEID_VB					4
#define NUM_VBs						3

#define POSITION_LOCATION			0
#define TEX_COORD_LOCATION			1
//#define NORMAL_LOCATION				2

#define ARRAY_SIZE_IN_ELEMENTS(a)	(sizeof(a)/sizeof(a[0]))
#define ZERO_MEM(a)					memset(a, 0, sizeof(a))


GLuint load_shader(GLenum type, const char* shaderSrc);

bool create_model(GLuint* vao, GLuint* vbo, GLuint* vs, GLuint* fs, GLuint* shader_programme);
void destroy_model(GLuint* vao, GLuint* vbo, GLuint* vs, GLuint* fs, GLuint* shader_programme);