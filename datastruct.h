#ifndef __DATASTRUCT__H__
#define __DATASTRUCT__H__
#include <string>
#include "stdafx.h"
//using namespace std;

typedef struct
{
	float x;
	float y;
}Point;


typedef struct
{
	float height;
	float width;
}Size;

typedef struct
{
	int RefCount;
	GLuint Tex;
	std::string name;
}Texture;

enum picType
{
	PNG
};

typedef struct
{
	GLuint tex;
	Point pos;
	Point anchorPoint;
	Size size;
	float rotation;
	bool flipX;
	bool flipY;
	int layer;
	int id;
}sprite_Lite;//ÓÃÓÚsocket´«Êä

typedef struct
{
	ALuint buf;
	float volume;
	bool loop;
	int id;
}music_Lite;

typedef struct
{
	char str[128];
	Point pt;
}Text;
#endif//__DATASTRUCT__H__