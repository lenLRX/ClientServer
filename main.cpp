// win32gl1.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "main.h"
#include "printText.h"
#include "loadpng.h"
#include "gamelogic.h"
#include "director.h"
#include "socketHandler.h"
#include "audioEngine.h"
#include "spritePool.h"
#include "bufferPool.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <algorithm>
#include <winsock2.h>
//#include <commctrl.h>
//#include <mmsystem.h>
#include <mutex>
#pragma comment(lib, "ws2_32.lib")
//using namespace std;

#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名
HDC hDC;
PIXELFORMATDESCRIPTOR  pfd;
HGLRC ghRC;
bool pauseFlag;
bool openglInited = false;

LPDWORD SocketThread = NULL;
HANDLE SocketThreadHANDLE = NULL;

LPDWORD DrawingThread = NULL;
HANDLE DrawingThreadHANDLE = NULL;
HWND hWnd;
float FPS = 60;
float LastCycleTime = 0.0001;//上一帧的时间
POINT mousepos;//鼠标位置
char lastPressedKey = '\0';
volatile bool shouldRun = false;
extern std::mutex SceneMutex;
WORD wVersionRequested;
WSADATA wsaData;
int ret, nLeft, length;
volatile struct sockaddr_in saServer, saClient; //地址信息   
char *ptr;//用于遍历信息的指针   
volatile SOCKET sFirst, sSecond; //套接字
volatile float socketSleepTime = 20;
volatile float openglSleepTime = 0;

std::mutex mainMutex;

// 此代码模块中包含的函数的前向声明: 
void SceneShow(GLvoid);

void openglInit();//初始化OPENGL

void showFPS();//显示FPS

void setFPS(float fps);//设置FPS

void getLastCycleTime();//获得上一帧的时间（注意！每个循环只能调用一次！！！）

void getKeyBoardState();

bool lessSprite(Sprite* s1, Sprite* s2)
{
	return s1->getlayer() < s2->getlayer();
}

void initClientSocket();

void initServerSocket();

void cleanSocket();

DWORD WINAPI  SocketLoop(LPVOID lpParameter);                    //socket主循环

DWORD WINAPI  DrawingLoop(LPVOID lpParameter);                   //绘图主循环

ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	// TODO:  在此放置代码。

	pauseFlag = false;

	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CLIENTSERVER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化: 
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENTSERVER));

	// 主消息循环: 
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  函数:  MyRegisterClass()
//
//  目的:  注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_SAVEBITS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32GL1));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_CLIENTSERVER);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	return RegisterClassEx(&wcex);
}

//
//   函数:  InitInstance(HINSTANCE, int)
//
//   目的:  保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	Director::create();
	SpritePool::create();
	BufferPool::create();
	AudioEngine::create();
	socketHandler::create();
	socketHandler::getTheInstance()->setNetworkMode(SINGLE);
	std::vector<std::string> names;
	names.push_back("Thanatos.wav");
	names.push_back("boom1.wav");
	socketHandler::getTheInstance()->serverInitWithMusics(names);
	hInst = hInstance; // 将实例句柄存储在全局变量中
	//设置窗口大小
	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW&~WS_THICKFRAME&~WS_MAXIMIZEBOX,
		200, 200, WINDOW_WIDTH + 18, WINDOW_HEIGHT + 61, NULL, NULL, hInstance, NULL);
	//~WS_THICKFRAME禁止改变窗口大小
	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	shouldRun = true;
	if (!DrawingThreadHANDLE)
		DrawingThreadHANDLE = CreateThread(NULL, 0, DrawingLoop, NULL, 0, DrawingThread);
	// 获取窗口上的整个菜单栏的句柄  
	HMENU hmm = GetMenu(hWnd);
	  
	HMENU hfmn = GetSubMenu(hmm, 2);
	CheckMenuRadioItem(hfmn, ID_SINGLE, ID_CLIENT, ID_SINGLE, MF_BYCOMMAND);
	SetWindowText(hWnd, L"单机");
	return TRUE;
}

//
//  函数:  WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	HMENU hmm = GetMenu(hWnd);

	HMENU hfmn = GetSubMenu(hmm, 2);

	switch (message)
	{
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择: 
		switch (wmId)
		{
		case IDM_ABOUT:
			
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_PAUSE:
			Director::getTheInstance()->pause();
			break;
		case IDM_RESUME:
			Director::getTheInstance()->resume();
			break;
		case IDM_START:
			Director::getTheInstance()->setInitScene("test1");
			Director::getTheInstance()->start();
			break;
		case IDM_END:
			Director::getTheInstance()->end();
			break;
		case ID_SINGLE:
			CheckMenuRadioItem(hfmn, ID_SINGLE, ID_CLIENT, ID_SINGLE, MF_BYCOMMAND);
			SetWindowText(hWnd, L"单机");
			socketHandler::getTheInstance()->setNetworkMode(SINGLE);
			Director::getTheInstance()->setInitScene("game1");
			Director::getTheInstance()->start();
			break;
		case ID_CLIENT:
			CheckMenuRadioItem(hfmn, ID_SINGLE, ID_CLIENT, ID_CLIENT, MF_BYCOMMAND);
			SetWindowText(hWnd, L"作为客户端");
			socketHandler::getTheInstance()->setNetworkMode(CLIENT);
			Director::getTheInstance()->setInitScene("default");
			Director::getTheInstance()->start();
			break;
		case ID_SERVER:
			CheckMenuRadioItem(hfmn, ID_SINGLE, ID_CLIENT, ID_SERVER, MF_BYCOMMAND);
			SetWindowText(hWnd, L"作为服务器");
			socketHandler::getTheInstance()->setNetworkMode(SERVER);
			Director::getTheInstance()->setInitScene("game3");
			Director::getTheInstance()->start();
			break;
		case ID_SETIP:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, About);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		// TODO:  在此添加任意绘图代码...

		break;
	case WM_DESTROY:
		shouldRun = false;
		WaitForSingleObject(DrawingThreadHANDLE, INFINITE);
		WaitForSingleObject(SocketThreadHANDLE, INFINITE);
		CloseHandle(DrawingThreadHANDLE);
		CloseHandle(SocketThreadHANDLE);
		PostQuitMessage(0);
		break;
	case WM_LBUTTONDOWN:

		break;
	case WM_CHAR:
		lastPressedKey = wParam;
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	
	switch (message)
	{
	case WM_INITDIALOG:
		SetWindowTextA(GetDlgItem(hDlg, IDC_IPADDRESS1), socketHandler::getTheInstance()->getServerIP().c_str());
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			if (LOWORD(wParam) == IDOK)
			{
				char IP_addr[16] = { '\0' };
				GetWindowTextA(GetDlgItem(hDlg, IDC_IPADDRESS1), IP_addr,16);
				socketHandler::getTheInstance()->setServerIP(IP_addr);
			}
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

//  这里进行所有的绘图工作 

void  SceneShow(GLvoid)
{
	Director::getTheInstance()->CheckFlag();
	switch (Director::getTheInstance()->getState())
	{
	case SLEEPING:
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		SwapBuffers(hDC);
		break;
	case RUNNING:
		GetCursorPos(&mousepos);//获得鼠标位置

		getLastCycleTime();//每帧重置一下计时器

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		if (Director::getTheInstance()->getTotalScene())
		{
		
			if (hWnd == GetForegroundWindow())
			{
				Director::getTheInstance()->getKeyState();
			}
			Director::getTheInstance()->tickTimers();
			Scene* thescene = Director::getTheInstance()->getCurrentScene();
			Director::getTheInstance()->update();
			Director::getTheInstance()->getCurrentScene()->clearKeys();
			SceneMutex.lock();
			bufferMutex.lock();
			std::vector<Sprite*>::iterator begin;
			std::vector<Sprite*>::iterator end;
			if (socketHandler::getTheInstance()->getNetworkMode() == CLIENT)
			{
				begin = socketHandler::getTheInstance()->getSpriteBuffer().begin();
				end = socketHandler::getTheInstance()->getSpriteBuffer().end();
			}
			else
			{
				begin = thescene->getSpriteList().begin();
				end = thescene->getSpriteList().end();
			}
			
			std::sort(begin, end, lessSprite);
			for (std::vector<Sprite*>::iterator it = begin; it != end; ++it)
			{
				Point origin = (*it)->getpos();
				Point anchorPoint = (*it)->getAnchorPoint();
				Size size = (*it)->getsize();
				std::swap(size.height, size.width);
				float rot = (*it)->getrotation();
				glLoadIdentity();


				float r = sqrt(size.width*size.width + size.height*size.height);

				Point bl, br, tr, tl;

				bl.x = -(size.width) / r;
				bl.y = -(size.height) / r;

				br.x = (size.width) / r;
				br.y = -(size.height) / r;

				tr.x = (size.width) / r;
				tr.y = (size.height) / r;

				tl.x = -(size.width) / r;
				tl.y = (size.height) / r;



				glRotatef(90.0f + rot / M_PI * 180, 0.0f, 0.0f, 1.0f);
				//TODO:FIX IT
				glViewport(origin.x - r *anchorPoint.x, origin.y - r*anchorPoint.y, r, r);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glEnable(GL_TEXTURE_2D);
				glMatrixMode(GL_MODELVIEW);
				if ((*it)->getflipX())                                  //左右翻转
				{
					std::swap(bl.x, br.x);
					std::swap(bl.y, br.y);
					std::swap(tl.x, tr.x);
					std::swap(tl.y, tr.y);
				}

				if ((*it)->getflipY())                                  //上下翻转
				{
					std::swap(bl.x, tl.x);
					std::swap(bl.y, tl.y);
					std::swap(br.x, tr.x);
					std::swap(br.y, tr.y);;
				}
				glBindTexture(GL_TEXTURE_2D, (*it)->getTex());          //与纹理绑定
				glBegin(GL_POLYGON);

				glTexCoord2f(0.0f, 0.0f); glVertex3f(bl.x, bl.y, 0.0f);    //左下
				glTexCoord2f(0.0f, 1.0f); glVertex3f(br.x, br.y, 0.0f);      //右下
				glTexCoord2f(1.0f, 1.0f); glVertex3f(tr.x, tr.y, 0.0f);      //右上
				glTexCoord2f(1.0f, 0.0f); glVertex3f(tl.x, tl.y, 0.0f);      //左上

				glEnd();
				glDisable(GL_TEXTURE_2D);
				glDisable(GL_BLEND);
			}	
		}
		SceneMutex.unlock();
		///////////////////////////////////////////////////////////
		//任何glPrint应在此行之后执行（不然看不到）
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		glLoadIdentity();
		//打印文字
		if (socketHandler::getTheInstance()->getNetworkMode() == CLIENT)
		{
			Director::getTheInstance()->printTexts(socketHandler::getTheInstance()->getTextBuffer());
		}
		else
		{
			Director::getTheInstance()->printTexts();                                        
		}
		bufferMutex.unlock();
		glRasterPos2f(-1.0f, -0.8f);
		ScreenToClient(hWnd, &mousepos);
		Director::getTheInstance()->setMousePos(mousepos);
		glPrint("mouse pos(%ld,%ld)", mousepos.x, mousepos.y);
		glRasterPos2f(-1.0f, -0.6f);
		glPrint("Keyboard: %c", lastPressedKey);
		//   交换缓冲区 
		getKeyBoardState();
		showFPS();
		glRasterPos2f(-1.0f, -0.5f);
		glPrint("aswd control");
		SwapBuffers(hDC);
		break;
	}
}

void initDrawing()
{
	int  iFormat;

	openglInited = true;

	hDC = GetDC(hWnd);

	assert(hDC);

	ZeroMemory(&pfd, sizeof (pfd));

	pfd.nSize = sizeof (pfd);

	pfd.nVersion = 1;        //  版本，一般设为 



	//  一组表明象素缓冲特性的标志位 

	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;

	pfd.iPixelType = PFD_TYPE_RGBA;     //  明象素数据类型是RGBA 还是颜色索引;  

	pfd.cColorBits = 32;       //  每个颜色缓冲区中颜色位平面的数目，对颜色索引方式是缓冲区大小 

	pfd.cDepthBits = 16;

	pfd.iLayerType = PFD_MAIN_PLANE;  //  被忽略，为了一致性而包含的 



	iFormat = ChoosePixelFormat(hDC, &pfd); //  选择一个像素格式 



	assert(SetPixelFormat(hDC, iFormat, &pfd));  //  设置到DC 中 



	ghRC = wglCreateContext(hDC);      //  创建绘图描述表 

	assert(wglMakeCurrent(hDC, ghRC));       //  使之成为当前绘图描述表 


	openglInit();
}

DWORD WINAPI  DrawingLoop(LPVOID lpParameter)
{
	if (!openglInited)
		initDrawing();
	std::vector<std::string> names;
	names.push_back("core2.png");
	names.push_back("CloseNormal.png");
	names.push_back("CloseSelected.png");
	names.push_back("sprite_enemy_mediumboat_rearturret_0.png");
	names.push_back("sprite_enemy_largeboat_turret_0.png");
	names.push_back("sprite_enemy_shot_small_blue_0.png");
	names.push_back("sprite_enemy_shot_small_purple_0.png");
	names.push_back("alert.png");
	names.push_back("bar1.png");
	names.push_back("bar2.png");
	names.push_back("bar3.png");
	Director::getTheInstance()->prepareTextures(names);
	if (!SocketThreadHANDLE)
		SocketThreadHANDLE = CreateThread(NULL, 0, SocketLoop, NULL, 0, SocketThread);
	while (shouldRun)
	{
		mainMutex.lock();
		SceneShow();
		Sleep(openglSleepTime);
		mainMutex.unlock();
	}
	Director::getTheInstance()->reset();
	assert(wglMakeCurrent(NULL, NULL));
	assert(wglDeleteContext(ghRC));
	assert(ReleaseDC(hWnd, hDC));
	ghRC = NULL;
	hDC = NULL;
	return 0;
}

//OpenGL 初始化开始 

void openglInit()
{
	BuildFont(hDC);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);        //   黑色背景 

	glColor3f(1.0f, 1.0f, 1.0f);
	glShadeModel(GL_FLAT);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
}

void showFPS()
{
	glLoadIdentity();
	glRasterPos2f(-1.0f, -1.0f);

	static float lastFPS = 0;
	lastFPS = 1.0 / LastCycleTime;
	glPrint("FPS:%f", lastFPS);
}


void setFPS(float fps)
{
	FPS = fps;
}

void getLastCycleTime()
{
	LARGE_INTEGER LARGE;
	static LONGLONG lasttimeL = 0;
	LONGLONG nowL = 0;
	double freq = 0;
	QueryPerformanceFrequency(&LARGE);
	freq = (double)LARGE.QuadPart;//获得频率
	QueryPerformanceCounter(&LARGE);
	nowL = LARGE.QuadPart;
	LastCycleTime = ((float)(nowL - lasttimeL)) / freq;
	lasttimeL = nowL;
}

void getKeyBoardState()
{
	if (true)
	{
		glRasterPos2f(-1.0f, -0.2f);
		//glPrint("L button Pressed %d", GetAsyncKeyState(VK_LBUTTON) & 0x8000);
	}
}


DWORD WINAPI SocketLoop(LPVOID lpParameter)
{
	std::vector<std::string> names;
	names.push_back("core2.png");
	names.push_back("CloseNormal.png");
	names.push_back("CloseSelected.png");
	names.push_back("sprite_enemy_mediumboat_rearturret_0.png");
	names.push_back("sprite_enemy_largeboat_turret_0.png");
	names.push_back("sprite_enemy_shot_small_blue_0.png");
	names.push_back("sprite_enemy_shot_small_purple_0.png");
	names.push_back("alert.png");
	names.push_back("bar1.png");
	names.push_back("bar2.png");
	names.push_back("bar3.png");
	socketHandler::getTheInstance()->serverInitWithPics(names);
	while (shouldRun)
	{
		mainMutex.lock();
		socketHandler::getTheInstance()->doSth();
		Sleep(socketSleepTime);
		mainMutex.unlock();
	}
	//cleanSocket();
	return 0;
}

void initClientSocket()
{
	//WinSock初始化
	wVersionRequested = MAKEWORD(2, 2); //希望使用的WinSock DLL的版本
	ret = WSAStartup(wVersionRequested, &wsaData);
	if (ret != 0)
	{
		printf("WSAStartup() failed!\n");
		return;
	}
	//确认WinSock DLL支持版本2.2
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		printf("Invalid WinSock version!\n");
		return;
	}

	//创建Socket,使用TCP协议
	sSecond = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sSecond == INVALID_SOCKET)
	{
		WSACleanup();
		printf("socket() failed!\n");
		return;
	}

	//构建服务器地址信息
	openglSleepTime = 20.0f;
	socketSleepTime =0.0f;
	socketHandler::getTheInstance()->_state=CONNECTING;
}

void initServerSocket()
{
	//WinSock初始化
	wVersionRequested = MAKEWORD(2, 2); //希望使用的WinSock DLL 的版本
	ret = WSAStartup(wVersionRequested, &wsaData);
	if (ret != 0)
	{
		throw 0;
		printf("WSAStartup() failed!\n");
		return;
	}
	//创建Socket,使用TCP协议
	sFirst = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	unsigned long ul = 1;

	ioctlsocket(sFirst, FIONBIO, (unsigned long *)&ul);//设置非阻塞
	if (sFirst == INVALID_SOCKET)
	{
		throw 0;
		WSACleanup();
		printf("socket() faild!\n");
		return;
	}
	//构建本地地址信息
	saServer.sin_family = AF_INET; //地址家族
	saServer.sin_port = htons(SERVER_PORT); //注意转化为网络字节序
	saServer.sin_addr.S_un.S_addr = htonl(INADDR_ANY); //使用INADDR_ANY 指示任意地址
	
		//绑定
    ret = bind(sFirst, (struct sockaddr *)&saServer, sizeof(saServer));
	if (ret == SOCKET_ERROR)
	{
		throw 0;
		printf("bind() faild! code:%d\n", WSAGetLastError());
		closesocket(sFirst); //关闭套接字
		WSACleanup();
		return;
	}

	//侦听连接请求
	ret = listen(sFirst, 5);
	if (ret == SOCKET_ERROR)
	{
		throw 0;
		printf("listen() faild! code:%d\n", WSAGetLastError());
		closesocket(sFirst); //关闭套接字
		return;
	}
	socketHandler::getTheInstance()->_state=LISTENING;
}

void cleanSocket()
{
	openglSleepTime = 20;
	socketSleepTime = 0;
	int e;
	if (closesocket(sFirst) == SOCKET_ERROR)
	{
		e = WSAGetLastError();
		if (e != WSAENOTSOCK)
			throw 0;
	}
		
	if (closesocket(sSecond) == SOCKET_ERROR)
	{
		e = WSAGetLastError();
		if (e != WSAENOTSOCK)
		    throw 0;
	}
	if (WSACleanup() == SOCKET_ERROR)
	{
		e = WSAGetLastError();
		//if (e != WSAENOTSOCK)
			throw 0;
	}
}