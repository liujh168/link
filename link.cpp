//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
USEFORM("formlink.cpp", MainForm);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
        try
        {
                 HANDLE hMutex=CreateMutex(NULL,true,"_run_flag");//建立互斥信号量
                 if(GetLastError()==ERROR_ALREADY_EXISTS)     //此互斥量已经存在
                 {
                   ShowMessage("CchLink 已经在运行中!");
                   ReleaseMutex(hMutex);    //释放互斥量
                    return 0;     //退出
                 }
                 Application->Initialize();
                 Application->Title = "CchLink 0.01版";
                 Application->CreateForm(__classid(TMainForm), &MainForm);
                 Application->Run();
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
        }
        return 0;
}
//---------------------------------------------------------------------------
