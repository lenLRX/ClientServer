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
	virtual void init();                                                               //初始化场景
	virtual void clean();                                                              //清理场景
	virtual void update(float dt);                                                     //更新函数，需要每一帧调用
	virtual void pause();                                                              //暂停
	virtual void resume();                                                             //继续
	virtual void onKeyPressedCallback(int player,int key);                                        //按键回调函数
	virtual bool sceneMouseEvent(mouseEvent _event);                                   //场景鼠标事件处理
	void setname(std::string name);                                                    //设置名字
	std::string getname();                                                             //获取名字
	int addchild(Sprite* child);                                                       //加入精灵 返回该精灵在本场景中的ID
	void addChildWithoutID(Sprite* child);                                             //加入精灵 不改变其ID
	void removeChildByID(int id);                                                      //删除精灵
	Sprite* getChildByID(int id);                                                      //获得精灵
	void handleMouseEvent(Point pt);                                                   //响应鼠标事件
	std::vector<Sprite*>& getSpriteList();                                             //获取精灵列表
	void registerMouseEvent(Sprite* _sprite);                                          //注册鼠标事件
	void processMouseEvent(mouseEvent _event);                                         //处理鼠标事件
	void clearKeys();                                                                  //清空
protected:
	friend class Director;
	bool pausing;                                                                      //是否正在暂停
	std::string _name;                                                                 //每个场景唯一的名字
	std::vector<Sprite*> _sprites;                                                     //保存本场景的精灵们
	std::vector<Sprite*> _mouseEventList;                                              //注册了鼠标事件的精灵们
	std::vector<Sprite*> _keyBoardEventList;                                           //注册了键盘事件的精灵
	int _keys[MAXPLAYERNUMBER][256];                                                   //按键状态
	int id;                                                                            //用来标记每个精灵
	bool mutiplayer;                                                                   //是否支持多人游戏
};
#endif//__SCENE__H__