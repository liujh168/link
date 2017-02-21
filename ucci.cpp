#include <vcl.h>
#pragma hdrstop

#include "ucci.h"
CUcci :: CUcci()
{
      LoadEngine();
}
CUcci :: ~CUcci()
{
      UnLoadEngine();
}
bool CUcci::LoadEngine(void)
{
      bUcciOkay = false;
      pipeEngine.Open("engine.exe");

      pipeEngine.LineOutput("ucci");
      pipeEngine.LineOutput("isready");
      while(ReceiveUCCI()) ;
      return bUcciOkay;
}

bool CUcci::UnLoadEngine(void)
{
      pipeEngine.LineOutput("quit");
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
        bool       bBgThink;
//  　　MoveStruct　mv;

      if (!pipeEngine.LineInput(szLineStr))
      {
          return FALSE;
      }
      if (strncmp(szLineStr, "bestmove ", 9) == 0)
      {
  //          mv.Move(*(long *) (szLineStr + 9));
  //          MakeMove(mv);
            ShowMessage(szLineStr);
            if (bBgThink && strncmp(szLineStr + 13, " ponder ", 8) == 0)
            {
  //            mvPonder.Move(*(long *) (szLineStr + 21));
  //            RunEngine();
            }
  //          Ucci2QH.mvPonderFinished = mv;
      }
      else if (strcmp(szLineStr, "nobestmove") == 0)
      {
      }
      else if (strcmp(szLineStr, "bye") == 0)
      {
                bUcciOkay = FALSE;
      }
      else if (strcmp(szLineStr, "ucciok") == 0)
      {
                bUcciOkay = TRUE;
      }
      return TRUE;
}


char* CUcci::RunEngine(char * FenStr)
{
      int i;
      long dwMoveStr;
      char *lpLineChar;
      char buffer[1024];

      // 1. 发送局面信息，包括初始的不可逆FEN串和一系列后续着法(连同后台思考的猜测着法)；
      lpLineChar = buffer;
      lpLineChar += sprintf(lpLineChar, "position fen %s ", FenStr);
      pipeEngine.LineOutput( buffer );
      // 2. 发送思考指令。
      // sprintf(FenStr, nStatus == BUSY_PONDER ? "go ponder %s" : "go %s", FenStr);
       pipeEngine.LineOutput(" go depth 3");
}

  // 中止UCCI引擎的思考
  void CUcci::StopEngine(void)
  {
        pipeEngine.LineOutput("stop");
  }

