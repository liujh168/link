//PieceType
#define NOCHESS 0    //Ã»ÓÐÆå×Ó
#define B_KING       1  //ºÚË§
#define B_CAR        2  //ºÚ³µ
#define B_HORSE      3  //ºÚÂí
#define B_CANON      4  //ºÚÅÚ
#define B_BISHOP     5  //ºÚÊ¿
#define B_ELEPHANT   6  //ºÚÏó
#define B_PAWN       7  //ºÚ×ä
#define R_KING       8   //ºìË§
#define R_CAR        9   //ºì³µ
#define R_HORSE      10  //ºìÂí
#define R_CANON      11  //ºìÅÚ
#define R_BISHOP     12  //ºìÊ¿
#define R_ELEPHANT   13  //ºìÏó
#define R_PAWN       14  //ºì±ø
#define B_BEGIN      B_KING
#define B_END        B_PAWN

//PieceID
#define B_PAWN1       1  //ºÚ×ä
#define B_PAWN2       2  //ºÚ×ä
#define B_PAWN3       3  //ºÚ×ä
#define B_PAWN4       4 //ºÚ×ä
#define B_PAWN5       5 //ºÚ×ä
#define B_ELEPHANT1   6  //ºÚÏó
#define B_ELEPHANT2   7  //ºÚÏó
#define B_BISHOP1     8  //ºÚÊ¿
#define B_BISHOP2     9  //ºÚÊ¿
#define B_CANON1      10  //ºÚÅÚ
#define B_CANON2      11  //ºÚÅÚ
#define B_HORSE1      12 //ºÚÂí
#define B_HORSE2      13  //ºÚÂí
#define B_CAR1        14 //ºÚ³µ
#define B_CAR2        15  //ºÚ³µ
#define B_KING1		  16  //ºÚË§
#define R_BEGIN      R_KING
#define R_END        R_PAWN

#define R_PAWN1       17  //ºì×ä
#define R_PAWN2       18  //ºì×ä
#define R_PAWN3       19  //ºì×ä
#define R_PAWN4       20  //ºì×ä
#define R_PAWN5       21  //ºì×ä
#define R_ELEPHANT1   22  //ºìÏó
#define R_ELEPHANT2   23  //ºìÏó
#define R_BISHOP1     24  //ºìÊ¿
#define R_BISHOP2     25  //ºìÊ¿
#define R_CANON1      26  //ºìÅÚ
#define R_CANON2      27  //ºìÅÚ
#define R_HORSE1      28  //ºìÂí
#define R_HORSE2      29  //ºìÂí
#define R_CAR1        30  //ºì³µ
#define R_CAR2        31  //ºì³µ
#define R_KING1       32  //ºìË§

#define ISBLACK(x)  (x>=B_BEGIN&&x<=B_END)
#define ISRED(x)        (x>=R_BEGIN&&x<=R_END)
#define ISSAMESIDE(x,y)  ((ISBLACK(x)&&ISBLACK(y))||(ISRED(x)&&ISRED(y)))

//
//typedef struct _chessmove
//{
//	short	  	ChessID;
//	POINT	From;
//	POINT	To;
//	int             Score;
//    short           KillID;
//}CHESSMOVE;

//        ºÚÆåÔÚÉÏ£¬Ô­Ê¼×´Ì¬ÏÂ£¬ºÚÂí2½ø3±íÊ¾ÈçÏÂ£º
//        CHESSMOVE mv;
//        mv.ChessID=B_HORSE;
//        mv.From.x=1;
//        mv.From.y=0;
//        mv.To.x=2;
//        mv.To.y=2;
