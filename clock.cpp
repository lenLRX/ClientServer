#include "stdafx.h"
#include "clock.h"
#include <cassert>

Clock::Clock() :pauseHandler(NULL), compensation(0.0f)
{
	LARGE_INTEGER LARGE;
	QueryPerformanceFrequency(&LARGE);
	freq = (double)LARGE.QuadPart;
}

Clock::~Clock()
{
	if (pauseHandler)
		delete pauseHandler;
}

void Clock::start()
{
	if (!running)
	{
		running = true;
		LARGE_INTEGER LARGE;
		QueryPerformanceCounter(&LARGE);
		startTime = LARGE.QuadPart;
	}
}

void Clock::pause()
{
	if (!pauseHandler)
	{
		pauseHandler = new Clock();
		pauseHandler->start();
	}

}

void Clock::resume()
{
	if (pauseHandler)
	{
		compensation += pauseHandler->getTimeSinceStart();
		delete pauseHandler;
		pauseHandler = NULL;
	}
}

bool Clock::isRuning()
{
	return running;
}

float Clock::getTimeSinceStart()
{
	assert(running);
	LARGE_INTEGER LARGE;
	QueryPerformanceCounter(&LARGE);
	LONGLONG nowL = LARGE.QuadPart;
	if (!pauseHandler)
	{
		return ((float)(nowL - startTime)) / freq-compensation;
	}
	else
	{
		return ((float)(nowL - startTime)) / freq - compensation - pauseHandler->getTimeSinceStart();
	}
}