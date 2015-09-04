#ifndef __SPRITE__H__
#define __SPRITE__H__
#include <gl\gl.h> 
#include <gl\glu.h> 
#include <string>
#include "datastruct.h"
#include "loadpng.h"
#include "mouseevent.h"
#include "event.h"

//using namespace std;

//精灵类
class Sprite:public Object
{
public:
	Sprite() = delete;                //禁用默认构造函数
	Sprite(std::string pic, int layer);
	Sprite(GLuint _Texture, int layer);//使用纹理初始化
	~Sprite();                        //析构函数
	inline sprite_Lite getCore()            //获得方便网络传输的数据结构
	{
		return core;
	}
	inline void setCore(sprite_Lite _core)  //快速设置精灵信息
	{
		core = _core;
	}
	inline int getlayer() const             //获取绘图优先级
	{
		return core.layer;
	}
	inline void setlayer(int layer)         //设置绘图优先级
	{
		core.layer = layer;
	}
	inline Point getpos()                   //获得精灵位置
	{
		return core.pos;
	}
	inline void setpos(Point pos)           //设置精灵位置
	{
		core.pos = pos;
	}
	inline void setpos(float x, float y)    //设置精灵位置
	{
		core.pos.x = x;
		core.pos.y = y;
	}
	inline Point getAnchorPoint()           //获得精灵锚点位置
	{
		return core.anchorPoint;
	}
	inline void setAnchorPoint(Point anchorPoint)//设置锚点位置
	{
		core.anchorPoint = anchorPoint;
	}
	inline void setAnchorPoint(float x, float y)//设置锚点位置
	{
		core.anchorPoint.x = x;
		core.anchorPoint.y = y;
	}
	inline Size getsize()                   //获得精灵大小
	{
		return core.size;
	}
	inline void setsize(Size size)          //设置精灵大小
	{
		core.size = size;
	}
	inline void setsize(float _width, float _height)   //设置精灵大小
	{
		core.size.width = _width;
		core.size.height = _height;
	}
	inline void setrotation(float rot)      //设置旋转角度
	{
		core.rotation = rot;
	}
	inline float getrotation()              //获得旋转角度
	{
		return core.rotation;
	}
	inline void setflipX(bool b)            //设置绕X轴翻转
	{
		core.flipX = b;
	}
	inline bool getflipX()                  //获得绕X轴翻转
	{
		return core.flipX;
	}
	inline void setflipY(bool b)            //设置绕Y轴翻转
	{
		core.flipY = b;
	}
	inline bool getflipY()                  //获得绕Y轴翻转
	{
		return core.flipY;
	}
	inline void setTex(GLuint t)            //设置纹理ID
	{
		core.tex = t;
	}
	inline GLuint getTex()                  //获得纹理ID
	{
		return core.tex;
	}
	inline void setid(int id)               //设置id
	{
		core.id = id;
	}
	inline int getid()                      //获得id
	{
		return core.id;
	}
	inline void setTimeStamp(float t)       //设置时间戳
	{
		timeStamp = t;
	}
	inline float getTimeStamp()             //获得时间戳
	{
		return timeStamp;
	}
	inline void setUserData(void* data)     //设置特别的用户数据（你必须清楚自己附了什么类型的指针）
	{
		_userData = data;
	}
	inline void* getUserData()              //获得用户数据（你必须清楚这是什么类型的指针）
	{
		return _userData;
	}
	inline void setTag(float tag)
	{
		_tag = tag;
	}
	inline float getTag()
	{
		return _tag;
	}
	Point convertToLocalSpace(Point pt);//将点从全局坐标系转换到精灵坐标系
	virtual bool mouseEventCallBack(mouseEvent _event);//鼠标事件回调函数
	//bool operator< (const Sprite& cp);//重载<以便排序
protected:
	sprite_Lite core;                 
	//int _layer;                       //绘图优先级
	//Point _pos;                       //精灵位置
	//Point _anchorPoint;               //精灵锚点位置
	//Size _size;                       //精灵大小
	//GLuint Tex;                       //纹理ID
	//float rotation;                   //旋转角度（degree）
	//bool flipX;                       //X轴翻转
	//bool flipY;                       //Y轴翻转
	//int _id;                          //标记每个精灵的id
	float _tag;                          //标记数据
	float timeStamp;                  //时间戳
	void* _userData;                  //用户数据
};
#endif//__SPRITE__H__