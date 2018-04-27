#include "myheader.h"
using namespace cv;

int cut(Mat img, std::vector<Vec4i> divideBy, int direction, std::vector<Mat> &container, bool includeAll) {
	//direction: 0是竖直裁剪, 1是水平裁剪
	if (direction > 1) return 0;
	int from = (int)container.size();
	switch (direction) {
	case 0:
		//竖直裁剪

		if (includeAll) {
			if (divideBy[0][0] > 5) {
				Vec4i h = { 0,0,0,img.rows };
				divideBy.insert(divideBy.begin(), h);
			}
			if (divideBy.back()[0] < img.cols - 5) {
				Vec4i h = { img.cols,0,img.cols,img.rows };
				divideBy.insert(divideBy.begin() + divideBy.size(), h);
			}
		}
		for (int i = 0; i < (int)divideBy.size() - 1; i++) {
			Mat tmp;
			img(Range::all(), Range(divideBy[i][0], divideBy[i + 1][0])).copyTo(tmp);
			container.push_back(tmp);
		}
		break;
	case 1:
		//水平裁剪

		if (includeAll) {
			if (divideBy[0][1] > 5) {
				Vec4i h = { 0,0,img.cols,0 };
				divideBy.insert(divideBy.begin(), h);
			}
			if (divideBy.back()[1] < img.rows - 5) {
				Vec4i h = { 0,img.rows,img.cols,img.rows };
				divideBy.insert(divideBy.begin() + divideBy.size(), h);
			}
		}
		for (int i = 0; i < (int)divideBy.size() - 1; i++) {
			Mat tmp;
			img(Range(divideBy[i][1], divideBy[i + 1][1]), Range::all()).copyTo(tmp);
			container.push_back(tmp);
		}
	}
	return (int)container.size() - from;
}

bool split(Mat img, std::vector<space> &coll,bool &cat) {
	bool flag = false;
	for (int st = img.rows, i = 0; i < img.rows; i++) {
		//初步设为0.04 以后再说
		if (isEmptyLine(img, i, 0.001)) {
			if (!flag) {
				st = i;
				flag = true;
			}
		}
		else {
			if (flag) {
				if (i - 1 - st > 0) {
					coll.push_back({ st,i - 1 - st });
				}
				flag = false;
			}
		}
	}
	if (coll.size() < 2) {
		cat = false;
		std::cout << "裁剪失败，等待二次裁剪" << std::endl;
		//二次裁剪为缩减判断空行的范围，从之前的从像素x=0 至x=col到检测到的横线的x1至x2
		std::vector<cv::Vec4i> rows;
		int thick = 0;
		findRow(img, CV_PI / 18, rows, thick);
		if (rows.size() > 5) {
			std::cout << "二次裁剪开始." << std::endl;
			flag = false;
			for (int st = img.rows, i = 0; i < img.rows; i++) {
				if (isEmptyLine(img, i, std::max(std::min(rows[0][0], rows[0][2]), std::min(rows[5][0], rows[5][2])),
					std::min(std::max(rows[0][0], rows[0][2]), std::max(rows[5][0], rows[5][2])), 0.01)) {
					if (!flag) {
						st = i;
						flag = true;
					}
				}
				else {
					if (flag) {
						if (i - 1 - st > 0) coll.push_back({ st,i - 1 - st });
						flag = false;
					}
				}
			}

		}
		if (coll.size() < 2) {
			std::cout << "二次裁剪失败，请手动处理" << std::endl;
			imshow("WTF", img);
			cvWaitKey(0);
			return false;
		}
	}
	return true;
}

inline void extractNum(std::vector<Vec4i> &pos, std::vector<Mat> &nums, std::vector<Mat> section, std::vector<Vec4i> rows,int range) {
	for (size_t j = section.size() - range; j < section.size(); j++) {
		std::vector<std::vector<cv::Point>> cont;
		cv::Mat inv = 255 - section[j];
		//cv::Mat ccolor;
		//cvtColor(section.back(), ccolor, CV_GRAY2BGR);

		cv::findContours(inv, cont, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		for (int q = 0; q < cont.size(); q++) {
			cv::Vec4i tmp = { section.back().cols,section.back().rows,0,0 };
			for (int k = 0; k < cont[q].size(); k++) {
				tmp[0] = std::min(tmp[0], cont[q][k].x);
				tmp[2] = std::max(tmp[2], cont[q][k].x);
				tmp[1] = std::min(tmp[1], cont[q][k].y);
				tmp[3] = std::max(tmp[3], cont[q][k].y);

				pos.push_back(tmp);
			}
			//限定筛选
			if (tmp[3] - tmp[1] < rows[1][1] - rows[0][1]
				&& tmp[3] - tmp[1] > tmp[2] - tmp[0])
				nums.push_back(section[j](cv::Range(tmp[1], tmp[3] + 1), cv::Range(tmp[0], tmp[2] + 1)));
		}
	}
}