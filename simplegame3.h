#ifndef __SIMPLEGAME3__H__
#define __SIMPLEGAME3__H__
#include <Box2D\Box2D.h>
#include <string>
#include <mutex>
#include "scene.h"
#include "sprite.h"
class simplegame3 :public Scene, public b2ContactListener
{
public:
	simplegame3() = delete;
	simplegame3(std::string name);
	~simplegame3();
	virtual void init();
	virtual void clean();
	virtual void update(float dt);
	//virtual void pause();                         //暂停
	//virtual void resume();                        //继续
	virtual void BeginContact(b2Contact* contact);//继承开始碰撞的虚函数
	void initPhysics();                           //初始化物理模型
	void shoot(EventMsg msg);                     //发射子弹
	void onClientConnected(EventMsg msg);
	void Player1Damaged();
	void Player2Damaged();
private:
	std::pair<int,b2Body*> addPlayerSprite(std::string name, float width, float height, int _layer\
		, float x, float y, uint16 categoryBits, uint16 maskBits);
	int player1Sprite;                             //玩家精灵
	int player2Sprite;                             //玩家精灵
	int player1bulletInTheWorld;                   //玩家1已发射的子弹数量
	int player2bulletInTheWorld;                   //玩家2已发射的子弹数量
	int player1maxBulletInTheWorld;                      //可以同时存在最多的子弹数量
	int player2maxBulletInTheWorld;                      //可以同时存在最多的子弹数量
	int player1HP;
	int player2HP;
	volatile bool gamestart;
	bool gameover;
	b2Body* player1Body;                           //玩家的物理实体
	b2Body* player2Body;                           //玩家的物理实体
	std::vector<b2Body*> toDeleteBody;            //要删除的物体
	std::vector<int> toDeleteSprite;              //要删除的精灵
	b2World* world;
	std::mutex worldMutex;
};
#endif//__SIMPLEGAME3__H__