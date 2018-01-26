// ConsoleApplication1.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
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
int main()
{
	string imageName("D:\\test.jpg"); // 图片在电脑中的绝对地址
	Mat image, resImage;//Mat是OpenCV最基本的数据结构，这是定义一个图像矩阵类型
	image = imread(imageName.c_str(), IMREAD_COLOR);//读入图片数据
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

