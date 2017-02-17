//---------------------------------------------------------------------------
#ifndef progH
#define progH

#include "ucci.h"

typedef unsigned char      BYTE;
typedef int                BOOL;
#define FALSE                0
#define TRUE                 1
#define NOCHESS 0    //没有棋子
#define B_KING       1  //黑帅
#define B_CAR        2  //黑车
#define B_HORSE      3  //黑马
#define B_CANON      4  //黑炮
#define B_BISHOP     5  //黑士
#define B_ELEPHANT   6  //黑象
#define B_PAWN       7  //黑卒
#define B_BEGIN      B_KING
#define B_END        B_PAWN
#define R_KING       8   //红帅
#define R_CAR        9   //红车
#define R_HORSE      10  //红马
#define R_CANON      11  //红炮
#define R_BISHOP     12  //红士
#define R_ELEPHANT   13  //红象
#define R_PAWN       14  //红兵
#define R_BEGIN      R_KING
#define R_END        R_PAWN
#define IsBlack(x)  (x>=B_BEGIN&&x<=B_END)
#define IsRed(x)  (x>=R_BEGIN&&x<=R_END)
#define IsSameSide(x,y)  ((IsBlack(x)&&IsBlack(y))||(IsRed(x)&&IsRed(y)))

typedef struct _chessmanposition
{
	short x;
	short y;
}CHESSMANPOS;

typedef struct _chessmove
{
	short	  	ChessID;
	CHESSMANPOS	From;
	CHESSMANPOS	To;
	int               Score;
        short           KillID;
}CHESSMOVE;

class Cqishou
{
  public:
        char            m_ptcName[50];
        int             m_iChessMoveFrom;               //棋手类型（走步来源）：0-引擎；1、2-从其它进程中获取（文字或图像）；3、4-输入（键盘或鼠标或声音）；5－网络等其它途径
        bool            m_bProgBottom;                  //if computer above prog
        bool            m_bQiShouRed;                   // if red
        bool            m_bQiShouBottom;                //if i am bottom in  NewQiJu
        bool            m_bUseOpenBook;                 //是否使用开局库
        bool            m_forbid;                         //是否犯规 ，比如循环将军
        bool            m_bookfind;                       // if find move in openbook?

        CUcci           m_ucci;

        HWND            m_hWndProg;
        HWND            m_hWndBoard;                    //
        HWND            m_hWndSteps;                    // window handle of listbox steps;

        BITMAP          m_bm;
        HBITMAP         m_hbitmap;
        BYTE            * m_pBMPData;

        int             m_MaxParentWinCountBoard;
        char            m_ClassNameBoard[10][254];
        char            m_WinNameBoard[10][254];
        int             m_MaxParentWinCountSteps;
        char            m_ClassNameSteps[10][254];
        char            m_WinNameSteps[10][254];

        POINT           m_iLeftTop;                     //to Screnn
        POINT           m_OXY;                          //to Board client
        POINT           m_iPointInBoard ;               //to PROG Client
        POINT           m_iPointInSteps ;               //to Steps Client
        RECT            m_ProgWinRect  ;                //topwindow rect
        RECT            m_BoardWinRect  ;               //board rect
        RECT            m_StepsWinRect  ;               //board rect

        BYTE            m_dx;
        BYTE            m_dy;
        BYTE            m_iDI;                          //分辨率
        unsigned long int             m_iDD;                          //误差
        BYTE            m_position[10][9];
        long int        m_scores;                         //得分
        CHESSMOVE       m_cmQiShouStep;
        CHESSMOVE       m_cmMyHistroyMove[6];           //我的历史走步

        int             m_iFirstTime;
        int             m_iFirstSteps;                  //第一时限：共 m_iFirstTime分钟，必须走满m_iFirstSteps步
        int             m_iSecondTime;
        int             m_iSecondSteps;                 //第二时限：第m_iSecondTime分钟走m_iSecondStemps步
        int             m_iTotalTimes;                  //总用时

        HWND            FindWinBoardAndSteps(int maxparent, char winclass[10][254],char winname[10][254]);
        bool            Go(CHESSMOVE cmQiJuMove, bool bBottomGo);

        bool            CalcDx();
        bool            Similiar(POINT DestPt, POINT SrcPt );
        HBITMAP         GetBMPBoard();
        bool            LocateBlackCar(POINT DestPt, POINT SrcPt );
        bool            SaveBoardData(HBITMAP hbitmap);
        BYTE *          BMP2ChessBoard(HBITMAP hbitmap);

        Cqishou();
        ~Cqishou();

};

class  Cnewqiju
{
        public:
	        BYTE            m_ChessBoard[10][9];
                bool            m_bBottomGo;
                CHESSMOVE       m_cmQiJuMove;
                CHESSMOVE       m_cmQiPu[200];                  //本局棋谱    最长200步
                WCHAR           m_wsQiJuMove[8];                //

                WCHAR           m_wsBestMove[5];                //当前最好走步的中国式表示法
                WCHAR           m_wsBetterMove[5];              //当前次好走步的中国式表示法
                WCHAR           m_wsGoodMove[5];                //当前好走步的中国式表示法

                char            m_cStepBuff[20];
	        int             m_iTurn;
                bool            m_bGameOver;
                bool            m_bConnected;

                Cqishou         m_PlayerBottom;
                Cqishou         m_PlayerTop;
                Cqishou *       m_WhichPlayer;
                struct {
                        int year;
                        int month;
                        int day;
                        }  m_asChessDate ;              //比赛日期
                struct {
                        int hour;
                        int minitue;
                        int second;
                        }  m_asChessTime;               //比赛时间
                char m_asChessaddress[];                //比赛地点
                char m_asChessName[];                   //比赛名称
                BYTE result;                            //比赛结果


                void            WCHAR2CHESSMOVE(WCHAR wsChessMove[5], CHESSMOVE &cmChessMove );
                void            CHESSMOVE2WCHAR(CHESSMOVE cmChessMove , WCHAR wsChessMove[5]);
                BYTE            MakeMove(CHESSMOVE* move) ;
                void            UnMakeMove(CHESSMOVE* move,BYTE nChessID)  ;
                void            DrawCchessBorder(Cqishou player);
                void            DrawStepsBorder(Cqishou player) ;

                BOOL            GetQiShouMove(Cqishou *player);
                char*            MyGetListViewItem(Cqishou * player)  ;
	        BOOL 	        IsValidMove(BYTE position[10][9], int nFromX, int nFromY, int nToX, int nToY);
        	bool            SaveToFile(char * filename);
        	bool            GetFromFile(char * filename);
                void            Start();
                void            ContiSame(Cqishou playerto , Cqishou playerfrom );
                int             IsGameOver(BYTE position[10][9]);
                void            ChangeSide(BYTE position[10][9]);
                BYTE            GetBMPMove(Cqishou *player);

                char *           CopyJuMian(BYTE position[10][9] , bool RedGo );
                BYTE *           PasteJuMian(char FenStr[2054]);

//                CSearchEngine   *m_pSE;
		BYTE            computerthink();		        //根据当前法走一步棋

	        Cnewqiju();
                ~Cnewqiju();
};

const BYTE InitChessBoard[10][9]=
{
    {
        B_CAR,B_HORSE,B_ELEPHANT,B_BISHOP,B_KING,B_BISHOP,B_ELEPHANT,B_HORSE,B_CAR
    }
    ,
    {
        NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS
    }
    ,
    {
        NOCHESS,B_CANON,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,B_CANON,NOCHESS
    }
    ,
    {
        B_PAWN,NOCHESS,B_PAWN,NOCHESS,B_PAWN,NOCHESS,B_PAWN,NOCHESS,B_PAWN
    }
    ,
    {
        NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS
    }
    ,

    {
        NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS
    }
    ,
    {
        R_PAWN,NOCHESS,R_PAWN,NOCHESS,R_PAWN,NOCHESS,R_PAWN,NOCHESS,R_PAWN
    }
    ,
    {
        NOCHESS,R_CANON,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,R_CANON,NOCHESS
    }
    ,
    {
        NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS
    }
    ,
    {
        R_CAR,R_HORSE,R_ELEPHANT,R_BISHOP,R_KING,R_BISHOP,R_ELEPHANT,R_HORSE,R_CAR
    }
}
;




//---------------------------------------------------------------------------
#endif
