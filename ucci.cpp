#pragma hdrstop

#include <string>
#include "ucci.h"

CUcci ucci;

CUcci :: CUcci() 
{
	think_time=8000; 
	think_depth=8;
	model=1;
	if(LoadEngine())
	{
		strcpy_s(szEngineName, 1024, "myucci");
		printf("load engine ok!\n");
	}
}

CUcci :: ~CUcci()
{
	UnLoadEngine();
}

bool CUcci::LoadEngine(void)
{
	bUcciOkay = false;
	pipeEngine.Open("jqxq.exe");
	pipeEngine.LineOutput("ucci");
	pipeEngine.LineOutput("isready");
	Sleep(1000);//要延时一会，否则以下收不到信息！
	while(ReceiveUCCI()) ;
	return bUcciOkay;
}

bool CUcci::UnLoadEngine(void)
{
	pipeEngine.LineOutput("stop");
	pipeEngine.LineOutput("quit");
	Sleep(1000);
	while(ReceiveUCCI()) ;
	pipeEngine.Close();
	return !bUcciOkay;
}

void CUcci::UCCI2Engine( const char * eCommand )
{
	pipeEngine.LineOutput( eCommand );
}

// UCCI反馈信息的接收过程
bool CUcci::ReceiveUCCI(void)
{
	if (!pipeEngine.LineInput(szLineStr))
	{
		return FALSE;
	}
	if (strncmp(szLineStr, "bestmove ", 9) == 0)
	{
		nStatus =  IDLE_NONE ;

		strcpy_s(mvPonder,  szLineStr + 9);
		mvPonder[4] = 0;

		strcpy_s(mvPonderFinished,  szLineStr + 9);
		if ( strncmp(szLineStr + 13, " ponder ", 8) == 0)
		{
			nStatus =  IDLE_PONDER_FINISHED ;
			strcpy_s(mvPonder,  szLineStr +21);
		}
	}
	else if (strcmp(szLineStr, "nobestmove") == 0)
	{
		nStatus =  IDLE_NONE ;
	}
	else if (strcmp(szLineStr, "bye") == 0)
	{
		bUcciOkay = FALSE;
	}
	else if (strcmp(szLineStr, "ucciok") == 0)
	{
		nStatus =  IDLE_NONE ;
		bUcciOkay = TRUE;
	}
	return TRUE;
}


char* CUcci::RunEngine(char * FenStr)
{
	static const int BUFFERSIZE=1024;
	char *lpLineChar;
	static char buffer[BUFFERSIZE];

	// 0.引擎忙标志
	nStatus =  BUSY_THINK;

	// 1. 发送局面信息，包括初始的不可逆FEN串和一系列后续着法(连同后台思考的猜测着法)；
	lpLineChar = buffer;
	lpLineChar += sprintf_s(lpLineChar,BUFFERSIZE,"position fen %s ", FenStr);
	pipeEngine.LineOutput( buffer );

	// 2. 发送思考指令。
	 //sprintf(lpLineChar, nStatus == BUSY_PONDER ? "go ponder %s" : "go %s", FenStr);
	lpLineChar = buffer;

	if(model)		
		lpLineChar += sprintf_s(lpLineChar,BUFFERSIZE,"go time %d ", think_time);
	else
		lpLineChar += sprintf_s(lpLineChar,BUFFERSIZE,"go depth %d ", think_depth);

	pipeEngine.LineOutput( buffer );

	return buffer;
}

// 中止UCCI引擎的思考
void CUcci::StopEngine(void)
{
	pipeEngine.LineOutput("stop");
}


