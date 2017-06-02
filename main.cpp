#include "toolsCV.h"
#include "board.h"
#include "ucciAdapter.h"

//自动判断识别OpenCV的版本号，并据此添加对应的依赖库（.lib文件）的方法
#define CV_VERSION_ID       CVAUX_STR(CV_MAJOR_VERSION) CVAUX_STR(CV_MINOR_VERSION) CVAUX_STR(CV_SUBMINOR_VERSION)
#ifdef _DEBUG
#define cvLIB(name) "opencv_" name CV_VERSION_ID "d"
#else
#define cvLIB(name) "opencv_" name CV_VERSION_ID
#endif

#pragma comment( lib, cvLIB("core") )
#pragma comment( lib, cvLIB("imgproc") )
#pragma comment( lib, cvLIB("highgui") )
#pragma comment( lib, cvLIB("flann") )
#pragma comment( lib, cvLIB("features2d") )
#pragma comment( lib, cvLIB("calib3d") )
#pragma comment( lib, cvLIB("nonfree") )

using namespace cv;
using namespace std;

//2017年5月29日调试通过，核心代码可用
//1、可热键配合鼠标制作方案
//2、方案制作完成后，可识别
//3、方案保存及装入功能未完成，但已不影响使用了，只是不太方便
int main(int argc, char *argv[])
{
	CUcci ucci;
	CMatPlayer bmpPlayer(ucci);

	if (RegisterHotKey(NULL, 1, MOD_ALT | MOD_NOREPEAT, VK_F8))  // 'F8'
	{
		std::cout << "Hotkey 'ALT_F8' 可用于初始化方案! \n"<< std::endl;
		std::cout << "方法：1、移动鼠标左键指向棋盘左上角的黑车，2、然后按下组合键ALT_F8即可初始化方案! \n"<< std::endl;
	}
	if (RegisterHotKey(NULL, 2, MOD_ALT | MOD_NOREPEAT, VK_F1))  //
	{
		std::cout << "Hotkey 'ALT_F1' 在初始化方案后即可随时按下组合键进行识别，识别出来的Fen串同时保存在剪贴板上! \n"<< std::endl;
	}
	if (RegisterHotKey(NULL, 3, MOD_ALT | MOD_NOREPEAT, 0x73))  //0x73 's'
	{
		std::cout << "Hotkey 'Alt_s' save solution! \n"<< std::endl;
	}
	if (RegisterHotKey(NULL, 4, MOD_ALT | MOD_NOREPEAT, 0x6c))  // 'l'
	{
		std::cout << "Hotkey 'Alt_l' load solution! \n"<< std::endl;
	}
	
	MSG msg = {0};
	bool bExit=false;
	while (GetMessage(&msg, NULL, 0, 0) != 0)
	{
		switch (msg.message)
		{
		case WM_HOTKEY:
			if(msg.wParam==1)  //ALT_F8
			{
				POINT MousePoint;
				MousePoint.x = msg.pt.x;
				MousePoint.y = msg.pt.y;
				bmpPlayer.m_width=40;							 //分辨率 ，样本棋子宽 = 高
				bmpPlayer.build_solution(MousePoint);
				imwrite("BoardInit.jpg",bmpPlayer.m_imgBoardInit);
				std::cout << "ALT_F8，鼠标位置x: "<< MousePoint.x<< "mouse y: "<< MousePoint.y << std::endl << std::endl;
				std::cout<< bmpPlayer.BMP2Position(true) << std::endl;
				std::cout<< bmpPlayer.print() << std::endl;
			}
			else if(msg.wParam==2)  //ALT_F1
			{
				//以下是识别部分
					std::cout<< "\n按下组合键“Alt_F1”, 生成fen串: " << std::endl;
					std::cout<< bmpPlayer.BMP2Position(true) << std::endl;
					std::cout<< bmpPlayer.print() << std::endl;
					imwrite("Board.jpg",bmpPlayer.m_imgBoard);
			}
			else if(msg.wParam==3)  //Win_s
			{
				if(bmpPlayer.SaveSolution("link_test.lll"))
				{
					std::cout << "save ok!" << std::endl;
				}

			}
			else if(msg.wParam==4)	//Alt_l	
			{
				if(bmpPlayer.LoadSolution("link.lll"))		//做到构造函数里去
				{
					std::cout << "Load link.lll ok!" << std::endl;
					std::cout << bmpPlayer.BMP2Position(true)<<std::endl;
					imwrite("Boardlll.jpg",bmpPlayer.m_imgBoardInit);
				}

			}

			break;

		case WM_MOUSEMOVE:
			std::cout << "WM_MOUSE_MOVE! \n"<< std::endl;
			break;
		case WM_LBUTTONDOWN:
			std::cout << "WM_LBUTTONDOWN! \n"<< std::endl;
			break;
		case WM_LBUTTONUP:
			std::cout << "WM_LBUTTONUP! \n"<< std::endl;
			break;
		case WM_RBUTTONDOWN:
			bExit=true;
			std::cout << "WM_RBUTTONDOWN! EXIT! \n"<< std::endl;
			switch(msg.wParam)
			{
			case VK_LEFT:
				std::cout << "WM_Left! \n"<< std::endl;
				break;
			case VK_RIGHT:
				std::cout << "WM_Right! \n"<< std::endl;
				break;
			case VK_UP:
				std::cout << "WM_Up! \n"<< std::endl;
				break;
			case VK_DOWN:
				break;
			default:
				break;
			}
			break;
		case WM_QUIT:
			std::cout << "WM_QUIT! \n"<< std::endl;
			break;
		default:
			//std::cout << "not known message! \n"<< std::endl;
			break;

			TranslateMessage( &msg );  
			DispatchMessage( &msg );  

			if(bExit)break; 
		}
	}

	::UnregisterHotKey(NULL, 1); //注销HotKey, 释放资源。
	::UnregisterHotKey(NULL, 2); //注销HotKey, 释放资源。
	::UnregisterHotKey(NULL, 3); //注销HotKey, 释放资源。
	::UnregisterHotKey(NULL, 4); //注销HotKey, 释放资源。

	return 0;

	////其它工具测试
	//myCanny();			//边缘检测
	//mySobel( );
	//myKeyPoint();
	//mySurf();
	//ROI_AddImage();

	////测试屏幕截取，用程序名称找到窗口句柄，再用窗口句柄截取图像
	//用窗口句柄截取图像另外还有个函数：hwnd2mat
	//vector<HWND> list;
	//getHwndsByProcessName(L"bh.exe",list,true);
	////revealWindow(list[0]);
	//WindowCapture cap(list[0]);
	//Mat screen;               
	//cap.captureFrame(screen); 
	//imshow("测试屏幕截取", screen);
	//imwrite("截取BH屏幕.jpg",screen);
	//waitKey(0);
}