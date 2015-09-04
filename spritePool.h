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
	Sprite* getSprite();                                                 //��Զ��Ӧ��delete��Щ
	void reset();                                                        
private:
	static SpritePool* theInstance;
	void expand();
	std::vector<Sprite*> sprites;
	int index;
	int end;
	int capacity;                                                        //����
	float factor;                                                          //������������
};

#endif