/*************************************************************************************************************************
类名称   : CXXX
功能     : <简要说明该类所完成的功能>
异常类   : <属于该类的异常类（如果有的话）> 
--------------------------------------------------------------------------------------------------------------------------
备注     : <使用该类时需要注意的问题（如果有的话）> 
典型用法 : <如果该类的使用方法较复杂或特殊，给出典型的代码例子> 
--------------------------------------------------------------------------------------------------------------------------
作者     : <xxx>
*************************************************************************************************************************/

#pragma once

#include "bmpPlayer.h"

class  CNewQiJu
{
public:
        CNewQiJu();
        ~CNewQiJu();
        BYTE            MakeMove(CHESSMOVE* move) ;
        void            UnMakeMove(CHESSMOVE* move,BYTE nChessID)  ;
        void            Start();
        int             IsGameOver(BYTE position[10][9]);
        BYTE *          PasteJuMian(char FenStr[2054]);
        void            WCHAR2CHESSMOVE(WCHAR wsChessMove[5], CHESSMOVE &cmChessMove );

		
        static void     ChangeSide(BYTE position[10][9]);
        static BOOL     IsValidMove(BYTE position[10][9], CHESSMOVE mv);

public:
        BYTE            m_ChessBoard[10][9];
        bool            m_bBottomGo;
        CHESSMOVE       m_cmQiJuMove;
        bool            m_bConnected;

        CUcci           m_PlayerUcci;                //这个必须在后面两个棋手对象前面定义
        CMatPlayer      m_PlayerBottom;
        CMatPlayer      m_PlayerTop;
        CMatPlayer *    m_WhichPlayer;
private:
        int             m_iTurn;
        bool            m_bGameOver;
};