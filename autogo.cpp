//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop       

#include "autogo.h"
#include "formlink.h"
#include "prog.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall Unit1::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

__fastcall CAutoGo::CAutoGo(bool CreateSuspended)
        : TThread(CreateSuspended)
{
}
//---------------------------------------------------------------------------
void __fastcall CAutoGo::Execute()
{
        //---- Place thread code here ----
         MainForm->myqiju.Start();
//         Synchronize( beginplay);

}
//---------------------------------------------------------------------------

void __fastcall CAutoGo::beginplay(void)
{
        //TODO: Add your source code here
        MainForm->myqiju.Start();
}
