#ifndef __SPRITEPOOL__H__
#define __SPRITEPOOL__H__
#include "sprite.h"
#include <vector>

class SpritePool
{
public:
	SpritePool();
	~SpritePool();
	static void create();
	static SpritePool* getTheInstance();
	Sprite* getSprite();                                                 //永远不应该delete这些
	void reset();                                                        
private:
	static SpritePool* theInstance;
	void expand();
	std::vector<Sprite*> sprites;
	int index;
	int end;
	int capacity;                                                        //容量
	float factor;                                                          //容量增长因子
};

#endif