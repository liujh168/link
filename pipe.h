#ifndef PIPE_H
#define PIPE_H

    #include <windows.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdio.h>
    #include <conio.h>

    const int LINE_INPUT_MAX_CHAR = 1024;

    class PipeStruct
    {
     public:

          HANDLE hInput, hOutput;
          bool bConsole;
          int nBytesLeft;
          int nReadEnd;
          char szBuffer[LINE_INPUT_MAX_CHAR];

          void Open(const char *szExecFile = NULL);
          void Close(void) const;
          void ReadInput(void);
          bool CheckInput(void);
          bool GetBuffer(char *szLineStr);
          bool LineInput(char *szLineStr);
          void LineOutput(const char *szLineStr) const;
    } ;     // pipe

#endif

