#include "stdafx.h"
#include "testscene2.h"
#include "globalvar.h"
#include "director.h"
#include "button.h"
#include <cmath>
using namespace std;

testscene2::testscene2(string name) :Scene(name), shooterID()
{
	;
}
testscene2::~testscene2()
{
	delete world;
}
void testscene2::init()
{
	initPhysics();
	bulletID = 1;
	Sprite* Background = new Sprite("Helloworld.png", 0);
	Background->setsize(WINDOW_WIDTH, WINDOW_HEIGHT);
	Background->setpos(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
	addchild(Background);
	Sprite* shooter = new Sprite("CloseNormal.png", 2);
	shooter->setsize(50, 50);
	shooter->setpos(WINDOW_WIDTH / 2, 25);
	shooter->setrotation(0);
	shooterID=addchild(shooter);
	for (int i = 0; i != 10; i++)
	{
		Sprite* littlesprite = new Sprite("CloseNormal.png", 1);
		littlesprite->setsize(30, 30);
		littlesprite->setpos(WINDOW_WIDTH / 2 - 150 + 30 * i, WINDOW_HEIGHT / 2);
		addchild(littlesprite);

	}
	Director::getTheInstance()->registerKey(VK_A);
	Director::getTheInstance()->registerKey(VK_D);
	Director::getTheInstance()->registerEvent((this), &testscene2::mouseeventCallBack);
}
void testscene2::update(float dt)
{
	float step = dt;
	int32 velocityIterations = 8;
	int32 positionIterations = 1;
	world->Step(step, velocityIterations, positionIterations);
	
	if (shooterID){
		Sprite* shooter = getChildByID(shooterID);
		Point shoot = shooter->getpos();
		if (_keys[0][VK_A])
		{
			shoot.x -= 5;
		}
		if (_keys[0][VK_D])
		{
			shoot.x += 5;
		}
		shooter->setpos(shoot);
		Point mouse = Director::getTheInstance()->getMousePos();

		shooter->setrotation(atan2(shoot.y-mouse.y, shoot.x-mouse.x) + 90.0 / 180 * M_PI);
	}
	/*
	for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		if (b->GetUserData() != nullptr) {
			Sprite* sprite = (Sprite*)b->GetUserData();
			Point pt;
			pt.x = b->GetPosition().x *	PTM_RATIO;
			pt.y = b->GetPosition().y * PTM_RATIO;
			sprite->setpos(pt);
			float rot = b->GetAngle();
			sprite->setrotation(rot);
		}
		else
		{
			break;
		}
	}
	*/
	
	for (b2Body* b = world->GetBodyList(); b; b->GetNext())
	{
		if (b->GetUserData() != nullptr)
		{
			Sprite* p = (Sprite*)b->GetUserData();
			Point pt;
			pt.x = b->GetPosition().x*PTM_RATIO;
			pt.y = b->GetPosition().y*PTM_RATIO;
			p->setpos(pt);
		}
		else
		{
			break;
		}

	}		
	
}
void testscene2::initPhysics()
{
	b2Vec2 gravity;
	gravity.Set(0.0f, 0.0f);
	world = new b2World(gravity);
	world->SetAllowSleeping(true);
	world->SetContinuousPhysics(true);
	b2BodyDef groundBodyDef;

	groundBodyDef.position.Set(0, 0);

	b2Body* groundBody = world->CreateBody(&groundBodyDef);

	b2EdgeShape groundBox;
	groundBox.Set(b2Vec2(0, 0), b2Vec2(WINDOW_WIDTH / PTM_RATIO, 0));

	groundBody->CreateFixture(&groundBox, 0);

	groundBox.Set(b2Vec2(0, WINDOW_HEIGHT / PTM_RATIO), b2Vec2(WINDOW_WIDTH / PTM_RATIO, WINDOW_HEIGHT / PTM_RATIO));

	groundBody->CreateFixture(&groundBox, 0);

	groundBox.Set(b2Vec2(0, WINDOW_HEIGHT / PTM_RATIO), b2Vec2(0, 0));

	groundBody->CreateFixture(&groundBox, 0);

	groundBox.Set(b2Vec2(WINDOW_WIDTH / PTM_RATIO, WINDOW_HEIGHT / PTM_RATIO), b2Vec2(WINDOW_WIDTH / PTM_RATIO, 0));

	groundBody->CreateFixture(&groundBox, 0);
}
void testscene2::mouseeventCallBack(EventMsg msg)
{
		if (bulletID != 0&&msg._event.type == DOWN&&shooterID)
		{
			bulletID = 0;
			Sprite* bullet = new Sprite("CloseNormal.png", 2);
			addchild(bullet);
			Sprite* shooter = getChildByID(shooterID);
			Point shoot = shooter->getpos();
			Point mous = msg._event.point;
			bullet->setsize(20, 20);
			bullet->setpos(shoot);
			float dir = atan2(mous.x - shoot.x, mous.y - shoot.y);
			b2BodyDef BodyDef;
			BodyDef.bullet = true;
			BodyDef.type = b2_dynamicBody;
			BodyDef.position.Set(shoot.x / PTM_RATIO, shoot.y / PTM_RATIO);
			b2Body* body = world->CreateBody(&BodyDef);
			body->SetUserData(bullet);
			b2CircleShape circle;
			circle.m_radius = 0.5f;
			circle.m_p.Set(0, 0);
			// 夹具定义
			b2FixtureDef fixtureDef;
			fixtureDef.restitution = 1.0f;
			//设置夹具的形状
			fixtureDef.shape = &circle;
			//设置密度
			fixtureDef.density = 1.0f;
			//设置摩擦系数
			fixtureDef.friction = 0.0f;
			//使用夹具固定形状到物体上	
			body->CreateFixture(&fixtureDef);
			b2MassData massdata;
			body->GetMassData(&massdata);
			float Velocity = 20;
			body->SetLinearVelocity(b2Vec2(Velocity*cos(dir), Velocity*sin(dir)));
			body->SetAngularVelocity(0.25);
			
		}
}