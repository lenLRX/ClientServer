#include "stdafx.h"
#include "director.h"
#include "socketHandler.h"
#include "testscene1.h"
#include "testscene2.h"
#include "menuscene.h"
#include "simplegame1.h"
#include "simplegame2.h"
#include "simplegame3.h"
#include "printText.h"
#include <mutex>
using namespace std;
mutex mtx;
extern mutex SceneMutex;
extern volatile bool shouldRun;
extern void initDrawing();
extern bool openglInited;
mutex DirectorMutex;
Director* Director::theInstance = NULL;

Director::Director() :\
inited(false), lastLBUTTONState(0), currentScene(NULL), _state(SLEEPING)\
, shouldStart(false), shouldStop(false), shouldPause(false), shouldResume(false), FPS(60.0f)
{
	dt = 1 / FPS;
	_clock.start();
}

void Director::create()
{
	if (!theInstance)
	    theInstance = new Director();
}

void Director::startWithScene(string name)
{
	currentScene = _scenes[name];
	currentScene->init();
	if (currentScene->mutiplayer)
	{
		if (socketHandler::getTheInstance()->getNetworkMode() == SERVER)
		{
			if (socketHandler::getTheInstance()->getSocketState() == CONNECTING_INITED)
			{
				socketHandler::getTheInstance()->setSocketState(CONNECTING_NOT_INITED);
			}
		}
	}
	inited = true;
	_state = RUNNING;
}

Director* Director::getTheInstance()
{
	return theInstance;
}

void Director::gotoScene(string name)
{
	_state = RUNNING;
	if (currentScene)
		currentScene->clean();
	cleanEvents();
	cleanAllTimers();
	currentScene = _scenes[name];
	if (!currentScene)
	{
		_scenes.clear();
		_loadScenes();
		currentScene = _scenes[name];
	}
	currentScene->init();
	if (currentScene->mutiplayer)
	{
		if (socketHandler::getTheInstance()->getNetworkMode() == SERVER)
		{
			if (socketHandler::getTheInstance()->getSocketState() == CONNECTING_INITED)
			{
				socketHandler::getTheInstance()->setSocketState(CONNECTING_NOT_INITED);
			}
		}
	}
}


Scene* Director::getCurrentScene()
{
	return currentScene;
}

int Director::getTotalScene()
{
	return _scenes.size();
}

void Director::addScene(Scene* theScene)
{
	_scenes.insert(pair<string, Scene*>(theScene->getname(), theScene));
}

void Director::setMousePos(POINT mousepos)
{
	_mousepos = mousepos;
	if (_mousepos.x < 0)
		_mousepos.x = 0;
	if (_mousepos.y < 0)
		_mousepos.y = 0;
	if (_mousepos.x > WINDOW_WIDTH)
		_mousepos.x = WINDOW_WIDTH;
	if (_mousepos.y > WINDOW_HEIGHT)
		_mousepos.y = WINDOW_HEIGHT;
	_mousepos.y = WINDOW_HEIGHT - _mousepos.y;
}

Point Director::getMousePos()
{
	return POINT2Point(_mousepos);
}

void Director::registerKey(int key)
{
	_keyList.push_back(key);
}

void Director::getKeyState()
{
	if (_keyList.size())
	{
		for (vector<int>::iterator it = _keyList.begin(); it != _keyList.end(); it++)
		{
			if (GetAsyncKeyState((*it)) & 0x8000)
			{
				if (socketHandler::getTheInstance()->getNetworkMode() == CLIENT)
				{
					socketHandler::getTheInstance()->clientOnKeyPressed((*it));
				}
				else
				{
					getCurrentScene()->onKeyPressedCallback(0, (*it));
				}
			}
		}
	}
	int LBUTTONState = GetAsyncKeyState(VK_LBUTTON) & 0x8000;
	mouseEvent _event;
	
	if (LBUTTONState)
	{
		if (lastLBUTTONState)
		{
			_event.type = DRAG;
		}
		else
		{
			_event.type = DOWN;
		}
	}
	else
	{
		if (lastLBUTTONState)
		{
			_event.type = UP;
		}
		else
		{
			_event.type = MOVE;
		}
	}
	_event.point = POINT2Point(_mousepos);
	if (_event.point.x > 0 && _event.point.y>0 && _event.point.x<WINDOW_WIDTH && _event.point.y<WINDOW_HEIGHT)
	{
		lastLBUTTONState = LBUTTONState;
		EventMsg msg;
		msg._event = _event;
		msg.param = 0;
		raiseEvent(msg);
		raiseMouseEvent(_event);
		if (socketHandler::getTheInstance()->getNetworkMode() == CLIENT)
		{
			socketHandler::getTheInstance()->clientMouseEvent(_event);
		}
	}
	KeyUpdated = false;
}

void Director::tickTimers()
{
	if (timerBuffer.size())                                    //把缓冲池中的计时器加入到timers中（并不影响计时器精度）
	{
		vector<timer*>::iterator it = timerBuffer.begin();
		while (it != timerBuffer.end())
		{
			timers.push_back((*it));
			it++;
		}
	}
	timerBuffer.clear();
	if (timers.size())
	{
		vector<timer*>::iterator it = timers.begin();
		while (it != timers.end())
		{
			if (!(*it)->isRuning())                       //如果计时器已经触发就删除它
			{
				delete (*it);
				it = timers.erase(it);
			}
			else                                          //如果计时器正在运行就滴答一下
			{
				
				(*it)->tick();
				++it;
			}
		}
	}
}

void Director::addTimer(timer* _timer)  
{
	_timer->start();
	//timers.push_back(_timer);不能在容器正在迭代的时候插入元素
	timerBuffer.push_back(_timer);
}

void Director::cleanAllTimers()
{
	if (timers.size())
	{
		vector<timer*>::iterator it = timers.begin();
		while (it != timers.end())
		{
		    delete (*it);
			it++;
		}
		timers.clear();
	}
	if (timerBuffer.size())
	{
		vector<timer*>::iterator it = timerBuffer.begin();
		while (it != timerBuffer.end())
		{
			delete (*it);
			it++;
		}
		timerBuffer.clear();
	}
}

void Director::reset()
{
	//TODO:增加清理代码
	cleanAllTimers();
	cleanEvents();
	if (_scenes.size())
	{
		for (map<string, Scene*>::iterator it = _scenes.begin(); it != _scenes.end();)
		{
			delete (*it).second;
			it = _scenes.erase(it);
		}
	}
	currentScene = NULL;
	_keyList.clear();
	_state = SLEEPING;
}

void Director::cleanEvents()
{
	eventManager.clean();
}

void Director::raiseEvent(EventMsg msg)
{
	eventManager.sendEvent(msg);
}

void Director::raiseMouseEvent(mouseEvent _event)
{
	getCurrentScene()->processMouseEvent(_event);
}

Point POINT2Point(POINT pt)
{
	Point pt2;
	pt2.x = pt.x;
	pt2.y = pt.y;
	return pt2;
}

RunningState Director::getState()
{
	return _state;
}

void Director::start()
{
	//if (_state==SLEEPING)
	    shouldStart = true;
}

void Director::CheckFlag()
{
	if (shouldStart)
	{
		_start();
	}
	if (shouldStop)
	{
		_end();
	}
	if (shouldPause)
	{
		_pause();
	}
	if (shouldResume)
	{
		_resume();
	}
}

void Director::_start()
{
	if (currentScene)
		reset();
	_loadScenes();
	shouldStart = false;
	_state = RUNNING;
	Director::getTheInstance()->startWithScene(_initScene);
}

void Director::end()
{
	shouldStop = true;
}

void Director::prepareTextures(vector<string> names)
{
	if (names.size() > 0)
	{
		for (vector<string>::iterator it = names.begin(); it != names.end(); it++)
		{
			Sprite s((*it), 0);
		}
	}
}

GLuint Director::getATex(string name)
{
	for (vector<Texture>::iterator it = Textures.begin(); it != Textures.end(); it++)
	{
		if (name == (*it).name)
		{
			return (*it).Tex;
		}
	}
	return 0;
}

void Director::_end()
{
	shouldStop = false;
	reset();
	_state = SLEEPING;
}

void Director::quit()
{
	shouldRun = false;
}

void Director::pause()
{
	shouldPause = true;
}

void Director::_pause()
{
	shouldPause = false;
	if (timers.size())
	{
		for (vector<timer*>::iterator it = timers.begin(); it != timers.end(); ++it)
		{
			(*it)->pause();
		}
	}
	if (timerBuffer.size())
	{
		for (vector<timer*>::iterator it = timerBuffer.begin(); it != timerBuffer.end(); ++it)
		{
			(*it)->pause();
		}
	}
	currentScene->pause();
	dt = 0.0f;
}

void Director::resume()
{
	shouldResume = true;
}

void Director::_resume()
{
	shouldResume = false;
	if (timers.size())
	{
		for (vector<timer*>::iterator it = timers.begin(); it != timers.end(); ++it)
		{
			(*it)->resume();
		}
	}
	if (timerBuffer.size())
	{
		for (vector<timer*>::iterator it = timerBuffer.begin(); it != timerBuffer.end(); ++it)
		{
			(*it)->resume();
		}
	}
	currentScene->resume();
	dt = 1 / FPS;
}

void Director::update()
{
	currentScene->update(dt);
	SceneUpdated = false;
}

void Director::_loadScenes()
{
	///////////////////////////////////////////
	//在此加载场景
	Scene* p = (Scene*)new testscene1("test1");
	Scene* p5 = (Scene*)new testscene2("test2");
	Scene* p2 = (Scene*)new menuScene("menu");
	Scene* p3 = (Scene*)new simplegame1("game1");
	Scene* p4 = (Scene*)new simplegame2("game2");
	Scene* p7 = (Scene*)new simplegame3("game3");
	Scene* p6 = new Scene("default");
	
	Director::getTheInstance()->addScene(p);
	Director::getTheInstance()->addScene(p2);
	Director::getTheInstance()->addScene(p3);
	Director::getTheInstance()->addScene(p4);
	Director::getTheInstance()->addScene(p5);
	Director::getTheInstance()->addScene(p6);
	Director::getTheInstance()->addScene(p7);
}

void Director::setInitScene(std::string scene)
{
	_initScene = scene;
}

std::string Director::getInitScene()
{
	return _initScene;
}

void Director::printTextAtPoint(Text& _t)
{
	DirectorMutex.lock();
	_texts.push_back(_t);
	DirectorMutex.unlock();
}

void Director::printTexts()
{
	DirectorMutex.lock();
	vector<Text>::iterator end = _texts.end();
	for (vector<Text>::iterator it = _texts.begin();it!=end ; )
	{
		Point pt = (*it).pt;
		glRasterPos2f(pt.x / WINDOW_WIDTH * 2.0f - 1.0f, pt.y / WINDOW_HEIGHT * 2.0f - 1.0f);
		glPrint((*it).str);
		++it;
	}
	if (socketHandler::getTheInstance()->getNetworkMode() == SERVER)
	{
		socketHandler::getTheInstance()->setServerTextBuffer(_texts);
	}
	_texts.clear();
	DirectorMutex.unlock();
}

void Director::printTexts(vector<Text>& texts)
{
	vector<Text>::iterator end = texts.end();
	for (vector<Text>::iterator it = texts.begin(); it != end;)
	{
		Point pt = (*it).pt;
		glRasterPos2f(pt.x / WINDOW_WIDTH * 2.0f - 1.0f, pt.y / WINDOW_HEIGHT * 2.0f - 1.0f);
		glPrint((*it).str);
		++it;
	}
}

vector<Text>&  Director::getTexts()
{
	return _texts;
}