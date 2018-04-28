#pragma once
#include "cv.h"                             
#include "highgui.h"
#include "cvaux.h"
#include "cxcore.h"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

#define SUCCEED(x) (x>=0)
//Dodo.cpp
extern cv::Mat threshold(std::string);
extern bool isEmptyLine(cv::Mat img, int y, double rate);
extern bool isEmptyLine(cv::Mat img, int y, int from, int to, double rate);
extern cv::Mat trim(cv::Mat img, double threshold = 0.01);
extern void findRow(cv::Mat img, double rangeTheta, std::vector<cv::Vec4i> &lines, int& thickness);
extern void findCol(cv::Mat img, double rangeTheta, int upper,int lower, int thickness , std::vector<cv::Vec4i> &lines);

extern void saveNums(std::string folder, std::vector<cv::Mat> nums);
typedef struct _space {
	int start;
	int length;
}space;

extern bool* KClassify(std::vector<space> collection);
extern int interCheck(std::vector<space> &collection);
//eagle.cpp
extern int rec(cv::Mat character);
extern void train(std::string save = "tData.csv");
//framework.cpp
extern int cut(cv::Mat img, std::vector<cv::Vec4i> divideBy, int direction, std::vector<cv::Mat> &container, bool includeAll = true);
extern int split(cv::Mat img, std::vector<space> &coll,bool twiceCut = true);
extern void extractNum(std::vector<cv::Vec4i> &pos, std::vector<cv::Mat> &nums, std::vector<cv::Mat> section, std::vector<cv::Vec4i> rows,int range);