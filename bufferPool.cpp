#include "stdafx.h"
#include "bufferPool.h"
#define MSG_MAX_LENTH 1024

using namespace std;

BufferPool* BufferPool::theInstance = NULL;

BufferPool::BufferPool() :capacity(20), factor(1.2)
{
	for (int i = 0; i < capacity; i++)
	{
		buffers.push_back(new bufferList);
	}
	index = 0;
	end = buffers.size() - 1;
}

BufferPool::~BufferPool()
{
	vector<bufferList*>::iterator _end = buffers.end();
	for (vector<bufferList*>::iterator _it = buffers.begin(); _it != _end;)
	{
		delete (*_it);
		_it = buffers.erase(_it);
	}
}

void BufferPool::create()
{
	if (!theInstance)
		theInstance = new BufferPool();
}

BufferPool* BufferPool::getTheInstance()
{
	return theInstance;
}

bufferList* BufferPool::getBuffer()
{
	if (index == end)
	{
		expand();
	}
	bufferList* p = buffers[index];
	++index;
	return p;
}

void BufferPool::expand()
{
	int xcapacity = capacity*factor;
	for (int i = capacity; i < xcapacity; i++)
	{
		buffers.push_back(new bufferList);
	}
	capacity = xcapacity;
	end = buffers.size() - 1;
}

void BufferPool::reset()
{
	index = 0;
}