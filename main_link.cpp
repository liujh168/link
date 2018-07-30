#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <conio.h>

#include "linker.h"
#include "tools.h"
#include "lock.h"
#include "ucci.h"
#include "thread.h"
#include "hotkey.h"

using namespace cv;
using namespace std;

#define WM_THINK WM_USER+100

DWORD	dwThreadIDMain;
DWORD	dwThreadIdEngine;
BOOL	bSetHandle;
HANDLE	g_bmEvent;
HANDLE  hWaitForBestmove;
HANDLE	hLoopMove;

void	setup(void);						//启动代码
void	clean(void);						//清理代码
BOOL	WINAPI ConsoleCtrlHandler(DWORD dwCtrlType);  //控制台钩子函数	

int main(int argc, char* argv[])
{
	//#ifdef _DEBUG
	//	system("del temp\\*.jpg /S/Q >> NULL"); 
	//	link.mat2fen("screenshot.jpg");
	//	cout<< link.print() << endl;
	//	return 0;
	//#endif

	setup();

	MSG msg = {0};
	while (GetMessage(&msg, NULL, 0, 0) != 0)		//开始消息循环
	{
		switch (msg.message)
		{
		case WM_HOTKEY:
			onhotkey(msg);
			break;
		case WM_TIMER:								//定期更新盘面
			//link.update_fen();
			break;
		case WM_QUIT:		//控制台模式这里收不到？
			std::cout << "WM_QUIT! \n"<< std::endl;
			break;
		default:
			break;
		}    
		TranslateMessage( &msg );  
		DispatchMessage( &msg );  
	}   

	if(msg.message==WM_QUIT)
	{
		clean();
	}

	return 0;
}

HWND hwndConsole;
void setup(void)
{
	//放大窗口，设置标题（标题以后可用于显示引擎思考结果）
	SetConsoleTitle("link");
	ShowWindow(::GetConsoleWindow(),SW_MAXIMIZE);

	//在控制台窗口中显示原始棋盘
	hwndConsole = ::GetConsoleWindow();
	RECT rect;
	GetWindowRect( hwndConsole, &rect );
	if(link.isready())
	{
		//MatShow(link.m_imgInit, ::GetWindowDC(hwndConsole), rect );
		//Sleep(1000);
	}	

	//注册热键
	hotkey();

	//设置定时器
	dwThreadIDMain = GetCurrentThreadId();
	SetTimer(NULL, 10, 1000, NULL);

	//创建自动重置事件内核对象
	g_bmEvent = CreateEvent(NULL, FALSE, FALSE, "ucci"); //创建事件对象,注意参数的含义

	//保证应用程序只有一个实例
	if (g_bmEvent)
	{
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			cout<<"事件对象已存在!"<<endl;
			return;
		}
	}

	// 创建线程，用于输出引擎思考信息
	hLoopMove =			CreateThread(NULL, 0, ThreadProcLoopMove, 0, CREATE_SUSPENDED, &dwThreadIdEngine); 
	hWaitForBestmove =	CreateThread(NULL, 0, wait_bestmove_threadproc, 0, CREATE_SUSPENDED, &dwThreadIdEngine); 
	ResumeThread(hLoopMove);			//启动线程
	ResumeThread(hWaitForBestmove);	//启动线程

	//安装控制台钩子，处理WM_QUIT消息，以便有机会处理退出逻辑
	bSetHandle = SetConsoleCtrlHandler(ConsoleCtrlHandler, true);	

	//查找兵河是否已启动
	HWND bh = FindWindow(NULL,"BHGUI(test) - 新棋局"); 
	if(bh==NULL || !IsWindow(bh))
	{
		cout << "BHGUI(test) - 新棋局 not find!\n" <<  endl;
		//int iResult = (int)ShellExecute(NULL,"open","c:\\bh\\bh.exe",NULL,NULL,SW_SHOWNORMAL);    //执行应用程序
	}
	else
	{
		cout << "BHGUI(test) - 新棋局 running!\n" <<  endl;
	}

}

void clean(void)
{
	ucci.UnLoadEngine();		//

	//清理热键
	unhotkey();

	KillTimer(NULL, 10);

	if (bSetHandle) 
	{
		SetConsoleCtrlHandler(ConsoleCtrlHandler, false);   //删除控制台钩子
	}

	CloseHandle(g_bmEvent);

	CloseHandle(hLoopMove);   //并没有终止线程的运行；当不再需要线程句柄时，应将其关闭，让这个线程内核对象的引用计数减1
	CloseHandle(hWaitForBestmove);

	MessageBox(NULL, "get wm_quit to exit !  Program being closed!", "CEvent", MB_OK);
}

//BOOL bSetHandle = SetConsoleCtrlHandler(ConsoleCtrlHandler, true);	
//if (bSetHandle) 
//	SetConsoleCtrlHandler(ConsoleCtrlHandler, false);   //删除控制台钩子
BOOL WINAPI ConsoleCtrlHandler(DWORD dwCtrlType)
{
	if(dwCtrlType == CTRL_CLOSE_EVENT)
	{
		PostThreadMessage(dwThreadIDMain, WM_QUIT, 0, 0);
		//MessageBox(NULL, "Program being closed!", "CEvent", MB_OK);
		return TRUE;
	}
	return FALSE;
}
