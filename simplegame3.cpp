#include "stdafx.h"
#include "simplegame3.h"
#include "audioEngine.h"
#include "director.h"
#include "clock.h"

using namespace std;
simplegame3::simplegame3(string name) :Scene(name), player1HP(5), player2HP(5), gameover(false),gamestart(false)\
, player1bulletInTheWorld(0), player1maxBulletInTheWorld(5), player2bulletInTheWorld(0), player2maxBulletInTheWorld(5)
{

}

simplegame3::~simplegame3()
{
	clean();
	player1Sprite = 0;
	player2Sprite = 0;
}

void simplegame3::init()
{
	srand(time(NULL));
	initPhysics();
	Director::getTheInstance()->registerKey(VK_A);    //注册键盘事件
	Director::getTheInstance()->registerKey(VK_S);
	Director::getTheInstance()->registerKey(VK_D);
	Director::getTheInstance()->registerKey(VK_W);
	Director::getTheInstance()->registerEvent(this, &simplegame3::shoot);
	Director::getTheInstance()->registerEvent(this, &simplegame3::onClientConnected);
	pair<int, b2Body*> p1 = addPlayerSprite("sprite_enemy_largeboat_turret_0.png"\
		, 40, 40, 1, WINDOW_WIDTH / 2-100, WINDOW_HEIGHT / 2, 0x8000, 0x8000);
	player1Sprite = p1.first;
	player1Body = p1.second;
	((Sprite*)player1Body->GetUserData())->setTag(0);


	pair<int, b2Body*> p2 = addPlayerSprite("sprite_enemy_mediumboat_rearturret_0.png"\
		, 40, 40, 1, WINDOW_WIDTH / 2+100, WINDOW_HEIGHT / 2, 0x0800, 0x0800);
	player2Sprite = p2.first;
	player2Body = p2.second;
	((Sprite*)player2Body->GetUserData())->setTag(1);

	world->SetContactListener(this);
}

pair<int, b2Body*> simplegame3::addPlayerSprite(std::string name, float width ,float height, int _layer, \
	float x, float y, uint16 categoryBits, uint16 maskBits)
{
	Sprite* s = new Sprite(name, _layer);
	s->setpos(x, y);
	s->setsize(width, height);
	int playerID = addchild(s);

	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	bodyDef.position.Set(x/ PTM_RATIO, y / PTM_RATIO);
	b2Body *body = world->CreateBody(&bodyDef);
	body->SetUserData(s);
	bodyDef.bullet = true;

	b2CircleShape circle;

	circle.m_p.Set(0.0f, 0.0f);

	circle.m_radius = width/2 / PTM_RATIO;

	// 夹具定义
	b2FixtureDef fixtureDef;

	fixtureDef.filter.categoryBits = categoryBits;
	fixtureDef.filter.maskBits = maskBits;

	fixtureDef.restitution = 0.8f;
	//设置夹具的形状
	fixtureDef.shape = &circle;
	//设置密度
	fixtureDef.density = 1.0f;
	//设置摩擦系数
	fixtureDef.friction = 0.0f;

	body->CreateFixture(&fixtureDef);
	return pair<int, b2Body*>(playerID, body);
}

void simplegame3::update(float dt)
{
	Text _text;

	_text.pt.x = 400;
	_text.pt.y = 500;

	if (player1HP == 0)
	{
		sprintf(_text.str, "P2 WIN!");
		gameover = true;
	}
	else if (player2HP == 0)
	{
		sprintf(_text.str, "P1 WIN!");
		gameover = true;
	}
	else
	{
		sprintf(_text.str, "P1:%d HP  P2:%d HP", player1HP, player2HP);
	}

	Director::getTheInstance()->printTextAtPoint(_text);
	if (!gameover&&gamestart)
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
			player1Body->SetTransform(b2Vec2(pos.x / PTM_RATIO, pos.y / PTM_RATIO), player1Body->GetAngle());
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
			player2Body->SetTransform(b2Vec2(pos.x / PTM_RATIO, pos.y / PTM_RATIO), player2Body->GetAngle());
		}

		float timeStep = dt;
		int32 velocityIterations = 8;
		int32 positionIterations = 1;

		worldMutex.lock();
		world->Step(timeStep, velocityIterations, positionIterations);
		worldMutex.unlock();

		for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
		{
			if (b->GetUserData() != nullptr) {
				Sprite* sprite = (Sprite*)b->GetUserData();
				Point pt;
				pt.x = b->GetPosition().x *	PTM_RATIO;
				pt.y = b->GetPosition().y * PTM_RATIO;
				if (pt.x > WINDOW_WIDTH || pt.y > WINDOW_HEIGHT || pt.x < 0 || pt.y < 0)
				{
					toDeleteBody.push_back(b);
					toDeleteSprite.push_back(sprite->getid());
					continue;
				}
				if (((Sprite*)b->GetUserData())->getTag() == 0 && b != player1Body)
				{
					if (((Clock*)((Sprite*)b->GetUserData())->getUserData())->getTimeSinceStart()>0.1f)
					{
						if ((b->GetPosition() - player1Body->GetPosition()).Length() < 1)
						{
							delete ((Sprite*)b->GetUserData())->getUserData();
							toDeleteBody.push_back(b);
							toDeleteSprite.push_back(sprite->getid());
						}
					}
				}
				else if (((Sprite*)b->GetUserData())->getTag() == 1 && b != player2Body)
				{
					if (((Clock*)((Sprite*)b->GetUserData())->getUserData())->getTimeSinceStart()>0.1f)
					{
						if ((b->GetPosition() - player2Body->GetPosition()).Length() < 1)
						{
							delete ((Sprite*)b->GetUserData())->getUserData();
							toDeleteBody.push_back(b);
							toDeleteSprite.push_back(sprite->getid());
						}
					}
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
		size_t toDeleteBodysize = toDeleteBody.size();
		for (int i = 0; i < toDeleteBodysize; ++i)
		{
			world->DestroyBody(toDeleteBody[i]);
		}
		toDeleteBody.clear();
		size_t toDeleteSpritesize = toDeleteSprite.size();
		for (int i = 0; i < toDeleteSpritesize; ++i)
		{
			Sprite* s = getChildByID(toDeleteSprite[i]);
			if (s->getTag() == 0)
			{
				--player1bulletInTheWorld;
			}
			else if (s->getTag() == 1)
			{
				--player2bulletInTheWorld;
			}
			removeChildByID(toDeleteSprite[i]);
		}
		toDeleteSprite.clear();
	}
}

void simplegame3::initPhysics()
{
	b2Vec2 gravity;

	gravity.Set(0.0f, 0.0f);

	world = new b2World(gravity);

	world->SetAllowSleeping(true);

	world->SetContinuousPhysics(true);

	b2Filter filter;

	filter.categoryBits = 0x0001;

	filter.maskBits = 0x0001 + 0x0800 + 0x8000;

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

void simplegame3::BeginContact(b2Contact* contact)
{
	b2Body* bodyA = contact->GetFixtureA()->GetBody();
	b2Body* bodyB = contact->GetFixtureB()->GetBody();
	Sprite* spriteA = (Sprite*)bodyA->GetUserData();
	Sprite* spriteB = (Sprite*)bodyB->GetUserData();
	if (spriteA&&spriteB)
	{
		if (spriteA->getid() == player1Sprite && spriteB->getid() != player2Sprite && spriteB->getTag()==1)
		{
			Player1Damaged();
		}
		else if (spriteB->getid() == player1Sprite && spriteA->getid() != player2Sprite && spriteA->getTag() == 1)
		{
			Player1Damaged();
		}
		else if (spriteA->getid() == player2Sprite && spriteB->getid() != player1Sprite && spriteB->getTag() == 0)
		{
			Player2Damaged();
		}
		else if (spriteB->getid() == player2Sprite && spriteA->getid() != player1Sprite && spriteA->getTag() == 0)
		{
			Player2Damaged();
		}

	}
}

void simplegame3::shoot(EventMsg msg)
{
	if (!gameover)
	{
		if (msg.param == 0 && player1Sprite)
		{
			Sprite* c = getChildByID(player1Sprite);
			Point pt = c->getpos();
			Point p = msg._event.point;
			float dir = atan2(p.y - pt.y, p.x - pt.x);
			float dir2 = dir + M_PI / 2;
			c->setrotation(dir2);
			worldMutex.lock();
			player1Body->SetTransform(player1Body->GetPosition(), dir2);
			worldMutex.unlock();
			if (msg._event.type == DOWN && player1bulletInTheWorld < player1maxBulletInTheWorld)
			{
				player1bulletInTheWorld++;
				Sprite* sprite = new Sprite("sprite_enemy_shot_small_blue_0.png", 0);
				Clock* _clock = new Clock();
				_clock->start();
				sprite->setUserData(_clock);
				sprite->setTag(0);
				addchild(sprite);
				sprite->setsize(32, 32);
				sprite->setpos(pt);

				//物体定义
				b2BodyDef bodyDef;
				bodyDef.type = b2_dynamicBody;
				bodyDef.position.Set((pt.x + 20 * cos(dir)) / PTM_RATIO, (pt.y + 20 * sin(dir)) / PTM_RATIO);
				b2Body *body = world->CreateBody(&bodyDef);
				body->SetUserData(sprite);
				bodyDef.bullet = true;

				b2CircleShape circle;

				circle.m_p.Set(0.0f, 0.0f);

				circle.m_radius = 32 / 2 / PTM_RATIO;

				// 夹具定义
				b2FixtureDef fixtureDef;
				fixtureDef.filter.categoryBits = 0x0800;
				fixtureDef.filter.maskBits = 0x0800 + 0x8000 + 0x0001;
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
		else if (msg.param == 1 && player2Sprite)
		{
			Sprite* c = getChildByID(player2Sprite);
			Point pt = c->getpos();
			Point p = msg._event.point;
			float dir = atan2(p.y - pt.y, p.x - pt.x);
			float dir2 = dir + M_PI / 2;
			c->setrotation(dir2);
			worldMutex.lock();
			player2Body->SetTransform(player2Body->GetPosition(), dir2);
			worldMutex.unlock();
			if (msg._event.type == DOWN  && player2bulletInTheWorld < player2maxBulletInTheWorld)
			{
				player2bulletInTheWorld++;
				Sprite* sprite = new Sprite("sprite_enemy_shot_small_purple_0.png", 0);
				Clock* _clock = new Clock();
				_clock->start();
				sprite->setUserData(_clock);
				sprite->setTag(1);
				addchild(sprite);
				Sprite* c = getChildByID(player2Sprite);
				Point pt = c->getpos();
				Point p = msg._event.point;
				sprite->setsize(32, 32);
				sprite->setpos(pt);

				float dir = atan2(p.y - pt.y, p.x - pt.x);

				//物体定义
				b2BodyDef bodyDef;
				bodyDef.type = b2_dynamicBody;
				bodyDef.position.Set((pt.x + 20 * cos(dir)) / PTM_RATIO, (pt.y + 20 * sin(dir)) / PTM_RATIO);
				b2Body *body = world->CreateBody(&bodyDef);
				body->SetUserData(sprite);
				bodyDef.bullet = true;

				b2CircleShape circle;

				circle.m_p.Set(0.0f, 0.0f);

				circle.m_radius = 32 / 2 / PTM_RATIO;

				// 夹具定义
				b2FixtureDef fixtureDef;
				fixtureDef.filter.categoryBits = 0x8000;
				fixtureDef.filter.maskBits = 0x8000 + 0x0001 + 0x0800;
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
	}
}

void simplegame3::onClientConnected(EventMsg msg)
{
	if (!gamestart && msg.name == "Client Connencted")
	{
		gamestart = true;
		AudioEngine::getTheInstance()->playMusic("Thanatos.wav", 5, true);
	}
}

void simplegame3::clean()
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

void simplegame3::Player1Damaged()
{
	if (player1HP > 0 && !gameover)
	{
		--player1HP;
		AudioEngine::getTheInstance()->playMusic("boom1.wav", 8, false);
	}
}

void simplegame3::Player2Damaged()
{
	if (player2HP > 0 && !gameover)
	{
		--player2HP;
		AudioEngine::getTheInstance()->playMusic("boom1.wav", 8, false);
	}
}