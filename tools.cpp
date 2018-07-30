#include "tools.h"

#include "opencv2/legacy/legacy.hpp"
//#include "opencv2/nonfree/nonfree.hpp"

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

int g_biBitCount = 24;

//按行合并如下：
cv::Mat mergeRows(cv::Mat A, cv::Mat B)
{
	// cv::CV_ASSERT(A.cols == B.cols&&A.type() == B.type());
	int totalRows = A.rows + B.rows;
	cv::Mat mergedDescriptors(totalRows, A.cols, A.type());
	cv::Mat submat = mergedDescriptors.rowRange(0, A.rows);
	A.copyTo(submat);
	submat = mergedDescriptors.rowRange(A.rows, totalRows);
	B.copyTo(submat);
	return mergedDescriptors;
}

//按列合并如下：
cv::Mat mergeCols(cv::Mat A, cv::Mat B)
{
	// cv::CV_ASSERT(A.cols == B.cols&&A.type() == B.type());
	int totalCols = A.cols + B.cols;
	cv::Mat mergedDescriptors(A.rows,totalCols, A.type());
	cv::Mat submat = mergedDescriptors.colRange(0, A.cols);
	A.copyTo(submat);
	submat = mergedDescriptors.colRange(A.cols, totalCols);
	B.copyTo(submat);
	return mergedDescriptors;
}


//图像模板匹配
//一般而言，源图像与模板图像patch尺寸一样的话，可以直接使用上面介绍的图像相似度测量的方法；
//如果源图像与模板图像尺寸不一样，通常需要进行滑动匹配窗口，扫面个整幅图像获得最好的匹配patch。
//在OpenCV中对应的函数为：matchTemplate()：函数功能是在输入图像中滑动窗口寻找各个位置与模板图像patch的相似度。相似度的评价标准（匹配方法）有：
//CV_TM_SQDIFF 平方差匹配法（相似度越高，值越小），
//CV_TM_CCORR 相关匹配法（采用乘法操作，相似度越高值越大），
//CV_TM_CCOEFF 相关系数匹配法（1表示最好的匹配，-1表示最差的匹配）。
//通常,随着从简单的测量(平方差)到更复杂的测量(相关系数),我们可获得越来越准确的匹配(同时也意味着越来越大的计算代价). /
//最好的办法是对所有这些设置多做一些测试实验,以便为自己的应用选择同时兼顾速度和精度的最佳方案.//

//有一种新的用来计算相似度或者进行距离度量的方法：EMD，Earth Mover‘s Distances
//EMD is defined as the minimal cost that must be paid to transform one histograminto the other, where there is a “ground distance” between the basic featuresthat are aggregated into the histogram。
//光线变化能引起图像颜色值的漂移，尽管漂移没有改变颜色直方图的形状，但漂移引起了颜色值位置的变化，从而可能导致匹配策略失效。而EMD是一种度量准则，度量怎样将一个直方图转变为另一个直方图的形状，包括移动直方图的部分（或全部）到一个新的位置，可以在任意维度的直方图上进行这种度量。
//在OpenCV中有相应的计算方法：cvCalcEMD2()。结合着opencv支持库，计算直方图均衡后与原图的HSV颜色空间直方图之间的EMD距离。

//cv::Point pt;
//Mat image= imread("board.jpg");
//Mat tepl= imread("position.jpg");
//double d = match(image, tepl, &pt,  CV_TM_SQDIFF) //CV_TM_SQDIFF_NORMED  CV_TM_CCORR CV_TM_CCOEFF CV_TM_CCORR_NORMED CV_TM_CCOEFF_NORMED
double match(cv::Mat image, cv::Mat templ, cv::Point &matchLoc, int method)
{
	int result_cols =  image.cols - templ.cols + 1;
	int result_rows = image.rows - templ.rows + 1;

	cv::Mat result = cv::Mat( result_cols, result_rows, CV_32FC1 );
	cv::matchTemplate( image, templ, result, method );
	//cv::normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat());

	double minVal, maxVal, matchVal;
	cv::Point minLoc, maxLoc;
	cv::minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );

	//CV_TM_SQDIFF平方差匹配法（相似度越高，值越小）
	//CV_TM_CCOEFF相关系数匹配法（1表示最好的匹配，-1表示最差的匹配）。
	//CV_TM_CCORR相关匹配法（采用乘法操作，相似度越高值越大），
	switch(method)
	{
	case CV_TM_SQDIFF:								//CV_TM_SQDIFF平方差匹配法（相似度越高，值越小）
		matchLoc = minLoc;								//
		matchVal = minVal / (templ.cols * templ.cols);	//去掉模板大小对匹配度的影响：
		break;
	case CV_TM_CCORR:
	case CV_TM_CCOEFF:
		matchLoc = maxLoc;
		matchVal = maxVal / (templ.cols * templ.cols);
		break;
	case CV_TM_SQDIFF_NORMED:		
		matchLoc = minLoc;
		matchVal =  minVal;
		break;
	case CV_TM_CCORR_NORMED:
	case CV_TM_CCOEFF_NORMED:
		matchLoc = maxLoc;
		matchVal =  maxVal;
		break;
	default:
		matchLoc = maxLoc;
		matchVal =  maxVal;
		break;
	}
#ifdef _DEBUG
	// 看看最终结果
	//rectangle(image, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );
	//imshow( "image", image );
	//waitKey(0);
#endif
	return matchVal;
}

void StrToClip(char* pstr) //拷贝FEN串到剪贴板
{
	if(::OpenClipboard (NULL))//打开剪贴板
	{
		HANDLE hClip;
		char* pBuf;
		::EmptyClipboard();//清空剪贴板

		//写入数据
		hClip=::GlobalAlloc(GMEM_MOVEABLE,strlen(pstr)+1);
		pBuf=(char*)GlobalLock(hClip);
		strcpy_s(pBuf, strlen(pstr)+1, pstr);
		::GlobalUnlock(hClip);//解锁
		::SetClipboardData(CF_TEXT,hClip);//设置格式

		//关闭剪贴板
		::CloseClipboard();
	}
}

void ClipToStr(char* pstr) //从剪贴板拷贝到pstr  这个函数有点问题
{
	//判断剪贴板的数据格式是否可以处理。
	if (!IsClipboardFormatAvailable(CF_TEXT))
	{
		return;
	}

	//打开剪贴板。
	if (!OpenClipboard(NULL))
	{
		return;
	}

	//获取UNICODE的数据。
	HGLOBAL hMem = GetClipboardData(CF_TEXT);
	if (hMem != NULL)
	{
		//获取UNICODE的字符串。
		LPTSTR lpStr = (LPTSTR)GlobalLock(hMem);
		if (lpStr != NULL)
		{
			//显示输出。
			strcpy_s(pstr, strlen(lpStr)+1, lpStr);

			//释放锁内存。
			GlobalUnlock(hMem);
		}
	}

	//关闭剪贴板。
	CloseClipboard();
}

//找到兵河窗口并模拟新建，粘贴局面，开始分析
void start_bh(string fen, bool turn)
{
	HWND bh = FindWindow(NULL,"BHGUI(test) - 新棋局"); 
	if(bh==NULL || !IsWindow(bh))
	{
		return;
		//int iResult = (int)ShellExecute(NULL,"open","c:\\bh\\bh.exe",NULL,NULL,SW_SHOWNORMAL);    //执行应用程序
		//Sleep(3000);
		//HWND bh = FindWindow(NULL,"BHGUI(test) - 新棋局"); 
	}
	BringWindowToTop(bh);  
	SetForegroundWindow(bh);

	keybd_event(VK_CONTROL, 0, 0, 0);	//Alt Pres
	keybd_event('N', 0, 0, 0);			//Alt Pres
	keybd_event('N', 0, KEYEVENTF_KEYUP, 0);		
	keybd_event(VK_CONTROL,0,KEYEVENTF_KEYUP,0);	
	Sleep(1000);        //停顿一秒
	std::cout << "兵河开新局!"<< std::endl;

	StrToClip((char*)fen.c_str()); //拷贝FEN串到剪贴板
	keybd_event(VK_MENU, 0xb8, 0, 0);	//Alt 
	keybd_event('C', 0, 0, 0);			
	keybd_event('C', 0, KEYEVENTF_KEYUP, 0);		
	keybd_event('P', 0, 0, 0);			
	keybd_event('P', 0, KEYEVENTF_KEYUP,  0);		
	Sleep(1000);        //停顿一秒
	std::cout << "兵河粘贴局面:  "<< fen <<  std::endl;

	if(turn)
	{
		keybd_event('E', 0, 0, 0);			//Alt Pres
		keybd_event('A', 0, 0, 0);			//Alt Pres
		keybd_event('E', 0, KEYEVENTF_KEYUP, 0);		
		keybd_event('A', 0, KEYEVENTF_KEYUP, 0);		
		Sleep(1000);        //停顿一秒
		keybd_event(VK_MENU,0xb8, KEYEVENTF_KEYUP,0);		
		std::cout << "兵河开始分析!"<< std::endl;
	}
}


void hwnd2mat(cv::Mat& dst, HWND hwnd)
{
	//HWND hwnd=GetDesktopWindow();
	HDC hwindowDC,hWndCompatibleDC;
	int height,width,srcheight,srcwidth;
	HBITMAP hBitmap;
	BITMAPINFOHEADER  bi;

	hwindowDC = GetDC(NULL);
	hWndCompatibleDC=CreateCompatibleDC(hwindowDC);
	SetStretchBltMode(hWndCompatibleDC,COLORONCOLOR);  

	RECT wndsize;    // get the height and width of the screen
	GetWindowRect(hwnd, &wndsize);

	srcheight = wndsize.bottom-wndsize.top;
	srcwidth = wndsize.right-wndsize.left;
	height = srcheight;  //change this to whatever size you want to resize to
	width = srcwidth;

	// create a bitmap
	hBitmap = CreateCompatibleBitmap( hwindowDC, width, height);
	bi.biSize = sizeof(BITMAPINFOHEADER);    //http://msdn.microsoft.com/en-us/library/windows/window/dd183402%28v=vs.85%29.aspx
	bi.biWidth = width;    
	bi.biHeight = -height;  //this is the line that makes it draw upside down or not
	bi.biPlanes = 1;    
	bi.biBitCount = g_biBitCount;		//此处参数应与CV_8UC4匹配才行。32位时兵河识别时崩溃。如果改用.jpg格式，则此处即使使用32位深度，存盘后也是24位的（win7默认jpg位深24位，PNG32位？）
	bi.biCompression = BI_RGB;    
	bi.biSizeImage = 0;  
	bi.biXPelsPerMeter = 0;    
	bi.biYPelsPerMeter = 0;    
	bi.biClrUsed = 0;    
	bi.biClrImportant = 0;

	// use the previously created device context with the bitmap
	SelectObject(hWndCompatibleDC, hBitmap);
	// copy from the window device context to the bitmap device context
	StretchBlt( hWndCompatibleDC, 0,0, width, height, hwindowDC, wndsize.left, wndsize.top, srcwidth, srcheight, SRCCOPY);	//change SRCCOPY to NOTSRCCOPY for wacky colors !
	
	Mat src;
	src.create(height,width, g_biBitCount==24? CV_8UC3:CV_8UC4);

	GetDIBits(hWndCompatibleDC, hBitmap, 0, height, src.data, (BITMAPINFO *)&bi, DIB_RGB_COLORS);			//copy from hWndCompatibleDC to hBitmap

	// avoid memory leak
	DeleteObject (hBitmap); DeleteDC(hWndCompatibleDC); ReleaseDC(hwnd, hwindowDC);

	src.copyTo(dst);
}

//这个方式可以阻挡窗口，参考screenCapture.cpp
void hwnd3mat(cv::Mat& dst, HWND hwnd)
{
	//HWND hwnd=GetDesktopWindow();

	HDC hwindowDC,hWndCompatibleDC;

	int height,width,srcheight,srcwidth;
	HBITMAP hBitmap;
	Mat src;
	BITMAPINFOHEADER  bi;

	hwindowDC=GetDC(hwnd);
	hWndCompatibleDC=CreateCompatibleDC(hwindowDC);
	SetStretchBltMode(hWndCompatibleDC,COLORONCOLOR);  

	RECT wndsize;    // get the height and width of the screen
	GetClientRect(hwnd, &wndsize);

	srcheight = wndsize.bottom;
	srcwidth = wndsize.right;
	height = wndsize.bottom;  //change this to whatever size you want to resize to
	width = wndsize.right;

	src.create(height,width,CV_8UC4);
	//src.create(height,width,CV_16UC4);


	// create a bitmap
	hBitmap = CreateCompatibleBitmap( hwindowDC, width, height);
	bi.biSize = sizeof(BITMAPINFOHEADER);    //http://msdn.microsoft.com/en-us/library/windows/window/dd183402%28v=vs.85%29.aspx
	bi.biWidth = width;    
	bi.biHeight = -height;  //this is the line that makes it draw upside down or not
	bi.biPlanes = 1;    
	bi.biBitCount = 32;    
	bi.biCompression = BI_RGB;    
	bi.biSizeImage = 0;  
	bi.biXPelsPerMeter = 0;    
	bi.biYPelsPerMeter = 0;    
	bi.biClrUsed = 0;    
	bi.biClrImportant = 0;

	// use the previously created device context with the bitmap
	SelectObject(hWndCompatibleDC, hBitmap);
	// copy from the window device context to the bitmap device context
	StretchBlt( hWndCompatibleDC, 0,0, width, height, hwindowDC, 0, 0,srcwidth,srcheight, SRCCOPY);	//change SRCCOPY to NOTSRCCOPY for wacky colors !
	GetDIBits(hWndCompatibleDC,hBitmap,0,height,src.data,(BITMAPINFO *)&bi,DIB_RGB_COLORS);			//copy from hWndCompatibleDC to hBitmap
	
	// avoid memory leak
	DeleteObject (hBitmap); DeleteDC(hWndCompatibleDC); ReleaseDC(hwnd, hwindowDC);
}

void hwnd5mat(cv::Mat& dst, HWND hwnd)//这个最简洁
{
	RECT wndsize;						 // get the height and width of the hwnd
	GetWindowRect(hwnd, &wndsize);

	int wndHeight =wndsize.bottom-wndsize.top;
	int wndWidth = wndsize.right-wndsize.left;

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);		//得到屏幕的分辨率的x
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);		//得到屏幕分辨率的y

	HDC hDesktopDC = GetDC( GetDesktopWindow() );			//得到屏幕的dc
	HDC hDesktopCompatibleDC = CreateCompatibleDC(hDesktopDC);
    HBITMAP hBitmap =CreateCompatibleBitmap(hDesktopDC,wndWidth,wndHeight);	
    SelectObject(hDesktopCompatibleDC,hBitmap); 
	
	StretchBlt( hDesktopCompatibleDC, 0, 0, wndWidth, wndHeight, hDesktopDC, wndsize.left, wndsize.top, wndWidth, wndHeight, SRCCOPY);	//第1种拷贝方法
	//BitBlt(hDesktopCompatibleDC, 0, 0, wndWidth,wndHeight, hDesktopDC, wndsize.left, wndsize.top,SRCCOPY);								//第2种拷贝方法

	Mat src(wndHeight, wndWidth, CV_8UC4);
	
	//第1种获取位图数据的方法, 比第2种简洁
	GetBitmapBits(hBitmap, wndWidth*wndHeight*4, src.data);	
	
	//第2种获取位图数据的方法
	//BITMAPINFO bi;
	//ZeroMemory(&bi, sizeof(BITMAPINFO));
	//bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	//bi.bmiHeader.biWidth = wndWidth;
	//bi.bmiHeader.biHeight = -wndHeight;		//negative so (0,0) is at top left
	//bi.bmiHeader.biPlanes = 1;
	//bi.bmiHeader.biBitCount = 32;			//注意与CV_8UC4这个参数的配套
	//bi.bmiHeader.biCompression = BI_RGB;    
	//bi.bmiHeader.biSizeImage = 0;  
	//bi.bmiHeader.biXPelsPerMeter = 0;    
	//bi.bmiHeader.biYPelsPerMeter = 0;    
	//bi.bmiHeader.biClrUsed = 0;    
	//bi.bmiHeader.biClrImportant = 0;
	//GetDIBits(hDesktopCompatibleDC, hBitmap, 0, wndHeight, src.data, &bi, DIB_RGB_COLORS);	

	//避免内在泄漏
	DeleteObject (hBitmap); ReleaseDC(NULL, hDesktopDC); ReleaseDC(NULL, hDesktopCompatibleDC);//避免内在泄漏

	//深拷贝到引用目标
	src.copyTo(dst);
}


//-----------------------------------【ShowHelpText( )函数】-----------------------------
//		 描述：输出一些帮助信息
//----------------------------------------------------------------------------------------------
void ShowHelpText()
{
	printf("\n\n\t\t\t   当前使用的OpenCV版本为：" CV_VERSION );
	printf("\n\n  ----------------------------------------------------------------------------\n");
}

int hist(  )
{

	//【1】载入素材图并显示
	Mat srcImage;
	srcImage=imread("tt2.jpg");
	imshow( "素材图", srcImage );

	system("color 3F");
	ShowHelpText();

	//【2】参数准备
	int bins = 256;
	int hist_size[] = {bins};
	float range[] = { 0, 256 };
	const float* ranges[] = { range};
	MatND redHist,grayHist,blueHist;
	int channels_r[] = {0};

	//【3】进行直方图的计算（红色分量部分）
	calcHist( &srcImage, 1, channels_r, Mat(), //不使用掩膜
		redHist, 1, hist_size, ranges,
		true, false );

	//【4】进行直方图的计算（绿色分量部分）
	int channels_g[] = {1};
	calcHist( &srcImage, 1, channels_g, Mat(), // do not use mask
		grayHist, 1, hist_size, ranges,
		true, // the histogram is uniform
		false );

	//【5】进行直方图的计算（蓝色分量部分）
	int channels_b[] = {2};
	calcHist( &srcImage, 1, channels_b, Mat(), // do not use mask
		blueHist, 1, hist_size, ranges,
		true, // the histogram is uniform
		false );

	//-----------------------绘制出三色直方图------------------------
	//参数准备
	double maxValue_red,maxValue_green,maxValue_blue;
	minMaxLoc(redHist, 0, &maxValue_red, 0, 0);
	minMaxLoc(grayHist, 0, &maxValue_green, 0, 0);
	minMaxLoc(blueHist, 0, &maxValue_blue, 0, 0);
	int scale = 1;
	int histHeight=256;
	Mat histImage = Mat::zeros(histHeight,bins*3, CV_8UC3);

	//正式开始绘制
	for(int i=0;i<bins;i++)
	{
		//参数准备
		float binValue_red = redHist.at<float>(i); 
		float binValue_green = grayHist.at<float>(i);
		float binValue_blue = blueHist.at<float>(i);
		int intensity_red = cvRound(binValue_red*histHeight/maxValue_red);  //要绘制的高度
		int intensity_green = cvRound(binValue_green*histHeight/maxValue_green);  //要绘制的高度
		int intensity_blue = cvRound(binValue_blue*histHeight/maxValue_blue);  //要绘制的高度

		//绘制红色分量的直方图
		rectangle(histImage,Point(i*scale,histHeight-1),
			Point((i+1)*scale - 1, histHeight - intensity_red),
			CV_RGB(255,0,0));

		//绘制绿色分量的直方图
		rectangle(histImage,Point((i+bins)*scale,histHeight-1),
			Point((i+bins+1)*scale - 1, histHeight - intensity_green),
			CV_RGB(0,255,0));

		//绘制蓝色分量的直方图
		rectangle(histImage,Point((i+bins*2)*scale,histHeight-1),
			Point((i+bins*2+1)*scale - 1, histHeight - intensity_blue),
			CV_RGB(0,0,255));

	}

	//在窗口中显示出绘制好的直方图
	imshow( "图像的RGB直方图", histImage );
	waitKey(0);
	return 0;
}


int hist1()
{
	//【1】载入原图并显示
	Mat srcImage = imread("tt2.jpg", 0);
	imshow("原图",srcImage);
	if(!srcImage.data) {cout << "fail to load image" << endl; 	return 0;}

	system("color 1F");
	ShowHelpText();

	//【2】定义变量
	MatND dstHist;       // 在cv中用CvHistogram *hist = cvCreateHist
	int dims = 1;
	float hranges[] = {0, 255};
	const float *ranges[] = {hranges};   // 这里需要为const类型
	int size = 256;
	int channels = 0;

	//【3】计算图像的直方图
	calcHist(&srcImage, 1, &channels, Mat(), dstHist, dims, &size, ranges);    // cv 中是cvCalcHist
	int scale = 1;

	Mat dstImage(size * scale, size, CV_8U, Scalar(0));
	//【4】获取最大值和最小值
	double minValue = 0;
	double maxValue = 0;
	minMaxLoc(dstHist,&minValue, &maxValue, 0, 0);  //  在cv中用的是cvGetMinMaxHistValue

	//【5】绘制出直方图
	int hpt = saturate_cast<int>(0.9 * size);
	for(int i = 0; i < 256; i++)
	{
		float binValue = dstHist.at<float>(i);           //   注意hist中是float类型    而在OpenCV1.0版中用cvQueryHistValue_1D
		int realValue = saturate_cast<int>(binValue * hpt/maxValue);
		rectangle(dstImage,Point(i*scale, size - 1), Point((i+1)*scale - 1, size - realValue), Scalar(255));
	}
	imshow("一维直方图", dstImage);
	waitKey(0);
	return 0;
}

int hist_hsv( )
{

	//【1】载入源图，转化为HSV颜色模型
	Mat srcImage, hsvImage;
	srcImage=imread("tt2.jpg");
	cvtColor(srcImage,hsvImage, CV_BGR2HSV);

	system("color 2F");
	ShowHelpText();

	//【2】参数准备
	//将色调量化为30个等级，将饱和度量化为32个等级
	int hueBinNum = 30;//色调的直方图直条数量
	int saturationBinNum = 32;//饱和度的直方图直条数量
	int histSize[ ] = {hueBinNum, saturationBinNum};
	// 定义色调的变化范围为0到179
	float hueRanges[] = { 0, 180 };
	//定义饱和度的变化范围为0（黑、白、灰）到255（纯光谱颜色）
	float saturationRanges[] = { 0, 256 };
	const float* ranges[] = { hueRanges, saturationRanges };
	MatND dstHist;
	//参数准备，calcHist函数中将计算第0通道和第1通道的直方图
	int channels[] = {0, 1};

	//【3】正式调用calcHist，进行直方图计算
	calcHist( &hsvImage,//输入的数组
		1, //数组个数为1
		channels,//通道索引
		Mat(), //不使用掩膜
		dstHist, //输出的目标直方图
		2, //需要计算的直方图的维度为2
		histSize, //存放每个维度的直方图尺寸的数组
		ranges,//每一维数值的取值范围数组
		true, // 指示直方图是否均匀的标识符，true表示均匀的直方图
		false );//累计标识符，false表示直方图在配置阶段会被清零

	//【4】为绘制直方图准备参数
	double maxValue=0;//最大值
	minMaxLoc(dstHist, 0, &maxValue, 0, 0);//查找数组和子数组的全局最小值和最大值存入maxValue中
	int scale = 10;
	Mat histImg = Mat::zeros(saturationBinNum*scale, hueBinNum*10, CV_8UC3);

	//【5】双层循环，进行直方图绘制
	for( int hue = 0; hue < hueBinNum; hue++ )
		for( int saturation = 0; saturation < saturationBinNum; saturation++ )
		{
			float binValue = dstHist.at<float>(hue, saturation);//直方图组距的值
			int intensity = cvRound(binValue*255/maxValue);//强度

			//正式进行绘制
			rectangle( histImg, Point(hue*scale, saturation*scale),
				Point( (hue+1)*scale - 1, (saturation+1)*scale - 1),
				Scalar::all(intensity),
				CV_FILLED );
		}

		//【6】显示效果图
		imshow( "素材图", srcImage );
		imshow( "H-S 直方图", histImg );

		waitKey();
		return 0;
}

double hist_comp(Mat srcImage_base, Mat srcImage_test1)
{
	Mat hsvImage_base;
	Mat hsvImage_test1;

	//srcImage_base = imread( "1.jpg",1 );
	//srcImage_test1 = imread( "2.jpg", 1 );

	//将图像由BGR色彩空间转换到 HSV色彩空间
	cvtColor( srcImage_base, hsvImage_base, CV_BGR2HSV );
	cvtColor( srcImage_test1, hsvImage_test1, CV_BGR2HSV );

	//初始化计算直方图需要的实参
	int h_bins = 50;	int s_bins = 60;	
	int histSize[] = { h_bins, s_bins };
	float h_ranges[] = { 0, 256 };		// hue的取值范围从0到256
	float s_ranges[] = { 0, 180 };		//saturation取值范围从0到180
	const float* ranges[] = { h_ranges, s_ranges };
	int channels[] = { 0, 1 };// 使用第0和第1通道

	//创建储存直方图的 MatND 类的实例:
	MatND baseHist;
	MatND testHist1;

	//计算基准图像，两张测试图像，半身基准图像的HSV直方图:
	calcHist( &hsvImage_base, 1, channels, Mat(), baseHist, 2, histSize, ranges, true, false );
	normalize( baseHist, baseHist, 0, 1, NORM_MINMAX, -1, Mat() );

	calcHist( &hsvImage_test1, 1, channels, Mat(), testHist1, 2, histSize, ranges, true, false );
	normalize( testHist1, testHist1, 0, 1, NORM_MINMAX, -1, Mat() );
	
	double base_test1 = compareHist( baseHist, testHist1, CV_COMP_CORREL );			//int compare_method = 0~3

	return base_test1;
}