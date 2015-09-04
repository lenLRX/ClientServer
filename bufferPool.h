#ifndef __BUFFERPOOL__H__
#define __BUFFERPOOL__H__
#include <vector>

struct bufferList
{
	char msg[1024];
	char lable[32];
	bufferList* next;
};


class BufferPool
{
public:
	BufferPool();
	~BufferPool();
	static void create();
	static BufferPool* getTheInstance();
	void reset();
	bufferList* getBuffer();
private:
	static BufferPool* theInstance;
	void expand();
	std::vector<bufferList *> buffers;
	int index;
	int end;
	int capacity;                                                        //容量
	float factor;                                                          //容量增长因子
};

#endif//__BUFFERPOOL__H__