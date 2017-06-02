
/*******************************************************************************
类名称   : CMatPlayer
-------------------------------------------------------------------------------
功能     : 根据方案解析棋盘图像，输出fen串。
		   1、利用人工定位左车位置的方式创立图形连线方案，保存及存取方案。
		   2、利用方案定位图形客户端棋盘窗口句柄；
		   3、根据棋盘窗口句柄实时获取棋盘图像；
--------------------------------------------------------------------------------
异常类   : <无> 
--------------------------------------------------------------------------------
备注     : 1、m_width可调，如识别结果错误，试着增加或减少直到正确识别
		   2、
--------------------------------------------------------------------------------
典型用法 : 
	CUcci ucci;
	CMatPlayer bmpPlayer(ucci);
	//以下建立方案等准备工作（涉及到成员变量：m_imgBoardInit、m_LRookInBoard、m_imgPiece[]）
	bmpPlayer.m_LRookInBoard.x=52;
	bmpPlayer.m_LRookInBoard.y=62;
	bmpPlayer.m_imgBoardInit=imread("board.png");
	bmpPlayer.CalcDxDy_CV();				
	//实时图m_imgBoard是用来识别的
	bmpPlayer.m_hWndBoard=NULL;
	bmpPlayer.m_imgBoard=imread("board.png");
	//以下识别出结果，输出标准的fen字串，可用其它棋软粘贴局面图
	std::cout<< bmpPlayer.BMP2Position(true)<<std::endl;
--------------------------------------------------------------------------------
作者     : <xxx>
*******************************************************************************/

#pragma once

#include <windows.h>

#include "types.h"
#include "ucciAdapter.h"

#include "opencv2/core/version.hpp"
#include "opencv2/core/core.hpp"

class CMatPlayer
{
  public:
        // CMatPlayer(char* winkeyname="CycloneGui");  //根据窗口关键字或文件名初始化
        CMatPlayer(CUcci& ucci, std::string config="link.ini", BYTE width=40, double f=0.9);
        ~CMatPlayer();

        bool            SaveSolution(char * filename);
        bool            LoadSolution(char * filename);             //找到各窗口及计算其它参数
        HWND            FindWinProg(void);              //根找程序窗口
        HWND            FindWinBoard(void);             //找程序棋盘窗口

        bool            BMPGo(CHESSMOVE cmQiJuMove, bool bBottomGo);   //图形棋手走棋

        bool            GetBMPBoard();                          //截取棋盘位图，返回位图句柄
        bool            SaveBoardBits(HBITMAP hbitmap);         //通过Getbitmapbits函数保存棋盘位图数据,到原始数据指针
        bool            SaveBoardBits(BYTE* &pdata) ;           //通过GetDibBits函数保存棋盘位图数据,到原始数据指针,固定256色
        bool            SaveBitmapToFile(LPSTR lpFileName);     //保存棋盘
        BYTE *          BMP2Position(void);                     //棋盘位图数据转换为棋盘数组

        CHESSMOVE       GetMove(BYTE pre_position[10][9] );     //pre_position:前一个棋盘数据数组（对方走子后，我方未走之前）
        void            DrawCchessBorder(void);     //

		//尽量把以下函数变成私有成员函数
        unsigned int CalcDxDy() ;    //找到另一个车，并计算出DX，DY,只在原始识别的时候调用一次
        unsigned long int Similiar(POINT DestPt, BYTE * pDstBMP,POINT SrcPt, BYTE * pSrcBMP ); //相似度计算，用于棋子匹配,返回相似度
        BYTE*           GetQiziDDB(HWND dstHWND,unsigned short ChessID);//取得某棋子的位图数据，宽高都为m_width

		//以下是新增加的函数：
		bool	build_solution(POINT MousePoint);			//按下热键时, 用于配合鼠标实时位置创建方案
		std::string BMP2Position(bool redGo);		//返回棋局盘面fen，同时拷贝到剪贴板

		static double	Similiar( const cv::Mat& i1, const cv::Mat& i2);	//计算相似度的另一个版本（用到了opencv）
		unsigned int	CalcDxDy_CV();	//计算m_dx的另一个版本（用到了opencv）
		bool			PiecesMat();	//从imgboard中分割出15个兵种图像存在m_imgPiece数组中。

		static void		ChangeSide(BYTE position[10][9]);
		static bool		IsValidMove(BYTE position[10][9],CHESSMOVE mv);
		static char *   ToFen(BYTE position[10][9] , bool RedGo );
		static BYTE *   PasteJuMian(char FenStr[2054]);
		const std::string	print(void) const ;

		//以下数据部分：
		//如果是图形客户端方案，则至少包含以下内容才能找到棋盘棋子状态
        char            m_ptcName[MAX_PLAYER_NAME];     //棋手名称,如果是图形客户端则为窗口标题关键字
        POINT           m_LRookInProg ;					//相对于程序顶层窗口黑车所在位置点 ，注意与m_LRookInBoard的区别
        BYTE            m_width;						//分辨率 ，样本棋子宽=高
        unsigned long int m_iDD;						//误差,相似度。
        BITMAP          m_bm;                           //原始位图信息
        BYTE          * m_pBoardBitmapBits;             //指向位图数据块（初始棋盘数据）

		//以下内容也可以保存作为另一种寻找棋盘窗口的办法
        int             m_MaxParentWinCountBoard;       //图形棋盘窗口其父窗口计数
        char            m_ClassNameBoard[10][254];      //图形棋盘窗口其父窗口类名
        char            m_WinNameBoard[10][254];        //图形棋盘窗口其父窗口标题

		//以下数据可以根据窗口句柄等计算出来，因此可不算在方案里
        POINT           m_iLeftTop;  //好像是多余的变量，未仔细检查。保存了最初鼠标指向黑车的位置，相对屏幕。
        POINT           m_LRookInBoard;                 //m_LRookInBoard黑车中心点相对于棋盘的位置，相当于棋盘原点，to Board client

        HBITMAP         m_hbitmap;                      //当前棋盘位图句柄

		HWND            m_hWndProg;                     //当前客户端程序窗口句柄（顶层）
        HWND            m_hWndBoard;                    //当前客户端棋盘窗口句柄（棋盘区域）
        RECT            m_ProgWinRect  ;                //top window rect
        RECT            m_BoardWinRect  ;               //board  rect

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        CUcci&          m_ucci;                         //应该设置为UCCI对象指针，根据不同的情况初始化以节省空间。待修改

        BYTE            m_position[10][9];              //当前棋盘数据数组
        bool            m_bQiShouRed;                   //是否拿红棋？
		int				m_iChessMoveFrom;
        bool            m_bQiShouBottom;                //if I am in bottom in  NewQiJu
        CHESSMOVE       m_Move;							//当前棋手走步

		//以下新增加的数据
		double	m_threshold;		//相似度阈值
		cv::Mat m_imgBoard;			//实时棋盘图像
		cv::Mat m_imgBoardInit;		//初始棋盘图像
		cv::Mat m_imgPiece[15];		//初始棋盘得来的各兵种图像，编号为1至14，B_KING＝1 R_PAWN=14; 0为空 与m_imgBoardInit为浅拷贝
private:
        bool            GetFromFile(char * filename);
        
		CHESSMOVE		Position2Move(BYTE pre_position[10][9] );    //通过比较两个position取得棋步
        bool			ValidPosition(BYTE position[10][9]);
		
		//以下数据部分
		char            m_cStepBuff[20];
        WCHAR           m_wsQiShouMove[8];
        long            m_dx;  //棋盘格宽，一般情况下均匀相间，且与格高相等
        long            m_dy;  //棋盘格高，一般情况下均匀相间，且与格宽相等，特殊情况下要考虑河界的差异
        BYTE          * m_pBMPQiziBits ;        //各兵种棋子的位图数据（初始棋盘数据）
        BYTE          * m_pCurBMPBoardBits;     //实时棋盘位图数据
};

//系统tools(全局函数)
CHESSMOVE Str2Move(const char str[5]);
void ErrorHappen(LPTSTR lpszFunction);
void StrToClip(char* pstr); //拷贝FEN串到剪贴板

extern const BYTE	InitChessBoard[10][9];		//初始棋盘定义
extern const POINT  m_InitPosOfPieceType[15];	//14个兵种起始位置（仅左半边）
