#include "stdafx.h"
#include "testscene1.h"
#include "globalvar.h"
#include "director.h"
#include "button.h"
#include <cmath>
#include "printText.h"
using namespace std;
testscene1::testscene1(string name) :Scene(name), followSprite(0)
{

}

testscene1::~testscene1()
{
	delete world;
}

void testscene1::init()
{
	initPhysics();
	srand(time(NULL));                              //初始化随机数发生器
	Sprite* S1 = new Sprite("HelloWorld.png", 0);   //创建背景精灵
	Sprite* S2 = new Sprite("CloseNormal.png", 0);  //创建发射者
	Button* btn = new Button("HelloWorld.png", 0);  //创建按钮
	Sprite* coinSprite = new Sprite("core2.png", 0);//金币精灵
	coinSprite->setsize(50, 50);
	coinSprite->setpos(200, 200);


	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	bodyDef.position.Set(200 / PTM_RATIO, 200 / PTM_RATIO);
	b2Body *body = world->CreateBody(&bodyDef);
	body->SetUserData(coinSprite);
	bodyDef.bullet = false;
	
	b2CircleShape circle;

	circle.m_p.Set(0.0f, 0.0f);

	circle.m_radius = 22.0f / PTM_RATIO;



	// 夹具定义
	b2FixtureDef fixtureDef;
	fixtureDef.restitution = 0.8f;
	//设置夹具的形状
	fixtureDef.shape = &circle;
	//设置密度
	fixtureDef.density = 1.0f;
	//设置摩擦系数
	fixtureDef.friction = 0.0f;
	fixtureDef.isSensor=true;
	//使用夹具固定形状到物体上	
	body->CreateFixture(&fixtureDef);
	
	world->SetContactListener(this);
	
	Size btnSize;                                   //设置按钮尺寸
	btnSize.height = 100;
	btnSize.width = 100;
	btn->setsize(btnSize);
	btn->setRectSize(btnSize);
	btn->setType(RECTANGLE);                        //设置按钮类型（矩形）
	btn->setpos(400, 400);                          //设置按钮位置
	btn->setrotation(-0.8*M_PI);                    //设置旋转(逆时针为正)
	S1->setflipY(true);                             //设置左右翻转
	S1->setsize(WINDOW_HEIGHT, WINDOW_WIDTH);
	S1->setpos(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
	S2->setsize(100, 100);
	S2->setpos(WINDOW_WIDTH / 2, WINDOW_HEIGHT/2);
	S2->setrotation(0);
	S2->setflipX(true);
	//addchild(S1);                                   //将背景加入到场景中使其显示出来
	followSprite=addchild(S2);                      //保留ID
	addchild(btn);
	coin=addchild(coinSprite);
	registerMouseEvent(S2);
	registerMouseEvent(btn);                          //注册鼠标事件
	Director::getTheInstance()->registerKey(VK_A);    //注册键盘事件
	Director::getTheInstance()->registerKey(VK_S);
	Director::getTheInstance()->registerKey(VK_D);
	Director::getTheInstance()->registerKey(VK_W);
	Director::getTheInstance()->registerKey(VK_R);
	//Director::getTheInstance()->registerMouseEvent((this), &testscene1::testCallback);
	
	Director::getTheInstance()->registerEvent((this), &testscene1::addNewSpriteCallback);
	btn->registerEvent(this, &testscene1::removeAllSprites, "remove");
}

void testscene1::clean()
{
	//第一步：删除精灵
	if (_sprites.size())
	{
		vector<Sprite*>::iterator it = _sprites.begin();
		while (it != _sprites.end())
		{
			delete (*it);
			it = _sprites.erase(it);
		}
	}

	if (_keyBoardEventList.size())
	{
		vector<Sprite*>::iterator it = _keyBoardEventList.begin();
		while (it != _keyBoardEventList.end())
		{
			it = _keyBoardEventList.erase(it);
		}
	}
	if (_mouseEventList.size())
	{
		vector<Sprite*>::iterator it = _mouseEventList.begin();
		while (it != _mouseEventList.end())
		{
			it = _mouseEventList.erase(it);
		}
	}
	//第二步，删除世界
	delete world;
}


void testscene1::update(float dt)
{
	if (followSprite)
	{
		Sprite* c=getChildByID(followSprite);
		///////////////////
		//跟随鼠标
		/*
		POINT MOUSE = Director::getTheInstance()->getMousePos();
		Size s = c->getsize();
		c->setpos(MOUSE.x-s.width/2, MOUSE.y-s.height/2);
		*/
		Point pos=c->getpos();
		if (_keys[0][VK_W])
		{
			pos.y += 5;
		}
		if (_keys[0][VK_S])
		{
			pos.y -= 5;
		}
		if (_keys[0][VK_D])
		{
			pos.x += 5;
		}
		if (_keys[0][VK_A])
		{
			pos.x -= 5;
		}
		
		c->setpos(pos);
		if (_keys[0][VK_R])
		{
			c->setpos(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
		}
		Point mouse = Director::getTheInstance()->getMousePos();
		
		c->setrotation(atan2(mouse.y-pos.y,mouse.x-pos.x)+90.0/180*M_PI);

	}

	float timeStep = dt;
	int32 velocityIterations = 8;
	int32 positionIterations = 1;

	world->Step(timeStep, velocityIterations, positionIterations);


	for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		if (b->GetUserData() != nullptr) {
			Sprite* sprite = (Sprite*)b->GetUserData();
			Point pt;
			pt.x = b->GetPosition().x *	PTM_RATIO;
			pt.y = b->GetPosition().y * PTM_RATIO;
			sprite->setpos(pt);
			float rot =b->GetAngle();
			sprite->setrotation(rot);
		}
		else
		{
			break;
		}
	}

}

void testscene1::BeginContact(b2Contact* contact)
{
	b2Body* bodyA = contact->GetFixtureA()->GetBody();
	b2Body* bodyB = contact->GetFixtureB()->GetBody();
	Sprite* spriteA = (Sprite*)bodyA->GetUserData();
	Sprite* spriteB = (Sprite*)bodyB->GetUserData();
	if (spriteA&&spriteB)
	{
		if (spriteA->getid() == coin || spriteB->getid() == coin)
		{
			glRasterPos2f(0, 0);
			glPrint("+1");
		}

	}
	
}

void testscene1::testCallback(EventMsg msg)
{
	Sprite* c;
	Point pt;
	Size sz;
	if (msg.name == "test")
	{
		switch (msg._event.type)
		{
		case DOWN:
		case DRAG:
			if (followSprite&&_sprites.size())
			{
				c = getChildByID(followSprite);
				pt = msg._event.point;
				sz = c->getsize();
				c->setpos(pt);

			}
			break;
		default:
			break;
		}
	}
}

void testscene1::initPhysics()
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

void testscene1::addNewSpriteCallback(EventMsg msg)
{
	if (msg._event.type == DOWN&&followSprite)
	{
		Sprite* sprite = new Sprite("CloseNormal.png", 0);
		addchild(sprite);
		Sprite* c=getChildByID(followSprite);
		Point pt = c->getpos();
		Point p = msg._event.point;
		sprite->setsize(32, 32);
		sprite->setpos(pt);

		float dir = atan2(p.y - pt.y, p.x - pt.x);

		//物体定义
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set(pt.x / PTM_RATIO, pt.y / PTM_RATIO);
		b2Body *body = world->CreateBody(&bodyDef);
		body->SetUserData(sprite);
		bodyDef.bullet = true;

		b2CircleShape circle;

		circle.m_p.Set(0.0f, 0.0f);

		circle.m_radius = 0.5f;



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
		//body->ApplyLinearImpulse(b2Vec2(rand() / (float)RAND_MAX * 10, rand() / (float)RAND_MAX * 10), b2Vec2(1.0f, 1.0f), true);
		b2MassData massdata;
		body->GetMassData(&massdata);
		float Velocity = 20;
		body->SetLinearVelocity(b2Vec2(Velocity*cos(dir),Velocity*sin(dir)));
		body->SetAngularVelocity((0.5 - rand() / (float)RAND_MAX));
	}

}

void testscene1::removeAllSprites(EventMsg msg)
{
	if (msg.name == "remove")
	{
		for (b2Body* b = world->GetBodyList(); b; )
		{
			if (b->GetUserData() != nullptr) {
				Sprite* sprite = (Sprite*)b->GetUserData();
				b2Body* p = b;
				b = b->GetNext();
				world->DestroyBody(p);
				removeChildByID(sprite->getid());

			}
			else
			{
				break;
			}
		}
	}
}