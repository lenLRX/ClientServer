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
	int capacity;                                                        //����
	float factor;                                                          //������������
};

#endif//__BUFFERPOOL__H__