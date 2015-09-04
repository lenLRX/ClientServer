#include "stdafx.h"
#include "bar.h"
#include "director.h"
using namespace std;
Bar::Bar(Scene* theScene, string background, string foreground, string CallbackName, int layer) \
:_theScene(theScene), Callback(CallbackName), value(1.0f), _layer(layer)
{
	_background = new Sprite(background, _layer-1);
	_foreground = new Sprite(foreground, _layer);
	_background->setAnchorPoint(0, 0.5);
	_foreground->setAnchorPoint(0, 0.5);
	backgroundID = _theScene->addchild(_background);
	foregroundID = _theScene->addchild(_foreground);
	Director::getTheInstance()->registerEvent(this, &Bar::onValueChange);
	updateByValue(1.0f);
}

Bar::~Bar()
{

}
void Bar::setSize(Size size)
{
	_size = size;
}

void Bar::setSize(float width, float height)
{
	_size.width = width;
	_size.height = height;
}

Size Bar::getSize()
{
	return _size;
}

void Bar::setPos(Point pos)
{
	_pos = pos;
}

void Bar::setPos(float x, float y)
{
	_pos.x = x;
	_pos.y = y;
}

Point Bar::getPos()
{
	return _pos;
}

void Bar::onValueChange(EventMsg msg)
{
	if (msg.name == Callback)
	{
		updateByValue(msg.param);
	}
}

void Bar::updateByValue(float _value)
{
	value = _value;
	_background->setsize(_size);
	_background->setpos(_pos);
	_foreground->setsize(_size.width*value, _size.height);
	_foreground->setpos(_pos);
}