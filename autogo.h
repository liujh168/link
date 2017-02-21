//---------------------------------------------------------------------------

#ifndef autogoH
#define autogoH
//---------------------------------------------------------------------------
#include <Classes.hpp>
//---------------------------------------------------------------------------
class CAutoGo : public TThread
{
private:
protected:
        void __fastcall Execute();
public:
        __fastcall CAutoGo(bool CreateSuspended);
        void __fastcall CAutoGo::beginplay(void);
};
//---------------------------------------------------------------------------
#endif
