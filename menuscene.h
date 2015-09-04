#ifndef __MENUSCENE__H__
#define __MENUSCENE__H__
#include "scene.h"
#include "bar.h"
class menuScene :public Scene
{
public:
	menuScene() = delete;
	menuScene(std::string name);
	~menuScene();
	virtual void init();
	virtual void update(float dt);
	virtual void clean();
	void gotoNextScene(EventMsg msg);
private:
	Bar* testbar;
};
#endif//__MENUSCENE__H__