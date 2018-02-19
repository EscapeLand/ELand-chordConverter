#include "cv.h"
#include "highgui.h"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
extern cv::Mat threshold(std::string);
extern bool isEmptyLine(cv::Mat img, int y, double rate);
extern cv::Mat trim(cv::Mat img, double threshold = 0.01) ;

typedef struct _space{
	int start;
	int length;
}space;

extern bool* KClassify(std::vector<space> collection);
extern void interCheck(std::vector<space> *collection);

int main() {
	std::string f;
	std::cin >> f;
	
	cv::Mat img = threshold(f);
	std::vector<space> coll;
	cv::Mat trimmed = trim(img);
	
	bool flag = false;
	for (int st = trimmed.rows,i = 0; i < trimmed.rows; i++) {
		//初步设为0.04 以后再说
		if (isEmptyLine(trimmed, i, 0)) {
			if (!flag) {
				st = i;
				flag = true;
			}
		}
		else {
			if (flag) {
				coll.push_back({ st,i - 1 - st });
				flag = false;
			}
		}
	}
	size_t n;
	do {
		n = coll.size();
		interCheck(&coll);
	} while (n > coll.size());
	bool* r = KClassify(coll);
	std::vector<space> toCut;
	for (int i = 0; i < n; i++) {
		if (r[i]) {#include "cv.h"
#include "highgui.h"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

#define SUCCEED(x) (x>=0)

extern cv::Mat threshold(std::string);
extern bool isEmptyLine(cv::Mat img, int y, double rate);
extern cv::Mat trim(cv::Mat img, double threshold = 0.01) ;

typedef struct _space{
	int start;
	int length;
}space;

extern bool* KClassify(std::vector<space> collection);
extern void interCheck(std::vector<space> *collection);
extern int interCheck(std::vector<space> *collection, bool reserved);

int main() {
	std::cout << "Drag image here to start: "<< std::endl ;
	std::string f;
	std::cin >> f;
	
	cv::Mat img = threshold(f);
	if (img.empty()) {
		std::cout << "Wrong format." << std::endl;
		system("pause");
		return 1;
	}
	std::vector<space> coll;
	cv::Mat trimmed = trim(img);
	bool flag = false;
	for (int st = trimmed.rows,i = 0; i < trimmed.rows; i++) {
		//初步设为0.04 以后再说
		if (isEmptyLine(trimmed, i, 0)) {
			if (!flag) {
				st = i;
				flag = true;
			}
		}
		else {
			if (flag) {
				coll.push_back({ st,i - 1 - st });
				flag = false;
			}
		}
	}
	size_t n;
	do {
		n = coll.size();
		interCheck(&coll);
	} while (n > coll.size());
	bool* r = KClassify(coll);
	std::vector<space> toCut;
	for (int i = 0; i < n; i++) {
		if (r[i]) {
			//line(trimmed, CvPoint(0, coll[i].start), CvPoint(trimmed.cols, coll[i].start), CvScalar(0, 0, 255));
			//line(trimmed, CvPoint(0, coll[i].start+ coll[i].length), CvPoint(trimmed.cols, coll[i].start + coll[i].length), CvScalar(0, 0, 255));
			toCut.push_back(coll[i]);
		}
	}
	bool dog = false;
	if (toCut.size()>2) {
		std::cout << "过滤算法正常" << std::endl;
		coll.clear();
	}
	else {
		toCut.swap(coll);
		dog = true;
	}
	delete[] r;
	cv::Mat *piece = new cv::Mat[toCut.size()+1];
	piece[0] = trimmed(cv::Range(0, toCut[0].start), cv::Range(0, trimmed.cols));
	for (int i = 1; i < toCut.size(); i++) piece[i] = trimmed(cv::Range(toCut[i - 1].start + toCut[i - 1].length + 1, toCut[i].start), cv::Range(0, trimmed.cols));
	piece[toCut.size()] = trimmed(cv::Range(toCut[toCut.size() - 1].start + toCut[toCut.size() - 1].length + 1, trimmed.rows), cv::Range(0, trimmed.cols));
	
	if (dog) {
		std::cout << "运行修补算法" << std::endl;
		std::vector<space> toJoin;
		for (int i = 1; i <= toCut.size(); i++) toJoin.push_back({0,piece[i].rows});
		bool* b = new bool[toCut.size()+1]();
		do{
			int a = interCheck(&toJoin, true);
			if(SUCCEED(a)) b[a] = true;
			else break;
		}while(1);
		r = KClassify(toJoin);
		for (int k=0,i = 1; i <= toCut.size(); i++) {
			if (b[i]) b[i] = false;
			else b[i] = r[k++];
		}
		delete[] r;
		for (int i = toCut.size(); i > 0; i--) {
			if (!b[i]) {
				//将piece[i]与piece[i-1]拼接在一起
				vconcat(piece[i-1],piece[i], piece[i - 1]);
				piece[i].release();
			}
		}
		delete[] b;
	}
	cvNamedWindow("2",1);
	for (int i = 0; i <= toCut.size(); i++) {
		if (piece[i].empty()) continue;
		imshow("2", piece[i]);
		cvWaitKey(1000);
	}
	
	delete[] piece;
	
	cvWaitKey();
}
			//line(trimmed, CvPoint(0, coll[i].start), CvPoint(trimmed.cols, coll[i].start), CvScalar(0, 0, 255));
			//line(trimmed, CvPoint(0, coll[i].start+ coll[i].length), CvPoint(trimmed.cols, coll[i].start + coll[i].length), CvScalar(0, 0, 255));
			toCut.push_back(coll[i]);
		}
	}
	coll.clear();
	delete[] r;
	cv::Mat *piece = new cv::Mat[toCut.size()+1];
	piece[0] = trimmed(cv::Range(0, toCut[0].start), cv::Range(0, trimmed.cols));
	for (int i = 1; i < toCut.size(); i++) piece[i] = trimmed(cv::Range(toCut[i - 1].start + toCut[i - 1].length + 1, toCut[i].start), cv::Range(0, trimmed.cols));
	piece[toCut.size()] = trimmed(cv::Range(toCut[toCut.size() - 1].start + toCut[toCut.size() - 1].length + 1, trimmed.rows), cv::Range(0, trimmed.cols));
	
	cvNamedWindow("2",1);
	for (int i = 0; i <= toCut.size(); i++) {
		imshow("2", piece[i]);
		cvWaitKey(1000);
	}
	delete[] piece;
	system("pause");
	
}
