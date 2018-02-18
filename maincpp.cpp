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
		if (r[i]) {
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
