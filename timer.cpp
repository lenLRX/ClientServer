#include "stdafx.h"
#include "timer.h"
#include "director.h"
#include <cassert>
using namespace std;
timer::timer() :running(false), delayTime(0), pauseHandler(NULL), compensation(0.0f)
{
	LARGE_INTEGER LARGE;
	QueryPerformanceFrequency(&LARGE);
	freq = (double)LARGE.QuadPart;
}

timer::~timer()
{
	if (pauseHandler)
		delete pauseHandler;
}

void timer::setDelayTime(float t)
{
	delayTime = t;
}

void timer::start()
{
	if (!running)
	{
		assert(delayTime>0);//延迟时间应大于0
		running = true;
		LARGE_INTEGER LARGE;
		QueryPerformanceCounter(&LARGE);
		startTime = LARGE.QuadPart;
	}
}

bool timer::isRuning()
{
	return running;
}

void timer::tick()
{
	if (!pauseHandler)
	{
		assert(running);
		LARGE_INTEGER LARGE;
		QueryPerformanceCounter(&LARGE);
		LONGLONG nowL = LARGE.QuadPart;
		float dt = ((float)(nowL - startTime)) / freq - compensation;
		if (dt >= delayTime)
		{
			running = false;
			Director::getTheInstance()->raiseEvent(_msg);
			
		}
	}
	
}

void timer::pause()
{
	if (!pauseHandler)
	{
		pauseHandler = new Clock();
		pauseHandler->start();
	}
		
}

void timer::resume()
{
	if (pauseHandler)
	{
		compensation += pauseHandler->getTimeSinceStart();
		delete pauseHandler;
		pauseHandler = NULL;
	}
}

void timer::registerEvent(string name)
{
	_msg.name = name;
}

void timer::registerEvent(EventMsg msg)
{
	_msg = msg;
}
