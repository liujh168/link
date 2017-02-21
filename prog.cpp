//---------------------------------------------------------------------------
#include <vcl.h>
//#include <winbase.h>

#include "formlink.h"  //just for accessing copyscrenn

#pragma hdrstop
#include "prog.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

#include<iostream>
#include<stdlib.h>
#include<stdio.h>


Cqishou :: Cqishou()
{
        m_dx = 35;
        m_dy = 35;

        m_iDI= 20;
        m_iDD = 0;
        m_pBMPData = NULL ;
        m_hbitmap  = NULL;

        m_hWndProg = NULL;
        m_hWndBoard = NULL;
        m_hWndSteps = NULL;
        m_iChessMoveFrom = 5 ;
        
        strcpy(m_ptcName,"nobody right now!");

}

Cqishou ::~Cqishou()
{

}

Cnewqiju :: Cnewqiju()
{
//    if(FileExists("link"))
//    {
//        GetFromFile("link");
//        m_bConnected = false;
//
////         m_PlayerTop.FindWinBoardAndSteps();
////         m_PlayerTop.FindWindowGloblStepsOff();
////         m_PlayerBottom.FindWinBoardAndSteps();
////         m_PlayerBottom.FindWindowGloblStepsOff();
//    }
//    else
    {
        m_PlayerTop.m_bProgBottom = false;
        m_PlayerTop.m_bQiShouBottom = false;
        m_PlayerTop.m_bQiShouRed = false;
        m_PlayerBottom.m_bProgBottom = true;
        m_PlayerBottom.m_bQiShouBottom = true;
        m_PlayerBottom.m_bQiShouRed = true;
        memcpy(m_ChessBoard,InitChessBoard,90);
        m_bBottomGo=true ;
        m_bConnected = false;
        m_bGameOver=false ;


        m_WhichPlayer =NULL;
    }
}

Cnewqiju ::~Cnewqiju()
{
//          if(FileExists("link"))
//          {
//              SaveToFile("link");
//          }
}

void Cnewqiju ::DrawCchessBorder(Cqishou player)
{
	HPEN hPen ,* hOldPen ;
        if(player.m_hWndBoard == NULL || !IsWindow(player.m_hWndBoard))
        {
            return;
        }
	HDC hdc = ::GetWindowDC( player.m_hWndBoard ) ;
	SetROP2( hdc , R2_NOT ) ;
	hPen = CreatePen ( PS_SOLID , 3 , RGB( 0 , 0 , 0 ) ) ;
	hOldPen = ( HPEN * )SelectObject( hdc , hPen ) ;
	SelectObject ( hdc ,GetStockObject ( NULL_BRUSH ) ) ;
        POINT pt1,pt2;
        int dx,dy;
        dx=player.m_dx;
        dy=player.m_dy;

        pt1.x = player.m_OXY.x;
        pt1.y = player.m_OXY.y;
        pt2.x=pt1.x+8*dx;
        pt2.y=pt1.y+9*dy;
        for(int i =0;i<10;i++)
        {
          MoveToEx(hdc,pt1.x,pt1.y+i*dy,NULL);
          LineTo(hdc,pt2.x,pt1.y+i*dy);
        }
        for(int i =0;i<9;i++)
        {
          MoveToEx(hdc,pt1.x+i*dx,pt1.y,NULL);
          LineTo(hdc,pt1.x+i*dx,pt2.y);
        }
	SelectObject( hdc , &hOldPen ) ;
	DeleteObject( hPen ) ;
	::ReleaseDC ( player.m_hWndBoard , hdc ) ;
}

void Cnewqiju ::DrawStepsBorder(Cqishou player)
{
	HPEN hPen ,* hOldPen ;
        if(player.m_hWndSteps == NULL || !IsWindow(player.m_hWndSteps))
        {
            return;
        }
	HDC hdc = ::GetWindowDC( player.m_hWndSteps ) ;
	SetROP2( hdc , R2_NOT ) ;
	hPen = CreatePen ( PS_SOLID , 3 , RGB( 0 , 0 , 0 ) ) ;
	hOldPen = ( HPEN * )SelectObject( hdc , hPen ) ;
	SelectObject ( hdc ,GetStockObject ( NULL_BRUSH ) ) ;

        POINT pt1,pt2 ,pt3,pt4;
        int dx,dy;

        pt1.x= player.m_StepsWinRect.left;
        pt1.y= player.m_StepsWinRect.top;

        pt2.x= player.m_StepsWinRect.right;
        pt2.y= pt1.y ;
        pt3.x= pt1.x ;
        pt3.y= player.m_StepsWinRect.bottom ;

        pt4.y= pt3.y ;
        pt4.x= pt2.x ;

        MoveToEx(hdc,pt1.x,pt1.y,NULL);
        LineTo(hdc,pt2.x,pt2.y);
        LineTo(hdc,pt4.x,pt4.y);
        LineTo(hdc,pt3.x,pt3.y);

	SelectObject( hdc , &hOldPen ) ;
	DeleteObject( hPen ) ;
	::ReleaseDC ( player.m_hWndSteps , hdc ) ;
}

BYTE  Cnewqiju ::GetBMPMove(Cqishou *player )
{
        POINT   pt1,pt2 ,       from,to;
        BYTE    qizi1,          qizi2;
        BYTE    killed ,ChessID;
        killed = ChessID = NOCHESS;

        bool    srcblackup = false ;
        bool    destblackup = false ;

       for(int m=3;m<6;m++)
        for(int n=0;n<3;n++)
        {
         if(player->m_position[n][m]== B_KING )
         {
              srcblackup = true;
              break;
         }
        }
       for(int m=3;m<6;m++)
        for(int n=0;n<3;n++)
        {
           if(m_ChessBoard[n][m]== B_KING )
           {
             destblackup = true;
             break;
           }
        }
        if( srcblackup ^ destblackup )
        {
            ChangeSide( player->m_position );
        }

        pt1.x = pt1.y=pt2.x=pt2.y=-1;
        from.x = from.y=to.x=to.y=-1;
        for(int i =0;i<10;i++)
        for(int j =0;j<9;j++)
        {
           qizi2 = m_ChessBoard[i][j];
           qizi1 = player->m_position[i][j];
           if(qizi1 != qizi2 )      // where qizi moved
           {
                if( qizi1 == NOCHESS )
                {
                        from.x = j;     from.y=i;
                }
                else
                {
                        to.x= j ;       to.y=i;
                        if ( qizi2!= NOCHESS )
                        {
                                killed = qizi2;
                        }
                        else
                        {
                                killed = NOCHESS;
                        }
                        ChessID = qizi1;
                 }
           }
        }

        if( srcblackup ^ destblackup )
        {
//             from.x = 9- from.x;
//             from.y = 10- from.y;
//             to.x   = 9- to.x;
//             to.y   = 10-to.y;
             ChangeSide( player->m_position );
        }

        player->m_cmQiShouStep.From.x   = from.x;
        player->m_cmQiShouStep.From.y   = from.y;
        player->m_cmQiShouStep.To.x     = to.x;
        player->m_cmQiShouStep.To.y     = to.y;
        player->m_cmQiShouStep.KillID   = killed ;
        player->m_cmQiShouStep.ChessID  = ChessID;

        m_cmQiJuMove=player->m_cmQiShouStep;

        return ChessID;

}


BOOL Cnewqiju ::GetQiShouMove(Cqishou * player)
{
      // this function get the qishou Move in different way
      //          check if size of board changed ?
      //////////////////////////////////////////////////////////////////////////////////
      ///通过引擎计算获取起步
       if( !player->m_iChessMoveFrom )
       {
             player->m_ucci.pipeEngine.LineInput(player->m_ucci.szLineStr);
             MainForm->mmo1->Lines->Add(m_PlayerBottom.m_ucci.szLineStr)  ;
             if (strncmp(player->m_ucci.szLineStr, "bestmove ", 9) == 0)
             {
                 
                 int i = 0;
                 int x1,y1,x2,y2;
                 x1= player->m_ucci.szLineStr[9]-97 ;
                 y1= player->m_ucci.szLineStr[10] -48 ;
                 x2= player->m_ucci.szLineStr[11] -97;
                 y2= player->m_ucci.szLineStr[12]-48 ;
                 y1=9-y1;   y2=9-y2;
//                 if( player->m_bQiShouBottom != player->m_bProgBottom  )
//                 {
//                      x1=8-x1;
//                      y1=9-y1;
//                      x2=8-x2;
//                      y2=9-y2;
//                 }
                 player->m_cmQiShouStep.From.x = x1;
                 player->m_cmQiShouStep.From.y = y1;
                 player->m_cmQiShouStep.To.x = x2;
                 player->m_cmQiShouStep.To.y = y2;
                 m_cmQiJuMove=player->m_cmQiShouStep;
                 // 应该增加检查合法性代码
                 return true;

             }
             else
             {
                 return false;
             }

       }
      //通过输入（键盘或鼠标）获取起步

      ///////////////////////////////////////如果能直接获取文字描述的起步，则直接转化成CCHESSMOVE
      if(player->m_hWndSteps != NULL)
      {
            if(!strcmp(player->m_ptcName,"象棋世家V5 - 91042"))
            {
                char * pchar = MyGetListViewItem(player);

                WCHAR buff[20];
                AnsiString temp(pchar);
                StringToWideChar(temp ,buff,10);
                WCHAR2CHESSMOVE(buff,player->m_cmQiShouStep);
                m_cmQiJuMove=player->m_cmQiShouStep;
                if(m_cmQiJuMove.ChessID != -1)
                {
                    return m_cmQiJuMove.ChessID;        //think step alawys correct!
                }
                else
                {
                    return false;
                }
            }

            if( strcmp(player->m_ptcName,"chess") ||true)
            {


                  int vItemCount , iStartPos;
                  m_cmQiJuMove.ChessID = -1;
//                  vItemCount=m_iTurn;
                  if(player->m_bQiShouRed)
                  {
                      iStartPos = 0;
                  }
                  else
                  {
                      iStartPos = 1;
                  }
                  long time ;
                  time = GetTickCount();
                  do
                  {
                      Application->ProcessMessages();
                      vItemCount = SendMessage(player->m_hWndSteps, LB_GETCOUNT, 0, 0);
                      if( !m_bConnected )
                         return false;
                  } while( vItemCount == m_iTurn );

                  for( int I = iStartPos ;I<=vItemCount - 1 ;I=I+2)
                  {
                          SendMessage(player->m_hWndSteps, LB_GETTEXT, I, (int) m_cStepBuff);
                  }

                  char tempstep[20];
                  WCHAR buff[20];
                  WCHAR wcstemp[8];
                  WCHAR * space=L" ";
                  AnsiString temp(strchr(m_cStepBuff,0x3a));

                  strcpy(tempstep,m_cStepBuff);
                  StringToWideChar(temp.SubString(2, 10) ,buff,10);
                  wcscpy(m_wsQiJuMove,buff);
                  int jj =   wcslen(m_wsQiJuMove);
                  int kk = 0;
                  for(int ii=0;ii<=jj;ii++)
                  {
                     if(m_wsQiJuMove[ii]!= space[0])
                     {
                          wcstemp[kk]=m_wsQiJuMove[ii];    //remove " "    or "　"
                          kk++;
                     }
                  }

                  WCHAR2CHESSMOVE(wcstemp,player->m_cmQiShouStep);
                  m_cmQiJuMove=player->m_cmQiShouStep;

            }

            if(m_cmQiJuMove.ChessID != -1)
            {
                return m_cmQiJuMove.ChessID;        //think step alawys correct!
            }
            else
            {
                return false;
            }

      }


//////////////////////////////////////////////////////////////////////////////////
//如果不能直接获取文字描述的起步，则通过扫描盘面变化获取起步。
      RECT  myrect;
      int width , height;

      width  = player->m_BoardWinRect.right -  player->m_BoardWinRect.left  ;
      height = player->m_BoardWinRect.bottom - player->m_BoardWinRect.top ;

      GetWindowRect(player->m_hWndBoard, &myrect);

      if(   myrect.right - myrect.left != width || myrect.bottom - myrect.top != height )
      {
          width  = player->m_ProgWinRect.right - player->m_ProgWinRect.left  ;
          height = player->m_ProgWinRect.bottom - player->m_ProgWinRect.top ;

          MoveWindow(player->m_hWndProg, player->m_ProgWinRect.left, player->m_ProgWinRect.right ,width,height ,true );
      }

      BringWindowToTop( player->m_hWndProg );
      try
      {

          if(player->m_hWndSteps == NULL)
          {
              if( player->GetBMPBoard()!=NULL )
              {
                    player->BMP2ChessBoard(player->m_hbitmap) ;
                    GetBMPMove( player );
              }
          }
          // chesk valide step;
          BYTE position[10][9];
          memcpy(position,  m_ChessBoard ,90);
          CHESSMOVE steptemp = m_cmQiJuMove;
          if( player->m_bQiShouRed != m_bBottomGo )
          {
                ChangeSide( position);
                steptemp.From.x = 8-steptemp.From.x;
                steptemp.From.y = 9-steptemp.From.y;
                steptemp.To.x = 8-steptemp.To.x;
                steptemp.To.y = 9-steptemp.To.y;

          }
          return( IsValidMove( position, steptemp.From.x ,steptemp.From.y, steptemp.To.x, steptemp.To.y) );

      }
      catch (...)
      {
          Application->MessageBox("link","GetBMPMove error!",0);
      }
}

bool  Cqishou ::Go(CHESSMOVE cmQiJuMove, bool bBottomGo)
{
        POINT from,to;
        bool min = false  , issuccess = true ;
        BYTE position[10][9];

        if( !m_iChessMoveFrom )
        {
                return true;
        }

        if(!IsWindow(m_hWndBoard)|| !IsWindow(m_hWndProg))
        {
	        return false;
        }

        m_cmQiShouStep =cmQiJuMove ;
        if( m_bProgBottom == bBottomGo )
        {
            m_cmQiShouStep.From.x = 8 -m_cmQiShouStep.From.x ;
            m_cmQiShouStep.From.y = 9 -m_cmQiShouStep.From.y ;
            m_cmQiShouStep.To.x = 8 -m_cmQiShouStep.To.x ;
            m_cmQiShouStep.To.y = 9 -m_cmQiShouStep.To.y ;
         }

        from.x=  m_OXY.x + m_cmQiShouStep.From.x* m_dx;
        from.y=  m_OXY.y + m_cmQiShouStep.From.y* m_dy;
        to.x=  m_OXY.x + m_cmQiShouStep.To.x* m_dx;
        to.y=  m_OXY.y + m_cmQiShouStep.To.y* m_dy;

        if(min=IsIconic(m_hWndProg))
        {
                ShowWindow(m_hWndProg,SW_SHOW);
        }

//        BringWindowToTop( m_hWndProg );
        int vItemCount1 ,vItemCount2;

        if ( m_hWndSteps!=NULL )
        {
            vItemCount1 = SendMessage(m_hWndSteps, LB_GETCOUNT, 0, 0);
        }
 try
 {
        GetBMPBoard();
        BMP2ChessBoard(m_hbitmap);
        memcpy(position,m_position,90);

	position[m_cmQiShouStep.To.y][m_cmQiShouStep.To.x] = m_position[m_cmQiShouStep.From.y][m_cmQiShouStep.From.x];
	position[m_cmQiShouStep.From.y][m_cmQiShouStep.From.x] = NOCHESS;

        SendMessage(m_hWndBoard, WM_LBUTTONDOWN, 0, from.y<<16 | from.x & 0x0000ffff);
        SendMessage(m_hWndBoard, WM_LBUTTONDOWN, 0, to.y<<16 | to.x & 0x0000ffff);

//                 check due to different moving way;

        if ( m_hWndSteps!=NULL )
        {
            if(!strcmp( m_ptcName,"象棋世家V5 - 91042"))
            {
                  return true;

                  const       nMaxLen=1023;
                  char        szBuf[nMaxLen+1];
                  HWND        hWindow = m_hWndSteps ;
                  int          nLVItemCount ;
                  DWORD        dwProcessID;
                  HANDLE       hProcess;
                  LVITEM       lvItemLocal;
                  DWORD        dwBytesRead, dwBytesWrite;
                  bool         bSuccess,bWriteOK;

                  GetWindowThreadProcessId(hWindow,&dwProcessID);
                  hProcess=OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwProcessID);
                  if(!hProcess)  //得不到指定进程的句柄
                      return NULL;
                  //在指定进程内分配存储空间
                  LPVOID lpTextRemote=VirtualAllocEx(hProcess,NULL,nMaxLen+1,MEM_COMMIT,PAGE_READWRITE);
                  LPVOID lpListItemRemote=VirtualAllocEx(hProcess,NULL,sizeof(LVITEM),MEM_COMMIT,PAGE_READWRITE);
                  if((!lpTextRemote) || (!lpListItemRemote)) //不能在指定进程内分配存储空间
                      return NULL;

//                  nLVItemCount=ListView_GetItemCount(hWindow);
//                  if(nLVItemCount == m_iTurn/2)             // eror!
//                  {
//                      issuccess=false;
//    //                          ShowMessage("Goes incorrectly! ");
//                  }
//                  else
//                  {
//                      issuccess=true;
//                  }

             }



              vItemCount2 = SendMessage(m_hWndSteps, LB_GETCOUNT, 0, 0);
              if(vItemCount2 == vItemCount1)       //if not add below ,not correct!
              {
                  issuccess=false;
//                          ShowMessage("Goes incorrectly! ");
              }
              else
              {
                  issuccess=true;
              }
        }
        else
        {
//            check if it is same as m_ChessBoard;
              GetBMPBoard();
              BMP2ChessBoard(m_hbitmap);
              for(int i = 0;i<10;i++)
               for(int j = 0;j<9 ;j++)
               {
                   if( m_position[i][j]!= position[i][j] )
                   {
                         issuccess = false;
                         break;
                   }
                   else
                   {
                         issuccess=true;
                   }
               }
        }

//        if(min)
//        {
//          ShowWindow(m_hWndProg,SW_SHOWMINNOACTIVE);
//        }
//
        return issuccess;
   }
   catch(...)
   {
        return false;
   }
}

void Cnewqiju :: WCHAR2CHESSMOVE(WCHAR wsChessMove[5], CHESSMOVE &cmChessMove )
{
    WCHAR * qian = L"前" ;
    WCHAR * hou = L"后" ;
    WCHAR * jing = L"进" ;
    WCHAR * tui  =  L"退" ;
    WCHAR * ping = L"平" ;
    //WCHAR   qizi[]=L"空将车马炮士象卒帅车马炮仕相兵" ;
    WCHAR   qizi[]=L"空将车马炮士象卒帅车马炮士相兵" ;
    WCHAR   weiziblack[]=L"123456789" ;
    WCHAR   weiziblackq[]=L"１２３４５６７８９" ;
    WCHAR   weizired[]=L"一二三四五六七八九" ;
    bool    Red;
    WCHAR   shi[10]=L"仕士";

    int     ChessID = NOCHESS ;
    int     x1,y1,x2,y2 ;
//   always assume black is above;
//    if(!m_PlayerBottom.m_bQiShouRed)
//    {
//         ChangeSide(m_ChessBoard);
//    }
//
   WCHAR * pweizi ;
   int 	   i,j;
   WCHAR *wchar =L"a";
   for(int oo=0;oo<4;oo++)
   {
      for(int ll=0;ll<9;ll++)
      {
          if(wsChessMove[oo]==weiziblackq[ll] )
          {
             wsChessMove[oo]=weiziblack[ll];
          }
          if( wsChessMove[oo]==shi[0])
          {
             wsChessMove[oo]=shi[1];
          }
      }
   }
    if(m_bBottomGo)
    {
        Red = m_PlayerBottom.m_bQiShouRed;
    }
    else
    {
        Red = m_PlayerTop.m_bQiShouRed;
    }
    if(Red)
    {
         pweizi=weizired;
         i=8;
    }
    else
    {
        pweizi=weiziblack;
         i=0;
    }
    if((wsChessMove[0]==qian[0])||(wsChessMove[0]==hou[0]))
    {
        wchar[0]=wsChessMove[1];
    }
    else
    {
        wchar[0]=wsChessMove[0];
    }

    for( i; i<15; i++ )
     {
         if( wchar[0]  == qizi[i] )
         {
               ChessID=i;  break;

         }
     }

     if(wsChessMove[0]==qian[0] && m_bBottomGo || wsChessMove[0]==hou[0] && !m_bBottomGo )
        {
              for( int ii=0;ii<9;ii++)
              for( int jj=0;jj<10;jj++)
             {
                  if( m_ChessBoard[jj][ii] == ChessID)
                  {
                       x1=ii;       y1=jj;     break;
                  }
             }
         }
     else if(wsChessMove[0]==qian[0] && !m_bBottomGo  || wsChessMove[0]==hou[0]&&m_bBottomGo )
        {
              for( int ii=0;ii<9;ii++)
              for( int jj=9;jj>=0;jj--)
             {
                  if( m_ChessBoard[jj][ii] == ChessID)
                  {
                       x1=ii;       y1=jj;  break;
                  }
             }
         }

   else
     {
         for( j=0;j<9;j++)
         {
             if( wsChessMove[1] == pweizi[j] )
             {
                   if(m_bBottomGo)   x1=8-j;
                   else          x1=j;
                   break;
             }
         }

         for( int jj=0;jj<10;jj++)
         {
              if( m_ChessBoard[jj][x1] == ChessID )
              {
                        y1=jj;     break;
              }
         }
     }

////////////////////////////////////////////////////////////////////////
     if(wsChessMove[2] == ping[0])
     {
           for( j=0;j<15;j++)
           {
               if( wsChessMove[3] == pweizi[j] )
               {
                     if(m_bBottomGo)  x2=8-j;
                     else         x2=j;
                     y2=y1;
                     break;
               }
           }
     }

   if(wsChessMove[2] == jing[0])
     {
           for( j=0;j<9;j++)
           {
               if( wsChessMove[3] == pweizi[j] )
               {
                     if(ChessID==B_HORSE ||ChessID==R_HORSE)
                     {
                              if(m_bBottomGo)x2=8-j; else x2=j;
                              if(abs(x1-x2)==1)
                              {
                                 if(m_bBottomGo)  y2=y1-2;  else y2=y1+2;
                              }
                              else
                              {
                                 if(m_bBottomGo)  y2=y1-1;  else y2=y1+1;
                              }
                              break;
                     }
                     else if(ChessID==B_BISHOP ||ChessID==R_BISHOP)
                     {
                                if(m_bBottomGo)  x2=8-j;     else x2=j;
                                if(m_bBottomGo)
                                {
                                        y2=y1-1;
                                        if(y2<7)
                                        {
                                                y1+=2;
                                                y2=y1-1;
                                        }
                                }
                                else
                                {
                                        y2=y1+1;
                                        if(y2>2)
                                        {
                                                y1-=2;
                                                y2=y1+1;
                                        }
                                }
                                break;
                     }
                     else if(ChessID==B_ELEPHANT ||ChessID==R_ELEPHANT)
                     {
                                if(m_bBottomGo)x2=8-j; else x2=j;
                                if(m_bBottomGo)
                                {
                                        y2=y1-2;
                                        if(y2<5)
                                        {
                                                y1+=4;
                                                y2=y1-2;
                                        }
                                }
                                else
                                {
                                        y2=y1+2;
                                        if(y2>4)
                                        {
                                                y1-=4;
                                                y2=y1+2;
                                        }
                                }
                                break;
                     }
                     else
                     {
                                 x2=x1;
                                 if(m_bBottomGo) y2=y1-j-1;     else y2=y1+j+1;
                                 break;

                     }
               }
           }
     }

    if(wsChessMove[2] == tui[0])
     {
           for( j=0;j<9;j++)
           {
               if( wsChessMove[3] == pweizi[j] )
               {
                     if(ChessID==B_HORSE ||ChessID==R_HORSE)
                     {
                              if(m_bBottomGo)x2=8-j; else x2=j;
                              if(abs(x1-x2)==1)
                              {
                                    if(m_bBottomGo)  y2=y1+2;   else y2=y1-2;
                              }
                              else
                              {
                                    if(m_bBottomGo)  y2=y1+1;   else y2=y1-1;
                              }
                              break;
                     }
                     else if(ChessID==B_BISHOP ||ChessID==R_BISHOP) //error
                     {
                                if(m_bBottomGo)x2=8-j; else x2=j;
                                if(m_bBottomGo)
                                {
                                        y2=y1+1;
                                        if(y2>9)
                                        {
                                                y1-=2;
                                                y2=y1+1;
                                        }
                                }
                                else
                                {
                                        y2=y1-1;
                                        if(y2<0)
                                        {
                                                y1+=2;
                                                y2=y1-1;
                                        }
                                }
                                break;
                     }
                     else if(ChessID==B_ELEPHANT ||ChessID==R_ELEPHANT) //error
                     {
                                if(m_bBottomGo)x2=8-j; else x2=j;
                                if(m_bBottomGo)
                                {
                                        y2=y1+2;
                                        if(y2>9)
                                        {
                                                y1-=4;
                                                y2=y1+2;
                                        }
                                }
                                else
                                {
                                        y2=y1-2;
                                        if(y2<0)
                                        {
                                                y1+=4;
                                                y2=y1-2;
                                        }
                                }
                                break;
                     }
                     else
                     {
                                 x2=x1;
                                 if(m_bBottomGo)  y2=y1+j+1; else y2=y1-j-1;
                                 break;
                     }
               }
           }
     }

    cmChessMove.ChessID=ChessID;
    cmChessMove.From.x=x1;
    cmChessMove.From.y=y1;
    cmChessMove.To.x=x2;
    cmChessMove.To.y=y2;
    //check if it is valid
    //change back
//    if(!m_PlayerBottom.m_bQiShouRed)
//    {
//         ChangeSide(m_ChessBoard);
//    }

}

void Cnewqiju :: ChangeSide(BYTE position[10][9])
{
    BYTE temp[10][9];
    int i,j ;
    memcpy(temp,position,90);
    for(i=0;i<10;i++)
    for(j=0;j<9;j++)
    {
        if(IsBlack(temp[i][j]))
        {
            temp[i][j] += 7 ;
        }
        else if(IsRed(temp[i][j]))
        {
            temp[i][j] -= 7 ;
        }
    }
    memcpy(position,temp,90);
}

int Cnewqiju :: IsGameOver(BYTE position[10][9])
{
    BOOL RedLive=FALSE,BlackLive=FALSE ;
    int i,j ;
    for(i=7;i<10;i++)
    for(j=3;j<6;j++)
    {
        if(position[i][j]==B_KING)
        BlackLive=TRUE ;
        if(position[i][j]==R_KING)
        RedLive=TRUE ;
    }

    for(i=0;i<3;i++)
    for(j=3;j<6;j++)
    {
        if(position[i][j]==B_KING)
        BlackLive=TRUE ;
        if(position[i][j]==R_KING)
        RedLive=TRUE ;
    }
    if(RedLive&&BlackLive)
    {
        return FALSE ;
    }
    else
    {
        m_bGameOver=true ;
        return TRUE ;
    }
}


void Cnewqiju :: CHESSMOVE2WCHAR(CHESSMOVE cmChessMove , WCHAR wsChessMove[5])
{
   /* WCHAR *qian=L"前" ;
    WCHAR *hou=L"后" ;

    WCHAR * qizi ;
    WCHAR * weizi ;
    WCHAR qiziblack[]=L"空将车马炮士象卒帅车马炮仕相兵" ;
    WCHAR weiziblack[]=L"１２３４５６７８９" ;
    WCHAR qizired[]=L"空帅车马炮仕相兵将车马炮士象卒" ;
    WCHAR weizired[]=L"一二三四五六七八九" ;

    WCHAR *jing=L"进" ;
    WCHAR *tui=L"退" ;
    WCHAR *ping=L"平" ;
    wsChessMove[4]=0x0000;
    int x1,y1,x2,y2 ;
    int ChessID ;

    x1=cmChessMove.From.x ;
    y1=cmChessMove.From.y ;
    x2=cmChessMove.To.x ;
    y2=cmChessMove.To.y ;
    ChessID = m_ChessBoard[y1][x1];
    qizi= qiziblack;
    weizi = weiziblack ;
    if( x1<0 ||x1>8 || y1<0 || y1>9 || x2<0 ||x2>8 || y2<0 || y2>9 )
    {
      wcscpy(wsChessMove,L"out!");
      return 0;
    }
    if( !(IsRed(ChessID)||IsBlack(ChessID)) )
    {
      wcscpy(wsChessMove,L"NoRB");
    }
    if( ChessID ==NOCHESS )
    {
      wcscpy(wsChessMove,L"NoCC");
      return 0;
    }
    if(m_RedGo)
    else
    wsChessMove[0] = qizi[ChessID:ChessID ?];
    wsChessMove[1]=weizi[x1];
    for(int iii=0;iii<9; iii++)
    {
        if(ChessID == m_ChessBoard[iii][x1] && iii!=y1)
        {
            if(iii!=y2&& ChessID!=R_BISHOP&& ChessID!=R_ELEPHANT &&ChessID!=B_BISHOP&& ChessID!=B_ELEPHANT )
            {
              wsChessMove[1]=wsChessMove[0];
              if(iii<y2)
              {
                 wsChessMove[0] = qian[0];
              }
              else if(iii>y2)
              {
                 wsChessMove[0] = hou[0];
              }
             }
        }
    }

    // _itow(x1+1, &wsChessMove[1], 10);
    if(y1==y2)
    {
        wcscpy(&wsChessMove[2],L"平");
        wsChessMove[3]=weizi[x2];
    }
    else
    {
        if(y1<y2)
        {
            wcscpy(&wsChessMove[2],L"进");

        }
        if(y1>y2)
        {
            wcscpy(&wsChessMove[2],L"退");
        }
        switch(ChessID)
        {
            case B_HORSE :
            case B_BISHOP :
            case B_ELEPHANT :
                 wsChessMove[3]=weizi[x2];
                 break ;
            default :
                wsChessMove[3]=weizi[abs(y2-y1)-1];
                break ;
        }
    }
  }
  else
  {
    ChangeSide(m_ChessBoard);
    x1=8-cmChessMove.From.x  ;
    y1=9-cmChessMove.From.y ;
    x2=8-cmChessMove.To.x ;
    y2=9-cmChessMove.To.y ;
    ChessID = m_ChessBoard[y1][x1];
    qizi= qiziblack ;
    weizi = weiziblack ;
    if( m_PlayerTop.m_bQiShouRed )
    {
       qizi= qizired ;
       weizi = weizired ;
    }

    if( x1<0 ||x1>8 || y1<0 || y1>9 || x2<0 ||x2>8 || y2<0 || y2>9 )
    {
      wcscpy(wsChessMove,L"出界了啊");
      return 0;
    }
    if( ChessID ==NOCHESS )
    {
      wcscpy(wsChessMove,L"该位无子");
      return 0;
    }
    wsChessMove[0]=qizi[ChessID];
    wsChessMove[1]=weizi[x1];
    for(int iii=0;iii<9; iii++)
    {
        if(ChessID == m_ChessBoard[iii][x1] &&iii!=y1)
        {
            if(iii!=y2&& ChessID!=R_BISHOP&& ChessID!=R_ELEPHANT &&ChessID!=B_BISHOP&& ChessID!=B_ELEPHANT )
            {
              wsChessMove[1]=wsChessMove[0];
              if(iii<y2)
              {
                 wsChessMove[0] = qian[0];
              }
              else if(iii>y2)
              {
                 wsChessMove[0] = hou[0];
              }
             }
        }
    }

    // _itow(x1+1, &wsChessMove[1], 10);
    if(y1==y2)
    {
        wcscpy(&wsChessMove[2],L"平");
        wsChessMove[3]=weizi[x2];
    }
    else
    {
        if(y1<y2)
        {
            wcscpy(&wsChessMove[2],L"进");

        }
        if(y1>y2)
        {
            wcscpy(&wsChessMove[2],L"退");
        }
        switch(ChessID)
        {
            case B_HORSE :
            case B_BISHOP :
            case B_ELEPHANT :
                 wsChessMove[3]=weizi[x2];
                 break ;
            default :
                wsChessMove[3]=weizi[abs(y2-y1)-1];
                break ;
        }
    }
    ChangeSide(m_ChessBoard);
  }
  */
}



/*
//enum windows
BOOL CALLBACK EnumWindowsProc(HWND hWnd,int lParam)
{
    char lpszClassName[254],lpszWindowText[254];
    GetWindowText(hWnd,lpszWindowText,254);
    GetClassName(hWnd,lpszClassName,254);
    Form1->Memo1->Lines->Add(lpszWindowText);
    Form1->Memo1->Lines->Add(lpszClassName);
    Form1->Memo1->Lines->Add("--------------------");
    SetWindowText(hWnd,"hello,wolf!");
    return true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender)
{
    EnumWindows((WNDENUMPROC)EnumWindowsProc,0);
}
//---------------------------------------------------------
  */


bool Cnewqiju::SaveToFile(char * filename)
{
       // do it by C++builder file function
        int iFileHandle;
        BYTE * pBmpdataTop  ;
        BYTE * pBmpdataBottom  ;

        iFileHandle = FileCreate(filename);

        pBmpdataTop =   m_PlayerTop.m_pBMPData  ;
        pBmpdataBottom =   m_PlayerBottom.m_pBMPData  ;
        m_PlayerTop.m_pBMPData  =NULL;
        m_PlayerBottom.m_pBMPData  =NULL;

        //save data of this object
        FileWrite(iFileHandle, this, sizeof(*this));

        //save pic data   of both of player
        DWORD dwBitmapSize;

        if( pBmpdataTop !=NULL )
        {
            dwBitmapSize = (DWORD)m_PlayerTop.m_bm.bmWidthBytes* m_PlayerTop.m_bm.bmHeight * m_PlayerTop.m_bm.bmPlanes;
            FileWrite(iFileHandle, pBmpdataTop, dwBitmapSize  );

        }
        if( pBmpdataBottom !=NULL )
        {
            dwBitmapSize = (DWORD)m_PlayerBottom.m_bm.bmWidthBytes* m_PlayerBottom.m_bm.bmHeight * m_PlayerBottom.m_bm.bmPlanes;
            FileWrite(iFileHandle, pBmpdataBottom , dwBitmapSize  );
        }

        FileClose(iFileHandle);

        m_PlayerTop.m_pBMPData  = pBmpdataTop;
        m_PlayerBottom.m_pBMPData  =pBmpdataBottom;

        return true;
}

bool Cnewqiju::GetFromFile(char * filename)
{
       //do it by C++ libirary function
       FILE *fpOpenBook ;
       fpOpenBook = fopen(filename, "rb+");
       if (fpOpenBook == NULL )
       {
           return false;
       }
       BYTE * pdatatop  , * pdatabottom;
       DWORD dwBitmapSize ;

       pdatatop = m_PlayerTop.m_pBMPData ;
       pdatabottom =   m_PlayerBottom.m_pBMPData ;
       dwBitmapSize = (DWORD)m_PlayerTop.m_bm.bmWidthBytes* m_PlayerTop.m_bm.bmHeight * m_PlayerTop.m_bm.bmPlanes;

        //load data of myqiju
       int Len32= sizeof(*this) ;
       fread(this,Len32,1,fpOpenBook);

        if( pdatatop == NULL  )
        {
             // alloc mem to load pic data
              dwBitmapSize = (DWORD)m_PlayerTop.m_bm.bmWidthBytes* m_PlayerTop.m_bm.bmHeight * m_PlayerTop.m_bm.bmPlanes;
//              delete   m_PlayerTop.m_pBMPData ;
//              m_PlayerTop.m_pBMPData =NULL;
              m_PlayerTop.m_pBMPData = new BYTE [dwBitmapSize];

              fread(m_PlayerTop.m_pBMPData ,dwBitmapSize,1,fpOpenBook);
        }

        if( pdatabottom ==NULL )
        {
              dwBitmapSize = (DWORD)m_PlayerBottom.m_bm.bmWidthBytes* m_PlayerBottom.m_bm.bmHeight * m_PlayerBottom.m_bm.bmPlanes;

//              delete   m_PlayerBottom.m_pBMPData ;
//              m_PlayerBottom.m_pBMPData =NULL;
              m_PlayerBottom.m_pBMPData = new BYTE [dwBitmapSize];

              fread(m_PlayerBottom.m_pBMPData ,dwBitmapSize,1,fpOpenBook);
        }

        fclose(fpOpenBook);
        return true;
}

void Cnewqiju ::Start()
{

      while( m_bConnected )         ///多线程运行时不能重入VCL，除非.....
      {
          Application->ProcessMessages();

          m_WhichPlayer = m_bBottomGo? &m_PlayerBottom:&m_PlayerTop   ;

          if( !m_WhichPlayer->m_iChessMoveFrom  )
          {
              char * p;
              p = CopyJuMian(m_ChessBoard,m_bBottomGo);
              char buffer[1024];
              strcpy(buffer,"position fen ");
              strcat(buffer,p);

              MainForm->mmo1->Lines->Add(buffer);
              m_WhichPlayer->m_ucci.UCCI2Engine(buffer);
              MainForm->mmo1->Lines->Add("go depth 3");
              m_WhichPlayer->m_ucci.UCCI2Engine("go depth 3");

          }

          do
          {
              Application->ProcessMessages();
              //add time out code here ,to stop the engine and forbid it go immediately
              if( !m_bConnected )
                  return ;

          }while( !GetQiShouMove( m_WhichPlayer ) ) ;

 ///////////////////////////////////////////////////////////////////////////////
          m_WhichPlayer = !m_bBottomGo? &m_PlayerBottom:&m_PlayerTop   ;
          do
          {
              Application->ProcessMessages();
              if( !m_bConnected )
                  return ;
          }while( !m_WhichPlayer->Go(m_cmQiJuMove ,m_bBottomGo) );

          MakeMove(&m_cmQiJuMove);
          m_bBottomGo=!m_bBottomGo;
          m_iTurn++;
          m_WhichPlayer->m_cmQiShouStep.ChessID =NOCHESS;

     }
}



BYTE Cnewqiju ::MakeMove(CHESSMOVE* move)
{
	BYTE nChessID;
	nChessID = m_ChessBoard[move->To.y][move->To.x];
	m_ChessBoard[move->To.y][move->To.x] = m_ChessBoard[move->From.y][move->From.x];
	m_ChessBoard[move->From.y][move->From.x] = NOCHESS;
	return nChessID;
}

void Cnewqiju::UnMakeMove(CHESSMOVE* move,BYTE nChessID)
{
	m_ChessBoard[move->From.y][move->From.x] = m_ChessBoard[move->To.y][move->To.x];
	m_ChessBoard[move->To.y][move->To.x] = nChessID;
}

HWND Cqishou ::FindWinBoardAndSteps(int maxparent, char winclass[10][254],char winname[10][254])
{
//     首先求得顶级父窗口
    HWND   grandfather, parent,child;

//    strcpy(m_ptcName,&winname[maxparent][0]);
    if( maxparent < 0 )
    {
        return  NULL;
    }
    grandfather= FindWindow(NULL, m_ptcName)  ;

    char buffer[50]="Programe: ";
    strcat(buffer, m_ptcName) ;
    if(grandfather !=NULL)
    {
        m_hWndProg = grandfather;
    }
    else
    {
        strcat(buffer,"not find !") ;
        ShowMessage( buffer );
    }
    if( maxparent-1 < 0 )
    {
        return  grandfather;
    }
    parent = FindWindowEx(grandfather ,NULL, &winclass[maxparent-1][0], &winname[maxparent-1][0]);
    if( maxparent-2<0 )
    {
        return  parent ;
    }
    child = FindWindowEx(parent ,NULL, &winclass[maxparent-2][0], &winname[maxparent-2][0]);
//     逐次用FindWindowEx函数求出各级子窗口,untill find window board
    if( maxparent-3<0 )
    {
        return  child ;
    }
    for(int i=maxparent-3; i>=0; i--)
    {
        child = FindWindowEx(parent, NULL, &winclass[i][0], &winname[i][0]);
        if(child ==NULL)
        {
             i++;
             int count =10;
             do
             {      count--;
                    parent =  FindWindowEx(grandfather ,parent, &winclass[i][0], &winname[i][0]);
                    child = FindWindowEx(parent, NULL, &winclass[i][0], &winname[i][0]);
             } while( child ==NULL && count ) ;
             i--;
        }
    }
    return child;
}

BOOL Cnewqiju::IsValidMove(BYTE position[10][9], int nFromX, int nFromY, int nToX, int nToY)
{
        // only according to position: black on top and red below!

	int i, j;
	int nMoveChessID, nTargetID;
	
	if (nFromY ==  nToY && nFromX == nToX)
		return FALSE;//目的与源相同
	
	nMoveChessID = position[nFromY][nFromX];
	nTargetID = position[nToY][nToX];
	
	if (IsSameSide(nMoveChessID, nTargetID))
		return FALSE;//不能吃自己的棋
	
	switch(nMoveChessID)
	{
	case B_KING:     
		if (nTargetID == R_KING)//老将见面?
		{
			if (nFromX != nToX)
				return FALSE;
			for (i = nFromY + 1; i < nToY; i++)
				if (position[i][nFromX] != NOCHESS)
					return FALSE;
		}
		else
		{
			if (nToY > 2 || nToX > 5 || nToX < 3)
				return FALSE;//目标点在九宫之外
			if(abs(nFromY - nToY) + abs(nToX - nFromX) > 1)
				return FALSE;//将帅只走一步直线:
		}
		break;
	case R_BISHOP:   
		
		if (nToY < 7 || nToX > 5 || nToX < 3)
			return FALSE;//士出九宫	
		
		if (abs(nFromY - nToY) != 1 || abs(nToX - nFromX) != 1)
			return FALSE;	//士走斜线
		
		break;
		
	case B_BISHOP:   //黑士

		if (nToY > 2 || nToX > 5 || nToX < 3)
			return FALSE;//士出九宫	
		
		if (abs(nFromY - nToY) != 1 || abs(nToX - nFromX) != 1)
			return FALSE;	//士走斜线
		
		break;
		
	case R_ELEPHANT://红象
		
		if(nToY < 5)
			return FALSE;//相不能过河
		
		if(abs(nFromX-nToX) != 2 || abs(nFromY-nToY) != 2)
			return FALSE;//相走田字

		if(position[(nFromY + nToY) / 2][(nFromX + nToX) / 2] != NOCHESS)
			return FALSE;//相眼被塞住了
		
		break;
		
	case B_ELEPHANT://黑象 
		
		if(nToY > 4)
			return FALSE;//相不能过河
		
		if(abs(nFromX-nToX) != 2 || abs(nFromY-nToY) != 2)
			return FALSE;//相走田字

		if(position[(nFromY + nToY) / 2][(nFromX + nToX) / 2] != NOCHESS)
			return FALSE;//相眼被塞住了
		
		break;
		
	case B_PAWN:     //黑兵
		
		if(nToY < nFromY)
			return FALSE;//兵不回头
		
		if( nFromY < 5 && nFromY == nToY)
			return FALSE;//兵过河前只能直走
		
		if(nToY - nFromY + abs(nToX - nFromX) > 1)
			return FALSE;//兵只走一步直线:
		
		break;
		
	case R_PAWN:    //红兵
		
		if(nToY > nFromY)
			return FALSE;//兵不回头
		
		if( nFromY > 4 && nFromY == nToY)
			return FALSE;//兵过河前只能直走
		
		if(nFromY - nToY + abs(nToX - nFromX) > 1)
			return FALSE;//兵只走一步直线:

		break;
		
	case R_KING:     
		if (nTargetID == B_KING)//老将见面?
		{
			if (nFromX != nToX)
				return FALSE;//两个将不在同一列
			for (i = nFromY - 1; i > nToY; i--)
				if (position[i][nFromX] != NOCHESS)
					return FALSE;//中间有别的子
		}
		else
		{
			if (nToY < 7 || nToX > 5 || nToX < 3)
				return FALSE;//目标点在九宫之外
			if(abs(nFromY - nToY) + abs(nToX - nFromX) > 1) 
				return FALSE;//将帅只走一步直线:
		}
		break;
		
	case B_CAR:
	case R_CAR:      
		
		if(nFromY != nToY && nFromX != nToX)
			return FALSE;	//车走直线:
		
		if(nFromY == nToY)
		{
			if(nFromX < nToX)
			{
				for(i = nFromX + 1; i < nToX; i++)
					if(position[nFromY][i] != NOCHESS)
						return FALSE;
			}
			else
			{
				for(i = nToX + 1; i < nFromX; i++)
					if(position[nFromY][i] != NOCHESS)
						return FALSE;
			}
		}
		else
		{
			if(nFromY < nToY)
			{
				for(j = nFromY + 1; j < nToY; j++)
					if(position[j][nFromX] != NOCHESS)
						return FALSE;
			}
			else
			{
				for(j= nToY + 1; j < nFromY; j++)
					if(position[j][nFromX] != NOCHESS)
						return FALSE;
			}
		}
		
		break;
		
	case B_HORSE:    
	case R_HORSE:
		
		if(!((abs(nToX-nFromX)==1 && abs(nToY-nFromY)==2)
			||(abs(nToX-nFromX)==2&&abs(nToY-nFromY)==1)))
			return FALSE;//马走日字
		
		if	(nToX-nFromX==2)
		{
			i=nFromX+1;
			j=nFromY;
		}
		else if	(nFromX-nToX==2)
		{
			i=nFromX-1;
			j=nFromY;
		}
		else if	(nToY-nFromY==2)
		{
			i=nFromX;
			j=nFromY+1;
		}
		else if	(nFromY-nToY==2)
		{
			i=nFromX;
			j=nFromY-1;
		}

		if(position[j][i] != NOCHESS)
			return FALSE;//绊马腿
		
		break;
	
	case B_CANON:    
	case R_CANON:    
		
		if(nFromY!=nToY && nFromX!=nToX)
			return FALSE;	//炮走直线:
		
		//炮不吃子时经过的路线中不能有棋子
		
		if(position[nToY][nToX] == NOCHESS)
		{
			if(nFromY == nToY)
			{
				if(nFromX < nToX)
				{
					for(i = nFromX + 1; i < nToX; i++)
						if(position[nFromY][i] != NOCHESS)
							return FALSE;
				}
				else
				{
					for(i = nToX + 1; i < nFromX; i++)
						if(position[nFromY][i]!=NOCHESS)
							return FALSE;
				}
			}
			else
			{
				if(nFromY < nToY)
				{
					for(j = nFromY + 1; j < nToY; j++)
						if(position[j][nFromX] != NOCHESS)
							return FALSE;
				}
				else
				{
					for(j = nToY + 1; j < nFromY; j++)
						if(position[j][nFromX] != NOCHESS)
							return FALSE;
				}
			}
		}
		//炮吃子时
		else	
		{
			int count=0;
			if(nFromY == nToY)
			{
				if(nFromX < nToX)
				{
					for(i=nFromX+1;i<nToX;i++)
						if(position[nFromY][i]!=NOCHESS)
							count++;
						if(count != 1)
							return FALSE;
				}
				else
				{
					for(i=nToX+1;i<nFromX;i++)
						if(position[nFromY][i] != NOCHESS)
							count++;
						if(count!=1)
							return FALSE;
				}
			}
			else
			{
				if(nFromY<nToY)
				{
					for(j=nFromY+1;j<nToY;j++)
						if(position[j][nFromX]!=NOCHESS)
							count++;
						if(count!=1)
							return FALSE;
				}
				else
				{
					for(j=nToY+1;j<nFromY;j++)
						if(position[j][nFromX] != NOCHESS)
							count++;
						if(count!=1)
							return FALSE;
				}
			}
		}
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

void Cnewqiju::ContiSame( Cqishou playerto , Cqishou  playerfrom)
{
        memcpy(m_ChessBoard,playerfrom.m_position,90);     //position must be the same as BMP
//        Copy JUmian
        if (OpenClipboard(NULL))    //m_hWndBoard为程序窗口句柄
        {
              char * p=NULL;
              p= CopyJuMian(m_ChessBoard , true );

              DWORD dwLength = 2055; // 要复制的字串长度
              HANDLE hGlobalMemory = GlobalAlloc(GHND, dwLength + 1); // 分配内存
              LPBYTE lpGlobalMemory = (LPBYTE)GlobalLock(hGlobalMemory); // 锁定内存
              lstrcpy(lpGlobalMemory,p);
              ::EmptyClipboard(); // 清空剪贴板
              ::SetClipboardData(CF_TEXT, hGlobalMemory); // 将内存中的数据放置到剪贴板
              GlobalUnlock(hGlobalMemory); // 锁定内存块解锁

              HANDLE hClipMemory = ::GetClipboardData(CF_TEXT);// 获取剪贴板数据句柄
              dwLength = GlobalSize(hClipMemory); // 返回指定内存区域的当前大小
              LPBYTE lpClipMemory = (LPBYTE)GlobalLock(hClipMemory); // 锁定内存
              GlobalUnlock(hClipMemory); // 内存解锁
              ::CloseClipboard(); // 关闭剪贴板

        }
//        NewGame();
      HWND h=NULL,m,s,s1;
      h= FindWindow(NULL, "neuchess");

      int MAINMENU = 0 ;
      int SUBMENU  = 0 ;
      if(h!=0)
      {
          m=GetMenu(h);
          s=GetSubMenu(m,MAINMENU );
          s1=GetSubMenu(s,SUBMENU );
          SendMessage(h,WM_COMMAND,MAKELONG(GetMenuItemID(s,SUBMENU),0),(long)s1);
      }
      else
      {
        ShowMessage("cannot find window");
      }

//        PasteJuMian;
      MAINMENU = 4 ;
      SUBMENU  = 10 ;
      if(h!=0)
      {
          m=GetMenu(h);
          s=GetSubMenu(m,MAINMENU );
          s1=GetSubMenu(s,SUBMENU );
          SendMessage(h,WM_COMMAND,MAKELONG(GetMenuItemID(s,SUBMENU),0),(long)s1);
      }
//        RedGo;
      MAINMENU = 2 ;
      SUBMENU  = 0 ;
      if(h!=0)
      {
          m=GetMenu(h);
          s=GetSubMenu(m,MAINMENU );
          s1=GetSubMenu(s,SUBMENU );
          SendMessage(h,WM_COMMAND,MAKELONG(GetMenuItemID(s,SUBMENU),0),(long)s1);
      }
}

bool     Cqishou::CalcDx()
{
     POINT  src,dest;
//     src.x = m_iLeftTop.x ;
//     src.y = m_iLeftTop.y   ;
//    ::ScreenToClient(m_hWndBoard,&src);
//     src.x = src.x -m_iDI /2 ;
//     src.y = src.y -m_iDI /2 ;

     src.x = m_OXY.x -m_iDI /2 ;
     src.y = m_OXY.y -m_iDI /2 ;

    for(int ii = src.x + m_iDI *7 ;ii< m_BoardWinRect.right - m_BoardWinRect.left - m_iDI ;ii++ )
     {

           dest.x = ii;
           dest.y = src.y ;

           if( LocateBlackCar( dest , src ) )
           {
               m_dx = (dest.x -src.x )/8  ;
               m_dy = m_dx ;

              return true;
           }
     }

     return false;
}

bool     Cqishou::LocateBlackCar(POINT DestPt, POINT SrcPt )
{
     long int sumdd;
     BYTE     rgb;

     sumdd = 0 ;
     int i ,j;
     for( i  = 0  ; i < m_iDI ;     i++ )
     for( j  = 0  ; j < ( m_iDI*m_bm.bmBitsPixel/8 ) ;     j++ )
     {
          rgb= m_pBMPData[ (SrcPt.y+i)* m_bm.bmWidthBytes + SrcPt.x*m_bm.bmBitsPixel/8  +j ] ;
          rgb -= m_pBMPData[ (DestPt.y+i)* m_bm.bmWidthBytes + DestPt.x*m_bm.bmBitsPixel/8  +j ] ;
          if(rgb>0)
                  sumdd  =sumdd+ rgb;
          else
                  sumdd  =sumdd- rgb;
     }

     if(sumdd<= m_iDD )                     // similiar way ?
     {
         return true;
     }
     else
     {
        return false;
     }

}

bool     Cqishou::Similiar(POINT DestPt, POINT SrcPt )
{

}

BYTE *      Cqishou::BMP2ChessBoard(HBITMAP hbitmap)
{
     LPSTR    lpMem ;
     DWORD    dwBitmapSize ;
     BITMAP   bm;
     try
     {
           GetObject(hbitmap, sizeof(BITMAP), (LPSTR)&bm );
           dwBitmapSize = (DWORD)bm.bmWidthBytes* bm.bmHeight * bm.bmPlanes;
           if( dwBitmapSize !=  (DWORD)m_bm.bmWidthBytes* m_bm.bmHeight * m_bm.bmPlanes  )
           {
//                  Application->MessageBox("you can change the window size!","BMP2ChessBoard error!",0);
                  return NULL;
           }
           lpMem = new BYTE[dwBitmapSize] ;
           GetBitmapBits(hbitmap, dwBitmapSize, lpMem);      //the second BMP
     }
     catch(...)
     {
//           Application->MessageBox("BMP2ChessBoard error!","BMP2ChessBoard error!",0);
           return NULL;
     }
     POINT  src,dest,ooo;
//     ooo.x = m_iLeftTop.x ;
//     ooo.y = m_iLeftTop.y   ;
//
//     ShowWindow(m_hWndProg,SW_SHOW);
//    ::ScreenToClient(m_hWndBoard,&ooo);
     ooo.x = m_OXY.x -m_iDI /2 ;
     ooo.y = m_OXY.y -m_iDI /2 ;

      POINT QiziBMP[15];

      QiziBMP[NOCHESS].x =-1 ;     //black king
      QiziBMP[NOCHESS].y =-1 ;

      QiziBMP[B_KING].x =4 ;     //black king
      QiziBMP[B_KING].y =0 ;

      QiziBMP[B_CAR].x =0 ;
      QiziBMP[B_CAR].y =0 ;

      QiziBMP[B_HORSE].x =1 ;
      QiziBMP[B_HORSE].y =0 ;

      QiziBMP[B_CANON].x =1 ;
      QiziBMP[B_CANON].y =2 ;

      QiziBMP[B_BISHOP].x =3 ;
      QiziBMP[B_BISHOP].y =0 ;

      QiziBMP[B_ELEPHANT].x =2 ;
      QiziBMP[B_ELEPHANT].y =0 ;

      QiziBMP[B_PAWN].x =0 ;
      QiziBMP[B_PAWN].y =3 ;


      QiziBMP[R_KING].x =4 ;     //red king
      QiziBMP[R_KING].y =9 ;

      QiziBMP[R_CAR].x =0 ;
      QiziBMP[R_CAR].y =9 ;

      QiziBMP[R_HORSE].x =1 ;
      QiziBMP[R_HORSE].y =9 ;

      QiziBMP[R_CANON].x =1 ;
      QiziBMP[R_CANON].y =7 ;

      QiziBMP[R_BISHOP].x =3 ;
      QiziBMP[R_BISHOP].y =9 ;

      QiziBMP[R_ELEPHANT].x =2 ;
      QiziBMP[R_ELEPHANT].y =9 ;

      QiziBMP[R_PAWN].x =0 ;
      QiziBMP[R_PAWN].y =6 ;

      DWORD Sum[10][9][15];
      memset(Sum,0xff,10*9*15);

     for(int u=0;u<10;u++)
      for(int v=0;v<9;v++)
      {
         m_position[u][v] = 0;
         src.x = ooo.x + v * m_dx;
         src.y = ooo.y + u * m_dy ;

         for( int m =1 ;m<15; m++ )
         {
             dest.x = ooo.x + QiziBMP[m].x * m_dx;       //locate ma
             dest.y = ooo.y + QiziBMP[m].y  * m_dy;
             unsigned long int sumdd;
             BYTE     rgbsrc,rgbdest;
             sumdd=0 ;
             Sum[u][v][m]  = 0;
             int i ,j ;
             for( i  = 0  ; i < m_iDI ;     i++ )
             for( j  = 0  ; j < ( m_iDI*m_bm.bmBitsPixel/8 ) ;     j++ )
             {

                  if(   (src.y+i)* m_bm.bmWidthBytes + src.x*m_bm.bmBitsPixel/8  +j  > dwBitmapSize ||
                        (dest.y+i)* m_bm.bmWidthBytes + dest.x*m_bm.bmBitsPixel/8  +j>  dwBitmapSize   )
                  {
                        return NULL;
                  }
                   else
                  {
                        rgbsrc= lpMem[ (src.y+i)* m_bm.bmWidthBytes + src.x*m_bm.bmBitsPixel/8  +j  ] ;    //get rgb from different BMP
                        rgbdest= m_pBMPData[ (dest.y+i)* m_bm.bmWidthBytes + dest.x*m_bm.bmBitsPixel/8  +j ] ;
                        rgbsrc = rgbsrc-rgbdest;

                        if(rgbsrc >0)
                        {
                                sumdd  =sumdd+ rgbsrc;
                                Sum[u][v][m] = Sum[u][v][m]  +  rgbsrc;
                        }
                        else
                        {
                                sumdd  =sumdd- rgbsrc;
                                Sum[u][v][m] = Sum[u][v][m]  -  rgbsrc;
                        }
                  }
             }
             if(sumdd<= m_iDD )
             {
                   m_position[u][v] = m ;
             }

          }
      }

//      unsigned long int sumdd =0xffffffff ;
//      BYTE x,y;
//      for(int u=0;u<10;u++)
//       for(int v=0;v<9;v++)
//       {
//            m_position[u][v] = 0;
//       }
//      for( int m =1 ;m<15; m++)
//      {
//          sumdd =0xffffffff ;
//          x=-1;y=-1;
//          for(int u=0;u<10;u++)
//           for(int v=0;v<9;v++)
//           {
//                if( sumdd > Sum[u][v][m])
//                {
//                     sumdd = Sum[u][v][m] ;
//                     x = v;
//                     y = u;
//                }
//           }
//           if( !(x==-1 ||y==-1) )
//           {
//                m_position[y][x] = m ;
//           }
//      }

     delete [] lpMem ;

     return &m_position[0][0] ;  // may be incorrect due to local var.!!!
}

bool     Cqishou::SaveBoardData(HBITMAP hbitmap)
{
     BYTE *   lpMem ;
     GetObject(hbitmap, sizeof(BITMAP), (BYTE *)&m_bm );
     DWORD dwBitmapSize = (DWORD)m_bm.bmWidthBytes* m_bm.bmHeight * m_bm.bmPlanes;

     if(m_pBMPData!=NULL)
     {
         delete m_pBMPData;
         m_pBMPData = NULL;
     }
     lpMem = new BYTE[dwBitmapSize];
     GetBitmapBits(hbitmap, dwBitmapSize, lpMem);      //the second BMP
     m_pBMPData = lpMem;
}

HBITMAP  Cqishou::GetBMPBoard()       //  hbitmap copyscreentobitmap(lprect lprect)
{
        LPRECT          lprect;
        HDC             hscrdc, hmemdc;             // 屏幕和内存设备描述表
        HBITMAP         hbitmap, holdbitmap;    // 位图句柄
        int             nx, ny, nx2, ny2;           // 选定区域坐标
        int             nwidth, nheight;            // 位图宽度和高度
        int             xscrn, yscrn;               // 屏幕分辨率

        lprect = & m_BoardWinRect;
        if ( IsRectEmpty(lprect) )        // 确保选定区域不为空矩形
             return NULL;

        if(m_hWndBoard == NULL)
        {
             return NULL;

        }
        else if(!IsWindow(m_hWndBoard))
        {
             return NULL;
        }

        hscrdc = GetDC(m_hWndBoard);   //为window创建设备描述表
        hmemdc = CreateCompatibleDC(hscrdc);    //为window设备描述表创建兼容的内存设备描述表

        nx = lprect->left;                      // 获得选定区域坐标
        ny = lprect->top;
        nx2 = lprect->right;
        ny2 = lprect->bottom;
        // 获得屏幕分辨率
        xscrn = GetDeviceCaps(hscrdc, HORZRES );
        yscrn = GetDeviceCaps(hscrdc, VERTRES );
        //确保选定区域是可见的
        if (nx <0)
               nx = 0;
        if (ny < 0)
                ny = 0;
        if (nx2 > xscrn)
                nx2 = xscrn;
        if (ny2 > yscrn)
                ny2 = yscrn;
        nwidth = nx2 - nx;
        nheight = ny2 - ny;

        hbitmap = CreateCompatibleBitmap(hscrdc, nwidth, nheight);// 创建一个与屏幕设备描述表兼容的位图
        holdbitmap = SelectObject(hmemdc, hbitmap);  // 把新位图选到内存设备描述表中
        BitBlt(hmemdc, 0, 0, nwidth, nheight,hscrdc, 0, 0, SRCCOPY);// 把window拷贝到内存

        hbitmap = SelectObject(hmemdc, holdbitmap);  //得到window位图的句柄

        DeleteDC(hscrdc);     //清除
        DeleteDC(hmemdc);

//        if (OpenClipboard(m_hWndBoard))    //m_hWndBoard为程序窗口句柄
//        {
//
//              EmptyClipboard();   //清空剪贴板
//
//
//              SetClipboardData( CF_BITMAP, hbitmap);     //把屏幕内容粘贴到剪贴板上,  hbitmap 为刚才的屏幕位图句柄
//
//              CloseClipboard();  //关闭剪贴板
//        }

        m_hbitmap = hbitmap;
        return hbitmap;       // 返回位图句柄

}

/*
int savebitmaptofile(hbitmap hbitmap , lpstr lpfilename);
int savebitmaptofile(hbitmap hbitmap , lpstr lpfilename)
{
        //hbitmap 为刚才的屏幕位图句柄
        //lpfilename 为位图文件名

        hdc hdc;         //设备描述表

        int ibits;        //当前显示分辨率下每个像素所占字节数

        word wbitcount;    //位图中每个像素所占字节数

        //定义调色板大小， 位图中像素字节大小 ，位图文件大小 ， 写入文件字节数
        dword dwpalettesize=0, dwbmbitssize, dwdibsize, dwwritten;
        bitmap bitmap;      //位图属性结构

        bitmapfileheader bmfhdr;     //位图文件头结构

        bitmapinfoheader bi;         //位图信息头结构

        lpbitmapinfoheader lpbi;     //指向位图信息头结构

        handle fh, hdib, hpal,holdpal=null;    //定义文件，分配内存句柄，调色板句柄


        //计算位图文件每个像素所占字节数
        hdc = createdc("display",null,null,null);
        ibits = getdevicecaps(hdc, bitspixel) *
        getdevicecaps(hdc, planes);
        deletedc(hdc);
        if (ibits 〈 = 1)
        wbitcount = 1;
        else if (ibits 〈 = 4)
        wbitcount = 4;
        else if (ibits 〈 = 8)
        wbitcount = 8;
        else if (ibits 〈 = 24)
        wbitcount = 24;
        //计算调色板大小
        if (wbitcount 〈 = 8)
        dwpalettesize = (1 〈 〈 wbitcount) *
        sizeof(rgbquad);

        //设置位图信息头结构
        getobject(hbitmap, sizeof(bitmap), (lpstr)&bitmap);
        bi.bisize = sizeof(bitmapinfoheader);
        bi.biwidth = bitmap.bmwidth;
        bi.biheight = bitmap.bmheight;
        bi.biplanes = 1;
        bi.bibitcount = wbitcount;
        bi.bicompression = bi_rgb;
        bi.bisizeimage = 0;
        bi.bixpelspermeter = 0;
        bi.biypelspermeter = 0;
        bi.biclrused = 0;
        bi.biclrimportant = 0;

        dwbmbitssize = ((bitmap.bmwidth *
        wbitcount+31)/32)* 4
        *bitmap.bmheight ;
        //为位图内容分配内存
        hdib = globalalloc(ghnd,dwbmbitssize+
        dwpalettesize+sizeof(bitmapinfoheader));
        lpbi = (lpbitmapinfoheader)globallock(hdib);
        *lpbi = bi;
        // 处理调色板
        hpal = getstockobject(default_palette);
        if (hpal)
        {
        hdc = getdc(null);
        holdpal = selectpalette(hdc, hpal, false);
        realizepalette(hdc);
        }
        // 获取该调色板下新的像素值
        getdibits(hdc, hbitmap, 0, (uint) bitmap.bmheight,
        (lpstr)lpbi + sizeof(bitmapinfoheader)
        +dwpalettesize,
        (bitmapinfoheader *)
        lpbi, dib_rgb_colors);
        //恢复调色板
        if (holdpal)
        {
        selectpalette(hdc, holdpal, true);
        realizepalette(hdc);
        releasedc(null, hdc);
        }
        //创建位图文件
        fh = createfile(lpfilename, generic_write,
        0, null, create_always,
        file_attribute_normal | file_
        flag_sequential_scan, null);
        if (fh == invalid_handle_value)
        return false;
        // 设置位图文件头
        bmfhdr.bftype = 0x4d42; // "bm"
        dwdibsize = sizeof(bitmapfileheader)
        + sizeof(bitmapinfoheader)
        + dwpalettesize + dwbmbitssize;
        bmfhdr.bfsize = dwdibsize;
        bmfhdr.bfreserved1 = 0;
        bmfhdr.bfreserved2 = 0;
        bmfhdr.bfoffbits = (dword)sizeof
        (bitmapfileheader)
        + (dword)sizeof(bitmapinfoheader)
        + dwpalettesize;
        // 写入位图文件头
        writefile(fh, (lpstr)&bmfhdr, sizeof
        (bitmapfileheader), &dwwritten, null);
        // 写入位图文件其余内容
        writefile(fh, (lpstr)lpbi, dwdibsize,
        &dwwritten, null);
        //清除
        globalunlock(hdib);
        globalfree(hdib);
        closehandle(fh);
}



*/


//---------------------------------------------------------------------------
// 读取ListView中的Item
// hWindow为目标ListView的句柄
//---------------------------------------------------------------------------
char* Cnewqiju::MyGetListViewItem(Cqishou * player)
{
    const       nMaxLen=1023;
    char        szBuf[nMaxLen+1];
    HWND        hWindow = player->m_hWndSteps ;
    int          nLVItemCount ;
    DWORD        dwProcessID;
    HANDLE       hProcess;
    LVITEM       lvItemLocal;
    DWORD        dwBytesRead, dwBytesWrite;
    bool         bSuccess,bWriteOK;

    GetWindowThreadProcessId(hWindow,&dwProcessID);
    hProcess=OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwProcessID);
    if(!hProcess)  //得不到指定进程的句柄
        return NULL;
    //在指定进程内分配存储空间
    LPVOID lpTextRemote=VirtualAllocEx(hProcess,NULL,nMaxLen+1,MEM_COMMIT,PAGE_READWRITE);
    LPVOID lpListItemRemote=VirtualAllocEx(hProcess,NULL,sizeof(LVITEM),MEM_COMMIT,PAGE_READWRITE);
    if((!lpTextRemote) || (!lpListItemRemote)) //不能在指定进程内分配存储空间
        return NULL;

    long time ;
    time = GetTickCount();
    do
    {
        Application->ProcessMessages();
        nLVItemCount=ListView_GetItemCount(hWindow);
        if( !m_bConnected )
           return NULL;
    } while( nLVItemCount == m_iTurn/2 );

    ZeroMemory(szBuf,nMaxLen+1);
    bWriteOK= WriteProcessMemory(hProcess,lpTextRemote,(LPVOID)szBuf,nMaxLen+1,(LPDWORD)&dwBytesWrite);
    if(!bWriteOK) //写内存错误
    {
        return NULL;
    }

    lvItemLocal.iItem = nLVItemCount;
    if( player->m_bQiShouRed )
    {
            lvItemLocal.iSubItem=1;
    }
    else
    {
           lvItemLocal.iSubItem=2;
    }
    lvItemLocal.mask=LVIF_TEXT;
    lvItemLocal.cchTextMax=nMaxLen;
    lvItemLocal.pszText=(LPTSTR)lpTextRemote;
    dwBytesWrite=0;

    bWriteOK=WriteProcessMemory(hProcess,lpListItemRemote,(LPVOID)&lvItemLocal,sizeof(LVITEM),(LPDWORD)&dwBytesWrite);
    if(!bWriteOK) //写内存错误
    {
        return NULL ;
    }
    SendMessage(hWindow,LVM_GETITEMTEXT,(WPARAM) nLVItemCount-1,(LPARAM)lpListItemRemote);
    bSuccess=ReadProcessMemory(hProcess,lpTextRemote,szBuf,nMaxLen+1,&dwBytesRead);
    //从指定进程存储空间读取文本
    if(!bSuccess) //不能在指定进程内读取文本
    {
        return NULL ;
    }


    //在指定进程内释放存储空间
    VirtualFreeEx(hProcess,lpListItemRemote,0,MEM_RELEASE);
    VirtualFreeEx(hProcess,lpTextRemote,0,MEM_RELEASE);
    //关闭指定进程句柄
    CloseHandle(hProcess);
    return szBuf;
}


char*  Cnewqiju::CopyJuMian(BYTE position[10][9] , bool RedGo )
{
        char FenStr[2054];
        const char PieceChar[15] = { '/','k', 'r', 'n', 'c', 'a', 'b', 'p' ,'K', 'R', 'N','C', 'A','B', 'P' };
        strcpy(FenStr, "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR r - - 3 6");
        strcpy(FenStr, "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR r - - 3 6");
        memset(FenStr,0,2000);
	strcpy(FenStr, "");
        char * lines="/";
	// 保存棋盘
        int m =0;
	for(int i=0; i<10;  i++)
        {
              m =0;
              for(int j=0; j<9; j++)
              {
                      // "3akabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR b asdfdsafdsf- --- 56 67"
                      if(position[i][j])
                      {
                              if(m)
                              {
                                        char string[25];
                                        strcpy(string,"");
                                        string[1] = 0;
                                        strcat( FenStr, itoa( m, string, 10 ) );
                                        m=0;
                              }
                              char string[25];
                              strcpy(string,"");
                              string[0]= PieceChar[ position[i][j] ];
                              string[1] = 0;
                              strcat( FenStr , string );
                      }
                      else
                      {
                              m++;
                              if(j==8)
                              {
                                        char string[25];
                                        strcpy(string,"");
                                        string[1] = 0;
                                        strcat( FenStr, itoa( m, string, 10 ) );
                                        m=0;
                              }
                      }
              }
              if(i!=9)
              {
                strcat( FenStr ,lines );
              }
        }
        if(RedGo)
        {
              strcat( FenStr ," r - - 0 1");
        }
        else
        {
              strcat( FenStr ," b - - 0 1");
        }

	return FenStr;
}


BYTE *  Cnewqiju::PasteJuMian(char FenStr[2054])
{
        const char PieceChar[15] = { '/','k', 'r', 'n', 'c', 'a', 'b', 'p' ,'K', 'R', 'N','C', 'A','B', 'P' };
        strcpy(FenStr, "2bakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR r - - 3 6");
        BYTE position[10][9];
        for(int i=0;i<10;i++)
        for(int j=0;j<9;j++)
        {
                position[i][j]='f';
        }
        int len = strlen(FenStr);
        char ChessID ='0';

        int y =0 , x=0;
        for(int i = 0;i<len ;i++)
        {
              ChessID = FenStr[i] ;
//              ID= ChessID -48;
              if( ChessID <='9' && ChessID >='1' )
              {

                  for(int j=x ;j< x+ ChessID -48;j++)
                  {
                       position[y][j] = NOCHESS;

                  }
                  x = x+ ChessID -48;
              }
              else if( ChessID <='z' && ChessID >='a' ||   ChessID <='Z' && ChessID >='A' )

              {
                  for(int m=0 ;m< 15; m++)
                  if( ChessID == PieceChar[m])
                  {
                      position[y][x] = m;
                      x++;
                      break;
                  }
              }
              else if(ChessID == '/')
              {
                  y++;
                  x=0;
              }
              if(y>9 || ChessID==' '|| x>9)
              {
                     break;
              }
        }
	return &position[0][0];
}


