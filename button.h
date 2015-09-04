#ifndef __BUTTON__H__
#define __BUTTON__H__
#include "sprite.h"
#include "director.h"
enum buttonType
{
	RECTANGLE,
	CIRCLE
};

typedef struct{
	buttonType type;
	float radius;
	Size size;
} buttonInfo;


class Button :public Sprite
{
public:
	Button() = delete;
	Button(std::string pic, int layer);
	Button(GLuint _Texture, int layer);      //使用纹理初始化
	~Button();                               //析构函数
	void setType(buttonType type);           //设置类型
	buttonType getType();                    //获得类型
	void setRadius(float r);                 //设置半径
	float getRadius();                       //获得半径
	void setRectSize(Size size);                 //设置触摸矩形的长宽
	Size getRectSize();                          //获得触摸矩形的长宽
	template <class _func_type>              //
	void registerEvent(Object* obj, _func_type func, std::string name)
	{

		callbackName = name;
		Director::getTheInstance()->registerEvent(obj, func);
	}
	void raiseEvent(EventMsg msg);
	bool mouseEventCallBack(mouseEvent _event);//鼠标事件回调函数
private:
	std::string callbackName;
	buttonInfo _buttonInfo;
};
#endif//__BUTTON__H__