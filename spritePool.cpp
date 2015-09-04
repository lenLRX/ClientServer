#include "stdafx.h"
#include "spritePool.h"
using namespace std;
SpritePool* SpritePool::theInstance = NULL;

SpritePool::SpritePool() :capacity(20), factor(1.2)
{
	for (int i = 0; i < capacity; ++i)
	{
		sprites.push_back(new Sprite(0, 0));
	}
	index = 0;
	end = sprites.size()-1;
}

SpritePool::~SpritePool()
{
	vector<Sprite*>::iterator _end = sprites.end();
	for (vector<Sprite*>::iterator _it = sprites.begin(); _it != _end;)
	{
		delete (*_it);
		_it = sprites.erase(_it);
	}
}

void SpritePool::create()
{
	if (!theInstance)
		theInstance = new SpritePool();
}

SpritePool* SpritePool::getTheInstance()
{
	return theInstance;
}

Sprite* SpritePool::getSprite()
{
	if (index == end)
	{
		expand();
	}
	Sprite* p = sprites[index];
	++index;
	return p;
}

void SpritePool::reset()
{
	index = 0;
}

void SpritePool::expand()
{
	int xcapacity = capacity*factor;
	for (int i = capacity; i < xcapacity; i++)
	{
		sprites.push_back(new Sprite(0, 0));
	}
	capacity = xcapacity;
	end = sprites.size() - 1;
}