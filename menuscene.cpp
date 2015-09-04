#include "stdafx.h"
#include "menuscene.h"
#include "button.h"
#include <stdlib.h>
#include <time.h>
using namespace std;

menuScene::menuScene(string name) :Scene(name)
{

}

menuScene::~menuScene()
{

}

void menuScene::init()
{
	srand(time(NULL));
	Sprite* S1 = new Sprite("HelloWorld.png", 0);   //创建背景精灵
	Button* btn = new Button("CloseNormal.png", 1);  //创建按钮
	S1->setflipY(true);                             //设置左右翻转
	S1->setsize(WINDOW_WIDTH, WINDOW_HEIGHT);
	S1->setpos(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
	addchild(btn);
	addchild(S1);
	btn->setsize(100, 100);
	btn->setpos(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
	btn->setType(CIRCLE);
	btn->setRadius(50);
	btn->registerEvent(this, &menuScene::gotoNextScene, "NextScene");
	testbar = new Bar(this, "bar2.png", "bar1.png", "testbar",1);
	testbar->setPos(200, 200);
	testbar->setSize(300, 50);
	
	registerMouseEvent(btn);
}

void menuScene::clean()
{
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
}

void menuScene::update(float dt)
{
	EventMsg msg;
	msg.name = "testbar";
	msg.param = (float)rand() / RAND_MAX;
	if (msg.param < 0.2)
		msg.param = 0.2;
	Director::getTheInstance()->raiseEvent(msg);
}

void menuScene::gotoNextScene(EventMsg msg)
{
	if (msg.name == "NextScene")
	{
		Director::getTheInstance()->gotoScene("test");
	}
}