//与opencv有关的一些实用函数工具

#include <iostream>
#include <windows.h>

#include "opencv2/core/version.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"

using namespace cv;
using namespace std;

double match(cv::Mat image, cv::Mat tepl, cv::Point &point, int method);
int mySobel( );
int myKeyPoint( );
int myCanny();
int mySurf();
bool ROI_AddImage();
int mat2solution(Mat boardPath, Point leftRook);
double getPSNR(const Mat& I1, const Mat& I2);
Scalar getMSSIM( const Mat& i1, const Mat& i2);
double ssim(Mat &i1, Mat & i2);
double psnr(Mat &I1, Mat &I2);
cv::Mat mergeRows(cv::Mat A, cv::Mat B);
cv::Mat mergeCols(cv::Mat A, cv::Mat B);
cv::Mat hwnd2mat(HWND hwnd); //另一个版本的截屏函数

