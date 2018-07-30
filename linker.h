/*******************************************************************************
类名称  : CMatLinker
-------------------------------------------------------------------------------
功能    : 根据方案解析棋盘图像，输出fen串。
		   1、利用人工定位黑方（上方）左车位置的方式创立图形连线方案，方案可存取；
		   2、利用方案定位图形客户端棋盘窗口句柄，获取棋盘图像；
		   3、根据实时图像识别并返回fen串；
		   4、在客户端模拟鼠标动作自动走子
		   5、可利用第3方引擎（或兵河界面）进入分析模式，协助人机
--------------------------------------------------------------------------------
典型用法 : 	
		//归一化(相当于图像预处理，统一缩放到一定大小)
		//实时建立建立方案
*******************************************************************************/
#pragma once

#include "tools.h"
#include "..\\jqxq\\types.h"
#include "..\\jqxq\\piece.h"
#include "..\\jqxq\\square.h"

typedef struct {				
	string	titleKeyword;		//棋盘客户端程序窗口标题（或关键字），用于查找客户端程序窗口句柄，兼作原始棋盘图文件名fnInit.jpg
	POINT	ptBRookInProg;		//黑左车在客户端程序窗口中的位置（左上角为（0，0）），用于配合客户端程序窗口句柄查找棋盘窗口句柄（还有一种方法是通过实时建立方案时取得的窗口信息数组来查找棋盘窗口句柄）
	POINT	ptBRookInBoard;		//黑左车在棋盘图中的位置（左上角为（0，0）与ptBoard[0][0]）值相同，用于配合原始棋盘图文件确定棋盘格宽。如果是非标准棋盘，则用ptBoard[10][9]来确定其余各棋子位置
	int		widthInit;			//原始棋子模板分辨率，用于获取原始棋子模板图数据
	int		widthReal;			//原始棋子模板分辨率，用于获取原始棋子模板图数据		可用以下变量替代	//int		delt;			//widthInit-delt=实时棋子分辨率，对于标准棋盘图为0
	double	threshold;			//阈值
	Mat*	pieces;				//原始棋子模板数据
	//以下数据方案中非必需，有则更完美
	POINT	ptBoard[10][9];		//棋子在棋盘图中的位置（用于对付不规则棋盘）。一般不需要，如简单一点在截取棋子图时（deltY=y;deltX=x/2）
	int     maxParents;			//
	vector <string> className;	//图形棋盘窗口及其父窗口类名（向量大小＝图形棋盘窗口及其父窗口计数，因此可省略掉上面参数？）
	vector <string> titleName;	//图形棋盘窗口及其父窗口标题

	void write_solution(void);
	void read_solution(void);
} SOLUTION;						//方案定义

class CMatLinker
{
  public:
        CMatLinker(string config="CycloneGui 优化者：阿♂姚.ini", BYTE width=35, BYTE width_real=30, double f=0.035);
        ~CMatLinker();

		bool			isready(void) { return (((m_flag=="91069") && find_win_prog()&&find_win_board())? true:false); };		//返回方案就绪标志
		bool			make_solution(POINT MousePoint);				//取得窗口信息、截屏、制作并保存方案
        bool            load_solution(string filename);								//找到各窗口及计算其它参数
		bool			make_move(POINT mvFrom, POINT mvTo)const;		//模拟鼠标点击操作自动走子
		void			update_fen(void);								
		const string	mat2fen(string fn, int match_method = 1);		//识别棋盘图文件。CV_TM_SQDIFF_NORMED=1
		Color			local_color(void);								//我方(下方)棋子颜色
		void			ChangeSide(void);								//旋转棋盘180℃
		const string	print(void) const;								//输出小棋盘图信息
		string			uci_pv(std::vector<Move> pv);								//输出中文PV
		Move			board2move(PieceID pre_board[SQUARE_NB]);		//通过比较两个position取得棋步
		int				board2diffs(PieceID pre_board[SQUARE_NB]);		//通过比较两个position取得位置不同的点的总数
		void			set(const std::string& fenStr );				
		const string	fen( ) const ;									
		const string	GetStepName( Move move )const;		
		void			do_move(Move move);

		void			set_threshold(double threshold)		{ m_threshold = threshold; };
		void     		set_width_init(int width_init)		{ m_width_init = width_init; };
		void			set_width_real(int width_real)		{ m_width_real = width_real; };
		double			get_threshold(void)					{ return m_threshold;};
		int     		get_width_init(void)				{ return m_width_init;};
		int				get_width_real(void)				{ return m_width_real;};
		PieceID			piece_on(Square s) const			{ return m_Board[s];};
		bool			empty(Square s) const				{ return piece_on(s)==NOCHESS;};

		PieceID 		m_Board[SQUARE_NB];					//当前局面，其值为兵种类型数据（也可为棋子数据），约定：红下黑上
		Color			sideToMove;							//轮到哪方下棋了？
		string			fenold;
		POINT			from, to;							//走子的位置

private:
		bool			build_solution(POINT pt, string fn);		//从原始棋盘图像创建方案
		bool			build_solution(POINT pt);					//按下热键时, 用于配合鼠标实时位置创建方案	
		bool            write_solution(string filename);			//
        bool            read_solution(string filename);				//
		const string	mat2fen(int match_method = 1);				//实时识别截图

		bool			pieces_all(int type =32);					//从原始棋盘盘图中分割出32个兵种图像。
		bool			calc_DxDy(void);							//寻找右黑车，计算m_dx
		bool			normalize(char* fn, cv::Point pt, int width = 40, int delt=6, double fLookRook=0.9);
		bool			normalize(char* fn);						//归一化(相当于图像预处理，统一缩放到一定大小)
		void			get_mat_real();								//截屏客户端棋盘窗口
        HWND            find_win_prog(void) ;						//寻找程序窗口
        HWND            find_win_board(void) ;						//寻找程序棋盘窗口
		cv::Mat&		fen2mat(string strfen, Mat& dst);			//把fen串转换输出相应的Mat
		void			LeftToRight(void);							//左右变换

		//以下私有数据部分：
		//实时变化的数据
		cv::Mat			m_imgReal;						//待识别的实时棋盘图像
        
		//配置文件标志兼方案就绪标志 if(m_flag=="91069")标志方案准备好
		string          m_flag;							

		//图形客户端方案中至少包含以下内容（见SOLUTION的定义）
        string          m_titleKeyword;					//窗口标题关键字
		POINT           m_ptBRookInBoard;               //黑车相对于客户端程序中的棋盘窗口位置，相当于棋盘原点，to m_hWndBoard client
		POINT			m_ptBRookInProg;				//黑车相对于客户端程序窗口位置，相当于棋盘原点，to m_hWndProg client
        int				m_width_init;					//分辨率。原始样本棋子宽=高  用于建立方案时
		int				m_width_real;					//分辨率。实时棋子宽=高  用于识别时提取实时棋子  m_width_init>=m_width_real
		double			m_threshold;					//相似度阈值
		//POINT			ptBoard[10][9];					//棋子在棋盘图中的位置（用于对付不规则棋盘）。一般不需要，如简单一点在截取棋子图时（deltY=y;deltX=x/2）
		//以下内容也可以保存作为另一种寻找棋盘窗口的办法
        int             m_maxParents;					//图形棋盘窗口其父窗口计数
        char            m_className[10][254];			//图形棋盘窗口其父窗口类名
        char            m_titleName[10][254];			//图形棋盘窗口其父窗口标题
		//SOLUTION        m_solution;					//以上是当前方案，本结构可打一包较方便，还可完成write_solution 和 read_solution功能。
		
		//以下数据可从方案中得出
		HWND            m_hWndProg;                     //当前客户端程序窗口句柄（顶层）
        HWND            m_hWndBoard;                    //当前客户端棋盘窗口句柄（棋盘区域）
        long            m_dx;							//棋盘格宽，一般情况下均匀相间，且与格高相等，可算出来
        long            m_dy;							//棋盘格高，一般情况下均匀相间，且与格宽相等，特殊情况下要考虑河界的差异
		cv::Mat			m_imgInit;						//初始棋盘图像, 方案中可以仅保存棋子模板数据（可减少文件大小），但调试时没保存棋盘方便。
		cv::Mat			m_imgPieceAll;					//初始棋盘得来的各兵种图像合并
};

extern CMatLinker link;