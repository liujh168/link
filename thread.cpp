#include <iostream>

#include "thread.h"
#include "linker.h"
#include "lock.h"
#include "ucci.h"
#include "hotkey.h"

extern HANDLE	g_bmEvent;	
int timeWaitBM	=	500;		//

//检测引擎运行状态线程
//获取并显示bestmove及pv等数据
//根据bestmove计算自动走子模拟所需的数据POINT
//设置准备好信号，唤醒走子线程
DWORD CALLBACK wait_bestmove_threadproc(PVOID pvoid)
{
	while(true)
	{
		CLockRegion _lock(&lock);	//_lock被创建，调用构造函数，加锁，进入临界区
		while(ucci.ReceiveUCCI())
		{	
			string lines(ucci.szLineStr);
			//以下针对检测到引擎返回的不同结果分别处理
			if( char* bestmove=strstr(ucci.szLineStr,"nobestmove") )		//这种检测方法，必须先检测nobestmove才不会出错
			{
				//CLockRegion _lock(&lock);	//_lock被创建，调用构造函数，加锁，进入临界区
				link.from.x = link.to.x = -1;
				link.from.y = link.to.y = -1;
                std::cout << "nobestmove\n" << std::endl;
			}
			else if( char* bestmove=strstr(ucci.szLineStr,"bestmove") )		//bestmove
			{
				string str(ucci.szLineStr);

				Square frm = square_from_string(str.substr(0+9, 2));
				Square tto = square_from_string(str.substr(2+9, 4));
				Move bstmv = Move( (frm << 8) | tto );

				string title = link.GetStepName(bstmv);
				if(link.local_color()==BLACK)	//下方为黑方，返回棋步不需变换，因为之前旋转过后交引擎返回走步（坐标形式）得到中文的表示法正好吻合旋转之后的棋盘。
				{  
					link.ChangeSide();
					title = link.GetStepName(bstmv);
					link.ChangeSide();
				}

		        //CLockRegion _lock(&lock);	//_lock被创建，调用构造函数，加锁，进入临界区
				//h7e7   (7,7)-(4,7)   炮二平五
				link.from.x = bestmove[9]-'a';
				link.from.y = 9 - (bestmove[10]-'0');
				link.to.x =   bestmove[11]-'a';
				link.to.y =   9 - (bestmove[12]-'0');
				if(link.local_color()==BLACK)			//如果下方为黑方，则返回的棋步要变换一下
				{
					link.from.x = 8 -link.from.x ;
					link.from.y = 9 -link.from.y ;
					link.to.x = 8 -link.to.x ;
					link.to.y = 9 -link.to.y ;
				}
				std::cout << "最佳着法已返回： " << title << std::endl;
				SetEvent(g_bmEvent);					//设置bestmove准备好信号状态
			}
			else if(char* pvstr = strstr(ucci.szLineStr,"pv"))				//返回 pvstr
			{
				string ucipv(pvstr);
				std::vector<Move> pv;	//pv.push_back(MOVE_NONE);

				//CLockRegion _lock(&lock);	//_lock被创建，调用构造函数，加锁，进入临界区

				if( ucipv.size()>= 7 )	//至少一个走法  "pv b2e2"  "pv b2e2 b7e7 a0a2";
				{
					CMatLinker curr;
					
					curr.set(link.fen());
					if(link.local_color()==BLACK)	//下方为黑方，返回棋步不需变换，因为之前旋转过后交引擎返回走步（坐标形式）得到中文的表示法正好吻合旋转之后的棋盘。
					{  
						curr.ChangeSide();
					}

					cout << "curr pv: " ;
					for(unsigned int i =0; i< (ucipv.size()-2)/5; i++)
					{
						string uci_move = ucipv.substr(0+3+5*i, 4);
						Square frm = square_from_string(uci_move.substr(0, 2));
						Square tto = square_from_string(uci_move.substr(2, 2));
						Move bstmv = Move( (frm << 8) | tto );
						cout << curr.GetStepName(bstmv) << " ";
						curr.do_move(bstmv);
					}
					cout << endl; 
				}
			}
			else if(char* pvstr = strstr(ucci.szLineStr,"depth"))
			{
				string ucipv(pvstr);
				//cout << ucipv << endl; 
			}
			else if(char* pvstr = strstr(ucci.szLineStr,"time"))
			{
				string ucipv(pvstr);
				//cout << ucipv << endl; 
			}
		}
        //函数块退出，_lock被析构，解锁，离开临界区
	}
	return 0;
}

//循环走子线程
//由主线程的定时器定期更新局面
//本线程函数判断是否局面发生变化及控制双方走子动作
//如轮到我方，则启动引擎计算，并阻塞等待另一个线程检测返回引擎计算结果bestmove
//如轮到远方走子，则等待或者后台思考对方招法
DWORD WINAPI ThreadProcLoopMove(LPVOID lparam)
{
	static CMatLinker old;

	while(true)
	{
		if(!bConnected)
		{
			continue;
		}
		while(bConnected && WAIT_OBJECT_0 != WaitForSingleObject(g_bmEvent, timeWaitBM))		//阻塞等待请求返回 bestmove，等待时间应设置为可变量，变相实现定时器：每隔一定时间更新盘面。
		{
			CLockRegion _lock(&lock);				//_lock被创建，调用构造函数，加锁，进入临界区
			//更新盘面
			link.update_fen();

			//非初始盘面，根据前后盘面变化情况确定当前走子方
			if(link.fenold!="")						
			{
				old.set((char*)link.fenold.c_str());
				int diffs = link.board2diffs(old.m_Board);
				if(diffs==0)	//盘面未发生变化，继续循环
				{
					continue;	
				}
				if(diffs==2)	//有且仅有一方走了子，交换走子权
				{
					link.sideToMove = 1- link.sideToMove ;	
				}
			}
			link.fenold = link.fen();			//更新旧串
			std::cout << link.print() << std::endl;
			std::cout << "\n现在轮到："<< (link.sideToMove==link.local_color()?"我方了":"对方了")<< std::endl;
			
			
			//启动引擎来分析（外置与内置）
			string fentoengine = link.fen();			//更新输送给引擎计算的fen串
			if(bBH)	start_bh(link.fen(), link.sideToMove ==  link.local_color());				//启动外部程序兵河来分析

			//启动内置引擎来分析
			if( link.sideToMove ==  link.local_color() )	//轮到我方走,注释掉则可分析双方
			{
				ucci.StopEngine();
				if(link.local_color()==BLACK)				//如果下方为黑方，修订一下Fen。满足ucci引擎的缺少约定（红下黑上）
				{
					link.ChangeSide();
					fentoengine = link.fen();	
					link.ChangeSide();
				}
				if(bAutoGo)
				{
					ucci.RunEngine((char*)fentoengine.c_str());
					std::cout << "自动走子打开！ 引擎启动分析！请等待返回结果（ALT_F7可切换深度或时间控制模式）......" << std::endl;
				}
				else if(bGoInfinite)
				{
					static const int BUFFERSIZE=1024;
					static char buffer[BUFFERSIZE];
					char *lpLineChar = buffer;
					lpLineChar += sprintf_s(lpLineChar, BUFFERSIZE, "position fen %s ", (char*)fentoengine.c_str());
					ucci.UCCI2Engine(buffer);					//发送局面信息
					ucci.UCCI2Engine("go infinite");            //开启无限分析模式
					std::cout << "自动走子关闭！ 引擎启动无限分析模式。（ALT_F12也可开启无限思考模式）" << std::endl;
				}
			}
		}

		//至此，bestmove返回。由另一个线程设置信号标志
		ResetEvent(g_bmEvent);										//系统会自动设置吗？
		std::cout << "bestmove 已返回！ ALT_F11切换自动走子！" << std::endl;
		if( bAutoGo && link.sideToMove == link.local_color())		//自动走子开关打开，并且轮到下方走子
		{
			link.make_move(link.from, link.to);						//模拟图形走子	
			link.from.x=link.from.y=link.to.x=link.to.y=-1;			//设置走子完成标志
			Sleep(500);												//（假定500毫秒后走子成功！更好的办法是检测下个盘面发生变化，且走子正好就是刚才的bestmove 则更好。
			std::cout << "500毫秒自动走子已完成！" << std::endl << std::endl;
		}
		//函数块退出，_lock被析构，解锁，离开临界区
	}
	return 0;
}