#pragma once

#include"Cuckoo.h"
#include "swan.h"
using namespace std;
#define mode 0  //0-main program  1-train  2-test

int col;
int main() {
#if mode == 1
	train();
	return 0;
#endif
	std::string f;
	size_t n;
	int cutTimes;
	bool flag = false, dog = false;
	std::vector<space> coll;
	std::vector<space> toCut;

	std::cout << "Drag image here to start: "<< std::endl ;
	std::cin >> f;
#if mode == 2
	std::vector<int> poss;
	std::cout << rec(cv::imread(f),poss) << std::endl;
	system("pause");
	return 0;
#endif
	cv::Mat img = threshold(f);
	if (img.empty()) {
		std::cout << "Wrong format." << std::endl;
		system("pause");
		return 1;
	}
	cv::Mat trimmed = trim(img);
	col = trimmed.cols;
	//trimmed = Morphology(trimmed, trimmed.cols / 400, true, false);
	//std::vector<std::vector<cv::Point>> cont;
	//cv::Mat inv = 255 - trimmed;
	//cv::findContours(inv, cont, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	//cv::Mat ccolor;
	//cvtColor(trimmed, ccolor, CV_GRAY2BGR);
	//for (int q = 0; q < cont.size(); q++) {
	//	cv::Vec4i tmp = { trimmed.cols,trimmed.rows,0,0 };
	//	for (int k = 0; k < cont[q].size(); k++) {
	//		tmp[0] = std::min(tmp[0], cont[q][k].x);
	//		tmp[2] = std::max(tmp[2], cont[q][k].x);
	//		tmp[1] = std::min(tmp[1], cont[q][k].y);
	//		tmp[3] = std::max(tmp[3], cont[q][k].y);

	//	}
	//	//限定筛选
	//	if (tmp[2] - tmp[0] > trimmed.cols /2)						//形状限定
	//	{
	//		rectangle(ccolor,cv::Point(tmp[0],tmp[1]),cv::Point(tmp[2],tmp[3]),cv::Scalar(0,0,255));
	//	}
	//}
	//ccolor = perspect(ccolor, 960 * ccolor.cols / ccolor.rows, 960);
	//imshow("2", ccolor); cvWaitKey();
	//return 0;
	cutTimes = split(trimmed, coll);
	if (cutTimes == 3) {
		return 1;
	}
	n = coll.size();
	bool *r = new bool[n];
	for (size_t i = 0; i < n; i++) r[i] = false;
	for (;;) {
		int a = interCheck(coll);
		if (a == -1) break;
		else r[a] = true;
	};
	bool* mm = KClassify(coll);
	for (size_t k = 0,i = 0; i < n; i++) {
		if (!r[i]) {
			r[i] = mm[k++];
		}
	}
	delete[] mm;
	n = coll.size();
	
	for (int i = 0; i < n; i++) {
		if (r[i]) {
			
			/*cv::Mat ccolor;
			cvtColor(trimmed, ccolor, CV_GRAY2BGR);
			line(ccolor, CvPoint(0, coll[i].start), CvPoint(trimmed.cols, coll[i].start), CvScalar(0, 0, 255));
			line(ccolor, CvPoint(0, coll[i].start + coll[i].length), CvPoint(trimmed.cols, coll[i].start + coll[i].length), CvScalar(0, 0, 255));
			imshow("2", ccolor); cvWaitKey();*/
			
			toCut.push_back(coll[i]);
		}
	}
	
	if (toCut.size() > 2) {
		std::cout << "过滤算法正常" << std::endl;
		coll.clear();
	}
	else {
		toCut.swap(coll);
		dog = true;
	}
	n = toCut.size();
	delete[] r;

	std::vector<cv::Mat> piece;
	std::vector<cv::Mat> chords;
	std::vector<measure> sections;
	std::vector<cv::Mat> timeValue;
	std::vector<cv::Mat> info;
	std::vector<cv::Mat> notes;
	std::vector<cv::Vec4i> pos;
	std::vector<cv::Mat> nums;
	cv::Mat toOCR;

	for (int i = 0; i <= n; i++) {
		cv::Mat tmp;
		piece.push_back(tmp);
	}
	//cut<space>(trimmed, toCut, [](space x) ->int& {return x.start; }, 1, piece);
	trimmed(cv::Range(0, toCut[0].start), cv::Range(0, trimmed.cols)).copyTo(piece[0]);
	for (int i = 1; i < n; i++) {
		trimmed(cv::Range(toCut[i - 1].start + toCut[i - 1].length + 1, toCut[i].start),
			cv::Range(0, trimmed.cols)).copyTo(piece[i]);
	}
	trimmed(cv::Range(toCut[n - 1].start + toCut[n - 1].length + 1, trimmed.rows), cv::Range(0, trimmed.cols)).copyTo(piece[n]);
	
	if (dog && cutTimes == 2) {
		std::cout << "运行修补算法" << std::endl;
		std::vector<space> toJoin;
		for (int i = 1; i <= n; i++) toJoin.push_back({0,piece[i].rows});
		bool* b = new bool[toCut.size()+1]();
		do{
			int a = interCheck(toJoin);
			if(SUCCEED(a)) b[a] = true;
			else break;
		}while(1);
		r = KClassify(toJoin);
		for (int k=0,i = 1; i <= n; i++) {
			if (b[i]) b[i] = false;
			else b[i] = r[k++];
		}
		delete[] r;
		for (size_t i = n; i > 0; i--) {
			if (!b[i]) {
				//将piece[i]与piece[i-1]拼接在一起
				vconcat(piece[i-1],piece[i], piece[i - 1]);
				piece[i].release();
			}
		}
		delete[] b;
	}
	
	
	int k = 1;
	for (int i = 0; i <= n; i++) {
		if (piece[i].empty()) continue;
		std::vector<cv::Vec4i> rows;
		int thick = 0;
		findRow(piece[i], CV_PI / 18, rows,thick);
		if (rows.size() > 5) {
			flag = true;
			chords.push_back(piece[i]);
			//为OCR去掉横线
			//用形态学腐蚀得到mask 将mask上的点置0
			cv::Mat eroded, dilated;
			cv::Mat hline = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(piece[i].cols / 30, 1));			//水平结构
			erode(255 - piece[i], eroded, hline);																//腐蚀
			dilate(eroded, dilated, hline);																		//膨胀
			toOCR = cv::max(dilated, piece[i]);
			
			std::vector<cv::Vec4i> lines;
			int max = std::min(rows[5][1], rows[5][3]);
			int min = std::max(rows[0][1], rows[0][3]);

			findCol(piece[i], CV_PI / 18 * 8, max, min, thick, lines);
			std::vector<cv::Mat> origin;
			std::vector<cv::Mat> section;
			if (lines.size()) {
				cut(toOCR, lines, 0, section, true);
				cut(piece[i], lines, 0, origin, true);
			}
			for (size_t j = 0; j < section.size(); j++) {
				measure newSec(origin[j],section[j],rows,(int)k++);
				sections.push_back(newSec);
			}
		}
		else {
			if(flag) notes.push_back(piece[i]);
			else info.push_back(piece[i]);
		}
	}
	piece.clear();

	//saveNums("C:\\Users\\Administrator\\Desktop\\oh", nums);
	//system("pause");

	/*system("pause");
	system("cls");
	for (measure &i : sections) {
		for (note &j : i.notes) {
			if(!j.chord) cout << " | " << j.timeValue << "   ";
			cout << j.notation.technical.string << "弦" << j.notation.technical.fret << "品";
		}
		cout << endl << endl;
	}
	system("pause");*/
	saveDoc finish("unknown","unknown","unknown","unknown","chordConverter","Escapeland");
	for (measure& i : sections) {
		finish.saveMeasure(i);
	}
	finish.save("newTab.xml");
}
