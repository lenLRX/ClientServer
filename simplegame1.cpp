#include "stdafx.h"
#include "simplegame1.h"
#include "globalvar.h"
#include "printText.h"
#include "timer.h"
#include <time.h>
using namespace std;
simplegame1::simplegame1(string name) :Scene(name), life(1.0), damage(0.1)
{

}

simplegame1::~simplegame1()
{
	delete world;
}

void simplegame1::init()
{
	srand(time(NULL));
	initPhysics();
	for (int i = 0; i < 5; i++)
	{
		timer* _t = new timer();
		_t->setDelayTime((i+1)*0.2f);
		_t->registerEvent("new alert");
		Director::getTheInstance()->addTimer(_t);
	}
	Director::getTheInstance()->registerEvent(this, &simplegame1::addNewBullet);
	Director::getTheInstance()->registerEvent(this, &simplegame1::addAlert);
	Sprite* s = new Sprite("core2.png", 1);
	s->setpos(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
	s->setsize(20, 20);
	playerSprite = addchild(s);
	Director::getTheInstance()->registerKey(VK_A);    //注册键盘事件
	Director::getTheInstance()->registerKey(VK_S);
	Director::getTheInstance()->registerKey(VK_D);
	Director::getTheInstance()->registerKey(VK_W);
	testbar = new Bar(this, "bar2.png", "bar1.png", "testbar",0);
	testbar->setPos(200, 25);
	testbar->setSize(600, 25);


	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	bodyDef.position.Set(WINDOW_WIDTH / 2 / PTM_RATIO, WINDOW_HEIGHT / 2 / PTM_RATIO);
	b2Body *body = world->CreateBody(&bodyDef);
	playerBody = body;
	body->SetUserData(s);
	bodyDef.bullet = false;

	b2CircleShape circle;

	circle.m_p.Set(0.0f, 0.0f);

	circle.m_radius = 10.0f / PTM_RATIO;



	// 夹具定义
	b2FixtureDef fixtureDef;
	fixtureDef.restitution = 0.8f;
	//设置夹具的形状
	fixtureDef.shape = &circle;
	//设置密度
	fixtureDef.density = 1.0f;
	//设置摩擦系数
	fixtureDef.friction = 0.0f;
	fixtureDef.isSensor = true;
	//使用夹具固定形状到物体上	
	body->CreateFixture(&fixtureDef);

	world->SetContactListener(this);


	_clock.start();
}

void simplegame1::clean()
{

}

void simplegame1::update(float dt)
{
	EventMsg msg;
	msg.name = "testbar";
	msg.param = life;
	Director::getTheInstance()->raiseEvent(msg);
	if (playerSprite && !pausing)
	{
		Sprite* c = getChildByID(playerSprite);
		///////////////////
		//跟随鼠标
		/*
		POINT MOUSE = Director::getTheInstance()->getMousePos();
		Size s = c->getsize();
		c->setpos(MOUSE.x-s.width/2, MOUSE.y-s.height/2);
		*/
		Point pos = c->getpos();
		float speed = 10;
		if (_keys[0][VK_W])
		{
			pos.y += speed;
		}
		if (_keys[0][VK_S])
		{
			pos.y -= speed;
		}
		if (_keys[0][VK_D])
		{
			pos.x += speed;
		}
		if (_keys[0][VK_A])
		{
			pos.x -= speed;
		}
		if (pos.x > WINDOW_WIDTH)
		{
			pos.x = WINDOW_WIDTH;
		}
		if (pos.y > WINDOW_HEIGHT)
		{
			pos.y = WINDOW_HEIGHT;
		}
		if (pos.x < 0)
		{
			pos.x = 0;
		}
		if (pos.y < 0)
		{
			pos.y = 0;
		}
		c->setpos(pos);
		playerBody->SetTransform(b2Vec2(pos.x / PTM_RATIO, pos.y / PTM_RATIO),0.0f);
		//Point mouse = Director::getTheInstance()->getMousePos();

		//c->setrotation(atan2(mouse.y - pos.y, mouse.x - pos.x) + 90.0 / 180 * M_PI);

	}

	float timeStep = dt;
	int32 velocityIterations = 8;
	int32 positionIterations = 1;

	world->Step(timeStep, velocityIterations, positionIterations);


	for (b2Body* b = world->GetBodyList(); b;)
	{
		if (b->GetUserData() != nullptr&&b->GetType()==b2_dynamicBody) {
			b2Vec2 v = b->GetLinearVelocity();
			if (v.Length() < 5)
			{
				Sprite* sprite = (Sprite*)b->GetUserData();
				b2Body* p = b;
				b = b->GetNext();
				world->DestroyBody(p);
				removeChildByID(sprite->getid());
			}
			else
			{
				Sprite* sprite = (Sprite*)b->GetUserData();
				Point pt;
				pt.x = b->GetPosition().x *	PTM_RATIO;
				pt.y = b->GetPosition().y * PTM_RATIO;
				sprite->setpos(pt);
				float rot = b->GetAngle();
				sprite->setrotation(rot);
				b = b->GetNext();
			}
		}
		else
		{
			break;
		}
	}
	Text _text;
	_text.pt.x = 100;
	_text.pt.y = 200;
	if (life > 0)//还活着
	{
		score = _clock.getTimeSinceStart();
		sprintf(_text.str, "Score:%f ", score);
	}
	else
	{
		sprintf(_text.str, "Score:%f (Dead)", score);
	}
	Director::getTheInstance()->printTextAtPoint(_text);
}

void simplegame1::pause()
{
	_clock.pause();
}

void simplegame1::resume()
{
	_clock.resume();
}

void simplegame1::BeginContact(b2Contact* contact)
{
	b2Body * bodyA = contact->GetFixtureA()->GetBody();
	b2Body* bodyB = contact->GetFixtureB()->GetBody();
	Sprite* spriteA = (Sprite*)bodyA->GetUserData();
	Sprite* spriteB = (Sprite*)bodyB->GetUserData();
	if (spriteA&&spriteB)
	{
		if (spriteA->getid() == playerSprite || spriteB->getid() == playerSprite)
		{
			damaged();
		}

	}
}

void simplegame1::initPhysics()
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

void simplegame1::addAlert(EventMsg msg)
{
	if (msg.name == "new alert")
	{
		timer* _t = new timer();
		Sprite* sprite = new Sprite("alert.png", 2);
		EventMsg msg;
		msg.name = "new bullet";
		msg.UserData = sprite;
		_t->setDelayTime(1.0f);
		_t->registerEvent(msg);
		Director::getTheInstance()->addTimer(_t);
		alertSprite=addchild(sprite);
		Point pt;
		pt.x = WINDOW_WIDTH*(float)rand() / RAND_MAX;
		pt.y = WINDOW_HEIGHT*(float)rand() / RAND_MAX;
		sprite->setsize(32, 32);
		sprite->setpos(pt);
	}
}

void simplegame1::addNewBullet(EventMsg msg)
{
	if (msg.name == "new bullet")
	{
		
		//再次注册定时器，完成循环
		timer* _t = new timer();
		_t->setDelayTime(1.0f);
		Director::getTheInstance()->addTimer(_t);
		Sprite* sprite = new Sprite("CloseNormal.png", 3);
		addchild(sprite);
		Point pt = getChildByID(((Sprite*)msg.UserData)->getid())->getpos();
		sprite->setsize(32, 32);
		sprite->setpos(pt);

		float dir = 0.5 - (float)rand() / RAND_MAX;
		//删除警报标志
		removeChildByID(((Sprite*)msg.UserData)->getid());
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
		float Velocity = 5;
		body->SetLinearVelocity(b2Vec2(Velocity*cos(dir * 2 * M_PI), Velocity*sin(dir * 2 * M_PI)));
		body->SetAngularVelocity((0.5 - rand() / (float)RAND_MAX));

		EventMsg msg2;
		msg2.name = "new alert";
		msg2.UserData = sprite;
		_t->registerEvent(msg2);
	}
	
}

void simplegame1::damaged()
{
	life -= damage;
	if (life < 0)
		life = 0.0f;
}