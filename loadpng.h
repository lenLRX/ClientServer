#ifndef __LOADPNG__H__
#define __LOADPNG__H__
#include "png.h"
#include "datastruct.h"
struct gl_texture_t
{
	GLsizei width;
	GLsizei height;

	GLenum format;
	GLint internalFormat;
	GLuint id;

	GLubyte *texels;
};
/* Texture id for the demo */

void GetPNGtextureInfo(int color_type, struct gl_texture_t *texinfo);
gl_texture_t *ReadPNGFromFile(const char *filename);
GLuint loadPNGTexture(const char *filename);
bool InitTexture(GLuint idx, const char* filename, picType type);
#endif//__LOADPNG__H__
