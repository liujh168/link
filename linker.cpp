
#include "linker.h"
#include "ucci.h"
#include "lock.h"
#include "hotkey.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <assert.h>9/9/9/9/9/9/9/1c2c4/9/rnbakabnr r  - - 0 1

#include "opencv2/imgproc/imgproc.hpp"

#pragma warning( disable:4996 )				//屏蔽函数安全性警告
#pragma warning( disable:4101 )				//屏蔽未使用变量警告

	cv::Mat imgPieceAll[32];		//初始棋盘得来的各棋子图像，编号为0至31. 与m_imgInit为浅拷贝
	cv::Mat imgPieceType[2][7];		//初始棋盘得来的各兵种图像

//CMatLinker link("天天象棋评测.lll", 45, 40, 0.035);
//CMatLinker link("逍遥模拟器.lll", 42, 38, 0.035);
//CMatLinker link("CycloneGui 优化者：阿♂姚.lll", 35, 30, 0.039);	//后面的优先，要修改构造函数
CMatLinker link("link.lll", 35, 30, 0.039);						//后面的优先，要修改构造函数
//CMatLinker link("BHGUI(test) - 新棋局.lll");						//后面的优先，要修改构造函数

static const  char ChessName[2][8][4]={   //（外部有定义）  
	{"﹢","兵","相","仕","炮","马","车","帅"},
	{"﹢","卒","象","士","包","马","车","将"}
};

	//所有棋子数组，方便遍历所有棋子用，在引擎piece.cpp里面有定义（外部有定义）
	uint8_t PieceAll[32]={ 
		R_PAWN1,R_PAWN2,R_PAWN3,R_PAWN4,R_PAWN5,R_ELEPHANT1,R_ELEPHANT2,R_BISHOP1,R_BISHOP2,R_CANNON1,R_CANNON2,R_HORSE1,R_HORSE2,R_ROOK1,R_ROOK2,R_KING1,		//红方 0~15
		B_PAWN1,B_PAWN2,B_PAWN3,B_PAWN4,B_PAWN5,B_ELEPHANT1,B_ELEPHANT2,B_BISHOP1,B_BISHOP2,B_CANNON1,B_CANNON2,B_HORSE1,B_HORSE2,B_ROOK1,B_ROOK2,B_KING1		//黑方 16~31
	}; 

	string PieceName[32]={ 
		"R_PAWN1","R_PAWN2","R_PAWN3","R_PAWN4","R_PAWN5","R_ELEPHANT1","R_ELEPHANT2","R_BISHOP1","R_BISHOP2","R_CANNON1","R_CANNON2","R_HORSE1","R_HORSE2","R_ROOK1","R_ROOK2","R_KING1",		//红方 0~15
		"B_PAWN1","B_PAWN2","B_PAWN3","B_PAWN4","B_PAWN5","B_ELEPHANT1","B_ELEPHANT2","B_BISHOP1","B_BISHOP2","B_CANNON1","B_CANNON2","B_HORSE1","B_HORSE2","B_ROOK1","B_ROOK2","B_KING1"		//黑方 16~31
	}; 

const int32_t    SquareFrom90[90]=  //（外部有定义）  
{
	A9,    B9,    C9,    D9,    E9,    F9,    G9,    H9,    I9,
	A8,    B8,    C8,    D8,    E8,    F8,    G8,    H8,    I8,
	A7,    B7,    C7,    D7,    E7,    F7,    G7,    H7,    I7,
	A6,    B6,    C6,    D6,    E6,    F6,    G6,    H6,    I6,
	A5,    B5,    C5,    D5,    E5,    F5,    G5,    H5,    I5,
	A4,    B4,    C4,    D4,    E4,    F4,    G4,    H4,    I4,
	A3,    B3,    C3,    D3,    E3,    F3,    G3,    H3,    I3,
	A2,    B2,    C2,    D2,    E2,    F2,    G2,    H2,    I2,
	A1,    B1,    C1,    D1,    E1,    F1,    G1,    H1,    I1,
	A0,    B0,    C0,    D0,    E0,    F0,    G0,    H0,    I0
};

using namespace cv;
using namespace std;

typedef struct {
	POINT pt;
	int width;
	int delt;
	double threshold;
	int dx;
	cv::Size sz;
} NORMALIZE;
static NORMALIZE m_normalize;					//为了对付小棋盘的，把放大再识别可能效果会好些

CMatLinker::CMatLinker(string config, BYTE width, BYTE width_real, double f)
{
	//读取方案。方案优先！
	if(read_solution(config))
	{
		cout << "build sulution " << (build_solution(m_ptBRookInBoard, m_titleKeyword+".jpg")? "OK":"NO") << endl;
		cout << "HWND WinProg " << (find_win_prog()?"Yes":"No") << endl; 
		cout << "HWND WinBoard " << (find_win_board()?"Yes":"No") << endl; 
	}
	else
	{
		set("rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w");			//原始局面
		m_titleKeyword = "";							
		m_width_init = width;
		m_width_real = width_real;
		m_threshold = f;
		m_dx = m_dy = -1;					//强制计算dx,dy；如果从方案中读取到合格的dx，dy就不用算了。
		m_flag = "";
		m_hWndBoard = NULL;
		m_hWndProg = NULL;
	}
	//其它参数初始化
	fenold="";

#ifdef _DEBUG
	//cout<< print()<<endl;
#endif
}

CMatLinker::~CMatLinker()
{
}

HWND CMatLinker::find_win_prog(void)
{
	//assert( m_maxParents >= 0 );
	char bufferclass[1024];
	char buffertitle[1024];
	HWND   parent = NULL, brother = NULL;

	//strcpy(buffertitle,"CycloneGui 优化者：阿♂姚");
	strcpy(buffertitle,"逍遥模拟器");

	//     一、首先根据关键字求得顶级父窗口
	m_hWndProg = FindWindow(NULL, m_titleKeyword.c_str()) ;		//用棋手名找，找到了更好
	if(m_hWndProg == NULL)	//没找到再用关键字找。这个比较麻烦，要枚举窗口，找到标题并比较
	{     
		parent= GetDesktopWindow();
		brother = GetWindow(parent,GW_CHILD);   //获得桌面第一个子窗口
		while (brother != NULL)
		{
			GetClassName(brother, bufferclass, 256);		//获得窗口类名
			GetWindowText(brother, buffertitle, 256);		//获得窗口标题
			if(strstr(buffertitle, m_titleKeyword.c_str()) != NULL)		//还可以增加比较类名是否相同
			{
				m_hWndProg = brother;
				break;
			}
			brother = GetWindow(brother,GW_HWNDNEXT); //继续下一个子窗口
		}
	}

	//找到了就闪一下，调试用
	FlashWindow(m_hWndProg,true);
	if(IsIconic(m_hWndProg))
	{
		ShowWindow(m_hWndProg,SW_SHOWNORMAL);
	}

	return m_hWndProg;
}

HWND CMatLinker::find_win_board(void)    //根据关键字和窗口信息数组找程序窗口和棋盘窗口
{
	//assert( m_maxParents >= 0 );

	HWND   WndChild=NULL;
	char bufferclass[1024];
	char buffertitle[1024];

	strcpy(buffertitle,"逍遥模拟器");
	//strcpy(buffertitle,"CycloneGui 优化者：阿♂姚");

	//     一、首先根据关键字求得顶级父窗口
	if(NULL == m_hWndProg)
	{
		m_hWndProg = find_win_prog();
		if(NULL == m_hWndProg)
		{
			return NULL;
		}
	}

	//      二、其次根据数组信息找到子窗口
	//&m_className[m_maxParents][0] //这个是顶层窗口类名。
	WndChild = m_hWndProg ;
	for(int i=m_maxParents-1; i>=0; i--)
	{
		WndChild = FindWindowEx(WndChild, NULL, &m_className[i][0], &m_titleName[i][0]);
		if(WndChild!=NULL)
		{
			m_hWndBoard=WndChild;
		}
	}

	if(IsIconic(m_hWndProg))
	{
		ShowWindow(m_hWndProg,SW_SHOWNORMAL);
	}

	if(NULL == m_hWndBoard)  //没找到继续找，这次用鼠标点位置查找，但可能不准确！
	{
		BringWindowToTop( m_hWndProg );
		::ClientToScreen(m_hWndProg, &m_ptBRookInProg); //转换后m_ptBRookInProg to Screen 保存了鼠标指向黑车的位置，相对屏幕而言的坐标
		m_hWndBoard = WindowFromPoint(m_ptBRookInProg);
	}

	return m_hWndBoard;
}

bool CMatLinker::load_solution(string filename)
{
	bool ok1=false, ok2=false, ok3=false, ok4=false;

	//ok1 = read_solution(m_titleKeyword+".lll");		//可变文件名
	ok1 = read_solution(filename);		//可变文件名
	ok2 = (find_win_prog()!=NULL);
	ok3 = (find_win_board()!=NULL);

	//读入图像数据
	ok4 = build_solution(m_ptBRookInBoard, m_titleKeyword+".jpg");

	if(ok1 && ok2 && ok3 && ok4)
    {
		mat2fen(m_titleKeyword+".jpg");
    }

	return ok1 && ok2 && ok3 && ok4;
}

//关于方案的保存，必要信息：
//1、窗口标题（或者关键字）及左车相对于程序与棋盘窗口的位置，能够找到程序窗口及棋盘窗口
//2、有32个棋子的数据
//3、可以考虑合并成一个图像文件保存（数据保存在图像中）
bool CMatLinker::write_solution(string filename)
{
	if(filename.empty()) filename = m_titleKeyword;

	std::ofstream oFile(filename);
	if (!oFile.is_open()){
		return false;
	}
	
	oFile << "91069" << std::endl;		//写入文件标志  
	
	//以下一个方案，最好放在一行里面，以后一行代表一个方案？
	oFile << fen() << std::endl;
	oFile << m_titleKeyword << std::endl;
	oFile << m_ptBRookInProg.x << std::endl;
	oFile << m_ptBRookInProg.y << std::endl;
	oFile << m_ptBRookInBoard.x << std::endl;
	oFile << m_ptBRookInBoard.y << std::endl;
	oFile << m_width_init << std::endl;
	oFile << m_width_real << std::endl;
	oFile << m_threshold << std::endl;
	oFile << m_dx << std::endl;					//这个可以算出，非必要
	oFile << m_dy << std::endl;					//这个可以算出，非必要
	
	//以下数据暂未用到
	oFile << m_maxParents << std::endl;
	oFile.write((char*)&m_className, sizeof(m_className));
	oFile.write((char*)&m_titleName, sizeof(m_titleName));

	oFile.close();

	//保存图像数据
	if(m_imgInit.data!=NULL)
	{
		imwrite(m_titleKeyword+".jpg", m_imgInit);
	}
	return true;
}

//用C++文件流函数重写 2017.5.20. 
//与savesolution相对应，一个是写，一个是读
//本函数读取数据后，还需要做后续处理（用loadsolution函数）
bool CMatLinker::read_solution(string filename)
{
	if(filename.empty()) filename = m_titleKeyword+".lll";
	
	//判断文件存在否
	//if( (_access( filename, 0 )) != -1 ) 
	std::ifstream iFile(filename);
	if (!iFile.is_open())	return false;
	
	getline(iFile, m_flag); if(m_flag != "91069") return false;
	string fen;		getline(iFile, fen); set(fen);
	getline(iFile, m_titleKeyword);
	iFile >> m_ptBRookInProg.x;
	iFile >> m_ptBRookInProg.y;
	iFile >> m_ptBRookInBoard.x;
	iFile >> m_ptBRookInBoard.y;
	iFile >> m_width_init;
	iFile >> m_width_real;
	iFile >> m_threshold;
	iFile >> m_dx;
	iFile >> m_dy;

	//以下数据暂未用到（读入窗口信息数组）
	iFile >> m_maxParents;
	iFile.read((char*)&m_className, sizeof(m_className));
	iFile.read((char*)&m_titleName, sizeof(m_titleName));

	iFile.close();

	return true;
}

//找到另一个车，计算出DX，并保存兵种图（格宽也可手工给定）
//假定dy=dx，（对不相等的情况未做特殊处理）
bool CMatLinker::calc_DxDy( void)
{
#define MATCH_METHOD 1;
	int match_method = MATCH_METHOD;

	cv::Point pt;
	Mat rRook;
	Mat lRook;
	bool bFound=false;
	double r,max=0,min=0xffffffff;
	int iLRook=0;
	lRook = m_imgInit(Rect(m_ptBRookInBoard.x-m_width_init/2, m_ptBRookInBoard.y-m_width_init/2, m_width_init, m_width_init));

	//找到最大相似度的子图
	assert(m_ptBRookInBoard.x + 8*m_width_init <  m_imgInit.cols - m_width_init);	//这里对第一次建立方案时，如果m_width_init过大就会出问题，应该检查一下
	for(int x = m_ptBRookInBoard.x + 8*m_width_init; x < m_imgInit.cols - m_width_init ; x++)		//从至少第 8个子的x开始，可节省计算量。
	{
		rRook = m_imgInit(Rect(x - m_width_init/2, m_ptBRookInBoard.y-m_width_init/2, m_width_init, m_width_init));
		double r = match(rRook, lRook, pt, match_method); 
		Mat imgFound = rRook( Rect(pt.x, pt.y, lRook.rows,lRook.rows));

		////////////////////////////////////////////////////////////////////

		if( match_method  == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED ) 
		{
			if(r<min)
			{
				min=r;
				iLRook=x;
			}
		}
		else
		{
			if(r>max)
			{
				max=r;
				iLRook=x;
			}
		}
	}

	// 对于方法 SQDIFF 和 SQDIFF_NORMED, 越小的数值代表更高的匹配结果. 而对于其他方法, 数值越大匹配越好
	if( match_method  == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED ) 
	{ 
		//最大相似度超过阈值，认定为右车，据此计算dx,dy并保存兵种图
		if( max < m_threshold )  
		{
			m_dx=(iLRook-m_ptBRookInBoard.x)/8;
			m_dy=m_dx;
			bFound = true;
		}
	}
	else
	{ 
		//最大相似度超过阈值，认定为右车，据此计算dx,dy并保存兵种图
		if( min > m_threshold )  
		{
			m_dx=(iLRook-m_ptBRookInBoard.x)/8;
			m_dy=m_dx;
			bFound = true;
		}
	}

	return bFound;
}

bool CMatLinker::build_solution(POINT MousePoint, string fn)
{
	m_imgInit = imread(fn, CV_LOAD_IMAGE_ANYDEPTH|CV_LOAD_IMAGE_ANYCOLOR);		//图像来自文件
	if(!m_imgInit.data)
	{
		printf("read file error while buit_solution filename: %s\n\n", fn);
		return false;
	}

	assert(m_dx!=-1&&m_dy!=-1);

	pieces_all();				//取得各棋子兵种图

	return true;
}

bool CMatLinker::build_solution(POINT pt)
{
	assert(m_imgInit.data!=NULL);
	
	m_ptBRookInBoard.x = pt.x;  
	m_ptBRookInBoard.y = pt.y;

	bool bFound = calc_DxDy( );		//找到对称的黑车，算出m_dx，仅建立方案时需调用一次，识别时不再调用

	pieces_all();					//取得各棋子兵种图

	m_flag = bFound?"91069":"";

	return bFound;
}

//通过调用mat2fen()利用方案识别棋盘图形文件fn
const string CMatLinker::mat2fen(string fn, int match_method)
{
	m_imgReal = imread(fn, CV_LOAD_IMAGE_ANYDEPTH|CV_LOAD_IMAGE_ANYCOLOR);
	return m_imgReal.data? mat2fen(match_method) : "";
}

//利用方案识别棋盘数据结构mat
//1、更新position
//2、取得position对应的fen串，拷贝到系统剪贴板并返回
const string CMatLinker::mat2fen(int match_method)
{
	if(m_imgReal.data == NULL) return "";

	char buffer[2054]="";
	Mat img_being_checked;
	Mat imgFound, imgFoundOther;
	cv::Point pt;
	double hist_comp_result_1,hist_comp_result_2;

	ZeroMemory(m_Board, sizeof(m_Board));
	for(int y=0; y<10 ; y++)
	{
		for(int x=0; x<9 ; x++)
		{
			int deltY = 0;//y;						//对付棋盘不标准的家伙
			int deltX = 0;//x/2;					//对付棋盘不标准的家伙
			img_being_checked = m_imgReal( Rect(m_ptBRookInBoard.x+x*m_dx -m_width_real/2 + deltX, m_ptBRookInBoard.y+y*m_dy-m_width_real/2 + deltY, m_width_real, m_width_real) );		//实时棋子图

			double match_result = match(m_imgPieceAll, img_being_checked, pt, match_method); 
			imgFound = m_imgPieceAll( Rect(pt.x, pt.y, img_being_checked.rows,img_being_checked.rows));			//取出匹配的棋子图
			
			// 对于方法 SQDIFF 和 SQDIFF_NORMED, 越小的数值代表更高的匹配结果. 而对于其他方法, 数值越大匹配越好
			if( match_method  == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED ) 
			{ 
				PieceID ChessID = m_Board[SquareFrom90[y*9+x]] = (match_result < m_threshold)? (PieceAll[pt.x/m_width_init]) : 0;  //小于阀值，认为找到了。可能会有重复，但不影响输出fen串。
				
				if(match_result < m_threshold) //形状内容相似，进一步区分颜色
				{
#ifdef _DEBUG
					sprintf(buffer,"temp\\M (%d,%d)%.4f.jpg", x, y, match_result);
					imwrite(buffer, mergeCols(img_being_checked, imgFound));	//横向合并
#endif
					PieceType chesspt = GetType(ChessID);
					Color color = GetColor(ChessID);
					imgFoundOther =  imgPieceType[1-color][chesspt-1];

					hist_comp_result_1 = hist_comp(img_being_checked, imgFound);		
					hist_comp_result_2= hist_comp(img_being_checked, imgFoundOther);
					
					if(hist_comp_result_1 < hist_comp_result_2)		
					{
						m_Board[SquareFrom90[y*9+x]] =  PieceAll[(pt.x/m_width_init)>15? pt.x/m_width_init-16:pt.x/m_width_init+16];
#ifdef _DEBUG
						sprintf(buffer,"temp\\M (%d,%d)%.4f(%.2f %.2f)_.jpg", x, y, match_result, hist_comp_result_1, hist_comp_result_2);
						imwrite(buffer, imgFoundOther);

						sprintf(buffer,"temp\\M3 (%d,%d)%.4f(%.2f %.2f)_.jpg", x, y, match_result, hist_comp_result_1, hist_comp_result_2);
						Mat img3=mergeCols(img_being_checked, imgFound);
						img3=mergeCols(img3, imgFoundOther);
						//imshow("合并3个子",img3);
						//waitKey(0);
						imwrite(buffer, img3);
#endif
					}
				}
#ifdef _DEBUG	
				else		//未找到
				{
					sprintf(buffer,"temp\\No X%dY%dF%fH%f.jpg",x,y,match_result,hist_comp_result_1);
					imwrite(buffer, mergeCols(img_being_checked, imgFound));							//横向合并
				}
#endif
			}
			else
			{ 
				m_Board[SquareFrom90[y*9+x]] = (match_result > m_threshold)? (PieceAll[pt.x/m_width_init]) : 0;  //大于阀值，认为找到了.  没做fg进一步的判断颜色od判断
			}
		}
	}

	static string fen = this->fen();	
	StrToClip((char*)fen.c_str());	//把串拷贝到系统剪贴板上

	return fen;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
//根据棋盘初始图像与左车位置等参数，分解出14个兵种图像（缺省高宽见头文件定义）
//注意：未真实拷贝数据，需要保持m_imgInit数据不变！
bool CMatLinker::pieces_all(int type)
{
	const uint8_t PieceNo[256] =      //（外部有定义）  
	{
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   2,   3,   4,   0,   0,
		0,   5,   6,   0,   0,   0,   0,   0,   0,   7,   8,   0,   0,   0,   0,   0,
		0,   9,  10,   0,   0,   0,   0,   0,   0,  11,  12,   0,   0,   0,   0,   0,
		0,  13,  14,   0,   0,   0,   0,   0,   0,  15,   0,   0,   0,   0,   0,   0,

		0,   0,   0,   0,   0,   0,   0,   0,   0,  16,  17,  18,  19,  20,   0,   0,
		0,  21,  22,   0,   0,   0,   0,   0,   0,  23,  24,   0,   0,   0,   0,   0,
		0,  25,  26,   0,   0,   0,   0,   0,   0,  27,  28,   0,   0,   0,   0,   0,
		0,  29,  30,   0,   0,   0,   0,   0,   0,  31,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
	};   //已知PIECEID查表求得棋子序号0～31，可用来索引数组,  应该把其它0改成FF更准确。

	assert(m_imgInit.rows > m_width_init*5);
	
	int totals=0;
	char buffer[2054];
	string fn;

	for(int y=0; y<10 ; y++)		
	{
		for(int x=0; x< 9 ; x++) 
		{
			//if(int ChessID = ChessBoard[y][x])								//不同的制作方案采用不同的初始化值
			if(int ChessID = m_Board[SquareFrom90[y*9+x]])						//不同的制作方案采用不同的初始化值
			{	
				totals++;

				PieceType pt = GetType(ChessID);
				Color color = GetColor(ChessID);

				int deltY = 0;//y;		//对付棋盘不标准的家伙
				int deltX = 0;//x/2;	//对付棋盘不标准的家伙
				imgPieceAll[PieceNo[ChessID]] = m_imgInit( Rect(m_ptBRookInBoard.x + x*m_dx - m_width_init/2 + deltX, m_ptBRookInBoard.y + y*m_dy - m_width_init/2 + deltY , m_width_init, m_width_init) );
				imgPieceType[color][pt-1] = imgPieceAll[PieceNo[ChessID]];
#ifdef _DEBUG
				sprintf(buffer,"temp\\样本%s%s%d.jpg", color==0?"红":"黑", ChessName[color][pt], PieceNo[ChessID]);			//最好用sprintf_s;   写入样本棋子图
				imwrite(buffer, imgPieceAll[PieceNo[ChessID]]);
#endif
			}
		}
	}

	//补充棋子图
	for(int i=0; i<32; i++)
	{
		if(imgPieceAll[i].data == NULL)
		{
			PieceID pc = PieceAll[i];
			assert(PieceNo[pc]==i);

			PieceType pt = GetType(PieceAll[i]);
			Color color = GetColor(PieceAll[i]);
			if(imgPieceType[color][pt-1].data!=NULL)
			{
				imgPieceAll[i] = imgPieceType[color][pt-1];
			}
			else
			{
				cout << "兵种数据不全" <<  ChessName[color][pt] << endl;
			}
		}
	}

#ifdef _DEBUG
	//合并各棋子图（横）
	int skip;
	Mat temp;
	for(int x=0; x< 32 ; x++)	
	{
		if(imgPieceAll[x].data!=NULL)
		{		
			skip=x;
			m_imgPieceAll = imgPieceAll[x];	
			temp = imgPieceAll[x];	
			break;
		}
	}
	for(int x=0; x< 32 ; x++)	
	{
		if((imgPieceAll[x].data!=NULL) && x!=skip)
		{
			m_imgPieceAll = mergeCols(m_imgPieceAll, imgPieceAll[x]);		//横
		}
	}
	sprintf(buffer,"PieceAll%d_1 .jpg",totals);		//最好用sprintf_s;
	imwrite(buffer, m_imgPieceAll);					//横

	//合并各棋子图（竖）
	for(int x=0; x< 32 ; x++)	
	{
		if(imgPieceAll[x].data!=NULL && x!=skip)
			temp = mergeRows(temp, imgPieceAll[x]);
	}

	sprintf(buffer,"PieceAll%d_2 .jpg",totals);		
	//imwrite(buffer, temp);					//竖
#endif

	return true;
}

const string CMatLinker::print(void)  const
{
	std::ostringstream ss;
	unsigned char txtboard[22][40]=
	{
		"     ----小棋盘打印 ver 1.00!------- \n",
		"9 　┌--┬--┬--┬--┬--┬--┬--┬--┐ ",
		". 　│　│　│　│＼│／│　│　│　│ ",
		"8 　├--┼--┼--┼--※--┼--┼--┼--┤ ",
		". 　│　│　│　│／│＼│　│　│　│ ",
		"7 　├--┼--┼--┼--┼--┼--┼--┼--┤ ",
		". 　│　│　│　│　│　│　│　│　│ ",
		"6 　├--┼--┼--┼--┼--┼--┼--┼--┤ ",
		". 　│　│　│　│　│　│　│　│　│ ",
		"5 　├--┴--┴--┴--┴--┴--┴--┴--┤ ",
		". 　│　　　楚　河　　汉　界　　　　│ ",
		"4 　├--┬--┬--┬--┬--┬--┬--┬--┤ ",
		". 　│　│　│　│　│　│　│　│　│ ",
		"3 　├--┼--┼--┼--┼--┼--┼--┼--┤ ",
		". 　│　│　│　│　│　│　│　│　│ ",
		"2 　├--┼--┼--┼--┼--┼--┼--┼--┤ ",
		". 　│　│　│　│＼│／│　│　│　│ ",
		"1 　├--┼--┼--┼--※--┼--┼--┼--┤ ",
		". 　│　│　│　│／│＼│　│　│　│ ",
		"0 　└--┴--┴--┴--┴--┴--┴--┴--┘ ",
		". 　ａ　ｂ　ｃ　ｄ　ｅ　ｆ　ｇ　ｈ　ｉ ",
		"                                       "
	};

	for(int i=0; i<10;  i ++ )
	{
		for(int j=0; j<9; j ++ )
		{
			unsigned char ChessID = m_Board[ (0x30+ (i<<4))|(0x03+j)];
			unsigned char BufferB[6]={"[将]"};
			if(IsRed(ChessID) )
			{
				BufferB[0] ='(';	BufferB[1] = ChessName[REDCHESS][GetType(ChessID)][0];
				BufferB[3] =')';	BufferB[2] = ChessName[REDCHESS][GetType(ChessID)][1];
				for(int len = 0 ;len < 4; len++)
				{
					txtboard[2*i+1][j*4+3+len]=BufferB[len];
				}
			}
			else if (IsBlack(ChessID) )
			{
				BufferB[0] =0x5b;	BufferB[1] = ChessName[BLACKCHESS][GetType(ChessID)][0] ;
				BufferB[3] =']';	BufferB[2] = ChessName[BLACKCHESS][GetType(ChessID)][1] ;
				for(int len = 0 ;len < 4; len++)
				{
					txtboard[2*i+1][j*4+3+len]=BufferB[len];
				}
			}
		}
	}
	
	ss << "\n       --------日期：" << __DATE__ << " -------\n ";
	for(int i=0; i<22;  i ++ )
	{
		ss<<"\n"<<txtboard[i];
	}
	ss << "\n" << fen() << std::endl<< std::endl;

#ifdef _DEBUG
	//cout << "m_hWndBoard  " << m_hWndBoard << endl;
	//cout << "m_hWndProg  " << m_hWndProg << endl;
	//cout << "m_ptBRookInBoard.x  " << m_ptBRookInBoard.x << endl;
	//cout << "m_ptBRookInBoard.y  " << m_ptBRookInBoard.y << endl;
	//cout << "m_width_init  " << m_width_init << endl;
	//cout << "m_width_real  " << m_width_real << endl;
	//cout << "m_threshold  " << m_threshold << endl;
	//cout << "m_dx  " << m_dx << endl;
	//cout << "m_threshold  " << m_threshold << endl;

	for(int i=0; i<HOTKEYS; i++)
	{
		ss	<< kmess[i].c_str()<<endl;
	}
	ss	<< endl;
#endif

	return ss.str();
}

//根据m_Board输出棋盘图,未完成
Mat& CMatLinker::fen2mat(string strfen, Mat& dst) 
{
	dst = cv::imread("dst.bmp", CV_LOAD_IMAGE_ANYDEPTH|CV_LOAD_IMAGE_ANYCOLOR);
	cv::Mat mask = cv::imread("selected.bmp", CV_LOAD_IMAGE_ANYDEPTH|CV_LOAD_IMAGE_ANYCOLOR);
	//cv::Mat mask = cv::imread("bn.bmp",0);
	cv::Mat piece;
	POINT ptBd, ptPc;	
	for(int y=0; y<10;  y ++ )
	{
		for(int x=0; x<9; x ++ )
		{
			int ChessID =m_Board[SquareFrom90[y*9+x]];
			if(ChessID!=0)
			{
				ptPc.x = ChessID * m_width_init;
				ptPc.y = 0;
				piece = m_imgPieceAll( Rect(ptPc.x, ptPc.y, m_width_real, m_width_real));
#ifdef _DEBUG				
				imshow("piece", piece);
				waitKey(0);
#endif
				ptBd.x =  m_ptBRookInBoard.x + x*m_dx - m_width_real/2;
				ptBd.y =  m_ptBRookInBoard.y + y*m_dy - m_width_real/2;
				cv::Mat imageROI = dst(cv::Rect(ptBd.x, ptBd.y, piece.cols, piece.rows));
				
				piece.copyTo(imageROI, mask);
#ifdef _DEBUG				
				imshow("dst", dst);
				waitKey(0);
#endif
			}
		}
	}
#ifdef _DEBUG				
				imshow("dst", dst);
				waitKey(0);
#endif
	return dst;
}

bool CMatLinker::normalize(char* fn, cv::Point pt, int width, int delt, double fLookRook)
{
	bool ok=false;
	char buffer[2054];

	cv::Size sz(640,640/9*10);			//归一化后图像大小

	m_imgInit = imread(fn, CV_LOAD_IMAGE_ANYDEPTH|CV_LOAD_IMAGE_ANYCOLOR);		//图像来自文件
	if(m_imgInit.data==NULL)
	{
		printf("read file error while normalize! filename: %s\n\n", fn);
		return false;
	}

	//保存归一化原始棋盘的参数，以备归一化实时棋盘时用。
	m_normalize.pt.x = pt.x;  
	m_normalize.pt.y =pt.y;
	m_normalize.width = pt.x<width? pt.x : width;
	m_normalize.width = pt.y<width? pt.y : width;
	m_normalize.delt = delt;

	m_normalize.sz.width = sz.width;
	m_normalize.sz.height = sz.height;
	m_normalize.dx = -1;		//原始棋盘归一化时，初始化该参数为－1.正常大于 0

	m_threshold = fLookRook;

	m_ptBRookInBoard.x = pt.x;  
	m_ptBRookInBoard.y =pt.y;
	m_width_init = pt.x<width? pt.x : width;
	m_width_init = pt.y<width? pt.y : width;

	if(calc_DxDy( ))	//取得m_dx
	{
		ok=true;
		m_normalize.dx = m_dx;		//保存m_dx用于归一化实时图像时用

		int cx = m_dx*9 +  m_width_init/2 + delt;
		int cy = m_dx*10 + m_width_init/2 + delt*2;
		cx = cx<m_imgInit.cols? cx : m_imgInit.cols;
		cy = cy<m_imgInit.rows? cy : m_imgInit.rows;

		Mat img_mini = m_imgInit( Rect(pt.x-m_width_init , pt.y-m_width_init, cx, cy) );
		resize(img_mini,m_imgInit, m_normalize.sz);
		sprintf(buffer,"NI%s",fn);
		imwrite(buffer, m_imgInit); 
#ifdef _DEBUG
		//imshow("原始归一化后",m_imgInit);
#endif
	}

#ifdef _DEBUG
	//waitKey(0);
#endif

	return ok;
}

//归一化实时棋盘
bool CMatLinker::normalize(char* fn)
{
	char buffer[2054]="";

	m_imgReal = imread(fn, CV_LOAD_IMAGE_ANYDEPTH|CV_LOAD_IMAGE_ANYCOLOR);		//图像来自文件
	if(m_imgReal.data==NULL || m_normalize.dx==-1)
	{
		printf("error read file(real position)! or m_normalize.dx not isready!");
		return false;
	}

	int cx = m_normalize.dx*9 +  m_normalize.width/2 + m_normalize.delt;
	int cy = m_normalize.dx*10 + m_normalize.width/2 + m_normalize.delt*2;
	cx = cx<m_imgReal.cols? cx : m_imgReal.cols;
	cy = cy<m_imgReal.rows? cy : m_imgReal.rows;

	Mat img_mini = m_imgReal( Rect(m_normalize.pt.x-m_normalize.width , m_normalize.pt.y-m_normalize.width, cx, cy) );
	resize(img_mini,m_imgReal, m_normalize.sz);
	sprintf(buffer,"NR%s",fn);
	imwrite(buffer, m_imgReal); 
#ifdef _DEBUG
	//imshow("实时归一化后",m_imgInit);
	//waitKey(0);
#endif
	return true;
}

//用热键配合鼠标实时坐标建立初始图形连线方案，随后计算出m_dx，最后进行positon识别
//只需要一个屏幕坐标就可以了：鼠标指向左车，按下热键后，传递鼠标的屏幕坐标做参数调用此函数即可
bool CMatLinker::make_solution(POINT MousePoint)
{
	//POINT MousePoint;
	//::GetCursorPos(&MousePoint);

	m_hWndBoard = ::WindowFromPoint(MousePoint);
	m_hWndProg = m_hWndBoard;
	if(!IsWindow(m_hWndBoard))return false;

	// 收集窗口信息
	// 1、当前窗口
	for(int i=0;i<10;i++)
	{
		for(int j=0;j<254;j++)  
		{
			m_className[i][j]=0;
			m_titleName[i][j]=0;
		}
	}

	::GetWindowText( m_hWndBoard ,&m_titleName[0][0], 254 );  //棋盘窗口的信息
	::GetClassName( m_hWndBoard ,&m_className[0][0], 254 );

	HWND hwndtemp=m_hWndBoard;

	// 2、locate top window
	m_maxParents=0;
	m_hWndProg = GetParent(m_hWndBoard) ;
	while( m_hWndProg!= NULL)
	{
		hwndtemp = m_hWndProg;
		m_maxParents++;
		::GetWindowText( m_hWndProg ,&m_titleName[m_maxParents][0], 254 );
		::GetClassName( m_hWndProg ,&m_className[m_maxParents][0], 254 );
		m_hWndProg = ::GetParent(m_hWndProg)  ;
	}

	m_hWndProg=hwndtemp;

	//strcpy( m_titleKeyword, &m_titleName[m_maxParents][0]);	//对于可变窗口标题，需要另用关键字填充棋手名
	m_titleKeyword = m_titleName[m_maxParents];	//对于可变窗口标题，需要另用关键字填充棋手名

	//以下建立方案（涉及到成员变量：m_imgInit、m_ptBRookInBoard。实时图m_imgReal是用来识别的）
	//该函数应用hWnd参数标识的窗口和POINT结构给定的屏幕坐标来计算用户坐标，
	//然后以用户坐标来替代屏幕坐标，新坐标是相对于指定窗口的领域的左上角

	m_ptBRookInProg = MousePoint;
	m_ptBRookInBoard = MousePoint ;
	::ScreenToClient( m_hWndProg, &m_ptBRookInProg);
	::ScreenToClient( m_hWndBoard,&m_ptBRookInBoard);

	//::GetWindowRect(m_hWndBoard, &m_BoardWinRect);

	hwnd2mat(m_imgInit, m_hWndBoard);		//图像来自截屏
//
#ifdef _DEBUG
	//imwrite("boardinit.jpg", m_imgInit);	//save方案时也保存了，重复了，不过是调试也没关系
#endif
//
	if(build_solution(m_ptBRookInBoard))		
	{
		write_solution(m_titleKeyword+".lll");
		m_imgReal=m_imgInit;					//第一次建立方案时，就识别原始图
		mat2fen();
		return true;
	}
	return false;
}

////h7e7   (7,7)-(4,7)   炮二平五
bool  CMatLinker::make_move(POINT mvFrom, POINT mvTo)const   //图形棋手走棋
{
	POINT from,to;
	//assert(IsWindow(m_hWndBoard)&&IsWindow(m_hWndProg));
	from.x=  m_ptBRookInBoard.x + mvFrom.x* m_dx;
	from.y=  m_ptBRookInBoard.y + mvFrom.y* m_dy;
	to.x=  m_ptBRookInBoard.x + mvTo.x* m_dx;
	to.y=  m_ptBRookInBoard.y + mvTo.y* m_dy;
	
	if(!IsWindow(m_hWndBoard))
	{
		return false;
	}
	
	if(IsIconic(m_hWndProg))
	{
		ShowWindow(m_hWndProg,SW_SHOW);
	}
	
	BringWindowToTop(m_hWndProg);       //可以试一下不同的函数效果
	SetForegroundWindow(m_hWndProg);
	
	//m_hWndBoard=HWND(0x000F03F4);			//测试代码时用的固定窗口句柄
	SendMessage(m_hWndBoard, WM_LBUTTONDOWN, 0, from.y<<16 | from.x & 0x0000ffff);
	SendMessage(m_hWndBoard, WM_LBUTTONUP, 0, from.y<<16 | from.x & 0x0000ffff);		 //删除本行也可以？
	Sleep(100);
	SendMessage(m_hWndBoard, WM_LBUTTONDOWN, 0, to.y<<16 | to.x & 0x0000ffff);
	SendMessage(m_hWndBoard, WM_LBUTTONUP, 0, to.y<<16 | to.x & 0x0000ffff);
	Sleep(100);
	return true;
	
}

//根据鼠标位置获取下层窗口，可以用(WindowFromPoint, ChildWindowFromPoint, ChildWindowFromPointEx,RealChildWindowFromPoint)。只有ChildWindowFromPointEx这个API有窗口过滤功能。 
//通过调用ChildWindowFromPointEx(hWndDesktop,ptCursor, CWP_SKIPINVISIBLE|CWP_SKIPTRANSPARENT), 可以过滤掉不可见的和Layered窗口，然后通过递归调用该API，就可以获取里面的子窗口了。

void CMatLinker::get_mat_real()
{
	assert(m_hWndBoard!=NULL);

	//hwnd2mat(m_imgInit, m_hWndBoard);		//图像来自截屏
	hwnd2mat(m_imgReal, m_hWndBoard);		//图像来自截屏
	return ;
}

//根据远程图形棋盘，更新数据盘面，返回fen串
void CMatLinker::update_fen(void)
{
	//if(!bConnected) return;
	if(!isready()) return;

	if(m_hWndBoard==NULL || !IsWindow(m_hWndProg))	{
		return;

	}
	if(IsIconic(m_hWndProg))
	{
		ShowWindow(m_hWndProg, SW_SHOWNORMAL);
	}

	BringWindowToTop(m_hWndProg );

	hwnd2mat(m_imgReal, m_hWndBoard);		//截屏。如截屏是32位深，则要写入文件再读一次，调用match()时才不会出错，原因不明。
#ifdef _DEBUG
	imwrite("screenshot.jpg", m_imgReal);
	m_imgReal=imread("screenshot.jpg");
#endif

#ifdef _DEBUG
	system("del temp\\*.jpg /S/Q >> NULL"); 
#endif
	mat2fen("screenshot.jpg");			  //更新局面
	//退出作用域则离开临界区
}

/// Position::fen() returns a FEN representation of the position.This is mainly a debugging function.
const std::string  CMatLinker::fen( ) const 
{	

	int emptyCnt;
	std::ostringstream ss;
	static const char* PieceToChar[2] = {" PBACNRK"," pbacnrk"};

	for (Rank r = Rank9; r <= Rank0; r++)
	{
		for (File f = FileA; f <= FileI; f++)
		{
			for (emptyCnt = 0; f <= FileI && empty(make_square(f, r));  f++)
				++emptyCnt;

			if (emptyCnt)
				ss << emptyCnt;

			if (f <= FileI)
			{
				PieceID pc = piece_on(make_square(f, r));
				ss << PieceToChar[GetColor(pc)][GetType(pc)];
			}
		}

		if (r < Rank0)
			ss << '/';
	}

	ss << (sideToMove == RED ? " r " : " b ") << " - - 0 1";

	return ss.str();
}

/// Position::from_fen(const std::string& Str, Thread* th) initializes the position object with the given FEN string.
/// This function is not very robust - make sure that input FENs are correct,
/// this is assumed to be the responsibility of the GUI.
/// 示例："2baka2r/r8/4b4/p2R4p/2p3p2/4R4/c1P3P1P/1C7/9/2BAKAB2 w - - 0 1 moves b2e2 f9e8 e4d4 e9f9 d6f6"
/// 注意：本函数没有处理后续 moves !
void CMatLinker::set(const std::string& fenStr )
{
	uint8_t	pieceCount[COLOR_NB][PIECE_TYPE_NB];	//各兵种数量
	char token;
	size_t idx;
	Square sq = A9;
	std::istringstream ss(fenStr);

	//所有带颜色的兵种用,  //（外部有定义）  
	const unsigned char PieceColorType[15] =  { PIECE_TYPE_NONE, B_PAWN,B_ELEPHANT , B_BISHOP, B_CANNON, B_HORSE, B_ROOK, B_KING, R_PAWN,R_ELEPHANT , R_BISHOP, R_CANNON, R_HORSE, R_ROOK, R_KING};

	const string PieceToChar = " pbacnrkPBACNRK";
	
	memset(m_Board, 0, sizeof(m_Board));
	memset(pieceCount, 0, sizeof(pieceCount));

	ss >> std::noskipws;

	// 1. Piece placement
	while ((ss >> token) && !isspace(token))
	{
		if (isdigit(token))
			sq += Square(token - '0'); // Advance the given number of files

		else if (token == '/')
		{
			sq = make_square(FILE_LEFT, square_rank(sq+Square(0x10))) ;
		}
		else if ((idx = PieceToChar.find(token)) != string::npos)		// put_piece;
		{
			unsigned char colortype = PieceColorType[idx];				//字符转换为该兵种类型编码（区分颜色的）
			Color c=GetColor(colortype);
			PieceType pt=GetType(colortype);							//不区分颜色的兵种
			PieceID ChessID =  colortype|(++pieceCount[c][pt]);			//得到棋子ID
			m_Board[sq] = ChessID;
			++sq;
		}
	}

	// 2. Active color
	ss >> token;
	sideToMove = (token == 'b' ? BLACK : RED);	//chinese chess RED is "r", chess is "w"

	// 3. Halfmove clock and fullmove number
	// 合法性检验 ,已改成不理会Fen串带来的参数了，始终是0和1, 其后的moves也没有在本处处理
	//for (int i=0; i<5; i++) { ss>>token; }
	//	ss >> std::skipws >> st->rule50 >> gamePly;

	// 4. 此处不处理 moves ，延后到 …………处理，参见……函数
	
//	assert(pos_is_ok());
}

const string CMatLinker::GetStepName( Move move )const
{
	static const char PostionName[2][9][4] ={
		{"九","八","七","六","五","四","三","二","一"},
		{"１","２","３","４","５","６","７","８","９"}
	};

	unsigned char  from,to;
	unsigned char  ChessType,ChessID;
	unsigned char  y1,x1,y2,x2;
	unsigned short Player;
	//int pos;

	static char step_name[12];

	from = from_sq(move) ;
	to = to_sq(move);
	ChessID = m_Board[from];
	ChessType = GetType(ChessID);

	Player = GetColor(ChessID);

	y1 = (from >> 4) - 3;  //行
	x1 = (from & 0xf) - 3; //列
	y2 = (to >> 4) - 3;
	x2 = (to & 0xf) - 3;


	strcpy( step_name, (char*)ChessName[Player][ChessType]);

	strcat( step_name, (char *)PostionName[Player][x1] );

	//检查此列x0是否存在另一颗成对的棋子.
	unsigned char  AnotherChessType, y, AnotherChessID; 		//y:行
	for(y=0;y<10;y++)
	{
		AnotherChessID =  m_Board[(y+3)*16+x1+3];
		AnotherChessType = GetType(AnotherChessID);
		if(AnotherChessType==0)	// 无子继续搜索
			continue;
		if( ChessType==ELEPHANT  || ChessType == BISHOP )	// 象、士不用区分
			continue;
		if(y==y1)           // 是同一颗棋子, 不必搜索.
			continue;
		assert(AnotherChessID);
		//assert(ChessID);
		if (!ChessID)
		{
			int i=0;
		}
		if( (AnotherChessType==ChessType) && IsSameSide(AnotherChessID,ChessID))
		{
			if(Player == REDCHESS)		        // 红子    //if(!Player)
			{
				if( y1 > y )
					strcpy( step_name, "后" );
				else
					strcpy( step_name, "前" );
				strcat( step_name, (char *)ChessName[Player][AnotherChessType] );
			}
			else if(Player==BLACKCHESS)	        // 黑子
			{
				if( y1 > y )
					strcpy( step_name, "前" );
				else
					strcpy( step_name, "后" );
				strcat( step_name, (char *)ChessName[Player][AnotherChessType] );
			}

			break;
		}
	}

	//进, 退, 平
	if(y1==y2)
	{
		strcat( step_name, "平" );
		strcat( step_name, PostionName[Player][x2]);					// 平，任何棋子都以绝对位置表示
	}
	else if((Player && y1<y2) || (!Player && y1>y2))
	{
		strcat( step_name, "进" );

		if(ChessType==ELEPHANT  || ChessType == BISHOP||ChessType==HORSE)// 马、象、士用绝对位置表示
			strcat( step_name, PostionName[Player][x2] );
		else if(Player==BLACKCHESS)												    // 将、车、炮、兵用相对位置表示
			strcat( step_name, PostionName[1][y2-y1-1] );			// 黑方
		else if(Player==REDCHESS)
			strcat( step_name, PostionName[0][9-y1+y2] );			// 红方
	}
	else     
	{
		strcat( step_name, "退" );
		if(ChessType==ELEPHANT  || ChessType == BISHOP || ChessType==HORSE )						// 马、象、士用绝对位置表示
			strcat( step_name, PostionName[Player][x2] );
		else if(Player)											    // 将、车、炮、兵用相对位置表示
			strcat( step_name, PostionName[1][y1-y2-1] );			// 红方
		else
			strcat( step_name, PostionName[0][9-y2+y1] );			// 黑方
	}

	return string(step_name);
}

void CMatLinker::ChangeSide(void)
{
	unsigned char temp[256];
	unsigned short  i;
	for(i=0;i<256;i++)
	{
			temp[i] = NOCHESS;
	}
	for(i=0x33;i<=0xcb;i++)
	{
		if(m_Board[i]==NONE || m_Board[i]==OUTSIDE)
			continue;

		temp[0xdc-i+0x22]=m_Board[i];
	}
	memcpy(m_Board,temp,256);
}

void CMatLinker::LeftToRight(void)
{
	unsigned short i;
	unsigned char temp[256];
	for(i=0;i<256;i++)
	{
			temp[i] = NOCHESS;
	}
	for(i=0x33;i<=0xcb;i++)
	{
		if(m_Board[i]==NONE||m_Board[i]==OUTSIDE)
			continue;

		temp[i&0xf0| (0x0c-(i&0x0f)+0x02)]=m_Board[i];
	}
	memcpy(m_Board,temp,256);
}

Color	CMatLinker::local_color()
{
	Color localcolor = BLACK;
	if(		m_Board[0x36]==B_KING1 || m_Board[0x37]==B_KING1 || m_Board[0x38]==B_KING1 
		||	m_Board[0x46]==B_KING1 || m_Board[0x47]==B_KING1 || m_Board[0x48]==B_KING1 
		||	m_Board[0x56]==B_KING1 || m_Board[0x57]==B_KING1 || m_Board[0x58]==B_KING1 
	)
		localcolor=RED;

	return localcolor;
}

//由新旧两个盘面的变化情况找到上一步走子状态
Move CMatLinker::board2move(PieceID pre_board[SQUARE_NB]) 
{
	POINT	pt1,pt2, from, to;
	PieceID	qizi1, qizi2;
	PieceID	killed, ChessID;
	killed = ChessID = NOCHESS;

	CMatLinker temp;
	memcpy(temp.m_Board, m_Board, sizeof(m_Board));//没有拷贝构造函数吗 ：）
	
	bool    srcblackup = false ;
    bool    destblackup = false ;

	srcblackup = local_color()==RED? true:false;
	destblackup = temp.local_color()==RED? true:false;

	if( srcblackup ^ destblackup )
	{
		ChangeSide();
	}

	pt1.x = pt1.y=pt2.x=pt2.y=-1;
	from.x = from.y=to.x=to.y=-1;
	for(int i =0;i<10;i++)
	{
		for(int j =0;j<9;j++)
		{
			qizi2 = pre_board[SquareFrom90[i*9+j]];
			qizi1 = m_Board[SquareFrom90[i*9+j]];
			if(qizi1 != qizi2 )      // where qizi moved
			{
				if( qizi1 == NOCHESS )
				{
					from.x = j;     from.y=i;
				}
				else
				{
					to.x= j ;       to.y=i;
					if ( qizi2!= NOCHESS )
					{
						killed = qizi2;
					}
					else
					{
						killed = NOCHESS;
					}
					ChessID = qizi1;
				}
			}
		}
	}

	if( srcblackup ^ destblackup )
	{
		ChangeSide();
	}

	Square from256   = SquareFrom90[from.y*9+from.x];
	Square to256   = SquareFrom90[to.y*9+to.x];
	killed   = killed ;
	ChessID  = ChessID;
	
	Move m = (from256<<8) | to256;  

	return m;
}

  // uci_pv() formats PV information according to the UCI protocol.
string  CMatLinker::uci_pv(std::vector<Move> pv)
{
	int j=0;
	std::stringstream ss;
	CMatLinker pos;

	pos.set(fen());

	ss << "\n";
	while(pv[j] != MOVE_NONE)
    {	
		ss <<  " " << string(pos.GetStepName(pv[j]));
		pos.do_move(pv[j]);
        j++;
	}
	
	return ss.str();
}

void CMatLinker::do_move(Move move)
{
	assert(move!=MOVE_NONE);
	const unsigned char src = GetSrcFromMove(move);
	const unsigned char dst = GetDstFromMove(move);
	const unsigned char chessid = m_Board[src];
	const unsigned char killid =  m_Board[dst];

	m_Board[dst] = m_Board[src];
	m_Board[src] = NOCHESS;

	sideToMove  =1-sideToMove;  
}


//由新旧两个盘面的变化情况找到上一步走子状态

int CMatLinker::board2diffs(PieceID pre_board[SQUARE_NB]) 
{
	PieceID	ID_Curr, ID_Prev;
	CMatLinker temp;

	//assert(sizeof(m_Board)==sizeof(pre_board));

	int len1=sizeof(m_Board);			//这个值256
	int len2=sizeof(pre_board);			//这个只是4

	memcpy(temp.m_Board, pre_board, sizeof(m_Board));//没有拷贝构造函数吗 ：）
	//cout<<temp.print()<<endl;

	bool srcblackup = local_color()==RED? true:false;
	bool destblackup = temp.local_color()==RED? true:false;

	if( srcblackup ^ destblackup )
	{
		temp.ChangeSide();
	}

	int m=0;
	
	for (Rank r = Rank9; r <= Rank0; r++)		//10行
	{
		for (File f = FileA; f <= FileI; f++)	//9列
		{
				ID_Curr = m_Board[make_square(f, r)];   
				ID_Prev = temp.m_Board[make_square(f, r)];
				if(GetType(ID_Curr) != GetType(ID_Prev) )      ////set(fen());并不会完全复原。因此要比较兵种
				{
					m++;
				}
		}
	}

	if( srcblackup ^ destblackup )
	{
		temp.ChangeSide();
	}

	return m;
}
