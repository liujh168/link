#include <windows.h>
#include <Shlwapi.h>

#include "hotkey.h"
#include "linker.h"
#include "lock.h"
#include "ucci.h"
//#include "..\ncch\file.h"

extern HANDLE  hWaitForBestmove;
extern HANDLE	hLoopMove;

bool	bAutoGo = false;						//自动走子开关
bool	bBH = true;								//控制是否启动兵河分析开关	
bool	bConnected = false;						//连线开关	
bool    bGoInfinite = false;					//无限分析开关

std::string kmess[HOTKEYS] = {					//热键提示信息
	"ALT_F1 红先连接控制开关!",
	"ALT_F6 断开连接!" ,
	"ALT_F7 连接测试，打印棋盘。同时切换当前思考模式（时间与深度）! 切换屏幕截图位深（24位或32位）",
	"ALT_F8 初始化方案(屏幕、文件方式)、保存方案!",
	"ALT_F9 黑先连接控制开关!",
	"ALT_F10 载入方案!",
	"ALT_F11 客户端自动走子标志!",
	"ALT_F12 开启无限分析模式",
	"CTRL_ALT_F1 ！循环修改参数!",
	"CTRL_ALT_F2 ！更改方案参数（样本大小、实时子大小及阈值 ；思考时间、深度等）",
	"CTRL_ALT_F3 ！兵河分析控制开关",
	"CTRL_ALT_F4 ！引擎停止思考，立即出步！",
};

#pragma comment( lib,"shlwapi.lib")
	
//注册HotKey。
void hotkey(void)
{
	if (RegisterHotKey(NULL, ALT_F1, MOD_ALT | MOD_NOREPEAT, VK_F1))	std::cout << ((kmess[ALT_F1-1]).c_str()) << std::endl;
	if (RegisterHotKey(NULL, ALT_F6, MOD_ALT | MOD_NOREPEAT, VK_F6))	std::cout << ((kmess[ALT_F6-1]).c_str()) << std::endl;
	if (RegisterHotKey(NULL, ALT_F7, MOD_ALT | MOD_NOREPEAT, VK_F7))	std::cout << ((kmess[ALT_F7-1]).c_str()) << std::endl;
	if (RegisterHotKey(NULL, ALT_F8, MOD_ALT | MOD_NOREPEAT, VK_F8))	std::cout << ((kmess[ALT_F8-1]).c_str()) << std::endl;
	if (RegisterHotKey(NULL, ALT_F9, MOD_ALT | MOD_NOREPEAT, VK_F9))	std::cout << ((kmess[ALT_F9-1]).c_str()) << std::endl;
	if (RegisterHotKey(NULL, ALT_F10, MOD_ALT | MOD_NOREPEAT, VK_F10))	std::cout << ((kmess[ALT_F10-1]).c_str()) << std::endl;
	if (RegisterHotKey(NULL, ALT_F11, MOD_ALT | MOD_NOREPEAT, VK_F11))	std::cout << ((kmess[ALT_F11-1]).c_str()) << std::endl;
	if (RegisterHotKey(NULL, ALT_F12, MOD_ALT | MOD_NOREPEAT, VK_F12))	std::cout << ((kmess[ALT_F12-1]).c_str()) << std::endl;

	if (RegisterHotKey(NULL, CTRL_ALT_F1, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, VK_F1))	std::cout << ((kmess[CTRL_ALT_F1-1]).c_str()) << std::endl;
	if (RegisterHotKey(NULL, CTRL_ALT_F2, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, VK_F2))	std::cout << ((kmess[CTRL_ALT_F2-1]).c_str()) << std::endl;
	if (RegisterHotKey(NULL, CTRL_ALT_F3, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, VK_F3))	std::cout << ((kmess[CTRL_ALT_F3-1]).c_str()) << std::endl;
	if (RegisterHotKey(NULL, CTRL_ALT_F4, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, VK_F4))	std::cout << ((kmess[CTRL_ALT_F4-1]).c_str()) << std::endl;
}

//注销HotKey, 释放资源。
void unhotkey(void)
{
	::UnregisterHotKey(NULL, ALT_F1);
	::UnregisterHotKey(NULL, ALT_F6);
	::UnregisterHotKey(NULL, ALT_F7);
	::UnregisterHotKey(NULL, ALT_F8);	
	::UnregisterHotKey(NULL, ALT_F9);	
	::UnregisterHotKey(NULL, ALT_F10);
	::UnregisterHotKey(NULL, ALT_F11);
	::UnregisterHotKey(NULL, ALT_F12);

	::UnregisterHotKey(NULL, CTRL_ALT_F1);
	::UnregisterHotKey(NULL, CTRL_ALT_F2);
	::UnregisterHotKey(NULL, CTRL_ALT_F3);
	::UnregisterHotKey(NULL, CTRL_ALT_F4);
}

extern HWND hwndConsole;
void onhotkey(MSG msg)
{
    std::string input;
	int width_init, width_real;
	double threshold;
	switch(msg.wParam)
	{
	case ALT_F1:	
	case ALT_F9:	
		if(link.isready())
		{
			link.fenold="";
			link.sideToMove = (msg.wParam==ALT_F1)? (link.local_color()):(!link.local_color());					//连接时轮到对方（远程）走子了
			bConnected = true;
			std::cout << "连线已开始!\n" << (link.sideToMove==link.local_color()?"轮到我（下）方":"轮到远（上）方") << std::endl;
		}
		else
		{
			std::cout << "方案未准备好！或未启动客户端！ ALT_F10可载入方案！" << std::endl;
		}
		break;

	case ALT_F6:	
		if(bConnected)
		{
			bConnected = false;
			//SuspendThread(hLoopMove);				//暂停线程
			//SuspendThread(hWaitForBestmove);		//暂停线程
			ucci.StopEngine();
			std::cout << "已断开连接，引擎停止思考！" << std::endl;
		}
		else
		{
			std::cout << "根本未连接，无需断开。" << std::endl;
		}
		break;

	case ALT_F7:	
		if(bConnected)
		{
			CLockRegion _lock(&lock);				//_lock被创建，调用构造函数，加锁，进入临界区
			link.update_fen();
			std::cout << link.print() << std::endl;				//显示当前盘面
			std::cout << "连接测试结果如上" << std::endl;		//显示当前盘面
			ucci.model=1-ucci.model;							//切换思考模式
			std::cout << "当前思考模式: " << (ucci.model?"时间":"深度") << std::endl;
		}
		else
		{
			std::cout << "未连接状态，ALT_F1或ALT_F9 可连接客户端！" << std::endl;
		}
		g_biBitCount = g_biBitCount==24? 32:24;
		std::cout << "当前屏幕截图位深: " <<  g_biBitCount << " 位" << std::endl;

		break;

	case ALT_F8:	
		//根据配置参数初始化为不同的局面，典型的：我方执黑时，红方左半边或右半边
		//制作方案时，复位初始盘面为原始局面，也可走一步棋或多步（只要确保两盘面一致即可）
		link.set("rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w");			

		std::cout<< ((link.make_solution(msg.pt))? "方案制作OK" : "方案制作不成功！") << std::endl;
		break;

	case ALT_F10:	
		bConnected = bConnected? false:true;
		OPENFILENAME myofn;
		char strFile[MAX_PATH];
		memset(&myofn,0,sizeof(OPENFILENAME));
		memset(strFile,0,sizeof(char)*MAX_PATH);
		myofn.nMaxFile=MAX_PATH;
		myofn.lStructSize=sizeof(OPENFILENAME);
		myofn.Flags=OFN_FILEMUSTEXIST;
		myofn.lpstrFilter="方案配置文件(*.lll)\0*.lll\0\0";	//要选择的文件后缀 
		myofn.lpstrInitialDir = ".";						//默认的文件路径 
		myofn.lpstrFile=strFile;							//存放文件的缓冲区  
		if(GetOpenFileName(&myofn))		//strFile得到用户所选择文件的路径和文件名 
		{
			PathStripPath(strFile);		//strFile得到文件名
		}

		if(link.load_solution(strFile))									
		{
			std::cout << link.print();
			std::cout << "Load solution ok!!\n" << std::endl;
			std::cout << "please link again!!\n" << std::endl;
		}
		break;

	case ALT_F11:	
		if(bConnected)
		{
			bAutoGo = !bAutoGo;
			std::cout << (bAutoGo? "自动走棋开!":"自动走棋关!") << std::endl;
			if(bAutoGo && link.from.x!=-1)
			{
				link.make_move(link.from, link.to);		//模拟图形走子	
				link.from.x=link.from.y=link.to.x=link.to.y=-1;	//设置走子完成标志
				Sleep(500);								//（假定500毫秒后走子成功！更好的办法是检测下个盘面发生变化，且走子正好就是刚才的bestmove 则更好。
				std::cout << "切换后有步未走！已走完？" << std::endl << std::endl;
			}
		}
		else
		{
			std::cout << "未连接状态，ALT_F1或ALT_F9 可连接客户端！" << std::endl;
		}
		break;

	case ALT_F12:	
		if(bConnected)
		{
			bGoInfinite = !bGoInfinite;
			if(bGoInfinite)ucci.StopEngine();
			std::cout << (bGoInfinite? "无限分析模式开!":"无限分析模式关!") << std::endl;
		}
		else
		{
			std::cout << "未连接状态，ALT_F1或ALT_F9 可连接客户端！" << std::endl;
		}
		break;

	case CTRL_ALT_F1:	
		if(bConnected)
		{
			std::cout << "连接状态，请 ALT_F6 先断开连接！" << std::endl;
		}
		else
		{
			cout << "CTRL_ALT_F1 pressed!\n" << endl;
			cout << "循环修改参数，‘q’退出!" << endl;
			cout << "s save solution!" << endl;
			cout << "l load solution!" << endl;
			char c;
			while((c=getchar())!='q')
			{
				switch(c)
				{
				case 's':
					cout << "方案已保存!" << endl;
					break;
				case 'l':
					cout << "方案已载入!" << endl;
					break;
				case 'c':
					g_biBitCount = g_biBitCount==24? 32:24;
					std::cout << "当前屏幕截图位深: " <<  g_biBitCount << " 位" << std::endl;
					break;
				default:
					cout << "\n循环修改参数，‘q’退出!" << endl;
					cout << "s save solution!" << endl;
					cout << "l load solution!" << endl;
					cout << "请选择!" << endl;
					break;
				}
			}
			cout << "已退出修改参数!" << endl;
		}
		break;

	case CTRL_ALT_F2:	
		if(bConnected)
		{
			std::cout << "连接状态，请 ALT_F6 先断开连接！" << std::endl;
		}
		else
		{
			std::cout << "当前样本大小："<< link.get_width_init() << "请输入新的样本大小: ";
			std::cin >> width_init;
			std::cout << "当前实时子大小："<< link.get_width_real() << "请输入新的实时子大小: ";
			std::cin >> width_real;
			std::cout << "当前分辨率："<< link.get_threshold() << "请输入新的分辨率: ";
			std::cin >> threshold;
			link.set_width_init(width_init);
			link.set_width_real(width_real);
			link.set_threshold (threshold);
			std::cout << "\n更新样本大小：" << link.get_width_init() << std::endl;
			std::cout << "更新实时子大小：" << link.get_width_real() << std::endl;
			std::cout << "更新分辨率：" << link.get_threshold() << std::endl;

			std::cout << "当前等待时间："<< timeWaitBM << " 请输入新的最佳着法超时等待时间（缺省1000）: ";
			std::cin >> timeWaitBM;
			std::cout << "最佳着法超时等待时间：" << timeWaitBM << std::endl;
		
			int time;
			std::cout << "请输入思考时间（缺省15000毫秒）: ";
			std::cin >> time;
			ucci.set_time(time);
			std::cout << "已设置新的思考时间!" << std::endl;

			std::cout << "请输入思考深度（缺省8层）: ";
			std::cin >> time;
			ucci.set_depth(time);
			std::cout << "已设置新的思考深度!" << std::endl;
			std::cout << "已退出参数设置!" << std::endl;
		}
		break;

	case CTRL_ALT_F3:	
		if(bConnected)
		{
			bBH = !bBH;
			std::cout << (bBH? "兵河分析开!":"兵河分析关!") << std::endl;
		}
		else
		{
			std::cout << "客户端未连接，请 ALT_F1 或 ALT_F9 先连接！" << std::endl;
		}
		break;

	case CTRL_ALT_F4:	
		if(bConnected)
		{
			ucci.StopEngine();
			std::cout << "引擎已停止思考，立即出步！！" << std::endl;
		}
		else
		{
			std::cout << "根本未连接，引擎处于空闲状态，停什么停 :) " << std::endl;
		}
		//link.sideToMove=1-link.sideToMove;
		//std::cout << "\n交换了走子权。现在轮到："<< (link.sideToMove==link.local_color()?"我方了":"对方了")<< std::endl;
		break;

	default:
		break;	
	}
}
