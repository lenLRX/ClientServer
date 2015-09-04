// win32gl1.cpp : ����Ӧ�ó������ڵ㡣
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

// ȫ�ֱ���: 
HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������
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
float LastCycleTime = 0.0001;//��һ֡��ʱ��
POINT mousepos;//���λ��
char lastPressedKey = '\0';
volatile bool shouldRun = false;
extern std::mutex SceneMutex;
WORD wVersionRequested;
WSADATA wsaData;
int ret, nLeft, length;
volatile struct sockaddr_in saServer, saClient; //��ַ��Ϣ   
char *ptr;//���ڱ�����Ϣ��ָ��   
volatile SOCKET sFirst, sSecond; //�׽���
volatile float socketSleepTime = 20;
volatile float openglSleepTime = 0;

std::mutex mainMutex;

// �˴���ģ���а����ĺ�����ǰ������: 
void SceneShow(GLvoid);

void openglInit();//��ʼ��OPENGL

void showFPS();//��ʾFPS

void setFPS(float fps);//����FPS

void getLastCycleTime();//�����һ֡��ʱ�䣨ע�⣡ÿ��ѭ��ֻ�ܵ���һ�Σ�������

void getKeyBoardState();

bool lessSprite(Sprite* s1, Sprite* s2)
{
	return s1->getlayer() < s2->getlayer();
}

void initClientSocket();

void initServerSocket();

void cleanSocket();

DWORD WINAPI  SocketLoop(LPVOID lpParameter);                    //socket��ѭ��

DWORD WINAPI  DrawingLoop(LPVOID lpParameter);                   //��ͼ��ѭ��

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
	// TODO:  �ڴ˷��ô��롣

	pauseFlag = false;

	MSG msg;
	HACCEL hAccelTable;

	// ��ʼ��ȫ���ַ���
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CLIENTSERVER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ִ��Ӧ�ó����ʼ��: 
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENTSERVER));

	// ����Ϣѭ��: 
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
//  ����:  MyRegisterClass()
//
//  Ŀ��:  ע�ᴰ���ࡣ
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
//   ����:  InitInstance(HINSTANCE, int)
//
//   Ŀ��:  ����ʵ�����������������
//
//   ע��: 
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
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
	hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����
	//���ô��ڴ�С
	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW&~WS_THICKFRAME&~WS_MAXIMIZEBOX,
		200, 200, WINDOW_WIDTH + 18, WINDOW_HEIGHT + 61, NULL, NULL, hInstance, NULL);
	//~WS_THICKFRAME��ֹ�ı䴰�ڴ�С
	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	shouldRun = true;
	if (!DrawingThreadHANDLE)
		DrawingThreadHANDLE = CreateThread(NULL, 0, DrawingLoop, NULL, 0, DrawingThread);
	// ��ȡ�����ϵ������˵����ľ��  
	HMENU hmm = GetMenu(hWnd);
	  
	HMENU hfmn = GetSubMenu(hmm, 2);
	CheckMenuRadioItem(hfmn, ID_SINGLE, ID_CLIENT, ID_SINGLE, MF_BYCOMMAND);
	SetWindowText(hWnd, L"����");
	return TRUE;
}

//
//  ����:  WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��:    ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
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
		// �����˵�ѡ��: 
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
			SetWindowText(hWnd, L"����");
			socketHandler::getTheInstance()->setNetworkMode(SINGLE);
			Director::getTheInstance()->setInitScene("game1");
			Director::getTheInstance()->start();
			break;
		case ID_CLIENT:
			CheckMenuRadioItem(hfmn, ID_SINGLE, ID_CLIENT, ID_CLIENT, MF_BYCOMMAND);
			SetWindowText(hWnd, L"��Ϊ�ͻ���");
			socketHandler::getTheInstance()->setNetworkMode(CLIENT);
			Director::getTheInstance()->setInitScene("default");
			Director::getTheInstance()->start();
			break;
		case ID_SERVER:
			CheckMenuRadioItem(hfmn, ID_SINGLE, ID_CLIENT, ID_SERVER, MF_BYCOMMAND);
			SetWindowText(hWnd, L"��Ϊ������");
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
		// TODO:  �ڴ���������ͼ����...

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

// �����ڡ������Ϣ�������
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

//  ����������еĻ�ͼ���� 

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
		GetCursorPos(&mousepos);//������λ��

		getLastCycleTime();//ÿ֡����һ�¼�ʱ��

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
				if ((*it)->getflipX())                                  //���ҷ�ת
				{
					std::swap(bl.x, br.x);
					std::swap(bl.y, br.y);
					std::swap(tl.x, tr.x);
					std::swap(tl.y, tr.y);
				}

				if ((*it)->getflipY())                                  //���·�ת
				{
					std::swap(bl.x, tl.x);
					std::swap(bl.y, tl.y);
					std::swap(br.x, tr.x);
					std::swap(br.y, tr.y);;
				}
				glBindTexture(GL_TEXTURE_2D, (*it)->getTex());          //�������
				glBegin(GL_POLYGON);

				glTexCoord2f(0.0f, 0.0f); glVertex3f(bl.x, bl.y, 0.0f);    //����
				glTexCoord2f(0.0f, 1.0f); glVertex3f(br.x, br.y, 0.0f);      //����
				glTexCoord2f(1.0f, 1.0f); glVertex3f(tr.x, tr.y, 0.0f);      //����
				glTexCoord2f(1.0f, 0.0f); glVertex3f(tl.x, tl.y, 0.0f);      //����

				glEnd();
				glDisable(GL_TEXTURE_2D);
				glDisable(GL_BLEND);
			}	
		}
		SceneMutex.unlock();
		///////////////////////////////////////////////////////////
		//�κ�glPrintӦ�ڴ���֮��ִ�У���Ȼ��������
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		glLoadIdentity();
		//��ӡ����
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
		//   ���������� 
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

	pfd.nVersion = 1;        //  �汾��һ����Ϊ 



	//  һ��������ػ������Եı�־λ 

	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;

	pfd.iPixelType = PFD_TYPE_RGBA;     //  ����������������RGBA ������ɫ����;  

	pfd.cColorBits = 32;       //  ÿ����ɫ����������ɫλƽ�����Ŀ������ɫ������ʽ�ǻ�������С 

	pfd.cDepthBits = 16;

	pfd.iLayerType = PFD_MAIN_PLANE;  //  �����ԣ�Ϊ��һ���Զ������� 



	iFormat = ChoosePixelFormat(hDC, &pfd); //  ѡ��һ�����ظ�ʽ 



	assert(SetPixelFormat(hDC, iFormat, &pfd));  //  ���õ�DC �� 



	ghRC = wglCreateContext(hDC);      //  ������ͼ������ 

	assert(wglMakeCurrent(hDC, ghRC));       //  ʹ֮��Ϊ��ǰ��ͼ������ 


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

//OpenGL ��ʼ����ʼ 

void openglInit()
{
	BuildFont(hDC);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);        //   ��ɫ���� 

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
	freq = (double)LARGE.QuadPart;//���Ƶ��
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
	//WinSock��ʼ��
	wVersionRequested = MAKEWORD(2, 2); //ϣ��ʹ�õ�WinSock DLL�İ汾
	ret = WSAStartup(wVersionRequested, &wsaData);
	if (ret != 0)
	{
		printf("WSAStartup() failed!\n");
		return;
	}
	//ȷ��WinSock DLL֧�ְ汾2.2
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		printf("Invalid WinSock version!\n");
		return;
	}

	//����Socket,ʹ��TCPЭ��
	sSecond = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sSecond == INVALID_SOCKET)
	{
		WSACleanup();
		printf("socket() failed!\n");
		return;
	}

	//������������ַ��Ϣ
	openglSleepTime = 20.0f;
	socketSleepTime =0.0f;
	socketHandler::getTheInstance()->_state=CONNECTING;
}

void initServerSocket()
{
	//WinSock��ʼ��
	wVersionRequested = MAKEWORD(2, 2); //ϣ��ʹ�õ�WinSock DLL �İ汾
	ret = WSAStartup(wVersionRequested, &wsaData);
	if (ret != 0)
	{
		throw 0;
		printf("WSAStartup() failed!\n");
		return;
	}
	//����Socket,ʹ��TCPЭ��
	sFirst = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	unsigned long ul = 1;

	ioctlsocket(sFirst, FIONBIO, (unsigned long *)&ul);//���÷�����
	if (sFirst == INVALID_SOCKET)
	{
		throw 0;
		WSACleanup();
		printf("socket() faild!\n");
		return;
	}
	//�������ص�ַ��Ϣ
	saServer.sin_family = AF_INET; //��ַ����
	saServer.sin_port = htons(SERVER_PORT); //ע��ת��Ϊ�����ֽ���
	saServer.sin_addr.S_un.S_addr = htonl(INADDR_ANY); //ʹ��INADDR_ANY ָʾ�����ַ
	
		//��
    ret = bind(sFirst, (struct sockaddr *)&saServer, sizeof(saServer));
	if (ret == SOCKET_ERROR)
	{
		throw 0;
		printf("bind() faild! code:%d\n", WSAGetLastError());
		closesocket(sFirst); //�ر��׽���
		WSACleanup();
		return;
	}

	//������������
	ret = listen(sFirst, 5);
	if (ret == SOCKET_ERROR)
	{
		throw 0;
		printf("listen() faild! code:%d\n", WSAGetLastError());
		closesocket(sFirst); //�ر��׽���
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