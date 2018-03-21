#pragma once
#include "cv.h"                             
#include "highgui.h"
#include "cvaux.h"
#include "cxcore.h"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

#define SUCCEED(x) (x>=0)

extern cv::Mat threshold(std::string);
extern bool isEmptyLine(cv::Mat img, int y, double rate);
extern bool isEmptyLine(cv::Mat img, int y, int from, int to, double rate);
extern cv::Mat trim(cv::Mat img, double threshold = 0.01);
extern void findRow(cv::Mat img, double rangeTheta, std::vector<cv::Vec4i> &lines);
extern void findCol(cv::Mat img, double rangeTheta, int upper,int lower, std::vector<cv::Vec4i> &lines);
extern void savePic(std::string folder, cv::Mat pic);
typedef struct _space {
	int start;
	int length;
}space;

extern bool* KClassify(std::vector<space> collection);
extern void interCheck(std::vector<space> *collection);
extern int interCheck(std::vector<space> *collection, bool reserved);