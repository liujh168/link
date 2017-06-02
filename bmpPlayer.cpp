/********************************************************************************
模块名         : 扩展库
文件名         : prog.cpp
相关文件       : prog.h
文件实现功能   : 类功能扩展
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

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <fstream>
#include <assert.h>
#include <iomanip>

#include <windows.h>

#include "toolsCV.h"
#include "board.h"
#include "bmpPlayer.h"
//#include "screenCapture.h"

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/legacy/legacy.hpp"

#pragma warning( disable:4996 )				//屏蔽函数安全性警告
#pragma warning( disable:4101 )				//屏蔽未使用变量警告

using namespace cv;
using namespace std;

	string nameID[15]={"nochess", "BKing", "BRook", "BKnight", "BCannon", "BBishop", "BAdvisor", "BPawn", "RKing", "RRook", "RKnight", "RCannon", "RBishop", "RAdvisor", "RPawn"};

	//CMatPlayer::CMatPlayer( )
//{
//    strcpy_s(m_ptcName, 50, "link.lll"); //棋手名称,如果是图形客户端则为窗口标题关键字
//    m_LRookInBoard.x = 52;
//    m_LRookInBoard.y = 62;
//    m_LRookInProg.x = 66;
//    m_LRookInProg.y = 120;
//    m_width=40;							 //分辨率 ，样本棋子宽 = 高
//    m_iDD=0;							 //误差,相似度。
//    m_pBoardBitmapBits = NULL;			 //指向位图数据块（初始棋盘数据）
//    m_pBMPQiziBits = NULL;
//    m_pCurBMPBoardBits = NULL;
//    if(GetFromFile(m_ptcName))
//    {
//        FindWinBoard(); //装入方案，主要还原以上参数信息
//    }
//}

CMatPlayer::CMatPlayer(CUcci& ucci, string config, BYTE width, double f):m_ucci(ucci)
{
	strcpy_s(m_ptcName, 50, config.c_str()); //棋手名称,如果是图形客户端则为窗口标题关键字
    m_LRookInBoard.x = 52;
    m_LRookInBoard.y = 62;
    m_LRookInProg.x = 66;
    m_LRookInProg.y = 120;
    m_width=width;						 //分辨率 ，样本棋子宽 = 高
    m_iDD=0;							 //误差,相似度。
    m_pBoardBitmapBits = NULL;			 //指向位图数据块（初始棋盘数据）
    m_pBMPQiziBits = NULL;
    m_pCurBMPBoardBits = NULL;
	m_threshold = f;

    if(GetFromFile(m_ptcName))
    {
        FindWinBoard(); //装入方案，主要还原以上参数信息
    }
}

CMatPlayer::~CMatPlayer()
{
    if( m_pBoardBitmapBits != NULL  )
    {
          delete   m_pBoardBitmapBits ;
          m_pBoardBitmapBits =NULL;
    }
    if( m_pCurBMPBoardBits != NULL  )
    {
          delete   m_pCurBMPBoardBits ;
          m_pCurBMPBoardBits =NULL;
    }
    if( m_pBMPQiziBits != NULL  )
    {
          delete   m_pBMPQiziBits ;
          m_pBMPQiziBits =NULL;
    }
}

bool  CMatPlayer::BMPGo(CHESSMOVE cmQiJuMove, bool bBottomGo)   //图形棋手走棋
{
        POINT from,to;
        bool issuccess = false ;
        BYTE position[10][9];

        //assert(IsWindow(m_hWndBoard)&&IsWindow(m_hWndProg));
        //assert(m_iChessMoveFrom);  //m_iChessMoveFrom非零时，肯定不是UCCI

        m_Move =cmQiJuMove ;
       // if( m_bProgBottom != bBottomGo ) //本棋手在下面，又轮下面下，转换棋步格式，上面不必
        {
            m_Move.From.x = 8 -m_Move.From.x ;
            m_Move.From.y = 9 -m_Move.From.y ;
            m_Move.To.x = 8 -m_Move.To.x ;
            m_Move.To.y = 9 -m_Move.To.y ;
         }

        from.x=  m_LRookInBoard.x + m_Move.From.x* m_dx;
        from.y=  m_LRookInBoard.y + m_Move.From.y* m_dy;
        to.x=  m_LRookInBoard.x + m_Move.To.x* m_dx;
        to.y=  m_LRookInBoard.y + m_Move.To.y* m_dy;

        if(IsIconic(m_hWndProg))
        {
                ShowWindow(m_hWndProg,SW_SHOW);
        }
//        BringWindowToTop( m_hWndProg );       //可以试一下不同的函数效果
//        SetForegroundWindow ( m_hWndProg );


        memcpy(position,m_position,90);        //走子之前保存棋盘数据

        //走棋，共两步，下面走两步不能要
//	position[m_Move.To.y][m_Move.To.x] = m_position[m_Move.From.y][m_Move.From.x];
//	position[m_Move.From.y][m_Move.From.x] = NOCHESS;

        SendMessage(m_hWndBoard, WM_LBUTTONDOWN, 0, from.y<<16 | from.x & 0x0000ffff);
        SendMessage(m_hWndBoard, WM_LBUTTONUP, 0, from.y<<16 | from.x & 0x0000ffff);

        SendMessage(m_hWndBoard, WM_LBUTTONDOWN, 0, to.y<<16 | to.x & 0x0000ffff);
        SendMessage(m_hWndBoard, WM_LBUTTONUP, 0, to.y<<16 | to.x & 0x0000ffff);
        return true;

//         check if it is same as m_ChessBoard;
        GetBMPBoard();
        BMP2Position();
        for(int i = 0;i<10;i++)
         for(int j = 0;j<9 ;j++)
         {
             if( m_position[i][j]!= position[i][j] )
             {
                   issuccess = true;
                   break;
             }
         }
        if(IsIconic(m_hWndProg))
        {
          ShowWindow(m_hWndProg,SW_SHOWMINNOACTIVE);
        }
        return issuccess;
}

//
HWND CMatPlayer::FindWinProg(void)
{
    HWND   parent=NULL,brother=NULL;
    m_iLeftTop = m_LRookInProg;    //通过 m_LRookInProg 找到棋盘窗口句柄
    char bufferclass[1024];
    char buffertitle[1024];
   //assert( m_MaxParentWinCountBoard >= 0 );

    strcpy(buffertitle,"CycloneGui 优化者：阿♂姚");

//     一、首先根据关键字求得顶级父窗口
    m_hWndProg = FindWindow(NULL, m_ptcName) ;//用棋手名找，找到了更好
    if(m_hWndProg==NULL)//没找到再用关键字找。这个比较麻烦，要枚举窗口，找到标题并比较
    {     parent= GetDesktopWindow();
          brother = GetWindow(parent,GW_CHILD);   //获得桌面第一个子窗口
          while (brother != NULL)
          {
              GetClassName(brother, bufferclass, 256);		//获得窗口类名
              GetWindowText(brother, buffertitle, 256);		//获得窗口标题
              if(strstr(buffertitle,m_ptcName)!=NULL)		//还可以增加比较类名是否相同
              {
					m_hWndProg= brother;
					break;
              }
              brother = GetWindow(brother,GW_HWNDNEXT); //继续下一个子窗口
          }
    }

    //找到了就闪一下，调试用
    FlashWindow(m_hWndProg,true);
    if(IsIconic(m_hWndProg))
    {
         ShowWindow(m_hWndProg,SW_SHOWNORMAL);
    }

    return m_hWndProg;
}

HWND CMatPlayer::FindWinBoard(void)    //根据关键字和窗口信息数组找程序窗口和棋盘窗口
{
    HWND   WndChild=NULL;
    char bufferclass[1024];
    char buffertitle[1024];
    //assert( m_MaxParentWinCountBoard >= 0 );
    strcpy(buffertitle,"CycloneGui 优化者：阿♂姚");

//     一、首先根据关键字求得顶级父窗口
    if(NULL==m_hWndProg)
    {
        m_hWndProg= FindWinProg();
		if(NULL==m_hWndProg)
		{
			return NULL;
		}
    }

//      二、其次根据数组信息找到子窗口
    int i;
//    &m_ClassNameBoard[m_MaxParentWinCountBoard][0] //这个是顶层窗口类名。
    WndChild = m_hWndProg ;
    for(int i=m_MaxParentWinCountBoard-1; i>=0; i--)
    {
        WndChild = FindWindowEx(WndChild, NULL, &m_ClassNameBoard[i][0], &m_WinNameBoard[i][0]);
        if(WndChild!=NULL)
        {
            m_hWndBoard=WndChild;
        }
    }

    if(IsIconic(m_hWndProg))
    {
          ShowWindow(m_hWndProg,SW_SHOWNORMAL);
    }
   
	if(NULL==m_hWndBoard)  //没找到继续找，这次用鼠标点位置查找，但可能不准确！
    {
		  BringWindowToTop( m_hWndProg );
		 ::ClientToScreen(m_hWndProg, &m_LRookInProg); //转换后m_iLeftTop to Screen 保存了鼠标指向黑车的位置，相对屏幕而言的坐标
		 m_hWndBoard = WindowFromPoint(m_iLeftTop);
    }
	
	return m_hWndBoard;
}

bool CMatPlayer::LoadSolution(char* filename)
{
	if( GetFromFile(filename) && (FindWinProg()!=NULL) && (FindWinBoard()!=NULL) )
	{
		m_imgBoardInit = hwnd2mat(m_hWndBoard);
		CalcDxDy_CV();								//计算棋盘格宽
		PiecesMat();								//保存各兵种图像
		return true;
	}
	return false;
}

//4、基于结构相似性（SSIM,structural similarity (SSIM) index measurement）的方法
//结构相似性理论认为，自然图像信号是高度结构化的，即像素间有很强的相关性，特别是空域中最接近的像素，这种相关性蕴含着视觉场景中物体结构的重要信息；HVS的主要功能是从视野中提取结构信息，可以用对结构信息的度量作为图像感知质量的近似。结构相似性理论是一种不同于以往模拟HVS低阶的组成结构的全新思想，与基于HVS特性的方法相比，最大的区别是自顶向下与自底向上的区别。这一新思想的关键是从对感知误差度量到对感知结构失真度量的转变。它没有试图通过累加与心理物理学简单认知模式有关的误差来估计图像质量，而是直接估计两个复杂结构信号的结构改变，从而在某种程度上绕开了自然图像内容复杂性及多通道去相关的问题.作为结构相似性理论的实现，结构相似度指数从图像组成的角度将结构信息定义为独立于亮度、对比度的，反映场景中物体结构的属性，并将失真建模为亮度、对比度和结构三个不同因素的组合。用均值作为亮度的估计，标准差作为对比度的估计，协方差作为结构相似程度的度量。
//数据以scalar格式保存，读取scalar内的数据即可获取相应的相似度值，其中值的范围在0～1之间，1为完全一致，0为完全不一至。
double CMatPlayer::Similiar( const Mat& i1, const Mat& i2)
{
	assert(i1.rows==i2.rows && i1.cols==i2.cols);			//图像大小一样才能比较相似性

	const double C1 = 6.5025, C2 = 58.5225;
	//***************************** INITS **********************************/
	int d     = CV_32F;

	Mat I1, I2;
	i1.convertTo(I1, d);           // cannot calculate on one byte large values
	i2.convertTo(I2, d);

	Mat I2_2   = I2.mul(I2);        // I2^2
	Mat I1_2   = I1.mul(I1);        // I1^2
	Mat I1_I2  = I1.mul(I2);        // I1 * I2

	//*************************** END INITS **********************************/

	Mat mu1, mu2;   // PRELIMINARY COMPUTING
	GaussianBlur(I1, mu1, Size(11, 11), 1.5);
	GaussianBlur(I2, mu2, Size(11, 11), 1.5);

	Mat mu1_2   =   mu1.mul(mu1);
	Mat mu2_2   =   mu2.mul(mu2);
	Mat mu1_mu2 =   mu1.mul(mu2);

	Mat sigma1_2, sigma2_2, sigma12;

	GaussianBlur(I1_2, sigma1_2, Size(11, 11), 1.5);
	sigma1_2 -= mu1_2;

	GaussianBlur(I2_2, sigma2_2, Size(11, 11), 1.5);
	sigma2_2 -= mu2_2;

	GaussianBlur(I1_I2, sigma12, Size(11, 11), 1.5);
	sigma12 -= mu1_mu2;

	///////////////////////////////// FORMULA ////////////////////////////////
	Mat t1, t2, t3;

	t1 = 2 * mu1_mu2 + C1;
	t2 = 2 * sigma12 + C2;
	t3 = t1.mul(t2);              // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))

	t1 = mu1_2 + mu2_2 + C1;
	t2 = sigma1_2 + sigma2_2 + C2;
	t1 = t1.mul(t2);               // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))

	Mat ssim_map;
	divide(t3, t1, ssim_map);      // ssim_map =  t3./t1;

	Scalar mssim = mean( ssim_map ); // mssim = average of ssim map

	return (mssim.val[0] + mssim.val[1] + mssim.val[2]) /3;
}

/**************************************************************************************************
 * 函数名称：Similiar()
 * 参数:
 *      POINT   DestPt          - 目标棋子左上角点
 *      BYTE *  pDstBMP         - 目标棋盘窗口位数据指针
 *      POINT   SrcPt           - 源棋子左上角点
 *      BYTE *  pSrcBMP         - 源棋盘窗口位数据指针
 * 返回值:
 *      unsigned long int       - 返回两个棋子的相似度。
 * 说明:
 *      两个棋子大小比较相等，否则可能出错。
 *      已把棋子转换成DIB数据256色，每位像素一个字节，方便计算。
 **************************************************************************************************/
unsigned long int  CMatPlayer::Similiar(POINT DestPt, BYTE * pDstBMP,POINT SrcPt, BYTE * pSrcBMP )
{
     assert(pSrcBMP!=NULL);
     assert(pDstBMP!=NULL);

     unsigned long int sumdd;
     BYTE     rgb;

     UINT bmWidthBytes=((m_bm.bmWidth *COLORBITSCOUNT+31)/32)* 4;    //DIB格式
     // 如果用DDE数据格式，则bmWidthBytes = m_bm.bmWidthBytes

     sumdd = 0 ;
     int i ,j;
     for( i  = 0  ; i < m_width ;     i++ )
     for( j  = 0  ; j < ( m_width*COLORBITSCOUNT/8 ) ;     j++ )     // 如果用DDE数据格式//，则用m_bm.bmBitsPixel代替COLORBITSCOUNT
     {
          rgb=   pSrcBMP[ (SrcPt.y+i)* bmWidthBytes + SrcPt.x*COLORBITSCOUNT/8  +j ] ;
          rgb -= pDstBMP[ (DestPt.y+i)* bmWidthBytes + DestPt.x*COLORBITSCOUNT/8  +j ] ;
          if(rgb>0)
                  sumdd  =sumdd+ rgb;
          else
                  sumdd  =sumdd- rgb;
     }
     return sumdd;
}
//
unsigned int CMatPlayer::CalcDxDy()     //找到另一个车，并计算出DX，DY
{
     POINT  ptLeftCar,ptRightCarPerhaps;

     ptLeftCar.x = m_LRookInBoard.x -m_width /2 ;
     ptLeftCar.y = m_LRookInBoard.y -m_width /2 ;

     unsigned long int imin=0xffffffff;//无穷大
     unsigned long int imax=0x0;
     unsigned long int iRV=0;         //两位图相似度系数，最小值为最相似
     long curdx,curdy;

    //
    for(int ii = ptLeftCar.x + m_width *7 ;ii< m_BoardWinRect.right - m_BoardWinRect.left - m_width ;ii++ )
     {
          // assert(ii<m_bm. );
           ptRightCarPerhaps.x = ii;
           ptRightCarPerhaps.y = ptLeftCar.y ;
            
           iRV=Similiar( ptRightCarPerhaps ,m_pBoardBitmapBits,ptLeftCar,m_pBoardBitmapBits) ;
           curdx = (ptRightCarPerhaps.x -ptLeftCar.x )/8 ;

           if( iRV<imin)
           {
                imin=iRV;
                m_dx=curdx ;
           }
           if ( iRV>imax)
           {
                imax=iRV;
           }
     }
     imax;
     imin;

     m_dy=m_dx;//假定宽高相同
     return m_dx;
}

/**************************************************************************************************
 * 函数名称：BMP2Position()
 * 参数:
 *      HBITMAP hbitmap        - 棋盘窗口位图句柄
 * 返回值:
 *      unsigned char*         - 成功返回指向棋盘数组指针，否则返回NULL。
 * 说明:
 *      该函数主要用来从棋盘窗口位图句柄得到棋盘棋子数组,主要办法是：
 *      1、在临时申请的内在块中，填入当前棋盘位图数据位
 *      2、针对每一个棋盘位置点遍历每个棋子，如果匹配，该位置就是该棋子
 *         每个棋子的原始位图数据位保存在m_pBMPData中，m_pBMPData为建立方案时保存的原始棋盘棋子数据
 **************************************************************************************************/
unsigned char*   CMatPlayer::BMP2Position(void)
{
//     1、在临时申请的内在块中，填入当前棋盘位图数据位
       if(SaveBoardBits(m_pCurBMPBoardBits)){
       }

       //     2、针对每一个棋盘位置点遍历每个棋子，如果匹配，该位置就是该棋子
//        每个棋子的原始位图数据位保存在m_pBMPData中
//        匹配的标准就是相似度小于一个阈值  THRESHOLD=100
     POINT  src,dest,ooo;

     ooo.x = m_LRookInBoard.x -m_width /2 ;         //m_LRookInBoard黑车中心点相对于棋盘的位置，相当于棋盘原点，to Board client
     ooo.y = m_LRookInBoard.y -m_width /2 ;         //ooo黑车左顶点相对于棋盘的位置。

     for(int u=0;u<10;u++)
      for(int v=0;v<9;v++)
      {
         m_position[u][v] = 0;
         src.x = ooo.x + v * m_dx;
         src.y = ooo.y + u * m_dy ;

         unsigned long int imin=0xfffff;//无穷大
         unsigned long int imax=0x0;
         unsigned long int iRV=0;
         int curm=0;

         for( int m =1 ;m<15; m++ )
         {
              dest.x = ooo.x + m_InitPosOfPieceType[m].x * m_dx;
              dest.y = ooo.y + m_InitPosOfPieceType[m].y  * m_dy;
              iRV=Similiar( dest, m_pBoardBitmapBits, src,m_pCurBMPBoardBits) ;
              curm = m ;
              //以下统计最大、最小值用于调试，找到相似度阈值
              if( iRV<imin) {
                 imin=iRV;
                 if(iRV<THRESHOLD) {
                          m_position[u][v] = curm ;
                 }
              }
              if ( iRV>imax){
                   imax=iRV;
              }
          }
      }
      //要校验m_position的正确性，不正确返回NULL。

     assert(   m_pCurBMPBoardBits!=NULL);
     delete [] m_pCurBMPBoardBits ;
     m_pCurBMPBoardBits=NULL;

     if(ValidPosition(NULL))
     {
        return &m_position[0][0] ;  // may be incorrect due to local var.!!!
     }  else
     {
        //Application->MessageBox("catch!","ValidPosition error!",0);
       return NULL;
     }
}

//要校验m_position的正确性，不正确返回NULL。
bool   CMatPlayer::ValidPosition(BYTE position[10][9])
{    bool bOK=true;   BYTE chessID=NOCHESS;
    int qizinum[15]; //棋子的数目

    ::ZeroMemory(qizinum, sizeof(int)*15);

      //for(int u=0;u<10;u++)
      //for(int v=0;v<9;v++)
      //{
      //      chessID=position==NULL?m_position[u][v]:position[u][v];
      //      if(chessID>R_PAWN)  bOK=false;
      //      qizinum[chessID]++;
      //}
      //if(qizinum[R_KING]!=1||qizinum[B_KING]!=1) bOK=false; //各方有且仅有一个将（帅），简单的判断
    return bOK;
}

bool  CMatPlayer::GetBMPBoard()
{
        HDC             hscrdc, hmemdc;             // 屏幕和内存设备描述表
        HBITMAP         holdbitmap;                 // 位图句柄
        int             nwidth, nheight;            // 位图宽度和高度
        int             xscrn, yscrn;               // 屏幕分辨率

        if(m_hWndBoard == NULL || !IsWindow(m_hWndBoard))   {
             return false;
        }

        GetWindowRect(m_hWndBoard, &m_BoardWinRect);
        //if ( IsRectEmpty(m_BoardWinRect) )        // 确保选定区域不为空矩形
        //     return false;

        nwidth = m_BoardWinRect.right - m_BoardWinRect.left;     //
        nheight = m_BoardWinRect.bottom - m_BoardWinRect.top;

        // 获得屏幕分辨率
//        xscrn = GetDeviceCaps(hscrdc, HORZRES );
//        yscrn = GetDeviceCaps(hscrdc, VERTRES );

        hscrdc = GetDC(m_hWndBoard);            //为window创建设备描述表
        hmemdc = CreateCompatibleDC(hscrdc);    //为window设备描述表创建兼容的内存设备描述表

        if(m_hbitmap!=NULL) {
            DeleteObject(m_hbitmap);  m_hbitmap=NULL;
        }
        m_hbitmap = CreateCompatibleBitmap(hscrdc, nwidth, nheight);// 创建一个与屏幕设备描述表兼容的位图
        holdbitmap = (HBITMAP)SelectObject(hmemdc, m_hbitmap);     // 把新位图选到内存设备描述表中
        BitBlt(hmemdc, 0, 0, nwidth, nheight,hscrdc, 0, 0, SRCCOPY);// 把window拷贝到内存
        m_hbitmap = (HBITMAP)SelectObject(hmemdc, holdbitmap);     //得到window位图的句柄

        ReleaseDC(m_hWndBoard,hscrdc);     //清除
        DeleteDC(hmemdc);

        return m_hbitmap==NULL?false:true;       // 返回位图句柄
}

bool     CMatPlayer::SaveBoardBits(HBITMAP hbitmap)
{
//   将用位图信息填充BITMAP结构的字段，不过，bmBits字段等于NULL
     GetObject(hbitmap, sizeof(BITMAP), (BYTE *)&m_bm );
//     typedef struct tagBITMAP {  /* bm */
//    int     bmType;//必须是BM
//    int     bmWidth;//指定位图的宽度（以象素为单位）
//    int     bmHeight;//指定位图的高度（以象素为单位）。
//    int     bmWidthBytes;//一行几个字节，4位对齐
//    BYTE    bmPlanes;//指定目标设备的位面数
//    BYTE    bmBitsPixel;//指定每个象素的位数
//    LPVOID  bmBits;//指向图象数据的指针
//} BITMAP;

     DWORD dwBitmapSize = (DWORD)m_bm.bmWidthBytes* m_bm.bmHeight * m_bm.bmPlanes;

     if(m_pBoardBitmapBits!=NULL)
     {
         delete m_pBoardBitmapBits;
         m_pBoardBitmapBits = NULL;
     }
     int wBitCount=24;    //bmBitsPixel
     dwBitmapSize = ((m_bm.bmWidth *wBitCount+31)/32)* 4*m_bm.bmHeight ;
     m_pBoardBitmapBits = new (std::nothrow) BYTE[dwBitmapSize];
     if(m_pBoardBitmapBits==NULL)return false;
     //  获取图像数据需要使用GetBitmapBits
     GetBitmapBits(hbitmap, dwBitmapSize, m_pBoardBitmapBits);      //the second BMP
     return true;
}

CHESSMOVE Str2Move(const char str[5])
{
    CHESSMOVE mv;
    mv.ChessID=0;
    mv.From.x=0;
    mv.From.y=0;
    mv.To.x=0;
    mv.To.y=0;
    if(strlen((char *)str)==4)
    {
        mv.From.x=str[0]- 'a';
        mv.From.y='9'-str[1];

        mv.To.x=str[2]- 'a';
        mv.To.y='9'-str[3];;
    }
    return mv;
}

CHESSMOVE  CMatPlayer::Position2Move(BYTE pre_position[10][9] ) //通过比较两个position取得棋步
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
         if(m_position[n][m]== B_KING )
         {
              srcblackup = true;
              break;
         }
        }
       for(int m=3;m<6;m++)
        for(int n=0;n<3;n++)
        {
           if(pre_position[n][m]== B_KING )
           {
             destblackup = true;
             break;
           }
        }
        if( srcblackup ^ destblackup )
        {
            ChangeSide( m_position );
        }

        pt1.x = pt1.y=pt2.x=pt2.y=-1;
        from.x = from.y=to.x=to.y=-1;
        for(int i =0;i<10;i++)
        for(int j =0;j<9;j++)
        {
           qizi2 = pre_position[i][j];
           qizi1 = m_position[i][j];
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
             ChangeSide( m_position );
        }

        m_Move.From.x   = from.x;
        m_Move.From.y   = from.y;
        m_Move.To.x     = to.x;
        m_Move.To.y     = to.y;
        m_Move.KillID   = killed ;
        m_Move.ChessID  = ChessID;

        return m_Move;
}

//
CHESSMOVE CMatPlayer::GetMove(BYTE pre_position[10][9])
{
      char * p="#pragma warning( disable:4996 )";
      char buffer[1024];
      CHESSMOVE mvtemp;
        m_Move.From.x =-1;
        m_Move.From.y =-1;
        m_Move.To.x =-1;
        m_Move.To.y =-1;
        m_Move.ChessID =NOCHESS;
	  int m_iChessMoveFrom = BMP_PLAYER;
      switch(m_iChessMoveFrom)
      {
          case ENGINE_PLAYER:      //通过引擎计算获取棋步

                //p = CNewQiJu::ToFen(MainForm->myqiju.m_ChessBoard,MainForm->myqiju.m_bBottomGo);
                StrToClip(p);
                
                strcpy(buffer,"position fen ");
                strcat(buffer,p);
                //strcpy(buffer,"position fen rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w");  //原始盘面

                m_ucci.UCCI2Engine(buffer);
                m_ucci.UCCI2Engine("go depth 5");

                while(m_ucci.ReceiveUCCI())
                {

                    if(4==strlen(m_ucci.mvPonder))
                    {
                      mvtemp= Str2Move(m_ucci.mvPonder);
                      strcpy(m_ucci.mvPonder,"move not ready!");
                      m_Move=mvtemp;

                      strcpy(m_ucci.mvPonderFinished,"move not ready!");
                   }
                }
                break;

          case  BMP_PLAYER:      //通过扫描棋盘获得棋步
                RECT  myrect;
                int m_width , height;

                m_width  = m_BoardWinRect.right -  m_BoardWinRect.left  ;
                height = m_BoardWinRect.bottom - m_BoardWinRect.top ;

                GetWindowRect(m_hWndBoard, &myrect);

                if(   myrect.right - myrect.left != m_width || myrect.bottom - myrect.top != height )
                {
                    m_width  = m_ProgWinRect.right - m_ProgWinRect.left  ;
                    height = m_ProgWinRect.bottom - m_ProgWinRect.top ;

                    MoveWindow(m_hWndProg, m_ProgWinRect.left, m_ProgWinRect.right ,m_width,height ,true );
                }

                //BringWindowToTop( m_hWndProg );
                try
                {

                    if( GetBMPBoard())
                    {
                          BMP2Position() ;
                          Position2Move(pre_position);  //比较两个棋盘数组取得棋步

                          // 检查棋步有效性;
                          BYTE position[10][9];

                          memcpy(position,  m_position ,90);
                          mvtemp = m_Move;
                          if(IsValidMove( position,m_Move))
                          {
                              if( m_bQiShouRed != m_bQiShouBottom )   //此处要调试修改
                              {
                                    ChangeSide( position);
                                    mvtemp.From.x = 8-mvtemp.From.x;
                                    mvtemp.From.y = 9-mvtemp.From.y;
                                    mvtemp.To.x = 8-mvtemp.To.x;
                                    mvtemp.To.y = 9-mvtemp.To.y;
                              }
                              m_Move=mvtemp;
                          }
                    }
                }
                catch (...)
                {
                    ErrorHappen("GetMove函数捕获异常！");
                }
                break;

          case EDIT_PLAYER:      //手动输入棋步。例：“h0g2”=下方马二进三

              break;

          case VOICE_PLAYER:      //通过语音获取棋步
               break;

		  default:
			    break;
      }
      return m_Move;
}

void CMatPlayer::DrawCchessBorder(void)
{
	HPEN hPen ,* hOldPen ;
	if(m_hWndBoard == NULL || !IsWindow(m_hWndBoard))
	{
		return;
	}
	HDC hdc = ::GetWindowDC( m_hWndBoard ) ;
	SetROP2( hdc , R2_NOT ) ;
	hPen = CreatePen ( PS_SOLID , 3 , RGB( 0 , 0 , 0 ) ) ;
	hOldPen = ( HPEN * )SelectObject( hdc , hPen ) ;
	SelectObject ( hdc ,GetStockObject ( NULL_BRUSH ) ) ;
	POINT pt1,pt2;
	int dx,dy;
	dx=m_dx;
	dy=m_dy;

	pt1.x = m_LRookInBoard.x;
	pt1.y = m_LRookInBoard.y;
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
	::ReleaseDC ( m_hWndBoard , hdc ) ;
}

//        如果是图形客户端方案，则至少包含以下内容才能找到棋盘棋子状态
//        char            m_ptcName[MAX_PLAYER_NAME];   //棋手名称,如果是图形客户端则为窗口标题关键字
//        POINT           m_LRookInProg ;             //相对于程序顶层窗口黑车所在位置点 ，注意与m_LRookInBoard的区别
//        BYTE            m_width;						//分辨率 ，样本棋子图形样本宽=高
//        unsigned long int     m_iDD;					//误差,相似度。
//        BYTE          * m_pBoardBitmapBits;           //指向位图数据块（初始棋盘数据）

//      以下内容也可以保存作为另一种寻找棋盘窗口的办法
//        int             m_MaxParentWinCountBoard;       //图形棋盘窗口其父窗口计数
//        char            m_ClassNameBoard[10][254];      //图形棋盘窗口其父窗口类名
//        char            m_WinNameBoard[10][254];        //图形棋盘窗口其父窗口标题


//用C++文件流函数重写 2017.5.20. 
//关于方案的保存，其实只要保存足够的信息，能够找到程序窗口及左车相对于程序或棋盘窗口的位置即可，
//随后可以根据程序窗口hwnd及左车位置point两个信息调用初始化方案建立函数build_solution即可重新建立方案
//无需保存图像数据后，方案配置文件小了许多
//最后参考loadsolution函数来看看哪些参数是必需保存为方案的参数
bool CMatPlayer::SaveSolution(char * filename)
{
		std::ofstream oFile(filename);
		if (!oFile.is_open()){
			return false;
		}

        long byteswriten=0;
        char* ptitle="link";

        oFile.write(ptitle, sizeof("link"));//写入文件标识		byteswriten += sizeof("link");

        oFile.write(m_ptcName,  sizeof(char)*MAX_PLAYER_NAME);	byteswriten +=  sizeof(char)*MAX_PLAYER_NAME;
        oFile.write((char*)&m_LRookInProg, sizeof(POINT));	byteswriten +=   sizeof(POINT);
		oFile.write((char*)&m_LRookInBoard,     sizeof(POINT));			byteswriten +=   sizeof(POINT);
		oFile.write((char*)&m_width,     sizeof(BYTE));			byteswriten +=   sizeof(BYTE);
		oFile.write((char*)&m_iDD,     sizeof(unsigned long int));	byteswriten +=   sizeof(unsigned long int);
		oFile.write((char*)&m_dx,      sizeof(BYTE));			byteswriten +=   sizeof(BYTE);		//
		oFile.write((char*)&m_dy,      sizeof(BYTE));			byteswriten +=   sizeof(BYTE);
		oFile.write((char*)&m_iChessMoveFrom, sizeof(int));			byteswriten +=   sizeof(int);
		oFile.write((char*)&m_bm, sizeof(BITMAP));			byteswriten +=   sizeof(BITMAP);

		//写入窗口信息数组
		oFile.write((char*)&m_MaxParentWinCountBoard, sizeof(int));			byteswriten +=   sizeof(int);
		oFile.write((char*)&m_ClassNameBoard, sizeof(m_ClassNameBoard));			byteswriten +=   sizeof(m_ClassNameBoard);
		oFile.write((char*)&m_WinNameBoard, sizeof(m_WinNameBoard));			byteswriten +=   sizeof(m_WinNameBoard);

		DWORD dwBitmapSize=0;//棋盘位图数据长度
        if( m_pBoardBitmapBits !=NULL )
        {
            dwBitmapSize = ((m_bm.bmWidth *COLORBITSCOUNT+31)/32)* 4*m_bm.bmHeight ;
			oFile.write((char*)m_pBoardBitmapBits, dwBitmapSize  );			byteswriten += dwBitmapSize;
        }

		oFile.write((char*)&dwBitmapSize, sizeof(DWORD)  );			byteswriten +=   sizeof(DWORD);
        byteswriten+= sizeof( long );

		oFile.write((char*)&byteswriten, sizeof( long ));			//最后写入本文件写入字节总数
		
		oFile.close();

		return true;
}

//用C++文件流函数重写 2017.5.20. 
//与savesolution相对应，一个是写，一个是读
//本函数读取数据后，还需要做后续处理（用loadsolution函数）
bool CMatPlayer::GetFromFile(char * filename)
{
	//if( (_access( filename, 0 )) != -1 ) //判断文件存在否

	std::ifstream iFile(filename);
	if (!iFile.is_open()){
		return false;
	}

	char buffer[10];			
	int Len32= sizeof(*this) ;
	long bytesread=0;

	iFile.read(buffer, sizeof("link"));
	if(strlen(buffer)!=strlen("link")||strcmp(buffer,"link")!=0){
		return false;
	}

	iFile.read( m_ptcName,		sizeof(char)*MAX_PLAYER_NAME);
	iFile.read((char*)&m_LRookInProg,		sizeof(POINT));										/////////////////////////
	iFile.read((char*)&m_LRookInBoard,        sizeof(POINT));
	iFile.read((char*)&m_width,        sizeof(BYTE));
	iFile.read((char*)&m_iDD,        sizeof(unsigned long int));
	iFile.read((char*)&m_dx,         sizeof(BYTE));
	iFile.read((char*)&m_dy,         sizeof(BYTE));
	iFile.read((char*)&m_iChessMoveFrom, sizeof(int));
	iFile.read((char*)&m_bm,         sizeof(BITMAP));
	
	//读入窗口信息数组
	iFile.read((char*)&m_MaxParentWinCountBoard, sizeof(int));
	iFile.read((char*)&m_ClassNameBoard, sizeof(m_ClassNameBoard));
	iFile.read((char*)&m_WinNameBoard, sizeof(m_WinNameBoard));

	//分配内存
	DWORD dwBitmapSize ;
	dwBitmapSize = ((m_bm.bmWidth *COLORBITSCOUNT+31)/32)* 4*m_bm.bmHeight ;
	if( m_pBoardBitmapBits != NULL  )
	{
		delete   m_pBoardBitmapBits ;
		m_pBoardBitmapBits =NULL;
	}
	m_pBoardBitmapBits = new(std::nothrow) BYTE [dwBitmapSize];
	iFile.read((char*)m_pBoardBitmapBits ,dwBitmapSize);
	
	iFile.read((char*)&dwBitmapSize ,sizeof(DWORD));
	iFile.read(buffer ,sizeof(long));

	int i=  atoi(buffer);//判断一下最后读出来的long 是否等于实际读取的总字节数
	
	//assert(atoi(buffer)==bytesread);

	iFile.close();

	return true;
}


/*************************************************************************
 * 函数名称：GetQiziDDB()
 * 参数:
 *      HWND dstHWND        - 输出窗口句柄
 *      unsigned short ChessID    - 棋子ID
 * 返回值:
 *      BYTE*              - 成功返回指向棋子DDB数据的指针，否则返回NULL。
 * 说明:
 *      该函数主要用来从棋盘DDB数据中找到对应棋子（ChessID）的DDB数据。
 ************************************************************************/
BYTE* CMatPlayer::GetQiziDDB(HWND dstHWND,unsigned short ChessID)
{
    assert(m_hbitmap!=NULL&&m_pBoardBitmapBits!=NULL);
    assert(m_hbitmap!=NULL);
    assert(m_pBoardBitmapBits!=NULL);
    assert(dstHWND!=NULL);
    assert(m_pBoardBitmapBits!=NULL);
    assert(ChessID<=R_PAWN&&ChessID>=B_KING);

    POINT  ptQiziLeftTop,ooo;
    BYTE byQiziBits;

//    int iDI=atoi(MainForm->edtrdi->Text.c_str());
    int iDI=m_width;      //调试正常后取消注释即可

    ooo.x = m_LRookInBoard.x -iDI /2 ;   //棋盘左上角
    ooo.y = m_LRookInBoard.y -iDI /2 ;

    ptQiziLeftTop.x = ooo.x + m_InitPosOfPieceType[ChessID].x * m_dx;     //棋子左上角
    ptQiziLeftTop.y = ooo.y + m_InitPosOfPieceType[ChessID].y * m_dy ;

    DWORD dwBitmapSize = (DWORD) ( iDI*m_bm.bmBitsPixel/8 )*iDI;
    if( m_pBMPQiziBits != NULL  )
    {
          delete   m_pBMPQiziBits ;
          m_pBMPQiziBits =NULL;
    }
    m_pBMPQiziBits = new BYTE [dwBitmapSize];

    for( int yDi  = 0  ; yDi < iDI ;     yDi++ )     //纵向像素循环
    for( int xbytes  = 0  ; xbytes < ( iDI*m_bm.bmBitsPixel/8 ) ;     xbytes++ )  //横向字节循环（ ( iDI*m_bm.bmBitsPixel/8 )：每行像素多少个字节）
    {
        m_pBMPQiziBits[yDi*( iDI*m_bm.bmBitsPixel/8 )+xbytes] = m_pBoardBitmapBits[ (ptQiziLeftTop.y+yDi)*m_bm.bmWidthBytes + ptQiziLeftTop.x*m_bm.bmBitsPixel/8  +xbytes];
    }

    HDC srcDC,dstDC;
    HBITMAP hOldbitmap,hBitmap;

    dstDC=GetDC(dstHWND);
    srcDC=CreateCompatibleDC(dstDC);

    hBitmap= CreateCompatibleBitmap(dstDC,iDI,iDI);
//    long int len=0;
    SetBitmapBits(hBitmap,dwBitmapSize,m_pBMPQiziBits);

    ReleaseDC(dstHWND,dstDC);
    DeleteDC(srcDC);
    return m_pBMPQiziBits;
}

//获取鼠标位置的最底层控件,包括disabled control
BOOL GetRealWindowFromPoint(HWND cHwnd);
BOOL GetRealWindowFromPoint(HWND cHwnd)
{
     POINT point,WindowPos={0};
     GetCursorPos(&point);
     ClientToScreen(cHwnd,&WindowPos);//转换成屏幕坐标,表示客户区窗口左上角的坐标
     point.x-=WindowPos.x;
     point.y-=WindowPos.y;
     HWND wmControl=ChildWindowFromPoint(cHwnd,point);//客户区坐标

    // getlasterr(GetLastError());
     if (wmControl!=NULL)
     {
          if(wmControl!=cHwnd)//wmControl==cHwnd时表示已经到达RealChildWindowFromPoint所能取到的最底层
           GetRealWindowFromPoint(wmControl);//递归

          else
          {
        //   ::GetControlInfo(wmControl);
           return TRUE;
          }
     }
     return true;
}


void ErrorHappen(LPTSTR lpszFunction)
{
    char szBuf[80];
    LPVOID lpMsgBuf;
    DWORD dw = ::GetLastError();
 
    ::FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                        NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                        (LPTSTR) &lpMsgBuf,0, NULL );
 
    sprintf(szBuf, "%s failed with error 0x%08u: %s",  lpszFunction, dw, lpMsgBuf);

    if(dw)
    {
        ::MessageBox(NULL,szBuf,NULL,1);
    }

    ::LocalFree(lpMsgBuf);
}

// 把DDB位图像素数据以DIB格式保存到形参pdata指针位置，pdata有数据时会先删除
// 颜色位数固定为8位，以方便计算
// 必要时保存为文件，可检查
bool  CMatPlayer::SaveBoardBits(BYTE* &pdata)
{
//        assert(m_hbitmap!=NULL);
//        assert(pdata!=NULL);
//        if(pdata==NULL)return false;
        if(m_hbitmap==NULL)return false;

//        assert(m_hbitmap!=NULL);
        if(m_hbitmap==NULL)return false;

	HDC hDC;        //设备描述句柄
	int iBits;        //当前显示分辨率下每个像素所占字节数
	WORD wBitCount;   //位图中每个像素所占字节数
	//定义调色板大小， 位图中像素字节大小 ，位图文件大小
	DWORD  dwPaletteSize=0,   dwBmBitsSize,	   dwDIBSize, dwWritten;
	BITMAPFILEHEADER bmfHdr;        //DIB位图文件头结构
	//BITMAP	 m_bm;                //DDB位图属性结构    改为成员变量
	BITMAPINFOHEADER	bi;     //位图信息头结构
	LPBITMAPINFOHEADER lpbi;        //指向位图信息头结构
	HANDLE   fh, hDib;              //定义文件，分配内存句柄
	HPALETTE hPal,hOldPal=NULL;     //调色板句柄

	//根据当前屏幕计算位图文件每个像素所占位数，也可自由设置为1、4、8、24等位数
	hDC = CreateDC("DISPLAY",NULL,NULL,NULL);
	iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
	DeleteDC(hDC);
	if (iBits <= 1)
		wBitCount = 1;
	else if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else if (iBits <= 24)
		wBitCount = 24;
	else wBitCount = 24;

        wBitCount=COLORBITSCOUNT;         //固定为8位，方便计算。

	//计算调色板大小
	if (wBitCount <= 8)
		dwPaletteSize = (1 << wBitCount) *sizeof(RGBQUAD);

	//获取DDB位图信息
	GetObject(m_hbitmap, sizeof(BITMAP), (LPSTR)&m_bm);

	//设置DIB位图信息头结构
	bi.biSize            = sizeof(BITMAPINFOHEADER);
	bi.biWidth           = m_bm.bmWidth;
	bi.biHeight          = -m_bm.bmHeight;
	bi.biPlanes          = 1;
	bi.biBitCount         = wBitCount;
	bi.biCompression      = BI_RGB;
	bi.biSizeImage        = 0;
	bi.biXPelsPerMeter     = 0;
	bi.biYPelsPerMeter     = 0;
	bi.biClrUsed         = 0;
	bi.biClrImportant      = 0;

	dwBmBitsSize = ((m_bm.bmWidth *wBitCount+31)/32)* 4*m_bm.bmHeight ;


	//为位图内容分配内存
	//hDib  = GlobalAlloc(GHND,dwBmBitsSize+dwPaletteSize+sizeof(BITMAPINFOHEADER));
	hDib  = GlobalAlloc(GHND,dwPaletteSize+sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
        //lpbi = (LPBITMAPINFOHEADER) new BYTE(sizeof(BITMAPINFOHEADER)+dwPaletteSize+dwBmBitsSize);   //用这种方式分配内在，后面GetDibbits得不以数据，不知何故？

	*lpbi = bi;   //设置内存中BI信息结构


	hPal = (HPALETTE)GetStockObject(DEFAULT_PALETTE);
	if (hPal) {
		hDC  = ::GetDC(NULL);
		hOldPal = SelectPalette(hDC, hPal, FALSE);
		RealizePalette(hDC);
	}

	//获取该调色板下新的像素值 ，位图的颜色表将附加在BITMAPINFO结构的后面
        //GetDIBits函数获取指定兼容位图的位，然后将其作一个DIB—设备无关位图（Device-Independent Bitmap）使用的指定格式复制到一个缓冲区中。
        //如果所需要的DIB格式与其内部格式相匹配，那么位图的RGB值将被复制。如果不匹配，那么将合成一个颜色表
        //通过将高度设为正数来指定一个自下而上的DIB，而自上而下的DIB则通过设置一个负的高度值来指定。
        //位图的颜色表将附加在BITMAPINFO结构的后面
        //
       UINT scanlines=0;
       //scanlines=GetDIBits(hDC, m_hbitmap, 0, (UINT)m_bm.bmHeight,(LPSTR)lpbi + sizeof(BITMAPINFOHEADER)+dwPaletteSize,(LPBITMAPINFO)lpbi, DIB_RGB_COLORS);

       //SaveBitmapToFile("BoardForTest.bmp");      //调试时用 用来检查保存的图像对不对

       if(pdata!=NULL)  {
         delete []pdata;
         pdata=NULL;
       }
       pdata = new(std::nothrow) BYTE[dwBmBitsSize] ;

       if(pdata==NULL) {
            ErrorHappen("new(std::nothrow) BYTE[dwBmBitsSize] 函数出现错误！ ");
            return NULL;
       }
       //::ZeroMemory(pdata, dwBmBitsSize);
       scanlines=GetDIBits(hDC, m_hbitmap, 0, (UINT)m_bm.bmHeight,pdata,(LPBITMAPINFO)lpbi, DIB_RGB_COLORS);
       //memcpy(pdata,(LPSTR)lpbi + sizeof(BITMAPINFOHEADER)+dwPaletteSize,dwBmBitsSize);

	//恢复调色板 ，调试成功后可以删除与调色板相关的代码试试？
	if (hOldPal) 	{
		SelectPalette(hDC, hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}

	//清除
	GlobalUnlock(hDib);
	GlobalFree(hDib);

        //delete []lpbi;
        
	return TRUE;
}


// 把DDB位图以DIB格式存到磁盘文件上.
// lpFileName 为位图文件名。调试通过。
bool  CMatPlayer::SaveBitmapToFile(LPSTR lpFileName)
{
//        assert(m_hbitmap!=NULL);
        if(m_hbitmap==NULL)return false;

	HDC hDC;        //设备描述句柄
	int iBits;        //当前显示分辨率下每个像素所占字节数
	WORD wBitCount;   //位图中每个像素所占字节数
	//定义调色板大小， 位图中像素字节大小 ，位图文件大小
	DWORD  dwPaletteSize=0,   dwBmBitsSize,	   dwDIBSize, dwWritten;
	BITMAPFILEHEADER bmfHdr;        //DIB位图文件头结构
	BITMAP	 Bitmap;                //DDB位图属性结构
	BITMAPINFOHEADER	bi;     //位图信息头结构
	LPBITMAPINFOHEADER lpbi;        //指向位图信息头结构
	HANDLE   fh, hDib;              //定义文件，分配内存句柄
	HPALETTE hPal,hOldPal=NULL;     //调色板句柄

	//根据当前屏幕计算位图文件每个像素所占位数，也可自由设置为1、4、8、24等位数
	hDC = CreateDC("DISPLAY",NULL,NULL,NULL);
	iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
	DeleteDC(hDC);
	if (iBits <= 1)
		wBitCount = 1;
	else if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else if (iBits <= 24)
		wBitCount = 24;
	else wBitCount = 24;

        wBitCount=8;//修改为8位如何？调试用。

	//计算调色板大小
	if (wBitCount <= 8)
		dwPaletteSize = (1 << wBitCount) *sizeof(RGBQUAD);

	//获取DDB位图信息
	GetObject(m_hbitmap, sizeof(BITMAP), (LPSTR)&Bitmap);

	//设置DIB位图信息头结构
	bi.biSize            = sizeof(BITMAPINFOHEADER);
	bi.biWidth           = Bitmap.bmWidth;
	bi.biHeight          = -Bitmap.bmHeight;
	bi.biPlanes          = 1;
	bi.biBitCount         = wBitCount;
	bi.biCompression      = BI_RGB;
	bi.biSizeImage        = 0;
	bi.biXPelsPerMeter     = 0;
	bi.biYPelsPerMeter     = 0;
	bi.biClrUsed         = 0;
	bi.biClrImportant      = 0;

	dwBmBitsSize = ((Bitmap.bmWidth *wBitCount+31)/32)* 4*Bitmap.bmHeight ;


	//为位图内容分配内存
	hDib  = GlobalAlloc(GHND,dwBmBitsSize+dwPaletteSize+sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;   //设置内存中BI信息结构

	hPal = (HPALETTE)GetStockObject(DEFAULT_PALETTE);
	if (hPal) {
		hDC  = ::GetDC(NULL);
		hOldPal = SelectPalette(hDC, hPal, FALSE);
		RealizePalette(hDC);
	}

	//获取该调色板下新的像素值 ，位图的颜色表将附加在BITMAPINFO结构的后面
	UINT scanlines=GetDIBits(hDC, m_hbitmap, 0, (UINT)Bitmap.bmHeight,(LPSTR)lpbi + sizeof(BITMAPINFOHEADER)+dwPaletteSize,(LPBITMAPINFO)lpbi, DIB_RGB_COLORS);

	//恢复调色板
	if (hOldPal) 	{
		SelectPalette(hDC, hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}

	//创建文件
	fh = CreateFile(lpFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (fh == INVALID_HANDLE_VALUE)  return FALSE;

	//设置位图文件头
	bmfHdr.bfType = 0x4D42;  // "BM"
	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)	+ dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER)+ dwPaletteSize;
	//写入文件
	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);

	//清除
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);

	return TRUE;
}

void StrToClip(char* pstr) //拷贝FEN串到剪贴板
{
    if(::OpenClipboard (NULL))//打开剪贴板
     {
          HANDLE hClip;
          char* pBuf;
          ::EmptyClipboard();//清空剪贴板

        //写入数据
          hClip=::GlobalAlloc(GMEM_MOVEABLE,strlen(pstr)+1);
          pBuf=(char*)GlobalLock(hClip);
          strcpy(pBuf,pstr);
          ::GlobalUnlock(hClip);//解锁
          ::SetClipboardData(CF_TEXT,hClip);//设置格式

        //关闭剪贴板
          ::CloseClipboard();
     }
}

void  CMatPlayer::ChangeSide(BYTE position[10][9])
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


bool CMatPlayer::IsValidMove(BYTE position[10][9],CHESSMOVE mv)
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

///////////////////////////////////////////////////////////////////////////////////////////////////////
//根据棋盘初始图像与左车位置等参数，分解出14个兵种图像（缺省高宽40）
//注意：未真实拷贝数据，需要保持m_imgBoardInit数据不变！
bool CMatPlayer::PiecesMat()
{
	assert(m_imgBoardInit.rows > m_width*5);

	string fn;
	for(int y=0; y<10 ; y++)		
	{
		for(int x=0; x<5 ; x++)//只取半边的
		{
			if(int ChessID = InitChessBoard[y][x])
			{
				m_imgPiece[ChessID] = m_imgBoardInit( Rect(m_LRookInBoard.x+x*m_dx -m_width/2, m_LRookInBoard.y+y*m_dy-m_width/2, m_width, m_width) );
				fn ="res\\Piece"+ nameID[ChessID]+".jpg";
				//imwrite(fn, m_imgPiece[ChessID]);
			}
		}
	}
	
	Mat temp=m_imgPiece[1];	
	for(int x=2; x<=14 ; x++)	
	{
		temp = mergeCols(temp, m_imgPiece[x]);
	}
	imwrite("res\\allPiece.jpg", temp);

	return true;
}

//找到另一个车，计算出DX，并保存兵种图
//假定dy=dx，（对不相等的情况未做特殊处理）
unsigned int CMatPlayer::CalcDxDy_CV(void)
{
	double rMax=0;

	Mat rRook;
	Mat lRook = m_imgBoardInit(Rect(m_LRookInBoard.x-m_width/2, m_LRookInBoard.y-m_width/2, m_width, m_width));
	imwrite("lRook.jpg", lRook);

	int RookX;
	double r;
	for(int x = m_LRookInBoard.x+8*m_width; x < m_imgBoardInit.cols-m_width/2 ; x++)		//从至少第 8个子的x开始，可节省计算量。
	{
		rRook = m_imgBoardInit(Rect(x - m_width/2, m_LRookInBoard.y-m_width/2, m_width, m_width));

		r= Similiar(lRook,rRook);

		if(r>rMax) 
		{
			rMax=r;
			RookX = x;
		}
	}

	//最大相似度超过阈值，认定为右车，据此计算dx,dy并保存兵种图
	if(rMax>m_threshold)
	{
		m_dx=(RookX-m_LRookInBoard.x)/8;
		m_dy=m_dx;

		cout<<  "ssim:  " << rMax << endl;
		cout<<  "m_dx:  " << m_dx << endl;
		imwrite("rRook.jpg",rRook);

		PiecesMat();
	}

	return m_dx;
}

//用热键配合鼠标实时坐标建立初始图形连线方案，随后计算出m_dx，最后进行positon识别
//只需要一个屏幕坐标就可以了：鼠标指向左车，按下热键后，传递鼠标的屏幕坐标做参数调用此函数即可
bool CMatPlayer::build_solution(POINT MousePoint)
{
	//::GetCursorPos(&MousePoint);
	
	m_hWndBoard = ::WindowFromPoint(MousePoint);
	::GetWindowRect(m_hWndBoard, &m_BoardWinRect);

	// 收集窗口信息
	// 1、当前窗口
	for(int i=0;i<10;i++)
	{
		for(int j=0;j<254;j++)  
		{
			m_ClassNameBoard[i][j]=0;
			m_WinNameBoard[i][j]=0;
		}
	}

	::GetWindowText( m_hWndBoard ,&m_WinNameBoard[0][0], 254 );  //棋盘窗口的信息
	::GetClassName( m_hWndBoard ,&m_ClassNameBoard[0][0], 254 );

	// 2、locate top window
	m_MaxParentWinCountBoard=0;
	m_hWndProg = GetParent(m_hWndBoard) ;
	while( m_hWndProg!= NULL)
	{
		m_MaxParentWinCountBoard++;
		::GetWindowText( m_hWndProg ,&m_WinNameBoard[m_MaxParentWinCountBoard][0], 254 );
		::GetClassName( m_hWndProg ,&m_ClassNameBoard[m_MaxParentWinCountBoard][0], 254 );
		m_hWndProg = ::GetParent(m_hWndProg)  ;
	}

	m_iLeftTop = MousePoint;
	m_LRookInProg = MousePoint;
	m_LRookInBoard = MousePoint ;
	strcpy( m_ptcName, &m_WinNameBoard[m_MaxParentWinCountBoard][0]);	//对于可变窗口标题，需要另用关键字填充棋手名

	//以下建立方案（涉及到成员变量：m_imgBoardInit、m_LRookInBoard。实时图m_imgBoard是用来识别的）
	//该函数应用hWnd参数标识的窗口和POINT结构给定的屏幕坐标来计算用户坐标，
	//然后以用户坐标来替代屏幕坐标，新坐标是相对于指定窗口的领域的左上角
	::ScreenToClient( m_hWndProg, &m_LRookInProg);
	::ScreenToClient( m_hWndBoard,&m_LRookInBoard);

	//建立方案
	m_imgBoardInit = hwnd2mat(m_hWndBoard);		//保存原始棋盘图像
	imwrite("BoardInit.jpg",m_imgBoardInit);
	CalcDxDy_CV();								//算m_dx，并保存兵种图，仅建立方案时需CalcDxDy_CV一次，识别时不再调用
	
	//识别
	string fen = BMP2Position(m_bQiShouRed);					

	return true;
}

//根据原始方案的棋子m_imgPiece[15]及实时棋盘m_imgBoard得到棋局fen串
//redGo先手标志，true=红先
//string CMatPlayer::BMP2Position(Mat imgboard, bool redGo, Mat imgBoardInit, POINT ptLRook)
string CMatPlayer::BMP2Position(bool redGo)
{
	double fSimiliar=0;
	double fMax=0;
	string fen="";
	int ChessID=0;
	int number=0;
	Mat imgtemp, imgPiece;
	bool bfound=false;
	
	if(m_hWndBoard!=NULL)
		m_imgBoard = hwnd2mat(m_hWndBoard);

	if( m_imgBoard.data == NULL ) return "";

	ZeroMemory(m_position, sizeof(m_position));

	for(int y=0; y<10 ; y++)
	{
		for(int x=0; x<9 ; x++)
		{
			imgtemp = m_imgBoard( Rect(m_LRookInBoard.x+x*m_dx -m_width/2, m_LRookInBoard.y+y*m_dy-m_width/2, m_width, m_width) );
			for(ChessID = 1; ChessID<15; ChessID++)
			{
				double fSimiliar= Similiar(imgtemp, m_imgPiece[ChessID]);
				if(fSimiliar>fMax) 
				{
					imgPiece = m_imgPiece[ChessID];
					fMax = fSimiliar;
					number=ChessID;
					bfound=true;
				}
			}

			if(fMax > m_threshold)
			{
				std::cout << "fMax " << std::setw(10) <<  fMax  << std::setw(10) << nameID[number]  << std::endl;
				fen ="res\\"+ nameID[number]+".jpg";
				imwrite(fen.c_str(), mergeCols(imgPiece, imgtemp));

				m_position[y][x] = number;

				fMax=0;
				number=0;
				bfound=false;
			}
		}
	}
	std::cout << std::endl;

	fen =ToFen(m_position, redGo);
	StrToClip((char*)fen.c_str());	//把fen串拷贝到系统剪贴板上

	return fen;
}

BYTE *  CMatPlayer::PasteJuMian(char FenStr[2054])
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

char*  CMatPlayer::ToFen(BYTE position[10][9] , bool RedGo )
{
        static char FenStr[2054];
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
              strcat( FenStr ," w - - 0 1");
        }
        else
        {
              strcat( FenStr ," b - - 0 1");
        }

	return FenStr;
}

//14个兵种起始位置（仅左半边）
const POINT m_InitPosOfPieceType[15]={   {-1,-1}, 
									     {4,0},{0,0},{1,0},{1,2},{3,0},{2,0},{0,3},   //黑方兵种
                                         {4,9},{0,9},{1,9},{1,7},{3,9},{2,9},{0,6}  };//红方兵种
//初始棋盘定义
const BYTE InitChessBoard[10][9]=
{
      { B_CAR,B_HORSE,B_ELEPHANT,B_BISHOP,B_KING,B_BISHOP,B_ELEPHANT,B_HORSE,B_CAR } ,
      { NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS}    ,
      { NOCHESS,B_CANON,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,B_CANON,NOCHESS}    ,
      { B_PAWN,NOCHESS,B_PAWN,NOCHESS,B_PAWN,NOCHESS,B_PAWN,NOCHESS,B_PAWN}    ,
      { NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS}  ,
      { NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS}  ,
      { R_PAWN,NOCHESS,R_PAWN,NOCHESS,R_PAWN,NOCHESS,R_PAWN,NOCHESS,R_PAWN}       ,
      { NOCHESS,R_CANON,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,R_CANON,NOCHESS}  ,
      { NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS}  ,
      { R_CAR,R_HORSE,R_ELEPHANT,R_BISHOP,R_KING,R_BISHOP,R_ELEPHANT,R_HORSE,R_CAR}
};



const string CMatPlayer::print(void) const 
{
	
	const  char ChessName[15][3]={"  ","将","车","马","包","士","象","卒","帅","车","马","炮","仕","相","兵"} ; //反序

	std::ostringstream ss;
	unsigned char txtboard[22][40]=
	{
		"     ----小棋盘打印 ver 1.00!------- \n",
		"9 　┌--┬--┬--┬--┬--┬--┬--┬--┐ ",
		". 　│　│　│　│＼│／│　│　│　│ ",
		"8 　├--┼--┼--┼--※--┼--┼--┼--┤ ",
		". 　│　│　│　│／│＼│　│　│　│ ",
		"7 　├--┼--┼--┼--┼--┼--┼--┼--┤ ",
		". 　│　│　│　│　│　│　│　│　│ ",
		"6 　├--┼--┼--┼--┼--┼--┼--┼--┤ ",
		". 　│　│　│　│　│　│　│　│　│ ",
		"5 　├--┴--┴--┴--┴--┴--┴--┴--┤ ",
		". 　│　　　楚　河　　汉　界　　　　│ ",
		"4 　├--┬--┬--┬--┬--┬--┬--┬--┤ ",
		". 　│　│　│　│　│　│　│　│　│ ",
		"3 　├--┼--┼--┼--┼--┼--┼--┼--┤ ",
		". 　│　│　│　│　│　│　│　│　│ ",
		"2 　├--┼--┼--┼--┼--┼--┼--┼--┤ ",
		". 　│　│　│　│＼│／│　│　│　│ ",
		"1 　├--┼--┼--┼--※--┼--┼--┼--┤ ",
		". 　│　│　│　│／│＼│　│　│　│ ",
		"0 　└--┴--┴--┴--┴--┴--┴--┴--┘ ",
		". 　ａ　ｂ　ｃ　ｄ　ｅ　ｆ　ｇ　ｈ　ｉ ",
		"                                       "
	};

	for(int i=0; i<10;  i ++ )
	{
		for(int j=0; j<9; j ++ )
		{
			unsigned char ChessID = m_position[i][j];
			unsigned char BufferB[6]={"[将]"};

			if(ChessID!=0)
			{
				BufferB[1] = ChessName[ChessID][0];
				BufferB[2] = ChessName[ChessID][1];

				if( ChessID > 7 )
				{
					BufferB[0] ='(';	BufferB[3] =')';

				} else if( ChessID > 0 )
				{
					BufferB[0] =0x5b;	BufferB[3] =']';	
				}
				for(int len = 0 ;len < 4; len++)
				{
					txtboard[2*i+1][j*4+3+len]=BufferB[len];
				}
			}
		}
	}
	
	ss << "\n       --------日期：" << __DATE__ << " -------\n ";
	for(int i=0; i<22;  i ++ )
	{
		ss<< "\n" << txtboard[i];
	}
	ss << "\nend!" << std::endl;

	return ss.str();
}