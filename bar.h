#ifndef __BAR__H__
#define __BAR__H__
#include "event.h"
#include "scene.h"
#include "sprite.h"
#include <string>

//using namespace std;

class Bar :public Object
{
public:
	Bar() = delete;
	Bar(Scene* theScene, std::string background, std::string foreground, std::string CallbackName, int layer);
	~Bar();
	void setSize(Size size);
	void setSize(float width, float height);
	Size getSize();
	void setPos(Point pos);
	void setPos(float x, float y);
	Point getPos();
	void onValueChange(EventMsg msg);
	void updateByValue(float _value);
protected:
	Scene* _theScene;
	Sprite* _background;
	int backgroundID;
	Sprite* _foreground;
	int foregroundID;
	std::string Callback;
	Point _pos;
	Size _size;
	int _layer;
	float value;
};

#endif//__BAR__H__