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
extern volatile struct sockaddr_in saServer, saClient; //��ַ��Ϣ
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
	static void create();                                                         //����ʵ��
	static socketHandler* getTheInstance();                                       //����Ψһʵ��
	void doSth();                                                                 //DoSomeThing
	void serverInitWithPics(std::vector<std::string> _names);                     //��ʼ��socket
	void serverInitWithMusics(std::vector<std::string> _MusicNames);              //��ʼ�������ļ�
	void serverTransferMsg();                                                     //���;���
	void setSocketState(SocketState state);                                       //����Socket״̬
	void clientOnKeyPressed(int key);                                             //�ͻ��˷���������Ϣ
	void clientMouseEvent(mouseEvent _event);                                     //�ͻ��˷��������Ϣ
	SocketState getSocketState();                                                 //����Socket״̬
	void serverRemoveSpriteByID(int _id);                                         //sever����ɾ�����������
	void setNetworkMode(NetworkMode mode);                                        //��������״̬
	NetworkMode getNetworkMode();                                                 //��ȡ����״̬
	void setServerIP(std::string IP);                                             //����IP
	std::string getServerIP();                                                    //��ȡIP
	void setServerTextBuffer(std::vector<Text>& buffer);                          //����Ҫ���͵�����
	std::vector<Sprite*>& getSpriteBuffer();                                      //��ȡ���黺��
	std::vector<Text>& getTextBuffer();                                           //��ȡ���ֻ���
	void serverSetNetworkConfig();                                                //
	void serverRegisterKeys(std::vector<int> keys);
private:
	friend void initClientSocket();
	friend void initServerSocket();
	friend void cleanSocket();
	friend class AudioEngine;
	inline void swapBuffer()                                                      //��������
	{
		bufferMutex.lock();
		SpriteBuffer1.swap(SpriteBuffer2);
		TextBuffer1.swap(TextBuffer2);
		bufferMutex.unlock();
	}
	void clientAddSprite(sprite_Lite _sprite);
	void printTextAtPoint(Text _t);
	void cleanBuffer2();
	void clientConnectToServer(std::string IP);                                   //���ӵ�ip
	void serverWaitForClient();                                                   //�ȴ��ͻ�����
	void clientProcessMsg();                                                      //�ͻ��˴�����Ϣ
	void serverProcessMsg();                                                      //������������Ϣ
	void clientPrepareTex();                                                      //�ͻ���׼������
	void clientsendKeyMouseMsg();                                                 //�ͻ��˷��Ͱ�����Ϣ
	bool serverAskClientPrepareTex();                                             //Ҫ��ͻ���׼����������
	void serverAskClientPrepareMusic();                                           //Ҫ��ͻ���׼������
	bool serverAskClientForTex(std::string picName);                              //Ҫ��ͻ���׼��ĳһ������
	bool serverAskClientForMusic(std::string MusicName);                          //Ҫ��ͻ���׼��ĳһ������
	void sendMsg(MsgType type, void* buffer, int lenth);                          //������Ϣ
	bufferList* recvMsg();//������Ϣ
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
	volatile NetworkMode _networkMode;      //����״̬
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