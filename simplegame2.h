#ifndef __SIMPLEGAME2__H__
#define __SIMPLEGAME2__H__
#include "scene.h"
#include "director.h"
#include "bar.h"
#include "clock.h"
#include <Box2D\Box2D.h>
#include <string>
//using namespace std;
class simplegame2 :public Scene, public b2ContactListener
{
public:
	simplegame2() = delete;
	simplegame2(std::string name);
	~simplegame2();
	virtual void init();
	virtual void clean();
	virtual void update(float dt);
	//virtual void pause();                         //��ͣ
	//virtual void resume();                        //����
	virtual void BeginContact(b2Contact* contact);//�̳п�ʼ��ײ���麯��
	void initPhysics();                           //��ʼ������ģ��
	void shoot(EventMsg msg);                     //�����ӵ�
private:
	int player1Sprite;                             //��Ҿ���
	int player2Sprite;                             //��Ҿ���
	int bulletInTheWorld;                         //�ѷ�����ӵ�����
	int maxBulletInTheWorld;                      //����ͬʱ���������ӵ�����
	b2Body* player1Body;                           //��ҵ�����ʵ��
	b2Body* player2Body;                           //��ҵ�����ʵ��
	std::vector<b2Body*> toDeleteBody;            //Ҫɾ��������
	std::vector<int> toDeleteSprite;              //Ҫɾ���ľ���
	b2World* world;
};


#endif//__SIMPLEGAME2__H__