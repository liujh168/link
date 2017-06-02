#include <windows.h>
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/legacy/legacy.hpp"
#include "toolsCV.h"

//图像相似度计算
//1、直方图方法
//方法描述：有两幅图像patch(当然也可是整幅图像)，分别计算两幅图像的直方图，并将直方图进行归一化，然后按照某种距离度量的标准进行相似度的测量。
//方法的思想：基于简单的向量相似度来对图像相似度进行度量。
//优点：直方图能够很好的归一化，比如256个bin条，那么即使是不同分辨率的图像都可以直接通过其直方图来计算相似度，计算量适中。比较适合描述难以自动分割的图像。
//缺点：直方图反应的是图像灰度值得概率分布，无图像空间位置信息，因此，常有误判；信息丢失量较大，因此单一的通过直方图进行匹配显得有点力不从心。
//（输入输出都是灰度图像，且设定的灰度级为8，及0-255）

double getHistSimilarity(const Mat& I1, const Mat& I2)
{
	int histSize = 256;
	float range[] = {0,256};
	const float* histRange = {range};
	bool uniform = true;
	bool accumulate = false;

	Mat hist1,hist2;

	calcHist(&I1,1,0,Mat(),hist1,1,&histSize,&histRange,uniform,accumulate);
	normalize(hist1,hist1,0,1,NORM_MINMAX,-1,Mat());

	calcHist(&I2,1,0,Mat(),hist2,1,&histSize,&histRange,uniform,accumulate);
	normalize(hist2,hist2,0,1,NORM_MINMAX,-1,Mat());

	return compareHist(hist1, hist2, CV_COMP_CORREL);

}


//2、基于特征点方法
//方法描述：统计两个图像patch中匹配的特征点数，如果相似的特征点数比例最大，则认为最相似，最匹配
//方法思想：图像可以中特征点来描述，比如sift特征点，LK光流法中的角点等等。这样相似度的测量就转变为特征点的匹配了。
//以前做过一些实验，关于特征点匹配的，对一幅图像进行仿射变换，然后匹配两者之间的特征点，选取的特征点有sift和快速的sift变形版本surf等。
//方法优点：能被选作特征点的大致要满足不变性，尺度不变性，旋转不变等。这样图像的相似度计算也就具备了这些不变性。
//方法缺点：特征点的匹配计算速度比较慢，同时特征点也有可能出现错误匹配的现象。


//3、基于峰值信噪比（PSNR）的方法
//当我们想检查压缩视频带来的细微差异的时候，就需要构建一个能够逐帧比较差视频差异的系统。最
//常用的比较算法是PSNR( Peak signal-to-noise ratio)。这是个使用“局部均值误差”来判断差异的最简单的方法，假设有这两幅图像：I1和I2，它们的行列数分别是i，j，有c个通道。每个像素的每个通道的值占用一个字节，值域[0,255]。注意当两幅图像的相同的话，MSE的值会变成0。这样会导致PSNR的公式会除以0而变得没有意义。所以我们需要单独的处理这样的特殊情况。此外由于像素的动态范围很广，在处理时会使用对数变换来缩小范围。
//在考察压缩后的视频时，这个值大约在30到50之间，数字越大则表明压缩质量越好。如果图像差异很明显，就可能会得到15甚至更低的值。PSNR算法简单，检查的速度也很快。但是其呈现的差异值有时候和人的主观感受不成比例。所以有另外一种称作 结构相似性 的算法做出了这方面的改进。
double getPSNR(const Mat& I1, const Mat& I2)  
{
	Mat s1;
	absdiff(I1, I2, s1);       // |I1 - I2|
	s1.convertTo(s1, CV_32F);  // cannot make a square on 8 bits
	s1 = s1.mul(s1);           // |I1 - I2|^2

	Scalar s = sum(s1);         // sum elements per channel

	double sse = s.val[0] + s.val[1] + s.val[2]; // sum channels

	if( sse <= 1e-10) // for small values return zero
		return 0;
	else
	{
		double  mse =sse /(double)(I1.channels() * I1.total());
		double psnr = 10.0*log10((255*255)/mse);
		return psnr;
	}
}

//以下是另外找来的代码，未测试
double psnr(Mat &I1, Mat &I2)
{
	Mat s1;
	absdiff(I1, I2, s1);
	s1.convertTo(s1, CV_32F);//转换为32位的float类型，8位不能计算平方
	s1 = s1.mul(s1);
	Scalar s = sum(s1);  //计算每个通道的和
	double sse = s.val[0] + s.val[1] + s.val[2];
	if( sse <= 1e-10) // for small values return zero
		return 0;
	else
	{
		double mse = sse / (double)(I1.channels() * I1.total()); //  sse/(w*h*3)
		double psnr = 10.0 * log10((255*255)/mse);
		return psnr;
	}
}

//4、基于结构相似性（SSIM,structural similarity (SSIM) index measurement）的方法
//结构相似性理论认为，自然图像信号是高度结构化的，即像素间有很强的相关性，特别是空域中最接近的像素，这种相关性蕴含着视觉场景中物体结构的重要信息；HVS的主要功能是从视野中提取结构信息，可以用对结构信息的度量作为图像感知质量的近似。结构相似性理论是一种不同于以往模拟HVS低阶的组成结构的全新思想，与基于HVS特性的方法相比，最大的区别是自顶向下与自底向上的区别。这一新思想的关键是从对感知误差度量到对感知结构失真度量的转变。它没有试图通过累加与心理物理学简单认知模式有关的误差来估计图像质量，而是直接估计两个复杂结构信号的结构改变，从而在某种程度上绕开了自然图像内容复杂性及多通道去相关的问题.作为结构相似性理论的实现，结构相似度指数从图像组成的角度将结构信息定义为独立于亮度、对比度的，反映场景中物体结构的属性，并将失真建模为亮度、对比度和结构三个不同因素的组合。用均值作为亮度的估计，标准差作为对比度的估计，协方差作为结构相似程度的度量。
//数据以scalar格式保存，读取scalar内的数据即可获取相应的相似度值，其中值的范围在0～1之间，1为完全一致，0为完全不一至。
Scalar getMSSIM( const Mat& i1, const Mat& i2)
{
	assert(i1.rows==i2.rows && i1.cols==i2.cols);			//图像大小一样才能比较相似性

	const double C1 = 6.5025, C2 = 58.5225;
	/***************************** INITS **********************************/
	int d     = CV_32F;

	Mat I1, I2;
	i1.convertTo(I1, d);           // cannot calculate on one byte large values
	i2.convertTo(I2, d);

	Mat I2_2   = I2.mul(I2);        // I2^2
	Mat I1_2   = I1.mul(I1);        // I1^2
	Mat I1_I2  = I1.mul(I2);        // I1 * I2

	/*************************** END INITS **********************************/

	Mat mu1, mu2;   // PRELIMINARY COMPUTING
	GaussianBlur(I1, mu1, Size(11, 11), 1.5);
	GaussianBlur(I2, mu2, Size(11, 11), 1.5);

	Mat mu1_2   =   mu1.mul(mu1);
	Mat mu2_2   =   mu2.mul(mu2);
	Mat mu1_mu2 =   mu1.mul(mu2);

	Mat sigma1_2, sigma2_2, sigma12;

	GaussianBlur(I1_2, sigma1_2, Size(11, 11), 1.5);
	sigma1_2 -= mu1_2;

	GaussianBlur(I2_2, sigma2_2, Size(11, 11), 1.5);
	sigma2_2 -= mu2_2;

	GaussianBlur(I1_I2, sigma12, Size(11, 11), 1.5);
	sigma12 -= mu1_mu2;

	///////////////////////////////// FORMULA ////////////////////////////////
	Mat t1, t2, t3;

	t1 = 2 * mu1_mu2 + C1;
	t2 = 2 * sigma12 + C2;
	t3 = t1.mul(t2);              // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))

	t1 = mu1_2 + mu2_2 + C1;
	t2 = sigma1_2 + sigma2_2 + C2;
	t1 = t1.mul(t2);               // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))

	Mat ssim_map;
	divide(t3, t1, ssim_map);      // ssim_map =  t3./t1;

	Scalar mssim = mean( ssim_map ); // mssim = average of ssim map
	return mssim;
}

//另一个公式，与上面基本一样
double ssim(Mat &i1, Mat & i2)
{
	const double C1 = 6.5025, C2 = 58.5225;
	int d = CV_32F;
	Mat I1, I2;
	i1.convertTo(I1, d);
	i2.convertTo(I2, d);
	Mat I1_2 = I1.mul(I1);
	Mat I2_2 = I2.mul(I2);
	Mat I1_I2 = I1.mul(I2);
	Mat mu1, mu2;
	GaussianBlur(I1, mu1, Size(11,11), 1.5);
	GaussianBlur(I2, mu2, Size(11,11), 1.5);
	Mat mu1_2 = mu1.mul(mu1);
	Mat mu2_2 = mu2.mul(mu2);
	Mat mu1_mu2 = mu1.mul(mu2);
	Mat sigma1_2, sigam2_2, sigam12;
	GaussianBlur(I1_2, sigma1_2, Size(11, 11), 1.5);
	sigma1_2 -= mu1_2;

	GaussianBlur(I2_2, sigam2_2, Size(11, 11), 1.5);
	sigam2_2 -= mu2_2;

	GaussianBlur(I1_I2, sigam12, Size(11, 11), 1.5);
	sigam12 -= mu1_mu2;
	Mat t1, t2, t3;
	t1 = 2 * mu1_mu2 + C1;
	t2 = 2 * sigam12 + C2;
	t3 = t1.mul(t2);

	t1 = mu1_2 + mu2_2 + C1;
	t2 = sigma1_2 + sigam2_2 + C2;
	t1 = t1.mul(t2);

	Mat ssim_map;
	divide(t3, t1, ssim_map);
	Scalar mssim = mean(ssim_map);
//	return mssim;
	double ssim = (mssim.val[0] + mssim.val[1] + mssim.val[2]) /3;
	return ssim;
}

//图像模板匹配
//
//一般而言，源图像与模板图像patch尺寸一样的话，可以直接使用上面介绍的图像相似度测量的方法；如果源图像与模板图像尺寸不一样，通常需要进行滑动匹配窗口，扫面个整幅图像获得最好的匹配patch。
//
//在OpenCV中对应的函数为：matchTemplate()：函数功能是在输入图像中滑动窗口寻找各个位置与模板图像patch的相似度。相似度的评价标准（匹配方法）有：CV_TM_SQDIFF平方差匹配法（相似度越高，值越小），CV_TM_CCORR相关匹配法（采用乘法操作，相似度越高值越大），CV_TM_CCOEFF相关系数匹配法（1表示最好的匹配，-1表示最差的匹配）。
//
//有一种新的用来计算相似度或者进行距离度量的方法：EMD，Earth Mover‘s Distances
//
//EMD is defined as the minimal cost that must be paid to transform one histograminto the other, where there is a “ground distance” between the basic featuresthat are aggregated into the histogram。
//
//光线变化能引起图像颜色值的漂移，尽管漂移没有改变颜色直方图的形状，但漂移引起了颜色值位置的变化，从而可能导致匹配策略失效。而EMD是一种度量准则，度量怎样将一个直方图转变为另一个直方图的形状，包括移动直方图的部分（或全部）到一个新的位置，可以在任意维度的直方图上进行这种度量。
//
//在OpenCV中有相应的计算方法：cvCalcEMD2()。结合着opencv支持库，计算直方图均衡后与原图的HSV颜色空间直方图之间的EMD距离。


//封装后再测试（两个封装函数，一个主程序测试）
double match(cv::Mat image, cv::Mat tepl, cv::Point &point, int method)
{
    int result_cols =  image.cols - tepl.cols + 1;
    int result_rows = image.rows - tepl.rows + 1;

    cv::Mat result = cv::Mat( result_cols, result_rows, CV_32FC1 );
    cv::matchTemplate( image, tepl, result, method );

    double minVal, maxVal;
    cv::Point minLoc, maxLoc;
    cv::minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );

    switch(method)
    {
    case CV_TM_SQDIFF:
    case CV_TM_SQDIFF_NORMED:
        point = minLoc;
        return minVal;
        break;

    default:
        point = maxLoc;
        return maxVal;
        break;
    }
}

//我们还可以去掉模板大小对匹配度的影响：
double match1(cv::Mat image, cv::Mat tepl, cv::Point &point, int method)
{
    int result_cols =  image.cols - tepl.cols + 1;
    int result_rows = image.rows - tepl.rows + 1;

    cv::Mat result = cv::Mat( result_cols, result_rows, CV_32FC1 );
    cv::matchTemplate( image, tepl, result, method );

    double minVal, maxVal;
    cv::Point minLoc, maxLoc;
    cv::minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );

    switch(method)
    {
    case CV_TM_SQDIFF:
        point = minLoc;
        return minVal / (tepl.cols * tepl.cols);
        break;
    case CV_TM_SQDIFF_NORMED:
        point = minLoc;
        return minVal;
        break;
    case CV_TM_CCORR:
    case CV_TM_CCOEFF:
        point = maxLoc;
        return maxVal / (tepl.cols * tepl.cols);
        break;
    case CV_TM_CCORR_NORMED:
    case CV_TM_CCOEFF_NORMED:
    default:
        point = maxLoc;
        return maxVal;
        break;
    }
}

int mat2solution(Mat boardPath, Point p)
{
	int width = 40;		//分辨率	Point p(52, 62);
	
	int dx=54;

	double rMax=0;

	Mat rRook;
	Mat lRook = boardPath(Rect(p.x-width/2, p.y-width/2, width, width));
	imwrite("lRook.jpg", lRook);

	Point ptRook;
	//计算dx, 假定dy=dx
	for(int RRookx = p.x+8*width; RRookx < boardPath.cols-width/2 ; RRookx++)		//从至少第 8个子的x开始，可节省计算量。
	{
		rRook = boardPath(Rect(RRookx - width/2, p.y-width/2, width, width));

		double r= ssim(lRook,rRook);

		//从相似度中最大的一个就认定为右车了，没考虑阈值。因此可能不准确
		if(r>rMax) 
		{
			rMax=r;
			ptRook.x=RRookx;
			ptRook.y=p.y;
			dx=(RRookx-p.x)/8;
			//dy=dx;

			cout<<  "ssim:  " << r << endl;
			cout<<  "dx:  " << dx << endl;
			imwrite("rRook.jpg",rRook);
			//imshow("候选右车：", rRook);
			//waitKey(0);
		}
	}
	
	return dx;
}

int myCanny()
{
	//载入原始图    
    Mat src = imread("qq.jpg");  //工程目录下应该有一张名为1.jpg的素材图  
    Mat src1=src.clone();  
  
    //显示原始图   
    //imshow("【原始图】Canny边缘检测", src);   
  
    //----------------------------------------------------------------------------------  
    //  一、最简单的canny用法，拿到原图后直接用。  
    //----------------------------------------------------------------------------------  
    Canny( src, src, 150, 100,3 );  
    imshow("【效果图】Canny边缘检测", src);   
  
      
    //----------------------------------------------------------------------------------  
    //  二、高阶的canny用法，转成灰度图，降噪，用canny，最后将得到的边缘作为掩码，拷贝原图到效果图上，得到彩色的边缘图  
    //----------------------------------------------------------------------------------  
    Mat dst,edge,gray;  
  
    // 【1】创建与src同类型和大小的矩阵(dst)  
    dst.create( src1.size(), src1.type() );  
  
    // 【2】将原图像转换为灰度图像  
    cvtColor( src1, gray, CV_BGR2GRAY );  
  
    // 【3】先用使用 3x3内核来降噪  
    blur( gray, edge, Size(3,3) );  
  
    // 【4】运行Canny算子  
    Canny( edge, edge, 3, 9,3 );  
    //imshow("【效果图】运行Canny算子 ", edge);   
  
    //【5】将g_dstImage内的所有元素设置为0   
    dst = Scalar::all(0);  
    //imshow("【效果图】将g_dstImage内的所有元素设置为0", dst);   
  
    //【6】使用Canny算子输出的边缘图g_cannyDetectedEdges作为掩码，来将原图g_srcImage拷到目标图g_dstImage中  
    src1.copyTo( dst, edge);  
  
    //【7】显示效果图   
    //imshow("【效果图】Canny边缘检测2", dst);   
  
    waitKey(0);   
  
    return 0;  
}

int mySobel( )  
{  
    //【0】创建 grad_x 和 grad_y 矩阵  
    Mat grad_x, grad_y;  
    Mat abs_grad_x, abs_grad_y,dst;  
  
    //【1】载入原始图    
    Mat src = imread("board.png");  //工程目录下应该有一张名为1.jpg的素材图  
  
    //【2】显示原始图   
    imshow("【原始图】sobel边缘检测", src);   
  
    //【3】求 X方向梯度  
    Sobel( src, grad_x, CV_16S, 1, 0, 3, 1, 1, BORDER_DEFAULT );  
    convertScaleAbs( grad_x, abs_grad_x );  
    imshow("【效果图】 X方向Sobel", abs_grad_x);   
  
    //【4】求Y方向梯度  
    Sobel( src, grad_y, CV_16S, 0, 1, 3, 1, 1, BORDER_DEFAULT );  
    convertScaleAbs( grad_y, abs_grad_y );  
    imshow("【效果图】Y方向Sobel", abs_grad_y);   
  
    //【5】合并梯度(近似)  
    addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, dst );  
    imshow("【效果图】整体方向Sobel", dst);   
  
    waitKey(0);   
    return 0;   
}  



//-----------------------------------【程序说明】----------------------------------------------??
//??????程序名称:：《【OpenCV入门教程之十七】OpenCV重映射?&?SURF特征点检测合辑?》?博文配套源码?之【SURF特征点检测】??
//??????开发所用IDE版本：Visual?Studio?2010??
//??????开发所用OpenCV版本：???2.4.9??
//??????2014年6月15日?Created?by?浅墨??
//??????配套博文链接：?http://blog.csdn.net/poem_qianmo/article/details/30974513??
//??????PS:程序结合配合博文学习效果更佳??
//??????浅墨的微博：@浅墨_毛星云?http://weibo.com/1723155442??
//??????浅墨的知乎：http://www.zhihu.com/people/mao-xing-yun??
//??????浅墨的豆瓣：http://www.douban.com/people/53426472/??
//----------------------------------------------------------------------------------------------??

int myKeyPoint()
{
	//【1】载入源图片并显示??
	Mat srcImage1= imread("board.png",1);
	Mat srcImage2 = imread("pai.jpg",1);
	if(!srcImage1.data|| !srcImage2.data)//检测是否读取成功??
	{ 
		printf("读取图片错误，请确定目录下是否有imread函数指定名称的图片存在~！\n");
		waitKey(0);
		return false;
	}
	imshow("原始图1",srcImage1);
	imshow("原始图2",srcImage2);
	
	//【2】定义需要用到的变量和类??
	int minHessian = 400;//定义SURF中的hessian阈值特征点检测算子??
	SurfFeatureDetector detector(minHessian);//定义一个SurfFeatureDetector（SURF）?特征检测类对象??
	std::vector<KeyPoint> keypoints_1, keypoints_2;//vector模板类是能够存放任意类型的动态数组，能够增加和压缩数据??
	
	//【3】调用detect函数检测出SURF特征关键点，保存在vector容器中??
	detector.detect( srcImage1, keypoints_1 );
	detector.detect( srcImage2, keypoints_2 );
	
	//【4】绘制特征关键点??
	Mat img_keypoints_1; Mat img_keypoints_2;
	drawKeypoints(srcImage1, keypoints_1, img_keypoints_1, Scalar::all(-1), DrawMatchesFlags::DEFAULT );
	drawKeypoints(srcImage2, keypoints_2, img_keypoints_2, Scalar::all(-1), DrawMatchesFlags::DEFAULT );
	
	//【5】显示效果图??
	imshow("特征点检测效果图1", img_keypoints_1 );
	imshow("特征点检测效果图2", img_keypoints_2 );
	
	waitKey(0);
	
	return 0;
}


int mySurf()
{	//这个效果不行啊
	//【1】载入源图片并显示??
	Mat srcImage1= imread("board.png",1);
	Mat srcImage2 = imread("pawn.png",1);
	if(!srcImage1.data|| !srcImage2.data)//检测是否读取成功??
	{ 
		printf("读取图片错误，请确定目录下是否有imread函数指定名称的图片存在~！\n");
		waitKey(0);
		return false;
	}
	imshow("原始图1",srcImage1);
	imshow("原始图2",srcImage2);

	//【2】使用SURF算子检测关键点??
	int minHessian = 700;	//SURF算法中的hessian阈值??
	SurfFeatureDetector detector( minHessian );		//定义一个SurfFeatureDetector（SURF）?特征检测类对象????
	std::vector<KeyPoint> keyPoint1, keyPoints2;	//vector模板类，存放任意类型的动态数组

	//【3】调用detect函数检测出SURF特征关键点，保存在vector容器中??
	detector.detect( srcImage1, keyPoint1);
	detector.detect( srcImage2, keyPoints2 );

	//【4】计算描述符（特征向量）  
	SurfDescriptorExtractor extractor;  
	Mat descriptors1, descriptors2;  
	extractor.compute( srcImage1, keyPoint1, descriptors1 );  
	extractor.compute( srcImage2, keyPoints2, descriptors2 );  

	//【5】使用BruteForce进行匹配  
	// 实例化一个匹配器  
	BruteForceMatcher< L2<float> > matcher;  
	std::vector< DMatch > matches;  
	//匹配两幅图中的描述子（descriptors）  
	matcher.match( descriptors1, descriptors2, matches );  

	//【6】绘制从两个图像中匹配出的关键点  
	Mat imgMatches;  
	drawMatches( srcImage1, keyPoint1, srcImage2, keyPoints2, matches, imgMatches );//进行绘制  

	//【7】显示效果图  
	imshow("匹配图", imgMatches );  

	waitKey(0);

	return 0;

}



//----------------------------------【ROI_AddImage(?)函数】----------------------------------??
//?函数名：ROI_AddImage（）??
//?????描述：利用感兴趣区域ROI实现图像叠加??
//----------------------------------------------------------------------------------------------??
bool ROI_AddImage()
{
	//【1】读入图像??
	Mat srcImage1= imread("board.png");
	Mat logoImage= imread("pawn.png");
	if(!srcImage1.data)
	{
		printf("你妹，读取srcImage1错误~！\n");
		return false;
	}
	if(!logoImage.data)
	{
		printf("你妹，读取logoImage错误~！\n");
		return false;
	}

	//【2】定义一个Mat类型并给其设定ROI区域??
	Mat imageROI= srcImage1(Rect(320,350,120,122));
	imshow("ROI",imageROI);

	////【3】加载掩模（必须是灰度图）??
	Mat mask= imread("pai.jpg",0);

	////【4】将掩膜拷贝到ROI
	logoImage.copyTo( imageROI, mask);
	//logoImage.copyTo( imageROI, imageROI);

	//【5】显示结果??
	//namedWindow("<1>利用ROI实现图像叠加示例窗口");
	//imshow("<1>利用ROI实现图像叠加示例窗口",srcImage1);

	waitKey(0);
	return 0;
}

//
void hwnd2mat()
{
    HWND hwnd;
    hwnd=GetDesktopWindow();
    HDC hwindowDC,hwindowCompatibleDC;

    int height,width,srcheight,srcwidth;
    HBITMAP hbwindow;
    Mat src;
    BITMAPINFOHEADER  bi;

    hwindowDC=GetDC(hwnd);
    hwindowCompatibleDC=CreateCompatibleDC(hwindowDC);
    SetStretchBltMode(hwindowCompatibleDC,COLORONCOLOR);  

    RECT windowsize;    // get the height and width of the screen
    GetClientRect(hwnd, &windowsize);

    srcheight = windowsize.bottom;
    srcwidth = windowsize.right;
    height = windowsize.bottom;  //change this to whatever size you want to resize to
    width = windowsize.right;

    src.create(height,width,CV_8UC4);

    // create a bitmap
    hbwindow = CreateCompatibleBitmap( hwindowDC, width, height);
    bi.biSize = sizeof(BITMAPINFOHEADER);   
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
    SelectObject(hwindowCompatibleDC, hbwindow);
    // copy from the window device context to the bitmap device context
    StretchBlt( hwindowCompatibleDC, 0,0, width, height, hwindowDC, 0, 0,srcwidth,srcheight, SRCCOPY); //change SRCCOPY to NOTSRCCOPY for wacky colors !
    GetDIBits(hwindowCompatibleDC,hbwindow,0,height,src.data,(BITMAPINFO *)&bi,DIB_RGB_COLORS);  //copy from hwindowCompatibleDC to hbwindow

    // avoid memory leak,非常重要，特别是实时提取需要循环时，不添加会使内存消耗至full，并使程序卡死
    DeleteObject (hbwindow); DeleteDC(hwindowCompatibleDC); ReleaseDC(hwnd, hwindowDC);
	
    //screenImg=src(Rect(0,0,sWidth,sHeight));//通过GetDIBits将数据流从Bitmap copy到Mat 
    //因为数据是在jin10.com公布的，right time right place,像EIA，每周三晚上11：30出将在，同一个网页的同一个地方（50*25）。
    //所以，只需要提前将“埋伏”的地方设好（还可以加个判断，只需要11：30后出的数据）
}

bool MyShowImage( const cv::Mat& img, HDC hdc, const RECT& rect )
{
	CvMat _img = img;
	const CvArr* arr = &_img;

	CvMat stub;
	CvMat* image = cvGetMat( arr, &stub );

	// 构造BITMAPINFO头
	SIZE size = { image->width, image->height };
	int channels = 3;
	BITMAPINFO binfo;
	memset( &binfo, 0, sizeof(binfo));
	BITMAPINFOHEADER& bmih = binfo.bmiHeader;
	bmih.biSize = sizeof(BITMAPINFOHEADER);
	bmih.biWidth = size.cx;
	bmih.biHeight = abs(size.cy);
	bmih.biPlanes = 1;
	bmih.biBitCount = (unsigned short)(channels*8);
	bmih.biCompression = BI_RGB;

	void* dst_ptr = 0;
	HBITMAP hb = CreateDIBSection( hdc, &binfo, DIB_RGB_COLORS, &dst_ptr, 0, 0 );

	HDC windowdc = ::CreateCompatibleDC( hdc );
	SelectObject( windowdc, hb );

	CvMat dst;
	cvInitMatHeader( &dst, size.cy, size.cx, CV_8UC3, dst_ptr, (size.cx*channels + 3)&-4 );

	int origin = ((IplImage*)arr)->origin;
	cvConvertImage(image, &dst, origin==0?CV_CVTIMG_FLIP : 0 );

	// 显示
	SetStretchBltMode( hdc, COLORONCOLOR );
	//BitBlt( hdc, 0, 0, size.cx, size.cy, windowdc, 0, 0, SRCCOPY );
	StretchBlt( hdc, 0, 0, rect.right-rect.left, rect.bottom-rect.top, windowdc, 0, 0, size.cx, size.cy, SRCCOPY );

	return 0;
}

int test_Myshowimage()
{
	const char* imagename = "1.tif";

	cv::Mat img = cv::imread( imagename );
	if( img.empty() || !img.data )
	{
		fprintf(stderr, "Can not load image %s\n", imagename);
		return -1;
	}

	//cv::namedWindow("image", CV_WINDOW_AUTOSIZE);
	//cv::imshow("image", img);
	//cv::waitKey();

	// 随便弄个窗体，然后显示上去
	HWND hwnd = ::GetConsoleWindow();
	RECT rect;
	GetWindowRect( hwnd, &rect );
	MyShowImage( img, ::GetWindowDC(hwnd), rect );

	return 0;
}


// hbitmap convert to IplImage   
IplImage* hBitmapToIpl(HBITMAP hBmp)   
{   
    BITMAP bmp;    
    GetObject(hBmp,sizeof(BITMAP),&bmp);    
  
    // get channels which equal 1 2 3 or 4    
    // bmBitsPixel :   
    // Specifies the number of bits    
    // required to indicate the color of a pixel.    
    int nChannels = bmp.bmBitsPixel == 1 ? 1 : bmp.bmBitsPixel/8 ;   
  
    // get depth color bitmap or grayscale   
    int depth = bmp.bmBitsPixel == 1 ? IPL_DEPTH_1U : IPL_DEPTH_8U;    
       
       
    // create header image   
    IplImage* img = cvCreateImage(cvSize(bmp.bmWidth,bmp.bmHeight),depth,nChannels);    
       
    // allocat memory for the pBuffer   
    BYTE *pBuffer = new BYTE[bmp.bmHeight*bmp.bmWidth*nChannels];    
       
    // copies the bitmap bits of a specified device-dependent bitmap into a buffer   
    GetBitmapBits(hBmp,bmp.bmHeight*bmp.bmWidth*nChannels,pBuffer);    
       
    // copy data to the imagedata   
    memcpy(img->imageData,pBuffer,bmp.bmHeight*bmp.bmWidth*nChannels);   
    delete pBuffer;    
  
    // create the image   
    IplImage *dst = cvCreateImage(cvGetSize(img),img->depth,3);   
    // convert color   
    cvCvtColor(img,dst,CV_BGRA2BGR);   
    cvReleaseImage(&img);   
    return dst;   
} 

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


//这是两个独立的截屏函数，可对照参考screenCapture.cpp
Mat hwnd2mat(HWND hwnd)
{

    HDC hwindowDC,hwindowCompatibleDC;

    int height,width,srcheight,srcwidth;
    HBITMAP hbwindow;
    Mat src;
    BITMAPINFOHEADER  bi;

    hwindowDC=GetDC(hwnd);
    hwindowCompatibleDC=CreateCompatibleDC(hwindowDC);
    SetStretchBltMode(hwindowCompatibleDC,COLORONCOLOR);  

    RECT windowsize;    // get the height and width of the screen
    GetClientRect(hwnd, &windowsize);

    srcheight = windowsize.bottom;
    srcwidth = windowsize.right;
    height = windowsize.bottom;  //change this to whatever size you want to resize to
    width = windowsize.right;

    src.create(height,width,CV_8UC4);
    //src.create(height,width,CV_16UC4);
	

    // create a bitmap
    hbwindow = CreateCompatibleBitmap( hwindowDC, width, height);
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
    SelectObject(hwindowCompatibleDC, hbwindow);
    // copy from the window device context to the bitmap device context
    StretchBlt( hwindowCompatibleDC, 0,0, width, height, hwindowDC, 0, 0,srcwidth,srcheight, SRCCOPY);	//change SRCCOPY to NOTSRCCOPY for wacky colors !
    GetDIBits(hwindowCompatibleDC,hbwindow,0,height,src.data,(BITMAPINFO *)&bi,DIB_RGB_COLORS);			//copy from hwindowCompatibleDC to hbwindow

    // avoid memory leak
    DeleteObject (hbwindow); DeleteDC(hwindowCompatibleDC); ReleaseDC(hwnd, hwindowDC);

    return src;
}

//Note that no error handling done here to make it simple to understand but you have to do error handling in your code!
int cap()
{
    int x_size = 800, y_size = 600; // <-- Your res for the image


    HBITMAP hBitmap; // <-- The image represented by hBitmap
    Mat matBitmap; // <-- The image represented by mat

    // Initialize DCs
    HDC hdcSys = GetDC(NULL); // Get DC of the target capture..
    HDC hdcMem = CreateCompatibleDC(hdcSys); // Create compatible DC 

    void *ptrBitmapPixels; // <-- Pointer variable that will contain the potinter for the pixels

    // Create hBitmap with Pointer to the pixels of the Bitmap
    BITMAPINFO bi; HDC hdc;
    ZeroMemory(&bi, sizeof(BITMAPINFO));
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = x_size;
    bi.bmiHeader.biHeight = -y_size;  //negative so (0,0) is at top left
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    hdc = GetDC(NULL);
    hBitmap = CreateDIBSection(hdc, &bi, DIB_RGB_COLORS, &ptrBitmapPixels, NULL, 0);
    // ^^ The output: hBitmap & ptrBitmapPixels

    // Set hBitmap in the hdcMem 
    SelectObject(hdcMem, hBitmap);

    // Set matBitmap to point to the pixels of the hBitmap
    matBitmap = Mat(y_size, x_size, CV_8UC4, ptrBitmapPixels, 0);
    //              ^^ note: first it is y, then it is x. very confusing
    // * SETUP DONE *
    // Now update the pixels using BitBlt
    BitBlt(hdcMem, 0, 0, x_size, y_size, hdcSys, 0, 0, SRCCOPY);

    // Just to do some image processing on the pixels.. (Dont have to to this)
    Mat matRef = matBitmap(Range(100, 200), Range(100, 200));
    //                            y1    y2          x1   x2
    bitwise_not(matRef, matRef); // Invert the colors in this x1,x2,y1,y2

    // Display the results through Mat
    //imshow("Title", matBitmap);

    // Wait until some key is pressed
    //waitKey(0);
    return 0;
}
