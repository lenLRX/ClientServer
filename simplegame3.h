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
	//virtual void pause();                         //��ͣ
	//virtual void resume();                        //����
	virtual void BeginContact(b2Contact* contact);//�̳п�ʼ��ײ���麯��
	void initPhysics();                           //��ʼ������ģ��
	void shoot(EventMsg msg);                     //�����ӵ�
	void onClientConnected(EventMsg msg);
	void Player1Damaged();
	void Player2Damaged();
private:
	std::pair<int,b2Body*> addPlayerSprite(std::string name, float width, float height, int _layer\
		, float x, float y, uint16 categoryBits, uint16 maskBits);
	int player1Sprite;                             //��Ҿ���
	int player2Sprite;                             //��Ҿ���
	int player1bulletInTheWorld;                   //���1�ѷ�����ӵ�����
	int player2bulletInTheWorld;                   //���2�ѷ�����ӵ�����
	int player1maxBulletInTheWorld;                      //����ͬʱ���������ӵ�����
	int player2maxBulletInTheWorld;                      //����ͬʱ���������ӵ�����
	int player1HP;
	int player2HP;
	volatile bool gamestart;
	bool gameover;
	b2Body* player1Body;                           //��ҵ�����ʵ��
	b2Body* player2Body;                           //��ҵ�����ʵ��
	std::vector<b2Body*> toDeleteBody;            //Ҫɾ��������
	std::vector<int> toDeleteSprite;              //Ҫɾ���ľ���
	b2World* world;
	std::mutex worldMutex;
};
#endif//__SIMPLEGAME3__H__