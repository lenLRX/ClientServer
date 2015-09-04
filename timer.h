#ifndef __TIMER__H__
#define __TIMER__H__
#include "clock.h"
#include "event.h"
#include <string>

//using namespace std;

class timer
{
public:
	timer();
	~timer();
	void setDelayTime(float t);             //设定延时
	void start();                           //开始计时器
	bool isRuning();                        //返回计时器是否正在运行
	void tick();                            //滴答（经过一段时间）
	void pause();                           //暂停
	void resume();                          //继续
	void registerEvent(std::string name);        //注册事件
	void registerEvent(EventMsg msg);
private:
	bool running;                           //正在运行
	Clock* pauseHandler;                    //定时器暂停补偿时钟
	float compensation;                     //时钟补偿值
	float delayTime;                        //设定的延迟时间
	LONGLONG startTime;                     //起始时间
	double freq;                            //频率
	EventMsg _msg;                          //发出的时间消息
};

#endif