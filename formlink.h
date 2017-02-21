//---------------------------------------------------------------------------

#ifndef formH
#define formH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include "prog.h"
#include "autogo.h"

#include <ImgList.hpp>
#include <Dialogs.hpp>
#include <Menus.hpp>
#include <ActnList.hpp>

//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
        TButton *btnstart;
        TButton *btnexit;
        TButton *btnconti;
        TTimer *tmr1;
        TButton *btnreset;
        TGroupBox *grptop;
        TEdit *edttopplayer;
        TEdit *edtbdx;
        TEdit *edtbdy;
        TCheckBox *chktop;
        TGroupBox *grpbottom;
        TEdit *edtbottomplayer;
        TEdit *edtrdx;
        TEdit *edtrdy;
        TCheckBox *chkbottom;
        TImageList *il1;
        TImage *imgtemp;
        TButton *btnsave;
        TSaveDialog *save;
        TOpenDialog *open;
        TButton *btnopen;
        TRadioButton *rbbsteps;
        TRadioButton *rbbboard;
        TCheckBox *chkred;
        TImage *imgb;
        TImage *imgr;
        TTimer *tmr2;
        TImage *imgboard;
        TLabel *lblbdxy;
        TLabel *lblrdxy;
        TMainMenu *mm1;
        TMenuItem *File1;
        TMenuItem *Edit1;
        TMenuItem *Open2;
        TMenuItem *View1;
        TMenuItem *Help1;
        TMenuItem *Copy1;
        TMenuItem *Alwaysontop1;
        TActionList *actlst1;
        TAction *actstart;
        TAction *actsave;
        TAction *actopen;
        TMenuItem *mniStart;
        TAction *actpause;
        TAction *actcontisame;
        TAction *actcheck;
        TMenuItem *mniContiSame1;
        TMenuItem *mniCheck1;
        TMenuItem *mniTopBoardImage1;
        TMenuItem *mniBottomBoardImage1;
        TMenuItem *mniPause2;
        TEdit *edtrdi;
        TEdit *edtrdd;
        TEdit *edtbdi;
        TEdit *edtbdd;
        TMenuItem *mniZoom1;
        TAction *actStop;
        TMemo *mmo1;
        TMenuItem *mniEngine1;
        void __fastcall edtbdxChange(TObject *Sender);
        void __fastcall edtbdyChange(TObject *Sender);
        void __fastcall edtrdxChange(TObject *Sender);
        void __fastcall edtrdyChange(TObject *Sender);
        void __fastcall imgbMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall imgbMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
        void __fastcall imgbMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall edtbottomplayerKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall chkbottomClick(TObject *Sender);
        void __fastcall chkredClick(TObject *Sender);
        void __fastcall chktopClick(TObject *Sender);
        void __fastcall btnresetClick(TObject *Sender);
        void __fastcall tmr1Timer(TObject *Sender);
        void __fastcall tmr2Timer(TObject *Sender);
        void __fastcall edtrdiChange(TObject *Sender);
        void __fastcall edtrddChange(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall FormActivate(TObject *Sender);
        void __fastcall Help1Click(TObject *Sender);
        void __fastcall Set1Click(TObject *Sender);
        void __fastcall Alwaysontop1Click(TObject *Sender);
        void __fastcall actstartExecute(TObject *Sender);
        void __fastcall actsaveExecute(TObject *Sender);
        void __fastcall actopenExecute(TObject *Sender);
        void __fastcall actcontisameExecute(TObject *Sender);
        void __fastcall actcheckExecute(TObject *Sender);
        void __fastcall mniTopBoardImage1Click(TObject *Sender);
        void __fastcall mniBottomBoardImage1Click(TObject *Sender);
        void __fastcall mniViewAll1Click(TObject *Sender);
        void __fastcall actpauseExecute(TObject *Sender);
        void __fastcall mniDX1Click(TObject *Sender);
        void __fastcall mniDI2Click(TObject *Sender);
        void __fastcall View1Click(TObject *Sender);
        void __fastcall onlyboard1Click(TObject *Sender);
        void __fastcall actStopExecute(TObject *Sender);
        void __fastcall edtbddChange(TObject *Sender);
        void __fastcall edtbdiChange(TObject *Sender);
        void __fastcall mniEngine1Click(TObject *Sender);

private:	// User declarations

        BEGIN_MESSAGE_MAP

              MESSAGE_HANDLER(WM_GETMINMAXINFO , TWMGetMinMaxInfo, GetWindowMinMaxInfo)

        END_MESSAGE_MAP(TForm)

        void __fastcall GetWindowMinMaxInfo(TWMGetMinMaxInfo & Message);
        void __fastcall TMainForm::WndProc(TMessage & Message)  ;

public:		// User declarations

        __fastcall TMainForm(TComponent* Owner);


        Cnewqiju        myqiju;
        CAutoGo *       autogo;
        bool            bMouseDown;
        HANDLE          hWindow, hOldWindow;
        TRect           ClassRect, OldClassRect;
        POINT           MousePoint, p1, p2;

        HBITMAP copyscreentobitmap(Cqishou & player );
        void    drawiconboard(BYTE position[10][9]);
        void    copymyqijutoform();
        bool    copyimg();           //just for debug :copy screnn partly

};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif

