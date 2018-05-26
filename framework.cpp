#include "myheader.h"
using namespace cv;

int cut(Mat img, std::vector<Vec4i> divideBy, int direction, std::vector<Mat> &container, bool includeAll) {
	//direction: 0����ֱ�ü�, 1��ˮƽ�ü�
	if (direction > 1) return 0;
	int from = (int)container.size();
	switch (direction) {
	case 0:
		//��ֱ�ü�

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
		//ˮƽ�ü�

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

int split(Mat img, std::vector<space> &coll) {
	int r = 1;
	bool flag = false;
	for (int st = img.rows, i = 0; i < img.rows; i++) {
		//������Ϊ0.04 �Ժ���˵
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
		coll.clear();
		r = 2;
		std::cout << "�ü�ʧ�ܣ��ȴ����βü�" << std::endl;
		//���βü�Ϊ�����жϿ��еķ�Χ����֮ǰ�Ĵ�����x=0 ��x=col����⵽�ĺ��ߵ�x1��x2
		std::vector<cv::Vec4i> rows;
		int thick = 0;
		findRow(img, CV_PI / 18, rows, thick);
		if (rows.size() > 5) {
			std::cout << "���βü���ʼ." << std::endl;
			flag = false;
			for (int st = img.rows, i = min(rows[0][1], rows[0][3]); i < img.rows; i++) {
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
			std::cout << "���βü�ʧ�ܣ����ֶ�����" << std::endl;
			imshow("WTF", img);
			cvWaitKey(0);
			return 3;
		}
		/*cv::Mat ccolor;
		for (int i = 0; i < coll.size(); i++) {
			cvtColor(img, ccolor, CV_GRAY2BGR);
			line(ccolor, CvPoint(0, coll[i].start), CvPoint(img.cols, coll[i].start), CvScalar(0, 0, 255));
			line(ccolor, CvPoint(0, coll[i].start + coll[i].length), CvPoint(img.cols, coll[i].start + coll[i].length), CvScalar(255, 0, 0));
		}
		imshow("2", ccolor); cvWaitKey();*/
	}
		return r;
	
}

inline void extractNum(std::vector<Vec4i> &pos, std::vector<Mat> &nums, std::vector<Mat> section, std::vector<Vec4i> rows,int &bottom,int range) {
	/*
	 * ������extractNum
	 * ���ܣ��Ӵ���ͼ������ȡ���ֵ�
	 * ������	pos��Vec4i���ֱ���λ����Ϣ���� �� �� ��
				nums��Mat����ȡ����ͼ��
				section��Mat������ͼ��
				rows��Vec4i�������������Ϣ�����ߣ�
				bottom��int����ȡ���ֵ��½磬������ʱֵɨ�躯��
				range��int����sectionβ����ʼ��Ҫ����ķ�Χ
	*/
	bottom = 0;
	for (size_t j = section.size() - range; j < section.size(); j++) {
		std::vector<std::vector<cv::Point>> cont;
		cv::Mat inv = 255 - section[j];
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
			//�޶�ɸѡ
			if (tmp[3] - tmp[1] < rows[1][1] - rows[0][1]					//�����޶�
				&& tmp[3] - tmp[1] > tmp[2] - tmp[0])						//��״�޶�
			{
				bottom = std::max(bottom, tmp[3]);
				nums.push_back(section[j](cv::Range(tmp[1], tmp[3] + 1), cv::Range(tmp[0], tmp[2] + 1)));
			}
		}
	}
}

void duration(Mat img, std::vector<space> &coll) {
	bool flag = false;
	for (int st = img.rows, i = 0; i < img.rows; i++) {
		//������Ϊ0.04 �Ժ���˵
		if (isEmptyLine(img, i, 0.001)) {
			if (!flag) {
				st = i;
				flag = true;
			} 
		}
		else {
			if (flag) {
				if (i - 1 - st > 0) {
					if (i - 1 - st > img.rows * 2 / 3) return;
					coll.push_back({ st,i - 1 - st });
				}
				flag = false;
			}
		}
	}
	
	cv::Mat ccolor;
	cvtColor(img, ccolor, CV_GRAY2BGR);
	for (int i = 0; i < coll.size(); i++) {
		line(ccolor, CvPoint(0, coll[i].start), CvPoint(img.cols, coll[i].start), CvScalar(0, 0, 255));
		line(ccolor, CvPoint(0, coll[i].start + coll[i].length), CvPoint(img.cols, coll[i].start + coll[i].length), CvScalar(255, 0, 0));
	}
	imshow("timeValue", ccolor); cvWaitKey();
	return;
}

Mat Morphology(Mat img, int len, bool horizontal ,bool open) {
	Mat dilated, eroded;
	Mat kernel = horizontal ?
		getStructuringElement(MORPH_RECT, Size(max(len, 1),1)):					//ˮƽ�ṹ
		getStructuringElement(MORPH_RECT, Size(1,max(len,1)));						//��ֱ�ṹ
	if (open) {
		dilate(img, dilated, kernel);																		//����
		erode(dilated, eroded, kernel);																		//��ʴ
		return eroded;
	}
	else {
		erode(img, eroded, kernel);																			//��ʴ
		dilate(eroded, dilated, kernel);																	//����
		return dilated;
	}
}