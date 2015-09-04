#include "stdafx.h"
#include "button.h"
#include "printText.h"
using namespace std;
Button::Button(string pic, int layer) :Sprite(pic, layer)
{

}

Button::Button(GLuint _Texture, int layer) : Sprite(_Texture, layer)
{

}

Button::~Button()
{

}

void Button::setType(buttonType type)
{
	_buttonInfo.type = type;
}

buttonType Button::getType()
{
	return _buttonInfo.type;
}

void Button::setRadius(float r)
{
	_buttonInfo.radius = r;
}

float Button::getRadius()
{
	return _buttonInfo.radius;
}

void Button::setRectSize(Size size)
{
	_buttonInfo.size = size;
}

Size Button::getRectSize()
{
	return _buttonInfo.size;
}

void Button::raiseEvent(EventMsg msg)
{
	Director::getTheInstance()->raiseEvent(msg);
}

bool Button::mouseEventCallBack(mouseEvent _event)
{
	Point pt = _event.point;
	EventMsg msg;
	msg.name = callbackName;
	pt = convertToLocalSpace(pt);
	if (_event.type == DOWN)
	{
		switch (_buttonInfo.type)
		{
		case RECTANGLE:
			if (pt.x > -core.size.width / 2 && pt.x<core.size.width / 2 && pt.y>-core.size.height / 2 && pt.y < core.size.height / 2)
			{
				raiseEvent(msg);
				return true;
			}
			else
				return false;
			break;
		case CIRCLE:
			if (sqrt(pt.x*pt.x + pt.y*pt.y) <= _buttonInfo.radius)
			{
				raiseEvent(msg);
				return true;
			}
			else
				return false;
			break;
		default:
			return false;
			break;
		}
	}
	return false;
}