//  this is the latest ver ...2006 04 03      22:00
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "formlink.h"
#include "ucci.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMainForm *MainForm;

//---------------------------------------------------------------------------

void __fastcall TMainForm::imgbMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
      if(myqiju.m_bConnected)
      {
              //ShowMessage("already start! please stop first");
              return;
      }
      if(Button == mbLeft)
      {
            hWindow = NULL;
            hOldWindow = NULL;
            ::SetCursor(LoadCursor(HInstance, "MyCur"));
            bMouseDown = true;
            ((TImage *) Sender)->Hide();

            tmr1->Enabled = false ;
      }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::imgbMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
    char bufClassName[255], bufClassCaption[255];
    int TextLength;

    if(myqiju.m_bConnected)
    {
        ////ShowMessage("already start! please stop first");
        return;
    }

    if(bMouseDown)
    {
        GetCursorPos(&MousePoint);

        HRGN hrgn;
        RECT rc;

        hWindow = WindowFromPoint(MousePoint);

        rc.left = MainForm->Left;
        rc.top = MainForm->Top;
        rc.right = MainForm->Left + MainForm->Width;
        rc.bottom = MainForm->Top + MainForm->Height;
        hrgn = CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
        if( PtInRegion(hrgn, MousePoint.x,MousePoint.y ))
        {
                return;

        }
        if(hWindow != NULL && hWindow != this->Handle && hWindow != MainForm->Handle )
        {
            if(hOldWindow != NULL)
            {
                Canvas->Handle = GetWindowDC(hOldWindow);
                Canvas->Pen->Width = 4;
                Canvas->Pen->Mode = pmNotXor;
                Canvas->Brush->Style = bsClear;
                Canvas->Rectangle(0, 0, OldClassRect.Width(), OldClassRect.Height());
             }

            GetWindowRect(hWindow, &ClassRect);
            if(hWindow != NULL)
            {
                Canvas->Handle = GetWindowDC(hWindow);
                Canvas->Pen->Width = 4;
                Canvas->Pen->Color = clRed;
                Canvas->Pen->Mode = pmNotXor;
                Canvas->Brush->Style = bsClear;
                Canvas->Rectangle(0, 0, ClassRect.Width(), ClassRect.Height());

            }
            OldClassRect = ClassRect;
            hOldWindow = hWindow;
        }
    }

}
//---------------------------------------------------------------------------

void __fastcall TMainForm::imgbMouseUp(TObject *Sender,TMouseButton Button, TShiftState Shift, int X, int Y)
{

        if(myqiju.m_bConnected || ! bMouseDown )
        {
                //ShowMessage("already start! please stop first");
                return;
        }

        if(hOldWindow != NULL)
        {
            Canvas->Handle = GetWindowDC(hOldWindow);
            Canvas->Pen->Width = 4;
            Canvas->Pen->Mode = pmNotXor;
            Canvas->Brush->Style = bsClear;
            Canvas->Rectangle(0, 0, OldClassRect.Width(), OldClassRect.Height());
        }

        if(  ((TImage *) Sender)->Name == "imgb")
        {
            myqiju.m_WhichPlayer = & myqiju.m_PlayerTop;  //
        }
        else  if(  ((TImage *) Sender)->Name == "imgr")
        {
            myqiju.m_WhichPlayer = & myqiju.m_PlayerBottom;  //
        }
        // collect window infomation
        HWND hWndTemp,hWndTop;
        char lpszClassName[10][254], lpszWindowText[10][254];

        int MyMaxParentWinCount = 0;
        // reset to 0
        for(int i=0;i<10;i++)
        for(int j=0;j<254;j++)
        {
               lpszClassName[i][j]=0;
               lpszWindowText[i][j]=0;
        }

        GetWindowText( hWindow ,&lpszWindowText[0][0], 254 );
        GetClassName( hWindow ,&lpszClassName[0][0], 254 );

        //locate top window
        hWndTemp = GetParent(hWindow) ;
        hWndTop=hWndTemp;
        while( hWndTemp!= NULL)
        {
              MyMaxParentWinCount++;
              hWndTop=hWndTemp;
              GetWindowText( hWndTemp ,&lpszWindowText[MyMaxParentWinCount][0], 254 );
              GetClassName( hWndTemp ,&lpszClassName[MyMaxParentWinCount][0], 254 );
              hWndTemp = GetParent(hWndTemp)  ;
        }

        myqiju.m_WhichPlayer->m_hWndProg = hWndTop;
        GetWindowRect(hWndTop, & myqiju.m_WhichPlayer->m_ProgWinRect);

        if(rbbsteps->Checked )    //
        {
                myqiju.m_WhichPlayer->m_MaxParentWinCountSteps = MyMaxParentWinCount ;
                for(int i=0;i<10;i++)
                for(int j=0;j<254;j++)
                {
                      myqiju.m_WhichPlayer->m_ClassNameSteps[i][j] = lpszClassName[i][j];
                      myqiju.m_WhichPlayer->m_WinNameSteps[i][j] = lpszWindowText[i][j];
                }
                myqiju.m_WhichPlayer->m_hWndSteps=hWindow;  //
                myqiju.m_WhichPlayer->m_iPointInSteps = MousePoint;
                ::ScreenToClient( myqiju.m_WhichPlayer->m_hWndProg, &myqiju.m_WhichPlayer->m_iPointInSteps);

                POINT pt1steps,pt4steps;

                GetWindowRect(hWindow, & myqiju.m_WhichPlayer->m_StepsWinRect);

                pt1steps.x = myqiju.m_WhichPlayer->m_StepsWinRect.left;
                pt1steps.y = myqiju.m_WhichPlayer->m_StepsWinRect.top;
                ::ScreenToClient( myqiju.m_WhichPlayer->m_hWndSteps, &pt1steps);
                myqiju.m_WhichPlayer->m_StepsWinRect.left = pt1steps.x;
                myqiju.m_WhichPlayer->m_StepsWinRect.top = pt1steps.y;

                pt4steps.x = myqiju.m_WhichPlayer->m_StepsWinRect.right;
                pt4steps.y = myqiju.m_WhichPlayer->m_StepsWinRect.bottom;
                ::ScreenToClient( myqiju.m_WhichPlayer->m_hWndSteps, &pt4steps);
                myqiju.m_WhichPlayer->m_StepsWinRect.right = pt4steps.x;
                myqiju.m_WhichPlayer->m_StepsWinRect.bottom = pt4steps.y;

        }
        if(rbbboard->Checked )
        {
              myqiju.m_WhichPlayer->m_MaxParentWinCountBoard = MyMaxParentWinCount ;
              for(int i=0;i<10;i++)
              for(int j=0;j<254;j++)
              {
                    myqiju.m_WhichPlayer->m_ClassNameBoard[i][j] = lpszClassName[i][j];
                    myqiju.m_WhichPlayer->m_WinNameBoard[i][j] = lpszWindowText[i][j];
              }
              myqiju.m_WhichPlayer->m_hWndBoard=hWindow;
              myqiju.m_WhichPlayer->m_iLeftTop=MousePoint;
              myqiju.m_WhichPlayer->m_iPointInBoard = MousePoint;
              ::ScreenToClient( myqiju.m_WhichPlayer->m_hWndProg, &myqiju.m_WhichPlayer->m_iPointInBoard);

              GetWindowRect(hWindow, & myqiju.m_WhichPlayer->m_BoardWinRect);
              myqiju.m_WhichPlayer->m_OXY = MousePoint ;
              ::ScreenToClient( myqiju.m_WhichPlayer->m_hWndBoard,&myqiju.m_WhichPlayer->m_OXY);

              myqiju.m_WhichPlayer->m_iPointInBoard = MousePoint;
              ::ScreenToClient( myqiju.m_WhichPlayer->m_hWndProg,&myqiju.m_WhichPlayer->m_iPointInBoard);


              ///////////////////////////////////////////////////////////////////////////////////////
             char buffer[255];
             char buffer1[255];

             strcpy( myqiju.m_WhichPlayer->m_ptcName, &lpszWindowText[MyMaxParentWinCount][0]);

             bool  bTemp = false;
             try
              {

//                  BringWindowToTop( myqiju.m_WhichPlayer->m_hWndProg );
                  if( myqiju.m_WhichPlayer->GetBMPBoard() ==NULL )
                  {
                        ShowMessage("Catch Board error!");
                  }
                  if(!myqiju.m_WhichPlayer->SaveBoardData( myqiju.m_WhichPlayer->m_hbitmap ))
                  {
                       ShowMessage("SaveBoardData error!");
                  }

                  bTemp = myqiju.m_WhichPlayer->CalcDx();
                  if( !bTemp )
                  {
                       ShowMessage("CalcDx error! Get step in another way!");
                  }
                  drawiconboard(myqiju.m_WhichPlayer->m_position);
              }
              catch(...)
              {
                      ShowMessage("ex :Catch Board Error! ");
              }
              itoa(myqiju.m_WhichPlayer->m_dx , buffer ,10);
              itoa(myqiju.m_WhichPlayer->m_dy , buffer1 ,10);
              tmr1->Enabled=true;

              if(  ((TImage *) Sender)->Name == "imgb")
              {
                   grptop->Caption=myqiju.m_WhichPlayer->m_ptcName;
                   edtbdx->Text= buffer;
                   edtbdy->Text= buffer1;
                   if(bTemp)
                   {
                       edtbdx->Text =edtbdx->Text + "OK!";   edtbdy->Text= edtbdy->Text+ "OK!";
                   }
              }
              else  if(  ((TImage *) Sender)->Name == "imgr")
              {
                   grpbottom->Caption=myqiju.m_WhichPlayer->m_ptcName;
                   edtrdx->Text= buffer;
                   edtrdy->Text= buffer1;
                   if(bTemp)
                   {
                       edtrdx->Text =edtrdx->Text + "OK!";   edtrdy->Text= edtrdy->Text+ "OK!";
                   }
              }
            ///////////////////////////////////////////////////////////////////////////////////////////////
        }

        ::SetCursor(LoadCursor(NULL, IDC_ARROW));
        ((TImage *) Sender)->Cursor = crHandPoint;
        ((TImage *) Sender)->Show();
        bMouseDown = false;
}

//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner):TForm(Owner)
{
}

void __fastcall TMainForm::edtbdxChange(TObject *Sender)
{
        if(myqiju.m_bConnected)
        {
                //ShowMessage("already start! please stop first");
                return;
        }
        myqiju.m_PlayerTop.m_dx = atoi(edtbdx->Text.c_str());
       
}

//---------------------------------------------------------------------------

void __fastcall TMainForm::edtbdyChange(TObject *Sender)
{
        if(myqiju.m_bConnected)
        {
                //ShowMessage("already start! please stop first");
                return;
        }
         myqiju.m_PlayerTop.m_dy = atoi(edtbdy->Text.c_str());
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::edtrdxChange(TObject *Sender)
{
        if(myqiju.m_bConnected)
        {
                //ShowMessage("already start! please stop first");
                return;
        }
        myqiju.m_PlayerBottom.m_dx = atoi(edtrdx->Text.c_str());
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::edtrdyChange(TObject *Sender)
{
        if(myqiju.m_bConnected)
        {
                //ShowMessage("already start! please stop first");
                return;
        }
        myqiju.m_PlayerBottom.m_dy = atoi(edtrdy->Text.c_str());
}

//---------------------------------------------------------------------------


void __fastcall TMainForm::edtbottomplayerKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
        WCHAR buff[10];
        if(Key==13)
        {

              StringToWideChar(edtbottomplayer->Text,buff,10);
              myqiju.WCHAR2CHESSMOVE( buff, myqiju.m_cmQiJuMove );
              myqiju.m_ChessBoard[myqiju.m_cmQiJuMove.To.y][myqiju.m_cmQiJuMove.To.x]= myqiju.m_ChessBoard[myqiju.m_cmQiJuMove.From.y][myqiju.m_cmQiJuMove.From.x];
              myqiju.m_ChessBoard[myqiju.m_cmQiJuMove.From.y][myqiju.m_cmQiJuMove.From.x]= NOCHESS;
              myqiju.m_bBottomGo = !  myqiju.m_bBottomGo;


              char * p;
              p = myqiju.CopyJuMian(myqiju.m_ChessBoard,myqiju.m_bBottomGo);
              char buffer[1024];
              strcpy(buffer,"position fen ");
              strcat(buffer,p);

              myqiju.m_PlayerTop.m_ucci.UCCI2Engine(buffer);
              myqiju.m_PlayerTop.m_ucci.UCCI2Engine("go depth 3");

//              strcpy(buffer,edtbottomplayer->Text.c_str());
//              myqiju.m_PlayerTop.m_ucci.UCCI2Engine(buffer);
//
//              while ( myqiju.m_PlayerBottom.m_ucci.pipeEngine.LineInput(myqiju.m_PlayerBottom.m_ucci.szLineStr))
//              {
//                 mmo1->Lines->Add(myqiju.m_PlayerBottom.m_ucci.szLineStr)  ;
//              }
              edtbottomplayer->Text = "";

              //            myqiju.m_PlayerBottom.Go( myqiju.m_cmQiJuMove ,myqiju.m_bBottomGo);
        }


}
//---------------------------------------------------------------------------





void __fastcall TMainForm::chkbottomClick(TObject *Sender)
{
        if(myqiju.m_bConnected)
        {
//                ShowMessage("already start! please stop first");
                chkbottom->Checked  =!chkbottom->Checked;
                return;
        }
        if(chkbottom->Checked)
        {
            myqiju.m_PlayerBottom.m_bProgBottom=true;
        }
        else
        {
            myqiju.m_PlayerBottom.m_bProgBottom=false;
        }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::chkredClick(TObject *Sender)
{
        if(myqiju.m_bConnected)
        {
//                ShowMessage("already start! please stop first");
                chkred->Checked =!chkred->Checked;
                return;
        }
      if(chkred->Checked)
      {
          myqiju.m_PlayerBottom.m_bQiShouRed = true;
          myqiju.m_bBottomGo = true;
          myqiju.m_PlayerTop.m_bQiShouRed = false;

          myqiju.ChangeSide(myqiju.m_ChessBoard);

          chkbottom->Font->Color = clRed;
          chktop->Font->Color = clWindowText;
          imgtemp->Picture = imgr->Picture ;
          imgr->Picture = imgb->Picture ;
          imgb->Picture = imgtemp->Picture ;
      }
      else
      {
          myqiju.m_PlayerBottom.m_bQiShouRed = false;
          myqiju.m_bBottomGo = false;
          myqiju.m_PlayerTop.m_bQiShouRed = true;

          myqiju.ChangeSide(myqiju.m_ChessBoard);

          chkbottom->Font->Color =clWindowText ;
          chktop->Font->Color = clRed;
          imgtemp->Picture = imgb->Picture;
          imgb->Picture = imgr->Picture ;
          imgr->Picture = imgtemp->Picture ;
      }

      drawiconboard(myqiju.m_ChessBoard);

}
//---------------------------------------------------------------------------

void __fastcall TMainForm::chktopClick(TObject *Sender)
{
        if(myqiju.m_bConnected)
        {
//                ShowMessage("already start! please stop first");
                chktop->Checked =!chktop->Checked;
                return;
        }
        if(chktop->Checked)
        {
            myqiju.m_PlayerTop.m_bProgBottom=false;
        }
        else
        {
            myqiju.m_PlayerTop.m_bProgBottom=true;
        }

}
//---------------------------------------------------------------------------

bool TMainForm::copyimg()
{
        TRect   MyRect, MyOther;
        HWND    hWindow;
        TColor qizi[20][20],qizi1[20][20];
        TColor c;
        int h ,w ;
        POINT lefttop;
        short dd1,dd2;

        hWindow = this->Handle;

        for(int i =0;i<20;i++)
        for(int j=0;j<20;j++)
                qizi[i][j]=0;

	HDC hdcTop = ::GetWindowDC( myqiju.m_PlayerTop.m_hWndBoard ) ;
	HDC hdcBottom = ::GetWindowDC( myqiju.m_PlayerBottom.m_hWndBoard ) ;
        dd1 =  myqiju.m_PlayerTop.m_dx;
        dd2 =  myqiju.m_PlayerBottom.m_dx;

        MyRect = Rect(10,10,100,100);
        MyOther = Rect(10,111,100, 201);

        COLORREF color1,color2;
        POINT from,to;
        from.x=  myqiju.m_PlayerTop.m_iLeftTop.x ;
        from.y=  myqiju.m_PlayerTop.m_iLeftTop.y ;
        to.x=  myqiju.m_PlayerBottom.m_iLeftTop.x ;
        to.y=  myqiju.m_PlayerBottom.m_iLeftTop.y ;
        LPRECT lpRect ;
        ::ScreenToClient(myqiju.m_PlayerTop.m_hWndBoard,&from);
        ::ScreenToClient(myqiju.m_PlayerBottom.m_hWndBoard,&to);
       // GetWindowRect( myqiju.m_PlayerTop.m_hWndBoard, lpRect );
        MainForm->Canvas->Handle = hdcTop;
        MainForm->Canvas->ClipRect.Left = 0;
        MainForm->Canvas->ClipRect.Top = 0;
        MainForm->Canvas->ClipRect.Bottom = from.x+100*dd1;      //recal;
        MainForm->Canvas->ClipRect.Right = from.y+100*dd1;

        for (int i = 0; i <= 16; i++)
        for (int j = 0; j <= 16; j++)
        {
                qizi[i][j]= MainForm->Canvas->Pixels[from.x+i-8][from.y+j-8];
                qizi1[i][j]= MainForm->Canvas->Pixels[from.x+i-8+7*dd1][from.y+j-8];
                if( qizi[i][j] != qizi1[i][j] )
                {
                   // ::ShowMessage("not as same as original");
                }
                //color1 = GetPixel(hdcTop,from.x+i-8, from.y+j-8 );
                color2 = SetPixel(hdcBottom,to.x+i, to.y+j, qizi[i][j]);
                color2 = SetPixel(hdcBottom,to.x+i, to.y+j, qizi1[i][j]);

        }

        int j=0;

}




void __fastcall TMainForm::btnresetClick(TObject *Sender)
{
      if(!myqiju.m_bConnected)
      {
          return;
      }
      if( btnreset->Caption == "pause")
      {
          btnreset->Caption = "conti" ;
          autogo->Suspend();
          tmr1->Enabled = false;
      }
      else
      {
          btnreset->Caption = "pause" ;
          autogo->Resume();
          tmr1->Enabled = true;
      }
}
//---------------------------------------------------------------------------








HBITMAP TMainForm::copyscreentobitmap(Cqishou & player)
//lprect 代表选定区域
{
      TRect MyRect ;
      int width,height;
      Graphics::TBitmap *MyBmp= new Graphics::TBitmap();
      TCanvas * MyCanvas=new TCanvas();
      bool min=false;

      if(!IsWindow(player.m_hWndBoard) || !IsWindow(player.m_hWndProg))
      {
           return NULL;
      }

      if(min=( IsIconic(player.m_hWndProg) || IsWindowVisible(player.m_hWndProg)))
      {
           ShowWindow(player.m_hWndProg,SW_SHOW);
      }

      GetWindowRect(player.m_hWndBoard, &MyRect) ;    // may be changed

      width  = MyRect.Width();
      height = MyRect.Height();
      MyRect.Left = 0;
      MyRect.Top = 0;
      MyRect.Right = width;
      MyRect.Bottom = height;

     if (IsRectEmpty(&MyRect))    // 确保选定区域不为空矩形
              return NULL;

     HDC MyDC = GetWindowDC(player.m_hWndBoard);
     MyCanvas->Handle = MyDC;

     MyBmp->Width = MyRect.right;
     MyBmp->Height = MyRect.Bottom;
     MyBmp->Canvas->CopyRect( MyRect, MyCanvas , MyRect);

     ReleaseDC(0,MyDC);
     MyCanvas->Handle = 0;
     imgtemp->Picture->Bitmap = MyBmp;
     player.m_hbitmap = imgtemp->Picture->Bitmap->Handle ;

      if(min)
      {
//           ShowWindow(player.m_hWndProg,SW_SHOWMINNOACTIVE);
      }


     delete MyCanvas;
     delete MyBmp;
     return player.m_hbitmap;     // 返回位图句柄
}




void __fastcall TMainForm::tmr1Timer(TObject *Sender)
{
        if(myqiju.m_bBottomGo)
        {
               MainForm->Caption = grpbottom->Caption + "  Go! (Bottom)";
        }
        else
        {
               MainForm->Caption =  grptop->Caption + "Go! (Top)";
        }

        if(! myqiju.m_bConnected )      //未开始时
        {
                if( myqiju.m_WhichPlayer->GetBMPBoard() == NULL )
                {
//                     ShowMessage("GetBMPBoard error in timer!");
                }
                if( NULL == myqiju.m_WhichPlayer->BMP2ChessBoard(myqiju.m_WhichPlayer->m_hbitmap))
                {
//                    ShowMessage("BMP2ChessBoard error in timer!");
                }
                drawiconboard(myqiju.m_WhichPlayer->m_position );
        }
        else
        {
                drawiconboard(myqiju.m_ChessBoard);
                //flash window

//                FLASHWINFO PFLASHWINFO;
//                PFLASHWINFO.cbSize = sizeof(FLASHWINFO);
//                PFLASHWINFO.hwnd=this->Handle;
//                PFLASHWINFO.dwFlags=FLASHW_ALL;
//                PFLASHWINFO.uCount = 10;
//                PFLASHWINFO.dwTimeout = 50  ;
//                FlashWindowEx(&PFLASHWINFO);

                FlashWindow(myqiju.m_WhichPlayer->m_hWndProg,true);

                 // hide or show window  atuomatically
//                POINT * pmypoint = new POINT;
//                GetCursorPos(pmypoint);
//                if((pmypoint->x > this->Left)&&(pmypoint->x < (this->Left + this->Width))&&
//                    (pmypoint->y > this->Top)&& (pmypoint->y < (this->Top+this->Height)))
//                {
//                     this->Show();
//                }
//                else
//                {
//                     this->Hide();
//                }
//                delete pmypoint;

        }

}
//---------------------------------------------------------------------------

void __fastcall TMainForm::tmr2Timer(TObject *Sender)
{
    char buffer[1024];
    strcpy(buffer,myqiju.m_PlayerBottom.m_ucci.szLineStr);
    if(myqiju.m_PlayerBottom.m_ucci.pipeEngine.LineInput(myqiju.m_PlayerBottom.m_ucci.szLineStr))
    {
             mmo1->Lines->Add(myqiju.m_PlayerBottom.m_ucci.szLineStr)  ;
             if (strncmp(myqiju.m_PlayerBottom.m_ucci.szLineStr, "bestmove ", 9) == 0)
             {
                 int i = 0;
                 int x1,y1,x2,y2;
                 x1= myqiju.m_PlayerBottom.m_ucci.szLineStr[9]-97 ;
                 y1= myqiju.m_PlayerBottom.m_ucci.szLineStr[10] -48 ;
                 x2= myqiju.m_PlayerBottom.m_ucci.szLineStr[11] -97;
                 y2= myqiju.m_PlayerBottom.m_ucci.szLineStr[12]-48 ;
                 y1=9-y1;   y2=9-y2;
                 myqiju.m_ChessBoard[y2][x2]=myqiju.m_ChessBoard[y1][x1];
                 myqiju.m_ChessBoard[y1][x1]=NOCHESS;
                 myqiju.m_bBottomGo = !myqiju.m_bBottomGo;
             }

    }
    MainForm->drawiconboard(myqiju.m_ChessBoard);

}
//---------------------------------------------------------------------------
void    TMainForm::drawiconboard(BYTE position[10][9])
{
        POINT pt1,pt2;
        int dx,dy;
        TCanvas * MyCanvas=new TCanvas();
//        HDC MyDC = GetWindowDC(imgboard->Canvas->Handle);    //img donot allow to draw !
        HDC MyDC = GetWindowDC(this->Handle);
        MyCanvas->Handle = MyDC;

//        MyCanvas->Handle = imgboard->Canvas->Handle ;
        dx=18;
        dy=18;


        pt1.x=imgboard->Left + 3;
        pt1.y=imgboard->Top +50;
//        pt1.x=imgboard->Left + 3;
//        pt1.y=imgboard->Top +30;

        MainForm->Refresh();
        for(int i =0;i<9; i++)
        for(int j=0;j<10;j++)
        {
             if(position[j][i]!= NOCHESS && position[j][i] >= 0 && position[j][i] <= 14 )
             {
                il1->Draw(MyCanvas,pt1.x+i*dx,pt1.y+j*dy,position[j][i], true);
             }
        }
        
      ReleaseDC(0,MyDC);
      MyCanvas->Handle = 0;
      delete MyCanvas;


}



void __fastcall TMainForm::edtrdiChange(TObject *Sender)
{
        if(myqiju.m_bConnected)
        {
                //ShowMessage("already start! please stop first");
                return;
        }
        myqiju.m_PlayerBottom.m_iDI = atoi(edtrdi->Text.c_str());
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::edtrddChange(TObject *Sender)
{
         if(myqiju.m_bConnected)
        {
                //ShowMessage("already start! please stop first");
                return;
        }
        myqiju.m_PlayerBottom.m_iDD = atoi(edtrdd->Text.c_str());    
}
//---------------------------------------------------------------------------



void __fastcall TMainForm::FormDestroy(TObject *Sender)
{
      try
      {
         if(myqiju.m_PlayerTop.m_pBMPData!= NULL)
         {
             delete myqiju.m_PlayerTop.m_pBMPData;
             myqiju.m_PlayerTop.m_pBMPData = NULL;
         }

         if(myqiju.m_PlayerBottom.m_pBMPData!=NULL)
         {
             delete myqiju.m_PlayerBottom.m_pBMPData;
             myqiju.m_PlayerBottom.m_pBMPData = NULL;
         }
      }
      catch(...)
      {
          ShowMessage("Error in FormDestroy !");
      }

}
//---------------------------------------------------------------------------


void    TMainForm::copymyqijutoform()
{
//       if( myqiju.m_PlayerTop.m_bProgBottom )
//       {
//                chktop->Checked=  false;
//       }
//       else
//       {
//              chktop->Checked =true;
//       }
//
//       if( myqiju.m_PlayerBottom.m_bProgBottom )
//       {
//              chkbottom->Checked = true;
//       }
//       else
//       {
//              chkbottom->Checked =false ;
//       }
//
//       if( myqiju.m_PlayerTop.m_bQiShouRed && !myqiju.m_PlayerBottom.m_bQiShouRed )
//       {
//            chkred->Checked=false   ;     //本次设置会引发chkred click事件！
//       }
//       else if( ! myqiju.m_PlayerTop.m_bQiShouRed && myqiju.m_PlayerBottom.m_bQiShouRed )
//       {
//         chkred->Checked=true   ;          //本次设置会引发chkred click事件！
//       }
//       else
//       {
//           chkred->Caption = "turn of both play error! ";
//       }

       char pchar[254];
       grptop->Caption = myqiju.m_PlayerTop.m_ptcName;
       edtbdx->Text = itoa(myqiju.m_PlayerTop.m_dx,pchar,10);
       edtbdy->Text = itoa(myqiju.m_PlayerTop.m_dy,pchar,10);

       grpbottom->Caption = myqiju.m_PlayerBottom.m_ptcName;
       edtrdx->Text = itoa(myqiju.m_PlayerBottom.m_dx,pchar,10);
       edtrdy->Text = itoa(myqiju.m_PlayerBottom.m_dy,pchar,10);

         
       edtrdi->Text = itoa(myqiju.m_PlayerBottom.m_iDI,pchar,10);
       edtrdd->Text = itoa(myqiju.m_PlayerBottom.m_iDD,pchar,10);

      if(myqiju.m_bBottomGo)
      {
             MainForm->Caption = grpbottom->Caption + "  Go! (Bottom)";
      }
      else
      {
             MainForm->Caption =  grptop->Caption + "Go! (Top)";
      }

     myqiju.DrawCchessBorder(myqiju.m_PlayerTop);
     myqiju.DrawStepsBorder(myqiju.m_PlayerTop);
     myqiju.DrawCchessBorder(myqiju.m_PlayerBottom);
     myqiju.DrawStepsBorder(myqiju.m_PlayerBottom);

}

void __fastcall TMainForm::FormCreate(TObject *Sender)
{
//        copymyqijutoform();
}
//---------------------------------------------------------------------------



void __fastcall TMainForm::FormActivate(TObject *Sender)
{
       drawiconboard(myqiju.m_ChessBoard);
        
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------


void __fastcall TMainForm::Help1Click(TObject *Sender)
{
        AnsiString StrTitle = Application->Title;
        AnsiString StrContent = "无为工作小组，版权所有";
        ShellAbout(MainForm->Handle,StrTitle.c_str(),StrContent.c_str(),Application->Icon->Handle);        
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::WndProc(TMessage & Message)
{
  switch(Message.Msg)
  {
       case WM_WINDOWPOSCHANGING:
            if((LPWINDOWPOS(Message.LParam))->y<20)
                (LPWINDOWPOS(Message.LParam))->y =0;
            if((LPWINDOWPOS(Message.LParam))->x<40)
                (LPWINDOWPOS(Message.LParam))->x =0;
            break;

       default:
            break;
  }

  TForm::WndProc(Message);
}



void __fastcall TMainForm::Set1Click(TObject *Sender)
{
    for(int i=0; i<this->ControlCount;i++)
    {
        this->Controls[i]->Visible = true;
        imgboard->Left = 8;
        imgboard->Top = 72;
        this->Width = 253;
        this->Height = 377;

    }
     mm1->Items->Checked=true;
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::Alwaysontop1Click(TObject *Sender)
{
        if( !Alwaysontop1->Checked )
        {
//             this->FormStyle = fsStayOnTop;
             SetWindowPos( MainForm->Handle,(HWND)-1 ,0,0,0,0,SWP_NOMOVE |SWP_NOSIZE );
//             SetWindowPos( Handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE||SWP_NOSIZE);
        }
        else
        {
//             this->FormStyle = fsNormal;
//             SetWindowPos( Handle, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE||SWP_NOSIZE);
             SetWindowPos( MainForm->Handle,(HWND)HWND_NOTOPMOST ,0,0,0,0,SWP_NOMOVE |SWP_NOSIZE );
        }
        Alwaysontop1->Checked = !Alwaysontop1->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::actstartExecute(TObject *Sender)
{
        myqiju.m_PlayerBottom.m_iChessMoveFrom =  0 ;//
//         if( !myqiju.m_PlayerTop.m_iChessMoveFrom || !myqiju.m_PlayerBottom.m_iChessMoveFrom )
//         {
//             if(  myqiju.m_PlayerTop.m_hWndBoard == NULL ||  myqiju.m_PlayerBottom.m_hWndBoard == NULL)
//             {
//                  ShowMessage("please reset all Player infomation!");
//                  return;
//             }
//             else
//             {
//                   if(!IsWindow( myqiju.m_PlayerTop.m_hWndBoard )||!IsWindow( myqiju.m_PlayerBottom.m_hWndBoard ))
//                   {
//                       ShowMessage("Not WinBoard!");
//                       return;
//                   }
//             }
//         }

        if( btnstart->Caption == "Start")
        {
            memcpy(myqiju.m_ChessBoard,InitChessBoard,90);
            myqiju.m_iTurn =0;

            if(!myqiju.m_PlayerBottom.m_bQiShouRed)
            {
                 myqiju.m_bBottomGo=false;
                 myqiju.m_WhichPlayer = & myqiju.m_PlayerTop   ;
                 myqiju.ChangeSide(myqiju.m_ChessBoard);
            }
            else
            {
                 myqiju.m_WhichPlayer =  &myqiju.m_PlayerBottom ;
                 myqiju.m_bBottomGo=true;
            }

            btnstart->Caption="Stop";
            btnreset->Caption = "Pause" ;

            myqiju.m_bConnected=true;
            tmr1->Enabled=true;
            tmr2->Enabled=false;

//          only display board ;
//            for(int i=0; i<this->ControlCount;i++)
//            {
//                this->Controls[i]->Visible = false;
//                imgboard->Visible = true;
//                imgboard->Left = 0;
//                imgboard->Top = 0;
//                this->Width = imgboard->Width + 10;
//                this->Height = imgboard->Height + 60;
//
//            }
//
            myqiju.Start();

//            autogo = new CAutoGo(true);
//            autogo->FreeOnTerminate = true;
//            autogo->Resume();


        }
        else
        {
      }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::actsaveExecute(TObject *Sender)
{

       if(myqiju.m_bConnected)
       {
              //ShowMessage("already start! please stop first");
          return;
       }

       if(myqiju.m_bBottomGo)
           save->FileName = grptop->Caption+"  vs  " + grpbottom->Caption;
       else
           save->FileName =grpbottom->Caption +"  vs  " +grptop->Caption ;

       if(!save->Execute())
       {
          return;
       }

      bool timerstatus;
      timerstatus = tmr1->Enabled ;
      tmr1->Enabled = false;

       myqiju.SaveToFile(save->FileName.c_str());

      tmr1->Enabled = timerstatus;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::actopenExecute(TObject *Sender)
{
      if(myqiju.m_bConnected)
      {
          //ShowMessage("already start! please stop first");
          return;
      }

      if(myqiju.m_bBottomGo)
          open->FileName = grptop->Caption+"  vs  " + grpbottom->Caption;
      else
          open->FileName =grpbottom->Caption +"  vs  " +grptop->Caption ;

      if(!open->Execute())
      {
          return;
      }
      myqiju.GetFromFile(open->FileName.c_str())  ;


      //find window board;
       POINT pt1;
       HWND hWndtop = NULL , hWndbottom =NULL ;
       HWND hLastWin =NULL ;

       myqiju.m_PlayerTop.m_hWndProg = FindWindow(NULL, myqiju.m_PlayerTop.m_ptcName);

       if( myqiju.m_PlayerTop.m_hWndProg == FindWindow(NULL, "中国象棋 （单机版） ") )
       {

              hLastWin = FindWindowEx(myqiju.m_PlayerTop.m_hWndProg, NULL, "AfxMDIFrame42", "");
              myqiju.m_PlayerTop.m_hWndBoard = FindWindowEx(hLastWin, NULL, "#32770", "");

              const int MyMaxParentWinCount = 5;
              char *A_szClassName[MyMaxParentWinCount] =
              {
                  "Afx:400000:b:10011:6:4e07bf",
                  "AfxMDIFrame42",
                  "AfxMDIFrame42",
                  "#32770",
                  "ListBox"
              };
              char *A_szWinName[MyMaxParentWinCount] =
              {
                  "中国象棋 （单机版） ",
                  "",
                  "",
                  "",
                  ""
              };
              hLastWin = FindWindow(NULL, A_szWinName[0]);
              for(int i=1; i<MyMaxParentWinCount; i++)
              {
                  hLastWin = FindWindowEx(hLastWin, NULL,
                      A_szClassName[i], A_szWinName[i]);
              }
              myqiju.m_PlayerTop.m_hWndSteps = hLastWin;
        }
        else
        {
             pt1 = myqiju.m_PlayerTop.m_iPointInBoard ;
             ::ClientToScreen(myqiju.m_PlayerTop.m_hWndProg ,&pt1 );
             hWndtop = WindowFromPoint(pt1);
             myqiju.m_PlayerTop.m_hWndBoard =hWndtop ;

             pt1 = myqiju.m_PlayerTop.m_iPointInSteps ;
             ::ClientToScreen(myqiju.m_PlayerTop.m_hWndProg ,&pt1 );
             hWndtop  = WindowFromPoint(pt1);
             myqiju.m_PlayerTop.m_hWndSteps = hWndtop ;
        }


       myqiju.m_PlayerBottom.m_hWndProg = FindWindow(NULL, myqiju.m_PlayerBottom.m_ptcName);
       if( myqiju.m_PlayerBottom.m_hWndProg == FindWindow("TApplication", "楚汉棋缘1.46") )
        {    // inline suport globall

              HWND hChessForm;
              HWND hLastWin ,hLastWin1 ;
              HWND hWndSteps,hWndBoard;

              hChessForm  = FindWindow("TChessForm","楚汉棋缘1.46注册版");
              hLastWin = FindWindowEx(hChessForm, NULL, "TPanel", "");
              while(hLastWin != NULL)
              {
                  hLastWin1 = FindWindowEx(hLastWin, NULL, "TlistBox", "");
                  if(hLastWin1 == NULL )
                  {
                       hLastWin = FindWindowEx(hChessForm, hLastWin, "TPanel", "");
                  }
                  else
                  {
                       hWndSteps = hLastWin1 ;
                       hWndBoard = FindWindowEx(hChessForm, hLastWin, "TPanel", "");
                       break;

                  }
              }

              myqiju.m_PlayerBottom.m_hWndBoard = hWndBoard ;   // hWndBoard
              myqiju.m_PlayerBottom.m_hWndSteps =   hWndSteps ;

        }
        else if(myqiju.m_PlayerBottom.m_hWndProg == FindWindow("TApplication", "楚汉棋缘V1.42" ))
        {
              HWND hChessForm = FindWindow("TChessForm","楚汉棋缘1.42版（未注册）" ) ;
              HWND hLastWin1 = FindWindowEx(hChessForm, NULL, "TPanel", "");
              HWND hLastWin2 = FindWindowEx(hChessForm, hLastWin1, "TPanel", "");
              HWND hLastWin3 = FindWindowEx(hChessForm, NULL, "TComboBox", "");
              HWND hLastWin4 = FindWindowEx(hChessForm, hLastWin3, "TPanel", "");
              HWND hLastWin5 = FindWindowEx(hChessForm, hLastWin4, "TPanel", "");
              HWND hLastWin6 = FindWindowEx(hChessForm, hLastWin5, "TPanel", "");
              HWND hLastWin7 = FindWindowEx(hLastWin4, NULL, "TListBox", "");

              myqiju.m_PlayerBottom.m_hWndBoard = hLastWin5 ;
              myqiju.m_PlayerBottom.m_hWndSteps = hLastWin7;
        }
        else
        {
             pt1 = myqiju.m_PlayerBottom.m_iPointInBoard ;
             ::ClientToScreen(myqiju.m_PlayerBottom.m_hWndProg ,&pt1 );
             hWndbottom = WindowFromPoint(pt1);
             myqiju.m_PlayerBottom.m_hWndBoard =hWndbottom ;

             pt1 = myqiju.m_PlayerBottom.m_iPointInSteps ;
             ::ClientToScreen(myqiju.m_PlayerBottom.m_hWndProg ,&pt1 );
             hWndbottom  = WindowFromPoint(pt1);
             myqiju.m_PlayerBottom.m_hWndSteps = hWndbottom ;

        }

      //refresh the mainform
      copymyqijutoform(); //may cause error

}
//---------------------------------------------------------------------------






void __fastcall TMainForm::actcontisameExecute(TObject *Sender)
{

//         if(myqiju.m_bConnected)
//         {
//              //ShowMessage("already start! please stop first");
//              return;
//         }
//         if( myqiju.m_PlayerTop.m_hWndBoard == NULL ||  myqiju.m_PlayerBottom.m_hWndBoard == NULL)
//         {
//              ShowMessage("please reset all Player infomation!");
//              return;
//         }
//         else
//         {
//               if(!IsWindow( myqiju.m_PlayerTop.m_hWndBoard )||!IsWindow( myqiju.m_PlayerBottom.m_hWndBoard ))
//               {
//                   ShowMessage("Not WinBoard!");
//                   return;
//               }
//         }
//
//        btnstart->Caption="stop";
//        myqiju.m_bConnected=true;
//        tmr1->Enabled=true;
//
        if( Application->MessageBox("from top to bottom？","my dear！",MB_YESNO) == IDYES )
        {
          myqiju.ContiSame(myqiju.m_PlayerBottom, myqiju.m_PlayerTop);
        }
//
//       autogo = new CAutoGo(true);
//       autogo->Resume();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::actcheckExecute(TObject *Sender)
{

//              myqiju.m_PlayerTop.m_ucci.LoadEngine();
//              myqiju.m_PlayerTop.m_ucci.RunEngine(p);
              //while(myqiju.m_PlayerTop.m_ucci.ReceiveUCCI()) ;

//              return;


//        myqiju.PasteJuMian("rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR r - - 3 6");
//       pt1.x= pt1.x +  myqiju.m_PlayerTop.m_iPointInBoard.x -myqiju.m_PlayerTop.m_OXY.x ;
//       pt1.y= pt1.y +  myqiju.m_PlayerTop.m_iPointInBoard.y -myqiju.m_PlayerTop.m_OXY.y ;

//
//       myqiju.m_PlayerTop.m_hWndBoard= myqiju.m_PlayerTop.FindWinBoardAndSteps(myqiju.m_PlayerTop.m_MaxParentWinCountBoard, myqiju.m_PlayerTop.m_ClassNameBoard, myqiju.m_PlayerTop.m_WinNameBoard ) ;
//       myqiju.m_PlayerTop.m_hWndSteps=myqiju.m_PlayerTop.FindWinBoardAndSteps(myqiju.m_PlayerTop.m_MaxParentWinCountSteps, myqiju.m_PlayerTop.m_ClassNameSteps, myqiju.m_PlayerTop.m_WinNameSteps ) ;
//
//       myqiju.m_PlayerBottom.m_hWndBoard=myqiju.m_PlayerBottom.FindWinBoardAndSteps(myqiju.m_PlayerBottom.m_MaxParentWinCountBoard, myqiju.m_PlayerBottom.m_ClassNameBoard, myqiju.m_PlayerBottom.m_WinNameBoard ) ;               //flash window
//       myqiju.m_PlayerBottom.m_hWndSteps= myqiju.m_PlayerBottom.FindWinBoardAndSteps(myqiju.m_PlayerBottom.m_MaxParentWinCountSteps, myqiju.m_PlayerBottom.m_ClassNameSteps, myqiju.m_PlayerBottom.m_WinNameSteps ) ;               //flash window

//                        return;
//          if(myqiju.m_bBottomGo )
//          MoveWindow(myqiju.m_PlayerBottom.m_hWndProg, 0, 0 ,320,500 ,true );
//          else
//          MoveWindow(myqiju.m_PlayerTop.m_hWndProg, 0, 0 ,432,500 ,true );
//

//        HWND hWnd = FindWindow(NULL,myqiju.m_PlayerTop.m_ptcName);
//        SetWindowText(hWnd,"found you! haha!");
//        return;

//         myqiju.m_WhichPlayer->GetBMPBoard();
//       SetWindowPos( MainForm->Handle,(HWND)-1 ,0,0,0,0,SWP_NOMOVE |SWP_NOSIZE );
////       SetActiveWindow( myqiju.m_WhichPlayer->m_hWndProg );
////       ShowWindow( myqiju.m_WhichPlayer->m_hWndProg ,SW_SHOWNORMAL);
//       BringWindowToTop( myqiju.m_WhichPlayer->m_hWndProg );
//       return;

       if(!myqiju.m_bConnected)
       {
             MainForm->drawiconboard(myqiju.m_ChessBoard);
//             if( !myqiju.m_bBottomGo )
             {
                   myqiju.DrawCchessBorder(myqiju.m_PlayerTop);
                   myqiju.DrawStepsBorder(myqiju.m_PlayerTop);
             }
//             else
             {
                   myqiju.DrawCchessBorder(myqiju.m_PlayerBottom);
                   myqiju.DrawStepsBorder(myqiju.m_PlayerBottom);
             }
       }
       else
       {
             if( !myqiju.m_bBottomGo )
             {
                   MainForm->drawiconboard(myqiju.m_PlayerTop.m_position);
             }
             else
             {
                   MainForm->drawiconboard(myqiju.m_PlayerBottom.m_position);
             }
       }
}


void __fastcall TMainForm::mniTopBoardImage1Click(TObject *Sender)
{
        myqiju.m_PlayerTop.GetBMPBoard();
//        if (OpenClipboard(this->Handle))    //m_hWndBoard为程序窗口句柄
//        {
//
//              EmptyClipboard();   //清空剪贴板
//
//
//              SetClipboardData( CF_BITMAP, myqiju.m_PlayerTop.m_hbitmap);     //把屏幕内容粘贴到剪贴板上,  hbitmap 为刚才的屏幕位图句柄
//
//              CloseClipboard();  //关闭剪贴板
//        }
//       imgtemp->Picture->Bitmap->Handle = myqiju.m_PlayerTop.m_hbitmap;

}
//---------------------------------------------------------------------------

void __fastcall TMainForm::mniBottomBoardImage1Click(TObject *Sender)
{
        myqiju.m_PlayerBottom.GetBMPBoard();
//        if (OpenClipboard(this->Handle))    //m_hWndBoard为程序窗口句柄
//        {
//
//              EmptyClipboard();   //清空剪贴板
//
//
//              SetClipboardData( CF_BITMAP, myqiju.m_PlayerBottom.m_hbitmap);     //把屏幕内容粘贴到剪贴板上,  hbitmap 为刚才的屏幕位图句柄
//
//              CloseClipboard();  //关闭剪贴板
//        }
//       imgtemp->Picture->Bitmap->Handle = myqiju.m_PlayerBottom.m_hbitmap;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::mniViewAll1Click(TObject *Sender)
{
    for(int i=0; i<this->ControlCount;i++)
    {
        this->Controls[i]->Visible = true;
        imgboard->Left = 8;
        imgboard->Top = 72;
        this->Width = 253;
        this->Height = 377;

    }
     mm1->Items->Checked=true;
}
//----------------


void __fastcall TMainForm::actpauseExecute(TObject *Sender)
{
      if(!myqiju.m_bConnected)
      {
          return;
      }
      if( btnreset->Caption == "pause")
      {
          btnreset->Caption = "conti" ;
          autogo->Suspend();
          tmr1->Enabled = false;
      }
      else
      {
          btnreset->Caption = "pause" ;
          autogo->Resume();
          tmr1->Enabled = true;
      }        
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::mniDX1Click(TObject *Sender)
{
        edtrdx->SetFocus();        
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::mniDI2Click(TObject *Sender)
{
        edtbdi->SetFocus();        
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::View1Click(TObject *Sender)
{
//    for(int i=0; i<this->ControlCount;i++)
//    {
//        this->Controls[i]->Visible = false;
//    }
//    imgboard->Left = 0;
//    imgboard->Top = 0;
//    this->Width = 10 + imgboard->Width  ;
//    this->Height = imgboard->Height + 60 ;
//    imgboard->Visible = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::onlyboard1Click(TObject *Sender)
{
    for(int i=0; i<this->ControlCount;i++)
    {
        this->Controls[i]->Visible = false;
    }
    imgboard->Left = 0;
    imgboard->Top = 0;
    this->Width = imgboard->Left + 50;
    this->Height = imgboard->Height + 50;
    imgboard->Visible = true;
}
//---------------------------------------------------------------------------




void __fastcall TMainForm::actStopExecute(TObject *Sender)
{
      if(btnstart->Caption=="Start")
      {
          return;
      }
      else
      {
              btnstart->Caption="Start";
              btnreset->Caption = "Pause" ;
              tmr1->Enabled = false;
              tmr2->Enabled = true;
              myqiju.m_bConnected=false;
//              autogo->Suspend();
//              autogo->Terminate();
//              if( autogo!=NULL )
//              {
//                delete autogo;
//              }

      //      display all interface;
              for(int i=0; i<this->ControlCount;i++)
              {
                  this->Controls[i]->Visible = true;
                  imgboard->Left = 8;
                  imgboard->Top = 72;
                  this->Width = 253;
                  this->Height = 377;

              }
      }

}
//---------------------------------------------------------------------------
void __fastcall TMainForm::GetWindowMinMaxInfo(TWMGetMinMaxInfo& Message)
{
        //设置主窗口的最小尺寸
//      MINMAXINFO * MinMaxInfo=Message.MinMaxInfo;
      Message.MinMaxInfo->ptMaxSize.x=this->Width;
      Message.MinMaxInfo->ptMaxSize.y=this->Height;
      Message.MinMaxInfo->ptMaxPosition.x=this->Left;
      Message.MinMaxInfo->ptMaxPosition.y=this->Top;
//      Message.MinMaxInfo->ptMaxTrackSize.x=400;
//      Message.MinMaxInfo->ptMaxTrackSize.y=300;
      Message.Result =0;
      TForm::Dispatch(&Message);
      return;
}

void __fastcall TMainForm::edtbddChange(TObject *Sender)
{
         if(myqiju.m_bConnected)
        {
                //ShowMessage("already start! please stop first");
                return;
        }
        myqiju.m_PlayerTop.m_iDD = atoi(edtbdd->Text.c_str());    

}
//---------------------------------------------------------------------------

void __fastcall TMainForm::edtbdiChange(TObject *Sender)
{
        if(myqiju.m_bConnected)
        {
                //ShowMessage("already start! please stop first");
                return;
        }
        myqiju.m_PlayerTop.m_iDI = atoi(edtbdi->Text.c_str());

}
//---------------------------------------------------------------------------









void __fastcall TMainForm::mniEngine1Click(TObject *Sender)
{
//     myqiju.m_PlayerBottom.m_iChessMoveFrom =  0 ;
//     ShowMessage("jia zai ying qing! ");
}
//---------------------------------------------------------------------------

