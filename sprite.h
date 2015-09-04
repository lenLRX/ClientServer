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

//������
class Sprite:public Object
{
public:
	Sprite() = delete;                //����Ĭ�Ϲ��캯��
	Sprite(std::string pic, int layer);
	Sprite(GLuint _Texture, int layer);//ʹ�������ʼ��
	~Sprite();                        //��������
	inline sprite_Lite getCore()            //��÷������紫������ݽṹ
	{
		return core;
	}
	inline void setCore(sprite_Lite _core)  //�������þ�����Ϣ
	{
		core = _core;
	}
	inline int getlayer() const             //��ȡ��ͼ���ȼ�
	{
		return core.layer;
	}
	inline void setlayer(int layer)         //���û�ͼ���ȼ�
	{
		core.layer = layer;
	}
	inline Point getpos()                   //��þ���λ��
	{
		return core.pos;
	}
	inline void setpos(Point pos)           //���þ���λ��
	{
		core.pos = pos;
	}
	inline void setpos(float x, float y)    //���þ���λ��
	{
		core.pos.x = x;
		core.pos.y = y;
	}
	inline Point getAnchorPoint()           //��þ���ê��λ��
	{
		return core.anchorPoint;
	}
	inline void setAnchorPoint(Point anchorPoint)//����ê��λ��
	{
		core.anchorPoint = anchorPoint;
	}
	inline void setAnchorPoint(float x, float y)//����ê��λ��
	{
		core.anchorPoint.x = x;
		core.anchorPoint.y = y;
	}
	inline Size getsize()                   //��þ����С
	{
		return core.size;
	}
	inline void setsize(Size size)          //���þ����С
	{
		core.size = size;
	}
	inline void setsize(float _width, float _height)   //���þ����С
	{
		core.size.width = _width;
		core.size.height = _height;
	}
	inline void setrotation(float rot)      //������ת�Ƕ�
	{
		core.rotation = rot;
	}
	inline float getrotation()              //�����ת�Ƕ�
	{
		return core.rotation;
	}
	inline void setflipX(bool b)            //������X�ᷭת
	{
		core.flipX = b;
	}
	inline bool getflipX()                  //�����X�ᷭת
	{
		return core.flipX;
	}
	inline void setflipY(bool b)            //������Y�ᷭת
	{
		core.flipY = b;
	}
	inline bool getflipY()                  //�����Y�ᷭת
	{
		return core.flipY;
	}
	inline void setTex(GLuint t)            //��������ID
	{
		core.tex = t;
	}
	inline GLuint getTex()                  //�������ID
	{
		return core.tex;
	}
	inline void setid(int id)               //����id
	{
		core.id = id;
	}
	inline int getid()                      //���id
	{
		return core.id;
	}
	inline void setTimeStamp(float t)       //����ʱ���
	{
		timeStamp = t;
	}
	inline float getTimeStamp()             //���ʱ���
	{
		return timeStamp;
	}
	inline void setUserData(void* data)     //�����ر���û����ݣ����������Լ�����ʲô���͵�ָ�룩
	{
		_userData = data;
	}
	inline void* getUserData()              //����û����ݣ�������������ʲô���͵�ָ�룩
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
	Point convertToLocalSpace(Point pt);//�����ȫ������ϵת������������ϵ
	virtual bool mouseEventCallBack(mouseEvent _event);//����¼��ص�����
	//bool operator< (const Sprite& cp);//����<�Ա�����
protected:
	sprite_Lite core;                 
	//int _layer;                       //��ͼ���ȼ�
	//Point _pos;                       //����λ��
	//Point _anchorPoint;               //����ê��λ��
	//Size _size;                       //�����С
	//GLuint Tex;                       //����ID
	//float rotation;                   //��ת�Ƕȣ�degree��
	//bool flipX;                       //X�ᷭת
	//bool flipY;                       //Y�ᷭת
	//int _id;                          //���ÿ�������id
	float _tag;                          //�������
	float timeStamp;                  //ʱ���
	void* _userData;                  //�û�����
};
#endif//__SPRITE__H__