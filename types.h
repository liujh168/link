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
#define IsRed(x)        (x>=R_BEGIN&&x<=R_END)
#define IsSameSide(x,y)  ((IsBlack(x)&&IsBlack(y))||(IsRed(x)&&IsRed(y)))

#define ENGINE_PLAYER           0  //棋步来自于UCCI引擎
#define BMP_PLAYER              1  //棋步来自于图形客户端
#define KEYBOARD_PLAYER         2  //棋步来自于鼠标操作
#define EDIT_PLAYER             3  //棋步来自于文本编辑框里的输入
#define INTERTNET_PLAYER        4  //棋步来自于网络
#define VOICE_PLAYER            5  //棋步来自于语音识别

#define MAX_PLAYER_NAME         50 //窗口标题或棋手名称最大长度
#define THRESHOLD               100//相似度阈值
#define COLORBITSCOUNT          8  //颜色位数

typedef struct _chessmanposition
{
	long x;
	long y;
}CHESSMANPOS;    //约定棋盘左上角为（0,0）
//        黑棋在上，原始状态下，黑马2进3表示如下：
//        CHESSMOVE mv;
//        mv.ChessID=B_HORSE;
//        mv.From.x=1;
//        mv.From.y=0;
//        mv.To.x=2;
//        mv.To.y=2;

typedef struct _chessmove
{
	short	  	ChessID;
	CHESSMANPOS	From;
	CHESSMANPOS	To;
	int             Score;
    short           KillID;
}CHESSMOVE;
