#include "pipe.h"
#include "prog.h"

#ifndef UCCI_H
#define UCCI_H


const int MAX_IRREV_POS = 33;   // 不可逆局面的最大个数，整个棋局吃子数不会超过32个
const int MAX_IRREV_MOVE = 160; // 不可逆局面的最大着法数，不吃子着法必须限制在80回合以内
const int MAX_BAN_MOVE = 128;   // 最多的禁止着法数
const int MAX_INFO = 16;        // 版本信息的最大行数
const int MAX_OPTION = 16;      // 选项设置的最大行数
const int MAX_LEVEL = 16;       // 难度的最高级别数

/* 以下常量代表适配器的思考状态，后台思考的处理是一大难点：
 * (1) 不启用后台思考时，空闲状态是"IDLE_NONE"，正常思考状态是"BUSY_THINK"，提示思考状态是"BUSY_HINTS"；
 * (2) 启用后台思考时，正常思考结束后，就进入后台思考状态(BUSY_PONDER)，而"mvPonder"则是猜测着法；
 * (3) "BUSY_PONDER"状态下，如果对手给出的着法没有让后台思考命中，则后台思考中断；
 * (4) "BUSY_PONDER"状态下，如果对手给出的着法让后台思考命中(和"mvPonder"一致)，就进入后台思考命中状态(BUSY_PONDERHIT)；
 * (5) "BUSY_PONDER"状态下，如果后台思考结束(在对手给出着法之前)，则进入后台思考完成状态(IDLE_PONDER_FINISHED)，而"mvPonderFinished"则保存后台思考的结果；
 * (6) "BUSY_PONDERHIT"状态下，如果收到思考指令，就转入正常思考状态(BUSY_THINK)；
 * (7) "BUSY_PONDERHIT"状态下，如果后台思考结束(在对手给出着法之前)，就转入后台思考完成并且命中状态(IDLE_PONDERHIT_FINISHED)，而"mvPonderFinished"则保存后台思考的结果；
 * (8) "IDLE_PONDER_FINISHED"状态下，如果对手给出的着法没有让后台思考命中，则程序重新开始思考。
 * (9) "IDLE_PONDER_FINISHED"状态下，如果对手给出的着法让后台思考命中，就转入后台思考完成并且命中状态(IDLE_PONDERHIT_FINISHED)；
 * (10) "IDLE_PONDERHIT_FINISHED"状态下，如果收到思考指令，就立即给出"mvPonderFinished"着法；
 */
const int IDLE_NONE = 0;
const int IDLE_PONDER_FINISHED = 1;
const int IDLE_PONDERHIT_FINISHED = 2;
const int BUSY_WAIT = 3;
const int BUSY_THINK = 4;
const int BUSY_HINTS = 5;
const int BUSY_PONDER = 6;
const int BUSY_PONDERHIT = 7;

class   CUcci
{
 public:
      PipeStruct pipeEngine;                        // UCCI引擎管道，参阅"pipe.cpp"

      // 适配器状态选项
      bool bDebug, bUcciOkay, bBgThink;             // 是否调试模式，UCCI引擎是否启动，后台思考是否启用
      int nLevel, nStatus;                          // 级别和状态
      char * mvPonder;
      char * mvPonderFinished;        // 后台思考的猜测着法和后台思考完成的着法
      // UCCI引擎配置信息
      char szUCCICommand[1024];                               // UCCI 命令串
      char szUCCIReplyLine[1024];                               // UCCI 返回串
      char szEngineName[1024], szEngineFile[1024];            // UCCI引擎名称和UCCI引擎程序文件的全路径
      char szInfoStrings[16][256], szOptionStrings[16][1024]; // 版本信息和选项设置
      char szLevelStrings[16][256], szThinkModes[16][256];    // 难度级别和各个难度级别下的思考模式

      char szLineStr[LINE_INPUT_MAX_CHAR];

      bool LoadEngine(void);      // 加载ＵＣＣＩ引擎
      bool UnLoadEngine(void);      // 去载ＵＣＣＩ引擎
      void UCCI2Engine( const char * eCommand );   // 给UCCI引擎发送各种指令
      bool ReceiveUCCI(void);      // UCCI反馈信息的接收过程

      char * RunEngine(char * FenStr);   // 给UCCI引擎发送思考指令
      void StopEngine(void);

      CUcci();
      ~CUcci();

} ;



#endif

