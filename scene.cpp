#include "stdafx.h"
#include "scene.h"
#include "socketHandler.h"
#include <stdlib.h>
#include <cassert>
#include <mutex>
#include <iostream>
#include <stdio.h>
using namespace std;
mutex SceneMutex;
Scene::Scene(string name) :_name(name), id(0), mutiplayer(false)
{

}

Scene::~Scene()
{

}

void Scene::init()
{
	;
}

void Scene::clean()
{
	;
}

void Scene::update(float dt)
{
	;
}

void Scene::pause()
{
	pausing = true;
}

void Scene::resume()
{
	pausing = false;
}

bool Scene::sceneMouseEvent(mouseEvent _event)
{
	return false;
}

void Scene::setname(string name)
{
	_name = name;
}

string Scene::getname()
{
	return _name;
}

int Scene::addchild(Sprite* child)
{
	id++;
	child->setid(id);
	SceneMutex.lock();
	_sprites.push_back(child);
	SceneMutex.unlock();
	return id;
}

void Scene::addChildWithoutID(Sprite* child)
{
	//SceneMutex.lock();
	_sprites.push_back(child);
	//SceneMutex.unlock();
}


void Scene::removeChildByID(int id)
{
	SceneMutex.lock();
	if (_sprites.size())
	{
		vector<Sprite*>::iterator it = _sprites.begin();
		while (it != _sprites.end())
		{
			if ((*it)->getid() == id)
			{
				it= _sprites.erase(it);
			}
			else
			{
				it++;
			}
		}
	}
	
	if (_keyBoardEventList.size())
	{
		vector<Sprite*>::iterator it = _keyBoardEventList.begin();
		while (it != _keyBoardEventList.end())
		{
			if ((*it)->getid() == id)
			{
				it = _keyBoardEventList.erase(it);
			}
			else
			{
				it++;
			}
		}
	}
	
	if (_mouseEventList.size())
	{
		vector<Sprite*>::iterator it = _mouseEventList.begin();
		while (it != _mouseEventList.end())
		{
			if ((*it)->getid() == id)
			{
				it = _mouseEventList.erase(it);
			}
			else
			{
				it++;
			}
		}
	}
	
	SceneMutex.unlock();
}

Sprite* Scene::getChildByID(int id)
{
		//SceneMutex.lock();
		if (_sprites.size())
		{
			for (vector<Sprite*>::iterator it = _sprites.begin(); it != _sprites.end(); it++)
			{
				if (id == (*it)->getid())//我们找到了对应ID的精灵
				{
					//SceneMutex.unlock();
					return (*it);
				}
					
			}
		}
		//SceneMutex.unlock();
		return NULL;
}

void Scene::handleMouseEvent(Point pt)
{
	;
}

vector<Sprite*>& Scene::getSpriteList()
{
	return _sprites;
}

void Scene::registerMouseEvent(Sprite* _sprite)
{
	_mouseEventList.push_back(_sprite);
}

void Scene::processMouseEvent(mouseEvent _event)
{
	if (sceneMouseEvent(_event))
	{
		return;
	}
	if (_mouseEventList.size())
	{
		for (vector<Sprite*>::iterator it = _mouseEventList.begin(); it != _mouseEventList.end(); it++)
		{
			if ((*it)->mouseEventCallBack(_event))
				return;                                 //当mouseEventCallBack返回值为真时，判断消息已经被处理完毕
		}
	}
}

void Scene::onKeyPressedCallback(int player,int key)
{
	SceneMutex.lock();
	assert(key < 256 && player < MAXPLAYERNUMBER);
	_keys[player][key]=1;
	SceneMutex.unlock();
}

void Scene::clearKeys()
{
	SceneMutex.lock();
	for (int j = 0; j < MAXPLAYERNUMBER; j++)
	{
		for (int i = 0; i < 256; i++)
		{
			_keys[j][i] = 0;
		}
	}
	SceneMutex.unlock();
}