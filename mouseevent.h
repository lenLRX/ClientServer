#ifndef __MOUSEEVENT__H__
#define __MOUSEEVENT__H__
#include "stdafx.h"
#include "datastruct.h"
enum mouseEventType
{
	DOWN,//按下鼠标
	UP,//抬起鼠标
	MOVE,//移动鼠标
	DRAG//拖动鼠标
};

typedef struct
{
	mouseEventType type;
	Point point;
}mouseEvent;


#endif//__MOUSEEVENT__H__