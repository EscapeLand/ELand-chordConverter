
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
}#include "cv.h"                             
#include "highgui.h"
#include "cvaux.h"
#include "cxcore.h"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"

using namespace cv;
using namespace std;

typedef struct _space {
	int start;
	int length;
}space;


Mat threshold(string picPath)
{
	
	Mat arigin, r;
	arigin = imread(picPath.c_str(), 0);    //将读入的彩色图像直接以灰度图像读入  
	//namedWindow("原图", 1);
	//imshow("原图", arigin);
	r = arigin.clone();
	//希望尽量小的参数3
	threshold(arigin, r, 210, 255, CV_THRESH_BINARY);
	//namedWindow("二值化图像",1);
	//imshow("二值化图像", r);
	//cvWaitKey();
	return r;
}

bool isEmptyLine(Mat img, int y,double rate) {
	//函数名 isEmptyLine 
	//功能 测试一行是否是空白行,返回布尔值
	//特殊参数：
	//	thrshold 越大，对“白”要求越严格
	//	rate 越小，对白色范围要求越严格
	int sum = 0;
	if (y >= img.rows) return true;
	uchar *ptr = img.ptr<uchar>(y);
	for (int w = 0; w < img.cols; w++)
	{
		
		if (!ptr[w]) sum++;
	}
	if ((double)sum / img.cols > rate) return false;
	else return true;
}

bool isEmptyCol(Mat img, int x, double rate) {
	//函数名 isEmptyCol
	//功能 测试一列是否是空白列,返回布尔值
	//特殊参数：
	//	thrshold 越大，对“白”要求越严格
	//	rate 越小，对白色范围要求越严格
	int sum = 0;
	if (x >= img.cols) return true;
	for (int y = 0; y < img.rows; y++)
	{
		uchar *ptr = img.ptr<uchar>(y);
		if (!ptr[x]) sum++;
	}
	if ((double)sum / img.rows > rate) return false;
	else return true;
}

Mat trim(Mat img, double threshold = 0.01) {
	int i;
	int upper;
	for (i = 0; i < img.rows; i++) if (!isEmptyLine(img, i, threshold)) {
		upper = i;
		break;
	}
	int lower = upper;
	for (i = img.rows-1; i >upper ; i--) if (!isEmptyLine(img, i, threshold)) {
		lower = i+1;
		break;
	}
	if (lower == upper) {
		Mat r;
		return r;
	}
	Mat ROI = img(Range(upper, lower),Range(0, img.cols)).clone();
	for (i = 0; i < img.cols; i++) if (!isEmptyCol(ROI, i, threshold)) {
		upper = i;
		break;
	}
	lower = upper;
	for (i = img.cols - 1; i >upper; i--) if (!isEmptyCol(ROI, i, threshold)) {
		lower = i + 1;
		break;
	}
	if (img.rows == ROI.rows && upper == 0 && lower == img.cols)
		return trim(255 - img);
	return ROI(Range(0, ROI.rows), Range(upper, lower)).clone();
}

bool* KClassify(vector<space> collection) {
	//函数名：LSMClassify
	//功能：分类对象是空白区域 struct _space，分类依据为起始和截止坐标
	//输出：算法将各区域分入两类，一类为false，一类为true；输出一个bool数组，依序对应各个区域的类别
	//		！！！切记用 delete[] 释放返回值！！！
	bool* r = new bool[collection.size()];
	int k1 = 21,k2 = 5;
	int min = collection[0].length, max = min;
	for (int i = 0; i < collection.size(); i++) {
		if (collection[i].length > max) {
			max = collection[i].length;
		}
		else if(collection[i].length < min){
			min = collection[i].length;
		}
	}
	for (int i = 0; i < collection.size(); i++) {
		if (min + k1 > collection[i].length) {
			r[i] = false;
		}
		else if (max - k2 < collection[i].length) {
			r[i] = true;
		}
		else {
			if (collection[i].length - min - k1 < max - k2 - collection[i].length) {
				k1 = collection[i].length - min + 1;
				r[i] = false;
			}
			else if (collection[i].length - min - k1 > max - k2 - collection[i].length) {
				k2 = max - collection[i].length + 1;
				r[i] = true;
			}
			else {
				//要是等于就不要他了hhh
				r[i] = false;
			}
		}
	}
	return r;
}

void interCheck(vector<space> *collection) {
	size_t n = collection->size();
	if (n == 1) return;
	int **pool = new int*[n];
	//初始化截止
	for (int i = 0; i < n;i++) pool[i] = new int[n]();
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (i == j) continue;
			pool[i][j] = abs((*collection)[i].length - (*collection)[j].length);
		}
	}
	//初值设置完毕
	//校验开始
	for (int i = 0; i < n; i++) {
		int min = 99999,max = 0;
		for (int j = 0; j < n; j++) {
			if (j == i) continue;
			if (min > pool[i][j]) {
				min = pool[i][j];
			}
			for (int q = 0; q < n; q++) {
				if (q == i) continue;
				if (max < pool[q][j]) {
					max = pool[q][j];
				}
			}
		}
		if (max < min) {
			(*collection).erase((*collection).begin() + i);
			for (int k = 0; k < n; k++) delete[] pool[k];
			delete[] pool;
			return;
		}
	}
	for (int k = 0; k < n; k++) delete[] pool[k];
	delete[] pool;
	return;
}

int interCheck(vector<space> *collection,bool reserved) {
	size_t n = collection->size();
	if (n == 1) return -1;
	int **pool = new int*[n];
	//初始化截止
	for (int i = 0; i < n; i++) pool[i] = new int[n]();
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (i == j) continue;
			pool[i][j] = abs((*collection)[i].length - (*collection)[j].length);
		}
	}
	//初值设置完毕
	//校验开始
	for (int i = 0; i < n; i++) {
		int min = 99999, max = 0;
		for (int j = 0; j < n; j++) {
			if (j == i) continue;
			if (min > pool[i][j]) {
				min = pool[i][j];
			}
			for (int q = 0; q < n; q++) {
				if (q == i) continue;
				if (max < pool[q][j]) {
					max = pool[q][j];
				}
			}
		}
		if (max < min) {
			(*collection).erase((*collection).begin() + i);
			for (int k = 0; k < n; k++) delete[] pool[k];
			delete[] pool;
			return i;
		}
	}
	for (int k = 0; k < n; k++) delete[] pool[k];
	delete[] pool;
	return -1;
}
#include "cv.h"                             //  OpenCV 文件头
#include "highgui.h"
#include "cvaux.h"
#include "cxcore.h"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"

using namespace cv;
using namespace std;

typedef struct _space {
	int start;
	int length;
}space;


int greyLevel(string picPath)
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

Mat threshold(string picPath)
{
	
	Mat arigin, r;
	arigin = imread(picPath.c_str(), 0);    //将读入的彩色图像直接以灰度图像读入  
	//namedWindow("原图", 1);
	//imshow("原图", arigin);
	r = arigin.clone();
	//希望尽量小的参数3
	threshold(arigin, r, 210, 255, CV_THRESH_BINARY);
	//namedWindow("二值化图像",1);
	//imshow("二值化图像", r);
	//cvWaitKey();
	return r;
}

bool isEmptyLine(Mat img, int y,double rate) {
	//函数名 isEmptyLine 
	//功能 测试一行是否是空白行,返回布尔值
	//特殊参数：
	//	thrshold 越大，对“白”要求越严格
	//	rate 越小，对白色范围要求越严格
	int sum = 0;
	if (y >= img.rows) return true;
	uchar *ptr = img.ptr<uchar>(y);
	for (int w = 0; w < img.rows; w++)
	{
		if (!ptr[w]) sum++;
	}
	if ((double)sum / img.rows > rate) return false;
	else return true;
}

bool isEmptyCol(Mat img, int x, double rate) {
	//函数名 isEmptyLine 
	//功能 测试一行是否是空白行,返回布尔值
	//特殊参数：
	//	thrshold 越大，对“白”要求越严格
	//	rate 越小，对白色范围要求越严格
	int sum = 0;
	if (x >= img.cols) return true;
	for (int y = 0; y < img.rows; y++)
	{
		uchar *ptr = img.ptr<uchar>(y);
		if (!ptr[x]) sum++;
	}
	if ((double)sum / img.cols > rate) return false;
	else return true;
}

Mat trim(Mat img, double threshold = 0.01) {
	int i;
	int upper;
	for (i = 0; i < img.rows; i++) if (!isEmptyLine(img, i, threshold)) {
		upper = i-1;
		break;
	}
	int lower = upper;
	for (i = img.rows-1; i >upper ; i--) if (!isEmptyLine(img, i, threshold)) {
		lower = i+1;
		break;
	}
	if (lower == upper) {
		Mat r;
		return r;
	}
	Mat ROI = img(Range(upper, lower),Range(0, img.cols)).clone();
	for (i = 0; i < img.cols; i++) if (!isEmptyCol(ROI, i, threshold)) {
		upper = i - 1;
		break;
	}
	lower = upper;
	for (i = img.cols - 1; i >upper; i--) if (!isEmptyCol(ROI, i, threshold)) {
		lower = i + 1;
		break;
	}
	return ROI(Range(0, ROI.rows), Range(upper, lower)).clone();
}

bool* KClassify(vector<space> collection) {
	//函数名：LSMClassify
	//功能：分类对象是空白区域 struct _space，分类依据为起始和截止坐标
	//输出：算法将各区域分入两类，一类为false，一类为true；输出一个bool数组，依序对应各个区域的类别
	//		！！！切记用 delete[] 释放返回值！！！
	bool* r = new bool[collection.size()];
	int k1 = 21,k2 = 5;
	int min = collection[0].length, max = min;
	for (int i = 0; i < collection.size(); i++) {
		if (collection[i].length > max) {
			max = collection[i].length;
		}
		else if(collection[i].length < min){
			min = collection[i].length;
		}
	}
	for (int i = 0; i < collection.size(); i++) {
		if (min + k1 > collection[i].length) {
			r[i] = false;
		}
		else if (max - k2 < collection[i].length) {
			r[i] = true;
		}
		else {
			if (collection[i].length - min - k1 < max - k2 - collection[i].length) {
				k1 = collection[i].length - min + 1;
				r[i] = false;
			}
			else if (collection[i].length - min - k1 > max - k2 - collection[i].length) {
				k2 = max - collection[i].length + 1;
				r[i] = true;
			}
			else {
				//要是等于就不要他了hhh
				r[i] = false;
			}
		}
	}
	return r;
}

void interCheck(vector<space> *collection) {
	size_t n = collection->size();
	int **pool = new int*[n];
	//初始化截止
	for (int i = 0; i < n;i++) pool[i] = new int[n]();
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (i == j) continue;
			pool[i][j] = abs((*collection)[i].length - (*collection)[j].length);
		}
	}
	//初值设置完毕
	//校验开始
	for (int i = 0; i < n; i++) {
		int min = 99999,max = 0;
		for (int j = 0; j < n; j++) {
			if (j == i) continue;
			if (min > pool[i][j]) {
				min = pool[i][j];
			}
			for (int q = 0; q < n; q++) {
				if (q == i) continue;
				if (max < pool[q][j]) {
					max = pool[q][j];
				}
			}
		}
		if (max < min) {
			(*collection).erase((*collection).begin() + i);
			return;
		}
	}
	return;
}

