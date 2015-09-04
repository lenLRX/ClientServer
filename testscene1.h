#ifndef __TESTSCENE1__H__
#define __TESTSCENE1__H__
#include "scene.h"
#include "event.h"
#include <stdlib.h>
#include <time.h>
#include <Box2D\Box2D.h>
#include <string>
#include <cstdio>

//using namespace std;
class testscene1 :public Scene,public b2ContactListener
{
public:
	testscene1() = delete;
	testscene1(std::string name);
	~testscene1();
	virtual void init();
	virtual void clean();
	virtual void update(float dt);
	virtual void BeginContact(b2Contact* contact);//继承开始碰撞的虚函数
	void initPhysics();                        //初始化物理模型
	void testCallback(EventMsg msg);
	void addNewSpriteCallback(EventMsg msg);   //点击一下发射一个精灵
	void removeAllSprites(EventMsg msg);       //删除所有精灵
private:
	int followSprite;
	int coin;                                  //击中加分的金币
	b2World* world;
};


#endif//__TESTSCENE1__h__