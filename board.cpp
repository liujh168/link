
/********************************************************************************
模块名         : 扩展库
文件名         : board.cpp
相关文件       : board.h
文件实现功能   : 
作者           :
版本           : 1.10
--------------------------------------------------------------------------------
备注           :
--------------------------------------------------------------------------------
修改记录 :
日  期          版本            修改人          修改内容
2003/08/27      1.0                             创建
2003/12/29      1.1                             新增正则表达式、

================================================================================
* 版权所有(c) 2014, 2015, , 保留所有权利
********************************************************************************/

#include "board.h"

#pragma warning( disable:4996 )				//屏蔽函数安全性警告
#pragma warning( disable:4101 )				//屏蔽未使用变量警告

CNewQiJu :: CNewQiJu(): m_PlayerTop(m_PlayerUcci) , m_PlayerBottom(m_PlayerUcci) //初始化列表方式初始化成员对象
{
      memcpy(m_ChessBoard,InitChessBoard,90);
      m_bBottomGo=true ;
      m_WhichPlayer =m_bBottomGo?&m_PlayerBottom:&m_PlayerTop;

      m_PlayerBottom.m_iChessMoveFrom =ENGINE_PLAYER ;
//      m_PlayerBottom.m_bProgBottom = true;

      m_PlayerTop.m_iChessMoveFrom = BMP_PLAYER ;
//      m_PlayerTop.m_bProgBottom=false;

      m_bConnected = false;
      m_bGameOver=false ;
}

CNewQiJu ::~CNewQiJu()
{
}

void CNewQiJu :: WCHAR2CHESSMOVE(WCHAR wsChessMove[5], CHESSMOVE &cmChessMove )
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
}

void CNewQiJu :: ChangeSide(BYTE position[10][9])
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

int CNewQiJu :: IsGameOver(BYTE position[10][9])
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

void CNewQiJu ::Start()
{
      unsigned char killed ;

      //保存起始盘面
      m_PlayerTop.GetBMPBoard();
      m_PlayerTop.BMP2Position();
      memcpy(m_ChessBoard, m_PlayerTop.m_position,90);

      while( m_bConnected )       //连接后循环  //多线程运行时不能重入VCL，除非.....
      {
          //一、谁走？
          m_WhichPlayer = m_bBottomGo? &m_PlayerBottom:&m_PlayerTop;

          //二、怎么走？
          bool  moveOK=false;
          //TDateTime startDateTime=Now(), currentDateTime;
          do
          {     //如果是图形客户端，此处考虑扫描频率
                //currentDateTime=Now();
                //if((currentDateTime.Val-startDateTime.Val)*360*24 > 1) //1分钟
                if(!m_bConnected)    //连接断开
                {
                     // Application->MessageBox("GetQiShouMove 超时！\n退出连接...","结果",MB_OK);
                     // m_bConnected=false;
                     break;
                }
                m_cmQiJuMove = m_WhichPlayer->GetMove(m_ChessBoard) ;
                moveOK = (m_cmQiJuMove.From.y!=-1 && m_cmQiJuMove.From.x!=-1);
          }while(!moveOK);

          if(moveOK)
          {
                //三、走吧
                killed = MakeMove(&m_cmQiJuMove);//仅更新Cnewqiju实例数据，交换了走子权，但不含客户端走子
                switch(m_WhichPlayer->m_iChessMoveFrom) //继续完成额外的处理
                {
                  case ENGINE_PLAYER:
                       if(strcmp(m_WhichPlayer->m_ucci.szLineStr, "nobestmove")==0)//引擎认输了
                       {
                           m_bConnected=false;    //断开连接
                           break;
                       }
                       break;

                  case BMP_PLAYER:  //我方是客户端，对手棋步还需在我方客户端上走一步      //这个不支持了
                       if(!m_WhichPlayer->BMPGo(m_cmQiJuMove ,!m_bBottomGo) )
                       {
                         //ShowMessage("客户端走子错误！");
                         UnMakeMove(  &m_cmQiJuMove,killed);
                       }
                       Sleep(1000);
                       break;

                  case VOICE_PLAYER:
                       break;

					default:
					  break;
                }
          }
     }
}

BYTE CNewQiJu ::MakeMove(CHESSMOVE* move)
{
	BYTE nChessID;
	nChessID = m_ChessBoard[move->To.y][move->To.x];
	m_ChessBoard[move->To.y][move->To.x] = m_ChessBoard[move->From.y][move->From.x];
	m_ChessBoard[move->From.y][move->From.x] = NOCHESS;
        m_bBottomGo=!m_bBottomGo;
        m_WhichPlayer=m_bBottomGo?&m_PlayerBottom:&m_PlayerTop;
        m_iTurn++;
	return nChessID;
}

void CNewQiJu::UnMakeMove(CHESSMOVE* move,BYTE nChessID)
{
	m_ChessBoard[move->From.y][move->From.x] = m_ChessBoard[move->To.y][move->To.x];
	m_ChessBoard[move->To.y][move->To.x] = nChessID;
        m_bBottomGo=!m_bBottomGo;
        m_WhichPlayer=m_bBottomGo?&m_PlayerBottom:&m_PlayerTop;
        m_iTurn--;
}

BOOL CNewQiJu::IsValidMove(BYTE position[10][9],CHESSMOVE mv)
{
        // only according to position: black on top and red below!

	int i, j;
	int nMoveChessID, nTargetID;
        int nFromX=mv.From.x;
        int nFromY=mv.From.y;
        int nToX=mv.To.x;
        int nToY=mv.To.y;

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