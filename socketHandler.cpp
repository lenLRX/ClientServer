#include "stdafx.h"
#include "socketHandler.h"
#include "audioEngine.h"
#include "director.h"
#include "printText.h"
#include "datastruct.h"
#include "spritePool.h"
#include <stdlib.h>
#include <cassert>
#include <mutex>
using namespace std;

extern void initClientSocket();

extern void initServerSocket();

extern void cleanSocket();

socketHandler* socketHandler::theInstance = NULL;

extern mutex SceneMutex;

mutex SocketMutex;

mutex bufferMutex;

socketHandler::socketHandler() : _networkMode(SINGLE), inputIndex(0)
{
	str2type.insert(pair<string, MsgType>("PING", PING));
	str2type.insert(pair<string, MsgType>("SPRITE", SPRITE));
	str2type.insert(pair<string, MsgType>("COMMAND", COMMAND));
	str2type.insert(pair<string, MsgType>("PREPARE_SPRITE", PREPARE_SPRITE));
	str2type.insert(pair<string, MsgType>("SPRITE_NAME", SPRITE_NAME));
	str2type.insert(pair<string, MsgType>("SPRITE_TEX", SPRITE_TEX));
	str2type.insert(pair<string, MsgType>("FLUSH", FLUSH));
	str2type.insert(pair<string, MsgType>("REMOVESPRITE", REMOVESPRITE));
	str2type.insert(pair<string, MsgType>("TEXT", TEXT));
	str2type.insert(pair<string, MsgType>("BEGIN", BEGIN));
	str2type.insert(pair<string, MsgType>("END", END));
	str2type.insert(pair<string, MsgType>("REGISTERKEY", REGISTERKEY));
	str2type.insert(pair<string, MsgType>("KEY", KEY));
	str2type.insert(pair<string, MsgType>("MOUSE", MOUSE));
	str2type.insert(pair<string, MsgType>("CONFIGBEGIN", CONFIGBEGIN));
	str2type.insert(pair<string, MsgType>("CONFIGEND", CONFIGEND));
	str2type.insert(pair<string, MsgType>("MUSIC_NAME", MUSIC_NAME));
	str2type.insert(pair<string, MsgType>("MUSIC_BUF", MUSIC_BUF));
	str2type.insert(pair<string, MsgType>("MUSIC", MUSIC));
	type2str.insert(pair<MsgType, string>(PING, "PING"));
	type2str.insert(pair<MsgType, string>(SPRITE, "SPRITE"));
	type2str.insert(pair<MsgType, string>(COMMAND, "COMMAND"));
	type2str.insert(pair<MsgType, string>(PREPARE_SPRITE, "PREPARE_SPRITE"));
	type2str.insert(pair<MsgType, string>(SPRITE_NAME, "SPRITE_NAME"));
	type2str.insert(pair<MsgType, string>(SPRITE_TEX, "SPRITE_TEX"));
	type2str.insert(pair<MsgType, string>(FLUSH, "FLUSH"));
	type2str.insert(pair<MsgType, string>(REMOVESPRITE, "REMOVESPRITE"));
	type2str.insert(pair<MsgType, string>(TEXT, "TEXT"));
	type2str.insert(pair<MsgType, string>(BEGIN, "BEGIN"));
	type2str.insert(pair<MsgType, string>(END, "END"));
	type2str.insert(pair<MsgType, string>(REGISTERKEY, "REGISTERKEY"));
	type2str.insert(pair<MsgType, string>(KEY, "KEY"));
	type2str.insert(pair<MsgType, string>(MOUSE, "MOUSE"));
	type2str.insert(pair<MsgType, string>(CONFIGBEGIN, "CONFIGBEGIN"));
	type2str.insert(pair<MsgType, string>(CONFIGEND, "CONFIGEND"));
	type2str.insert(pair<MsgType, string>(MUSIC_NAME, "MUSIC_NAME"));
	type2str.insert(pair<MsgType, string>(MUSIC_BUF,"MUSIC_BUF"));
	type2str.insert(pair<MsgType, string>(MUSIC,"MUSIC"));
	setServerIP("127.0.0.1");
}

void socketHandler::create()
{
	if (!theInstance)
		theInstance = new socketHandler();
}

socketHandler* socketHandler::getTheInstance()
{
	return theInstance;
}

void socketHandler::doSth()
{
	//TODO
	SocketMutex.lock();
	switch (_networkMode)
	{
	case CLIENT:
		switch (_state)
		{
		case CONNECTING:
			clientConnectToServer(serverIP);
			break;
		case CONNECTING_NOT_INITED:
			clientPrepareTex();
			break;
		case CONNECTING_INITED:
			clientProcessMsg();
			if (Director::getTheInstance()->KeyUpdated == false)
			{
				Director::getTheInstance()->KeyUpdated = true;
				clientsendKeyMouseMsg();
			}
			break;
		}
		break;
	case SERVER:
		switch (_state)
		{
		case LISTENING:
			serverWaitForClient();
			break;
		case CONNECTING_NOT_INITED:
			serverRegisterKeys(Director::getTheInstance()->_keyList);
			serverAskClientPrepareMusic();
			serverAskClientPrepareTex();
			break;
		case CONNECTING_INITED:
			if (Director::getTheInstance()->getState() == RUNNING&&Director::getTheInstance()->SceneUpdated == false)
			{
				Director::getTheInstance()->SceneUpdated = true;
				serverTransferMsg();
			}
			serverProcessMsg();
			break;
		}
		break;
	case SINGLE:
		break;
	}
	SocketMutex.unlock();
}

void socketHandler::serverTransferMsg()
{
	SceneMutex.lock();
	Scene* thescene = Director::getTheInstance()->getCurrentScene();
	vector<Sprite*> _spriteList = thescene->getSpriteList();
	SceneMutex.unlock();
	vector<Sprite*>::iterator it = _spriteList.begin();
	vector<Sprite*>::iterator end = _spriteList.end();
	sendMsg(BEGIN, "_", strlen("_"));
	for (; it != end; ++it)
	{
		sendMsg(SPRITE, &( (*it)->getCore()), sizeof(sprite_Lite));
	}
	ServerTextBufferMutex.lock();
	vector<Text> _ServerTextBuffer = ServerTextBuffer;
	ServerTextBufferMutex.unlock();
	vector<Text>::iterator t_it = _ServerTextBuffer.begin();
	vector<Text>::iterator t_end = _ServerTextBuffer.end();
	for (; t_it != t_end; ++t_it)
	{
		sendMsg(TEXT, &(*t_it), sizeof(Text));
	}
	sendMsg(END, "_", strlen("_"));
}

void socketHandler::clientConnectToServer(string IP)
{
	saServer.sin_family = AF_INET; //地址家族
	saServer.sin_port = htons(SERVER_PORT); //注意转化为网络节序
	saServer.sin_addr.S_un.S_addr = inet_addr(IP.c_str());
	if(connect(sSecond, (struct sockaddr *)&saServer, sizeof(saServer))!= SOCKET_ERROR)
	{
		_state = CONNECTING_NOT_INITED;
	}
}

void socketHandler::clientPrepareTex()
{
	GLuint t;
	ALuint s;
	int key;
	char chr[20];
	bool shouldbreak = false;
	while (!shouldbreak)
	{
		bufferList* p = recvMsg();
		while (p)
		{
			switch (str2type[p->lable])
			{
			case COMMAND:
				shouldbreak = true;
				_state = CONNECTING_INITED;
				break;
			case SPRITE_NAME:
				t = Director::getTheInstance()->getATex(p->msg);
				sprintf(chr, "%d", t);
				sendMsg(SPRITE_TEX, chr, strlen(chr));
				break;
			case REGISTERKEY:
				key = atoi(p->msg);
				Director::getTheInstance()->registerKey(key);
				sprintf(chr, "%d", key);
				sendMsg(REGISTERKEY, chr, strlen(chr));
				break;
			case MUSIC_NAME:
				s = AudioEngine::getTheInstance()->getBuffer(p->msg);
				sprintf(chr, "%u", s);
				sendMsg(MUSIC_BUF, chr, strlen(chr));
				break;
			default:
				shouldbreak = true;
				break;
			}
			p = p->next;
		}
	}
}

void* socketHandler::memprint(void* dest, const void* src, size_t lenth, string name,int* size)
{
	assert((dest != NULL) && (src != NULL));
	unsigned char* pdest = (unsigned char*)dest;
	unsigned char* psrc = (unsigned char*)src;
	*pdest = '/';
	pdest++;
	(*size)++;
	for (int i = 0; i<name.size(); i++)
	{
		*pdest = name[i];
		pdest++;
		(*size)++;
	}
	*pdest = ' ';
	pdest++;
	(*size)++;
	while (lenth-->0)
	{
		*pdest = *psrc;
		pdest++;
		psrc++;
		(*size)++;
	}
	*pdest = ' ';
	pdest++;
	(*size)++;
	for (int i = 0; i<name.size(); i++)
	{
		*pdest = name[i];
		pdest++;
		(*size)++;
	}
	*pdest = '/';
	(*size)++;
	return dest;
}

bool socketHandler::serverAskClientForMusic(std::string MusicName)
{
	assert(MusicName.size() < 1000);
	bool shouldbreak = false;
	int maxRetry = 20;
	int i = 0;
	char buffer[1024] = { '\0' };
	sprintf(buffer, "/%s %s %s/", "MUSIC_NAME", MusicName.c_str(), "MUSIC_NAME");
	send(sSecond, buffer, 1024, 0);
	while (!shouldbreak)
	{
		i++;
		if (i>maxRetry)
		{
			return false;
		}
		bufferList* p = recvMsg();
		while (p)
		{
			if (strcmp(p->lable, "MUSIC_BUF") == 0)
			{
				ALuint buf;
				sscanf(p->msg, "%u", &buf);
				if (buf > 0)
				{
					name2Sound.insert(pair<string, ALuint>(MusicName, buf));
					shouldbreak = true;
					break;
				}
			}
			p = p->next;
		}
		Sleep(10);
	}
	return true;
}

void socketHandler::serverAskClientPrepareMusic()
{
	assert(MusicNames.size()>0);
	for (vector<string>::iterator it = MusicNames.begin(); it != MusicNames.end(); it++)
	{
		bool ret = false;
		while (!ret)
		{
			ret = serverAskClientForMusic((*it));
			Sleep(10);
		}
	}
	int a = 0;
}

bool socketHandler::serverAskClientPrepareTex()
{
	assert(PicNames.size() > 0);
	for (vector<string>::iterator it = PicNames.begin(); it != PicNames.end(); it++)
	{
		bool ret = false;
		while (!ret)
		{
			ret = serverAskClientForTex((*it));
		}
	}
	sendMsg(COMMAND, "_", strlen("_"));
	EventMsg msg;
	msg.name = "Client Connencted";
	Director::getTheInstance()->raiseEvent(msg);
	_state = CONNECTING_INITED;
	return true;
}

void socketHandler::serverRegisterKeys(std::vector<int> keys)
{
	//注册按键很少，效率不重要
	for (std::vector<int>::iterator it = keys.begin(); it != keys.end(); ++it)
	{
		bool shouldbreak = false;
		while (!shouldbreak)
		{
			char buffer[1024] = { '\0' };
			sprintf(buffer, "/%s %d %s/", "REGISTERKEY", (*it), "REGISTERKEY");
			send(sSecond, buffer, 1024, 0);
			bufferList* p = recvMsg();
			while (p)
			{
				if (strcmp(p->lable,"REGISTERKEY")==0)
				{
					int key = atoi(p->msg);
					if ( (*it)==key)
					{
						shouldbreak = true;
						break;
					}
				}
				p = p->next;
			}
			Sleep(10);
		}
	}
	
}

bool socketHandler::serverAskClientForTex(string picName)
{
	assert(picName.size() < 1000);
	bool shouldbreak = false;
	while (!shouldbreak)
	{
		char buffer[1024] = { '\0' };
		sprintf(buffer, "/%s %s %s/", "SPRITE_NAME", picName.c_str(), "SPRITE_NAME");
		send(sSecond, buffer, 1024, 0);
		bufferList* p = recvMsg();
		while (p)
		{
			if (strcmp(p->lable,"SPRITE_TEX")==0)
			{
				GLuint tex = atoi(p->msg);
				if (tex > 0)
				{
					name2Tex.insert(pair<string, GLuint>(picName, tex));
					shouldbreak = true;
					break;
				}
			}
			p = p->next;
		}
		Sleep(10);
	}
	return true;
}

void socketHandler::serverProcessMsg()
{
	const mouseEvent *mouse;
	EventMsg eventMsg;
	int key;
	bufferList* p = recvMsg();
	while (p)
	{
		switch (str2type[p->lable])
		{
		case MOUSE:
			mouse = reinterpret_cast<const mouseEvent*>(p->msg);
			eventMsg._event = (*mouse);
			eventMsg.param = 1;
			Director::getTheInstance()->raiseEvent(eventMsg);
			break;
		case KEY:
			key = atoi(p->msg);
			Director::getTheInstance()->currentScene->onKeyPressedCallback(1, key);
			break;
		default:
			break;
		}
		p = p->next;
	}
}

void socketHandler::clientProcessMsg()
{
	const music_Lite *m;
	const sprite_Lite *l;
	const Text *t;
	bufferList* p = recvMsg();
	while (p)
	{
		switch (str2type[p->lable])
		{
		case SPRITE:
			l = reinterpret_cast<const sprite_Lite*>(p->msg);
			clientAddSprite(*l);
			break;
		case TEXT:
			t = reinterpret_cast<const Text*>(p->msg);
			TextBuffer2.push_back(*t);
			break;
		case MUSIC:
			m = reinterpret_cast<const music_Lite*>(p->msg);
			AudioEngine::getTheInstance()->playMusic(*m);
			break;
		case BEGIN:
			cleanBuffer2();
			break;
		case END:
			swapBuffer();
			break;
		default:
			break;
		}
		p = p->next;
	}
}

void socketHandler::serverWaitForClient()
{
	length = sizeof(saClient);
	sSecond = accept(sFirst, (struct sockaddr *)&saClient, &length);
	if (sSecond != INVALID_SOCKET)
	{
		_state = CONNECTING_NOT_INITED;
	}
}

void socketHandler::serverInitWithPics(vector<string> _PicNames)
{
	assert(_PicNames.size() > 0);
	PicNames=_PicNames;
}

void socketHandler::serverInitWithMusics(std::vector<std::string> _MusicNames)
{
	assert(_MusicNames.size() > 0);
	MusicNames = _MusicNames;
}

void socketHandler::sendMsg(MsgType type, void* buffer, int lenth)
{
	char buffer1[1024] = { '\0' };
	int bufferlenth = 0;
	memprint(buffer1, buffer, lenth, type2str[type], &bufferlenth);
	send(sSecond, buffer1, bufferlenth, 0);
}

bufferList* socketHandler::recvMsg()
{
	int bufferLenth = recv(sSecond, input+inputIndex, recvBufferLenth-inputIndex, 0);
	if (bufferLenth>0)
	{
		return processMsg(input, bufferLenth);
	}
	return NULL;
}

bufferList* socketHandler::processMsg(char* input, int lenth)
{

	int pos = 0, flag = 0;  //flag ： 0表示空状态 1表示在读入labels 2表示在读入msgs
	int lenth_label = 0, lenth_msg = 0;
	int next[10000];
	char label[1024], msg[1024];
	BufferPool::getTheInstance()->reset();
	bufferList* Head = NULL;
	bufferList* prev = NULL;
	bufferList* p = BufferPool::getTheInstance()->getBuffer();
	p->next = NULL;
	while (pos < lenth){
		//printf("%d %c\n",pos,input[pos]);
		//getchar();
		if (input[pos] == '/' && flag == 0){// 找到标签开头
			//printf("~~~~~~~~~~~~0\n");
			lenth_label = 0;
			flag = 1;
		}
		else if (flag == 1 && input[pos] != ' '){// 1过程：提取标签
			//printf("~~~~~~~~~~~~~~~0\n");
			p->lable[lenth_label] = input[pos];
			lenth_label++;

		}
		else if (flag == 1 && input[pos] == ' '){//提取标签结束，开始提取标签信息，进入2过程

			p->lable[lenth_label++] = '/';
			p->lable[lenth_label] = 0;

			flag = 2;
			lenth_msg = 0;

			int i = 0, j = -1;//构造失败指针
			next[0] = j;
			while (i < lenth){
				while (j != -1 && input[j] != input[i])
					j = next[j];
				i++, j++;
				if (j >= lenth)
					next[i] = next[j - 1];
				else
					next[i] = j;
			}

		}
		else if (flag == 2){// 2过程：提取信息

			int i = pos, j = 0;//kmp算法
			while (i < lenth){

				while (j != -1 && input[i] != p->lable[j]){
					j = next[j];
				}
				//printf("%d:%c %d:%c\n",i,input[i],j + 1,label[j + 1]);
				p->msg[lenth_msg++] = input[i];
				i++, j++;

				if (j == lenth_label){                 //提取信息结束
					p->msg[lenth_msg - lenth_label - 1] = '\0';
					lenth_msg -= lenth_label + 1;
					p->lable[lenth_label - 1] = 0;
					lenth_label--;
					pos = i - 1;
					flag = 0;
					if (Head)
					{
						prev->next = p;
						prev = p;
						p->next = NULL;
						p = BufferPool::getTheInstance()->getBuffer();;
					}
					else
					{
						Head = p;
						p = BufferPool::getTheInstance()->getBuffer();
						prev = Head;
						Head->next = NULL;
					}
					break;
				}
			}
			if (flag){
				break;
			}


		}
		pos++;
	}
	return Head;
}

void socketHandler::setNetworkMode(NetworkMode mode)
{
	SocketMutex.lock();
	switch (mode)
	{
	case SINGLE:
		switch (_networkMode)
		{
		case SINGLE:
			//SINGLE->SINGLE
			break;
		case SERVER:
			//SERVER->SINGLE
			//TODO:sever clean up
			cleanSocket();
			break;
		case CLIENT:
			//CLIENT->SINGLE
			//TODO:client clean up
			cleanSocket();
			break;
		}
		break;
	case SERVER:
		switch (_networkMode)
		{
		case SINGLE:
			//SINGLE->SERVER
			//TODO:server init
			initServerSocket();
			break;
		case SERVER:
			//SERVER->SERVER
			break;
		case CLIENT:
			//CLIENT->SERVER
			//TODO: client clean up -> server init
			cleanSocket();
			initServerSocket();
			break;
		}
		break;
	case CLIENT:
		switch (_networkMode)
		{
		case SINGLE:
			//SINGLE->CLIENT
			//TODO:client init
			initClientSocket();
			break;
		case SERVER:
			//SERVER->CLIENT
			//TODO:server clean up ->client init
			cleanSocket();
			initServerSocket();
			break;
		case CLIENT:
			//CLIENT->CLIENT
			break;
		}
	}
	_networkMode = mode;
	SocketMutex.unlock();
}

NetworkMode socketHandler::getNetworkMode()
{
	return _networkMode;
}

void socketHandler::serverRemoveSpriteByID(int _id)
{
	if (_state == CONNECTING_INITED && _networkMode==SERVER)
	{
		char debug[30] = { '\0' };
		sprintf(debug, "%d", _id);
		sendMsg(REMOVESPRITE, debug, strlen(debug));
	}

}

void socketHandler::setSocketState(SocketState state)
{
	SocketMutex.lock();
	_state = state;
	SocketMutex.unlock();
}

SocketState socketHandler::getSocketState()
{
	return _state;
}

void socketHandler::setServerIP(string IP)
{
	serverIP = IP;
}

std::string socketHandler::getServerIP()
{
	return serverIP;
}

std::vector<Sprite*>& socketHandler::getSpriteBuffer()
{
	return SpriteBuffer1;
}

void socketHandler::cleanBuffer2()
{
	bufferMutex.lock();
	SpriteBuffer2.clear();
	TextBuffer2.clear();
	SpritePool::getTheInstance()->reset();
	bufferMutex.unlock();
}

void socketHandler::clientAddSprite(sprite_Lite _sprite)
{
	Sprite* s = SpritePool::getTheInstance()->getSprite();
	s->setCore(_sprite);
	SpriteBuffer2.push_back(s);
}

std::vector<Text>& socketHandler::getTextBuffer()
{
	return TextBuffer1;
}

void socketHandler::setServerTextBuffer(std::vector<Text>& buffer)
{
	ServerTextBufferMutex.lock();
	ServerTextBuffer = buffer;
	ServerTextBufferMutex.unlock();
}

void socketHandler::serverSetNetworkConfig()
{
	SocketMutex.lock();

	SocketMutex.unlock();
}

void socketHandler::clientOnKeyPressed(int key)
{
	SocketMutex.lock();
	_keys.push_back(key);
	SocketMutex.unlock();
}

void socketHandler::clientMouseEvent(mouseEvent _event)
{
	SocketMutex.lock();
	_mouseevent = _event;
	SocketMutex.unlock();
}

void socketHandler::clientsendKeyMouseMsg()
{
	char chr[20] = { '\0' };
	size_t _size = _keys.size();
	for (int i = 0; i < _size; i++)
	{
		sprintf(chr, "%d",_keys[i]);
		sendMsg(KEY, chr, strlen(chr));
	}
	_keys.clear();
	sendMsg(MOUSE, &_mouseevent, sizeof(mouseEvent));
}