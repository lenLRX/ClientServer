#ifndef __SOCKETHANDLER__H__
#define __SOCKETHANDLER__H__
#include "datastruct.h"
#include "sprite.h"
#include "bufferPool.h"
#include <al.h>
#include <winsock2.h>
#include <vector>
#include <map>
#include <string>
#include <mutex>
#pragma comment(lib, "ws2_32.lib")
//using namespace std;

extern volatile SOCKET sFirst, sSecond;
extern int ret, nLeft, length;
extern volatile struct sockaddr_in saServer, saClient; //地址信息
extern std::mutex bufferMutex;
const int recvBufferLenth=8000;

enum MsgType
{
	PING,
	SPRITE,
	COMMAND,
	PREPARE_SPRITE,
	SPRITE_NAME,
	SPRITE_TEX,
	FLUSH,
	REMOVESPRITE,
	TEXT,
	BEGIN,
	END,
	REGISTERKEY,
	KEY,
	MOUSE,
	CONFIGBEGIN,
	CONFIGEND,
	MUSIC_NAME,
	MUSIC_BUF,
	MUSIC
};

enum SocketState
{
	LISTENING,
	CONNECTING,
	CONNECTING_NOT_INITED,
	CONNECTING_INITED
};

enum NetworkMode
{
	SINGLE,
	SERVER,
	CLIENT
};

class socketHandler
{
public:
	socketHandler();
	static void create();                                                         //创建实例
	static socketHandler* getTheInstance();                                       //返回唯一实例
	void doSth();                                                                 //DoSomeThing
	void serverInitWithPics(std::vector<std::string> _names);                     //初始化socket
	void serverInitWithMusics(std::vector<std::string> _MusicNames);              //初始化声音文件
	void serverTransferMsg();                                                     //发送精灵
	void setSocketState(SocketState state);                                       //设置Socket状态
	void clientOnKeyPressed(int key);                                             //客户端反馈按键信息
	void clientMouseEvent(mouseEvent _event);                                     //客户端反馈鼠标信息
	SocketState getSocketState();                                                 //返回Socket状态
	void serverRemoveSpriteByID(int _id);                                         //sever发出删除精灵的命令
	void setNetworkMode(NetworkMode mode);                                        //设置网络状态
	NetworkMode getNetworkMode();                                                 //获取网络状态
	void setServerIP(std::string IP);                                             //设置IP
	std::string getServerIP();                                                    //获取IP
	void setServerTextBuffer(std::vector<Text>& buffer);                          //设置要发送的文字
	std::vector<Sprite*>& getSpriteBuffer();                                      //获取精灵缓存
	std::vector<Text>& getTextBuffer();                                           //获取文字缓存
	void serverSetNetworkConfig();                                                //
	void serverRegisterKeys(std::vector<int> keys);
private:
	friend void initClientSocket();
	friend void initServerSocket();
	friend void cleanSocket();
	friend class AudioEngine;
	inline void swapBuffer()                                                      //交换缓存
	{
		bufferMutex.lock();
		SpriteBuffer1.swap(SpriteBuffer2);
		TextBuffer1.swap(TextBuffer2);
		bufferMutex.unlock();
	}
	void clientAddSprite(sprite_Lite _sprite);
	void printTextAtPoint(Text _t);
	void cleanBuffer2();
	void clientConnectToServer(std::string IP);                                   //连接到ip
	void serverWaitForClient();                                                   //等待客户连接
	void clientProcessMsg();                                                      //客户端处理消息
	void serverProcessMsg();                                                      //服务器处理消息
	void clientPrepareTex();                                                      //客户端准备纹理
	void clientsendKeyMouseMsg();                                                 //客户端发送按键消息
	bool serverAskClientPrepareTex();                                             //要求客户端准备所有纹理
	void serverAskClientPrepareMusic();                                           //要求客户端准备声音
	bool serverAskClientForTex(std::string picName);                              //要求客户端准备某一个纹理
	bool serverAskClientForMusic(std::string MusicName);                          //要求客户端准备某一个声音
	void sendMsg(MsgType type, void* buffer, int lenth);                          //发送消息
	bufferList* recvMsg();//接收消息
	bufferList* processMsg(char* input, int lenth);
	void* memprint(void* dest, const void* src, size_t lenth, std::string name, int* size);
	static socketHandler* theInstance;
	std::map<std::string, MsgType> str2type;
	std::map<MsgType, std::string> type2str;
	std::map<std::string, GLuint> name2Tex;
	std::map<std::string, ALuint> name2Sound;
	SocketState _state;
	std::vector<std::string> PicNames;
	std::vector<std::string> MusicNames;
	volatile NetworkMode _networkMode;      //网络状态
    std::string serverIP;
	std::vector<Sprite*> SpriteBuffer1;
	std::vector<Sprite*> SpriteBuffer2;
	std::vector<Text> TextBuffer1;
	std::vector<Text> TextBuffer2;
	std::mutex ServerTextBufferMutex;
	std::vector<Text> ServerTextBuffer;
	std::vector<int> _keys;
	mouseEvent _mouseevent;
	char input[recvBufferLenth];
	int inputIndex;
};

#endif//__SOCKETHANDLER__H__