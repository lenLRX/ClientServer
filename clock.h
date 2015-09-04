#ifndef __CLOCK__H__
#define __CLOCK__H__
#include "stdafx.h"
#include <string>

//using namespace std;

class Clock
{
public:
	Clock();
	~Clock();
	void start();                           //开始计时器
	bool isRuning();                        //返回计时器是否正在运行
	void pause();                           //暂停
	void resume();                          //继续
	float getTimeSinceStart();              //返回经过了多少时间
private:
	bool running;                           //正在运行
	Clock* pauseHandler;                    //时钟暂停补偿时钟
	float compensation;                     //时钟补偿值
	LONGLONG startTime;                     //起始时间
	double freq;                            //频率
};
#endif