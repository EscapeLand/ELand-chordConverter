#pragma once
#include "cv.h"                             
#include "highgui.h"
#include "cvaux.h"
#include "cxcore.h"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include "tinyxml2.h"

#include <iostream>
#include<vector>

typedef struct space {
	int start;
	int length;
}space;

enum Value {
	whole = 32,
	half = 16,
	quarter = 8,
	eighth = 4,
	sixteenth = 2,
	thirty_second = 1
};
typedef struct technical {
	int string;									//��
	int fret;									//Ʒ
}technical;

typedef struct notations {
	int dynamics;								//����
	technical technical;
}notations;

typedef struct note {
	bool chord = false;							//������ǣ�Ϊtrue����һ������ͬʱ����
	std::vector<int> possible;						//�������ܵ�Ʒ��
	Value timeValue = quarter;					//ʱֵ
	bool dot = false;							//����
	int voice = 1;								//��������
	int duration = 1;							//����ֵ
	notations notation;

	int pos;									//x����
}note;

typedef struct Time {
	int beats = 4;								//ÿС������
	int beat_type = 4;							//������������һ��
}Time;

typedef struct key {
	int fifth = 0;								//��������
	char* mode = (char*)"major";				//1-��׼��������
}key;

class measure {
private:
	int noteBottom = 0;
	int maxCharacterWidth = 0;
	void recNum(cv::Mat section, std::vector<cv::Vec4i> rows);
public:
	unsigned int id;
	unsigned int number;						//С����
	Time time;
	std::vector<note> notes;
	key key;
	measure(cv::Mat org, cv::Mat img, std::vector<cv::Vec4i> rows,int id);
};

class saveDoc {
private:
	tinyxml2::XMLDocument backup;
public:
	saveDoc(const char* title, const char* composer, const char* lyricist, const char* artist, const char* tabber, const char* irights);
	int save(const char* xmlPath);
	void saveMeasure(measure toSave);
};