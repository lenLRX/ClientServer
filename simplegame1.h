#ifndef __SIMPLEGAME1__H__
#define __SIMPLEGAME1__H__
#include "scene.h"
#include "director.h"
#include "bar.h"
#include "clock.h"
#include <Box2D\Box2D.h>
#include <string>
//using namespace std;
class simplegame1 :public Scene, public b2ContactListener
{
public:
	simplegame1() = delete;
	simplegame1(std::string name);
	~simplegame1();
	virtual void init();
	virtual void clean();
	virtual void update(float dt);
	virtual void pause();                         //暂停
	virtual void resume();                        //继续
	virtual void BeginContact(b2Contact* contact);//继承开始碰撞的虚函数
	void initPhysics();                           //初始化物理模型
	void addAlert(EventMsg msg);                  //添加警报
	void addNewBullet(EventMsg msg);              //增加新的子弹
	void damaged();                               //受伤
private:
	Bar* testbar;                                 //血条
	float score;                                  //分数
	int playerSprite;                             //玩家精灵
	int alertSprite;                              //报警精灵
	float life;                                   //生命值
	float damage;                                 //每次被碰到以后的伤害值
	Clock _clock;                                 //计分用的时钟
	b2Body* playerBody;                           //玩家的物理实体
	b2World* world;
};
#endif//__SIMPLEGAME1__H__