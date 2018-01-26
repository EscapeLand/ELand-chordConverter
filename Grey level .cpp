
#include "cv.h"                             //  OpenCV 文件头
#include "highgui.h"
#include "cvaux.h"
#include "cxcore.h"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <string>
using namespace cv;
using namespace std;

extern string picPath;

int greyLevel()
{
	
	Mat image, resImage;//Mat是OpenCV最基本的数据结构，这是定义一个图像矩阵类型
	image = imread(picPath.c_str(), IMREAD_COLOR);//读入图片数据
	if (image.empty())//读取失败时
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}
	namedWindow("原图", WINDOW_AUTOSIZE); // 创建一个窗口
	imshow("原图", image);    // 在窗口中显示图片
	cvtColor(image, resImage, CV_RGB2GRAY);//把图片转化为灰度图
										   //把图片写入到图片中
	imwrite("D:\\test1.jpg", resImage);
	namedWindow("灰度图", WINDOW_AUTOSIZE); // 创建一个窗口
	imshow("灰度图", resImage);    // 在窗口中显示图片
	waitKey(0); // 等待一次按键，程序结束
	return 0;
}

int threshold()
{
	
	Mat arigin, r;
	arigin = imread(picPath.c_str(), 0);    //将读入的彩色图像直接以灰度图像读入  
	namedWindow("原图", 1);
	imshow("原图", arigin);
	r = arigin.clone();
	//希望尽量小的参数3
	threshold(arigin, r, 210, 255, CV_THRESH_BINARY);
	namedWindow("二值化图像",1);
	imshow("二值化图像", r);
	waitKey(0);
	return 0;
}

