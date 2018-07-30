#pragma once

#include <windows.h>

//#define TIME_WAIT_BM  5000				//等待最佳着法时间

DWORD CALLBACK wait_bestmove_threadproc(PVOID pvoid);			// 线程回调函数,用于处理引擎思考信息连续显示
DWORD CALLBACK ThreadProcLoopMove(LPVOID lparam);
