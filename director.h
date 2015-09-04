#ifndef __DIRECTOR__H__
#define __DIRECTOR__H__
#include "scene.h"
#include "sprite.h"
#include <vector>
#include <map>
#include "globalvar.h"
#include "mouseevent.h"
#include "event.h"
#include "timer.h"
#include "clock.h"
//using namespace std;

enum RunningState
{
	RUNNING,                     //��������
	SLEEPING                     //��������ת״̬
};

Point POINT2Point(POINT pt);

class Director
{
public:
	Director();
	static void create();                                                                //����ʵ��
	void startWithScene(std::string name);                                               //��ʼ��
	static Director* getTheInstance();                                                   //���Ψһʵ��
	void gotoScene(std::string name);                                                    //ת���¸�����
	Scene* getCurrentScene();                                                            //��õ�ǰ����
	int getTotalScene();                                                                 //��õ�ǰ��������
	void addScene(Scene* theScene);                                                      //���ӳ���
	void setMousePos(POINT mousepos);                                                    //�趨���λ��
	Point getMousePos();                                                                 //��ȡ���λ��
	template <class _func_type>                                                          //ע���¼�
	void registerEvent(Object* obj, _func_type func)                                     //
	{                                                                                    //
		eventManager.associate(obj, func);                                               //
	}                                                                                    //
	template <class _func_type>                                                          //ע���¼�
	void unregisterEvent(Object* obj, _func_type func)                                   //
	{                                                                                    //
		eventManager.disAssociate(obj, func);                                            //
	}                                                                                    //
	void cleanEvents();                                                                  //��������¼�
	void raiseEvent(EventMsg msg);                                                       //�׳�һ���¼�
	void raiseMouseEvent(mouseEvent _event);                                             //����/�׳�һ������¼�
	void registerKey(int key);                                                           //ע������¼�
	void getKeyState();                                                                  //��������¼�
	void tickTimers();                                                                   //�����ʱ��
	void addTimer(timer* _timer);                                                        //����һ����ʱ��
	void cleanAllTimers();                                                               //ɾ�����м�ʱ��
	void start();                                                                        //��ʼ��Ϸ���趨����flag������Ϸ�̵߳���һ��ѭ��������Ϸ��
	void CheckFlag();                                                                    //���FLAG�Ƿ��ñ仯��Ϊʲô����ô�����أ�http://blog.csdn.net/c__allen/article/details/17687513��
	void reset();                                                                        //����
	void end();                                                                          //������Ϸ�������ת״̬
	void quit();                                                                         //�˳���Ϸ
	void pause();                                                                        //��ͣ
	void resume();                                                                       //����
	void update();                                                                       //���£�ÿ֡����һ�Σ�
	RunningState getState();                                                             //���״̬
	void prepareTextures(std::vector<std::string> names);                                //׼������
	GLuint getATex(std::string name);                                                    //���һ�������ID
	void setInitScene(std::string scene);                                                //������������
	std::string getInitScene();                                                          //�����������
	void printTextAtPoint(Text& _t);                                                     //��ָ��λ���������
	void printTexts();                                                                   //��ָ��λ���������(�˳�����Ӧ�ɳ�������)
	void printTexts(std::vector<Text>& texts);                                           //��ָ��λ���������(�˳�����Ӧ�ɳ�������)
	std::vector<Text>& getTexts();                                                       //�����Ҫ��ӡ����Ļ�ϵ�����
private:
	friend class socketHandler;
	bool SceneUpdated;
	bool KeyUpdated;
	float FPS;                                                                           //֡��
	float dt;                                                                            //ÿһ֡��ʱ��(1s/FPS)
	volatile bool shouldStart;                                                           //�趨Ӧ��������Flag
	volatile bool shouldStop;                                                            //�趨Ӧ��ֹͣ��Flag
	volatile bool shouldPause;                                                           //�趨Ӧ����ͣ��Flag
	volatile bool shouldResume;                                                          //�趨Ӧ�ü�����Flag
	void _start();                                                                       //��������������
	void _end();                                                                         //������Ϸ�������ת״̬
	void _pause();                                                                       //��ͣ
	void _resume();                                                                      //����
	void _loadScenes();                                                                  //���볡��
	bool inited;                                                                         //��ʼ����־
	static Director* theInstance;                                                        //Ψһʵ��
	std::map<std::string, Scene*> _scenes;                                               //���г���
	Scene* currentScene;                                                                 //ָ��ǰ������ָ��
	std::vector<int> _keyList;                                                           //�����İ����б�
	POINT _mousepos;                                                                     //����λ��
	int lastLBUTTONState;                                                                //�ϴε����״̬�����ж����϶�����̧�����ƶ�
	std::vector<timer*> timers;                                                          //��ʱ��
	std::vector<timer*> timerBuffer;                                                     //��ʱ�������
	std::vector<Sprite*> spriteBuffer;                                                   //���黺���
	Event<void, EventMsg> eventManager;                                                  //�¼�������
	volatile RunningState _state;                                                        //��Ϸ״̬
	Clock _clock;                                                                        //ʱ��
	std::string _initScene;                                                              //��������
	std::vector<Text> _texts;                                                            //��Ҫ��ӡ������
};
#endif//__DIRECTOCR__H__