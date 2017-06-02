#ifndef PIPE_H
#define PIPE_H

#include <windows.h>

const int LINE_INPUT_MAX_CHAR = 1024;

class PipeStruct
{
public:
      void Open(const char *szExecFile = NULL);
      void Close(void) const;
      bool LineInput(char *szLineStr);
      void LineOutput(const char *szLineStr) const;

private:
      HANDLE hInput, hOutput;
      BOOL bConsole;
      int nBytesLeft;
      int nReadEnd;
      char szBuffer[LINE_INPUT_MAX_CHAR];

      void ReadInput(void);
      bool CheckInput(void);
      bool GetBuffer(char *szLineStr);
};
#endif
