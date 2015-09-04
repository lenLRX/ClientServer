#ifndef __SCENE__H__
#define __SCENE__H__
#include "sprite.h"
#include "scene.h"
#include "mouseevent.h"
#include "globalvar.h"
#include "event.h"
#include <string>
#include <vector>
//using namespace std;


class Scene : public Object
{
public:
	Scene()=delete;
	Scene(std::string name);
	virtual ~Scene();
	virtual void init();                                                               //��ʼ������
	virtual void clean();                                                              //������
	virtual void update(float dt);                                                     //���º�������Ҫÿһ֡����
	virtual void pause();                                                              //��ͣ
	virtual void resume();                                                             //����
	virtual void onKeyPressedCallback(int player,int key);                                        //�����ص�����
	virtual bool sceneMouseEvent(mouseEvent _event);                                   //��������¼�����
	void setname(std::string name);                                                    //��������
	std::string getname();                                                             //��ȡ����
	int addchild(Sprite* child);                                                       //���뾫�� ���ظþ����ڱ������е�ID
	void addChildWithoutID(Sprite* child);                                             //���뾫�� ���ı���ID
	void removeChildByID(int id);                                                      //ɾ������
	Sprite* getChildByID(int id);                                                      //��þ���
	void handleMouseEvent(Point pt);                                                   //��Ӧ����¼�
	std::vector<Sprite*>& getSpriteList();                                             //��ȡ�����б�
	void registerMouseEvent(Sprite* _sprite);                                          //ע������¼�
	void processMouseEvent(mouseEvent _event);                                         //��������¼�
	void clearKeys();                                                                  //���
protected:
	friend class Director;
	bool pausing;                                                                      //�Ƿ�������ͣ
	std::string _name;                                                                 //ÿ������Ψһ������
	std::vector<Sprite*> _sprites;                                                     //���汾�����ľ�����
	std::vector<Sprite*> _mouseEventList;                                              //ע��������¼��ľ�����
	std::vector<Sprite*> _keyBoardEventList;                                           //ע���˼����¼��ľ���
	int _keys[MAXPLAYERNUMBER][256];                                                   //����״̬
	int id;                                                                            //�������ÿ������
	bool mutiplayer;                                                                   //�Ƿ�֧�ֶ�����Ϸ
};
#endif//__SCENE__H__