#ifndef __TESTSCENE2__H__
#define __TESTSCENE2__H__
#include "scene.h"
#include "event.h"
#include <stdlib.h>
#include <time.h>
#include <Box2D\Box2D.h>
#include <string>
#include <cstdio>


//using namespace std;
class testscene2 : public Scene
{
public:
	testscene2() = delete;
	testscene2(std::string name);
	~testscene2();
	void init();
	void update(float dt);
	void initPhysics();
	void mouseeventCallBack(EventMsg msg);
private:
	b2World* world;
	int shooterID;
	int bulletID;
};
#endif//__TESTSCENE2__h__