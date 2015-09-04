#include "stdafx.h"
#include "simplegame2.h"
#include "clock.h"

using namespace std;
simplegame2::simplegame2(string name) :Scene(name), bulletInTheWorld(0), maxBulletInTheWorld(5)
{

}

simplegame2::~simplegame2()
{
	delete world;
	clean();
	player1Sprite = 0;
	player2Sprite = 0;
}

void simplegame2::init()
{
	srand(time(NULL));
	initPhysics();
	Sprite* s = new Sprite("core2.png", 1);
	s->setpos(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
	s->setsize(20, 20);
	player1Sprite = addchild(s);
	Director::getTheInstance()->registerKey(VK_A);    //注册键盘事件
	Director::getTheInstance()->registerKey(VK_S);
	Director::getTheInstance()->registerKey(VK_D);
	Director::getTheInstance()->registerKey(VK_W);
	Director::getTheInstance()->registerEvent(this, &simplegame2::shoot);

	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	bodyDef.position.Set(WINDOW_WIDTH / 2 / PTM_RATIO, WINDOW_HEIGHT / 2 / PTM_RATIO);
	b2Body *body = world->CreateBody(&bodyDef);
	body->SetUserData(s);
	bodyDef.bullet = true;

	b2CircleShape circle;

	circle.m_p.Set(0.0f, 0.0f);

	circle.m_radius = 22.0f / PTM_RATIO;



	// 夹具定义
	b2FixtureDef fixtureDef;
	fixtureDef.filter.categoryBits = 0x8000;
	fixtureDef.filter.maskBits = 0x0800;
	fixtureDef.restitution = 0.8f;
	//设置夹具的形状
	fixtureDef.shape = &circle;
	//设置密度
	fixtureDef.density = 1.0f;
	//设置摩擦系数
	fixtureDef.friction = 0.0f;
	//fixtureDef.isSensor = true;
	//使用夹具固定形状到物体上	
	body->CreateFixture(&fixtureDef);
	player1Body = body;

	Sprite* s2 = new Sprite("CloseSelected.png", 1);
	s2->setpos(WINDOW_WIDTH / 2+50, WINDOW_HEIGHT / 2);
	s2->setsize(20, 20);
	player2Sprite = addchild(s2);

	b2BodyDef bodyDef2;
	bodyDef2.type = b2_staticBody;
	bodyDef2.position.Set((WINDOW_WIDTH / 2 + 50 )/ PTM_RATIO, WINDOW_HEIGHT / 2 / PTM_RATIO);
	b2Body *body2 = world->CreateBody(&bodyDef2);
	body2->SetUserData(s2);
	bodyDef2.bullet = true;

	b2CircleShape circle2;

	circle2.m_p.Set(0.0f, 0.0f);

	circle2.m_radius = 22.0f / PTM_RATIO;



	// 夹具定义
	b2FixtureDef fixtureDef2;
	fixtureDef2.filter.categoryBits = 0x0800;
	fixtureDef2.filter.maskBits = 0x0800;
	fixtureDef2.restitution = 0.8f;
	//设置夹具的形状
	fixtureDef2.shape = &circle2;
	//设置密度
	fixtureDef2.density = 1.0f;
	//设置摩擦系数
	fixtureDef2.friction = 0.0f;
	//fixtureDef.isSensor = true;
	//使用夹具固定形状到物体上	
	body2->CreateFixture(&fixtureDef2);
	player2Body = body2;

	world->SetContactListener(this);



	//throw 0;
}

void simplegame2::update(float dt)
{
	if (player1Sprite && !pausing)
	{
		Sprite* c = getChildByID(player1Sprite);
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
		player1Body->SetTransform(b2Vec2(pos.x / PTM_RATIO, pos.y / PTM_RATIO), 0.0f);
	}

	if (player2Sprite && !pausing)
	{
		Sprite* c = getChildByID(player2Sprite);
		Point pos = c->getpos();
		float speed = 10;
		if (_keys[1][VK_W])
		{
			pos.y += speed;
		}
		if (_keys[1][VK_S])
		{
			pos.y -= speed;
		}
		if (_keys[1][VK_D])
		{
			pos.x += speed;
		}
		if (_keys[1][VK_A])
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
		player2Body->SetTransform(b2Vec2(pos.x / PTM_RATIO, pos.y / PTM_RATIO), 0.0f);
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
			if (pt.x > WINDOW_WIDTH || pt.y > WINDOW_HEIGHT || pt.x < 0 || pt.y < 0&&sprite->getUserData())
			{
				delete sprite->getUserData();
				toDeleteBody.push_back(b);
				toDeleteSprite.push_back(sprite->getid());
			}
			sprite->setpos(pt);
			float rot = b->GetAngle();
			sprite->setrotation(rot);
		}
		else
		{
			break;
		}
	}
	if (toDeleteBody.size())
	{
		for (vector<b2Body*>::iterator it = toDeleteBody.begin(); it != toDeleteBody.end();)
		{
			//Sprite* p = (Sprite*)(*it)->GetUserData();
			//assert(p);
			//throw 0;
			//removeChildByID(p->getid());
			world->DestroyBody((*it));
			it = toDeleteBody.erase(it);
			bulletInTheWorld--;
		}
	}
	if (toDeleteSprite.size())
	{
		for (vector<int>::iterator it = toDeleteSprite.begin(); it != toDeleteSprite.end();)
		{
			removeChildByID((*it));
			it = toDeleteSprite.erase(it);
		}
	}

}

void simplegame2::initPhysics()
{
	b2Vec2 gravity;

	gravity.Set(0.0f, 0.0f);

	world = new b2World(gravity);

	world->SetAllowSleeping(true);

	world->SetContinuousPhysics(true);

	b2Filter filter;

	filter.categoryBits = 0x0001;

	filter.maskBits = 0x0001+0x0800+0x8000;

	b2BodyDef groundBodyDef;

	groundBodyDef.position.Set(0, 0);

	b2Body* groundBody = world->CreateBody(&groundBodyDef);

	b2EdgeShape groundBox;
	
	groundBox.Set(b2Vec2(0, 0), b2Vec2(WINDOW_WIDTH / PTM_RATIO, 0));

	groundBody->CreateFixture(&groundBox, 0)->SetFilterData(filter);

	groundBox.Set(b2Vec2(0, WINDOW_HEIGHT / PTM_RATIO), b2Vec2(WINDOW_WIDTH / PTM_RATIO, WINDOW_HEIGHT / PTM_RATIO));

	groundBody->CreateFixture(&groundBox, 0)->SetFilterData(filter);

	groundBox.Set(b2Vec2(0, WINDOW_HEIGHT / PTM_RATIO), b2Vec2(0, 0));

	groundBody->CreateFixture(&groundBox, 0)->SetFilterData(filter);

	groundBox.Set(b2Vec2(WINDOW_WIDTH / PTM_RATIO, WINDOW_HEIGHT / PTM_RATIO), b2Vec2(WINDOW_WIDTH / PTM_RATIO, 0));

	groundBody->CreateFixture(&groundBox, 0)->SetFilterData(filter);
}

void simplegame2::BeginContact(b2Contact* contact)
{
	b2Body* bodyA = contact->GetFixtureA()->GetBody();
	b2Body* bodyB = contact->GetFixtureB()->GetBody();
	Sprite* spriteA = (Sprite*)bodyA->GetUserData();
	Sprite* spriteB = (Sprite*)bodyB->GetUserData();
	if (spriteA&&spriteB)
	{
		b2Vec2 dpos = bodyA->GetPosition() - bodyB->GetPosition();
		if (spriteA->getid() == player1Sprite)
		{
			Clock* _clock = (Clock*)spriteB->getUserData();
			if (_clock->getTimeSinceStart() > 0.1f)
			{
				spriteB->setUserData(NULL);
				delete _clock;
				toDeleteBody.push_back(bodyB);
				toDeleteSprite.push_back(spriteB->getid());
			}
		}
		else if (spriteB->getid() == player1Sprite)
		{
			Clock* _clock = (Clock*)spriteA->getUserData();
			if (_clock->getTimeSinceStart() > 0.1f)
			{
				spriteA->setUserData(NULL);
				delete _clock;
				toDeleteBody.push_back(bodyA);
				toDeleteSprite.push_back(spriteA->getid());
			}
		}

	}
}

void simplegame2::shoot(EventMsg msg)
{
	if (msg._event.type == DOWN && player1Sprite && bulletInTheWorld < maxBulletInTheWorld)
	{
		bulletInTheWorld++;
		Sprite* sprite = new Sprite("CloseNormal.png", 0);
		Clock* _clock = new Clock();
		_clock->start();
		sprite->setUserData(_clock);
		addchild(sprite);
		Sprite* c = getChildByID(player1Sprite);
		Point pt = c->getpos();
		Point p = msg._event.point;
		sprite->setsize(32, 32);
		sprite->setpos(pt);

		float dir = atan2(p.y - pt.y, p.x - pt.x);

		//物体定义
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set((pt.x + 20*cos(dir)) / PTM_RATIO, (pt.y + 20*sin(dir)) / PTM_RATIO);
		b2Body *body = world->CreateBody(&bodyDef);
		body->SetUserData(sprite);
		bodyDef.bullet = true;

		b2CircleShape circle;

		circle.m_p.Set(0.0f, 0.0f);

		circle.m_radius = 0.5f;



		// 夹具定义
		b2FixtureDef fixtureDef;
		fixtureDef.filter.categoryBits = 0x0800;
		fixtureDef.filter.maskBits = 0x0800+0x0001;
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
		body->SetLinearVelocity(b2Vec2(Velocity*cos(dir), Velocity*sin(dir)));
		body->SetAngularVelocity((0.5 - rand() / (float)RAND_MAX));
	}
}

void simplegame2::clean()
{
	//第一步：删除精灵
	player1Sprite = 0;
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