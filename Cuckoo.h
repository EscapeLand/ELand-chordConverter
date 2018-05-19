#include"myheader.h"

using namespace std;

enum Value {
	whole = 32,
	half = 16,
	quarter = 8,
	eighth = 4,
	sixteenth = 2,
	thirty_second = 1
};
typedef struct technical {
	int string;								//弦
	int fret;									//品
}technical;

typedef struct notations {
	int dynamics;								//力度
	technical technical;
}notations;

typedef struct note {
	bool chord = false;							//和声标记，为true与上一个音符同时发声
	vector<int> possible;						//其他可能的品数
	Value timeValue = quarter;					//时值
	bool dot = false;							//附点
	int voice = 1;								//发声类型
	int duration = 1;							//延音值
	notations notation;

	int pos;									//x坐标
}note;

typedef struct Time {
	int beats = 4;								//每小节拍数
	int beat_type = 4;							//几分音符算作一拍
}Time;

typedef struct key {
	int fifth = 0;								//音调升降
	int mode = 1;								//1-标准音主序列
}key;

class measure {
private:
	void recNum(cv::Mat section, std::vector<cv::Vec4i> rows) {
		/*
		* 函数：Dsection::recNum
		* 功能：从传入图像中提取数字等
		* 参数：
		section，Mat，传入图像
		rows，Vec4i，传入的网格信息（谱线）
		*/
		std::vector<std::vector<cv::Point>> cont;
		cv::Mat inv = 255 - section;
		cv::findContours(inv, cont, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		for (int q = 0; q < cont.size(); q++) {
			cv::Vec4i tmp = { section.cols,section.rows,0,0 };
			for (int k = 0; k < cont[q].size(); k++) {
				tmp[0] = std::min(tmp[0], cont[q][k].x);
				tmp[2] = std::max(tmp[2], cont[q][k].x);
				tmp[1] = std::min(tmp[1], cont[q][k].y);
				tmp[3] = std::max(tmp[3], cont[q][k].y);


			}
			//限定筛选
			if (tmp[3] - tmp[1] < rows[1][1] - rows[0][1]					//网格限定
				&& tmp[3] - tmp[1] > tmp[2] - tmp[0])						//形状限定
			{
				note newNote;
				cv::Mat number = section(cv::Range(tmp[1], tmp[3] + 1), cv::Range(tmp[0], tmp[2] + 1)).clone();

				//savePic("C:\\Users\\Administrator\\Desktop\\oh\\", number);

				cvtColor(number, number, CV_GRAY2BGR);
				if (number.cols != 8 || number.rows != 10) number = perspect(number, 8, 10);
				newNote.notation.technical.string = whichLine(tmp, rows);										//几何关系判断string
				newNote.notation.technical.fret = rec(number, newNote.possible);										//识别数字fret
				if (!newNote.notation.technical.string) {
					for (size_t i = 0; i < newNote.possible.size();i++) std::cout << newNote.possible[i];
					std::cout << std::endl;
					continue;
				}
				
				newNote.pos = (tmp[0] + tmp[2]) / 2;
				
				this->notes.push_back(newNote);
			}
		}
		sort(this->notes.begin(),this->notes.end(),[](const note x,const note y) -> bool{
			return x.pos < y.pos || (x.pos == y.pos && x.notation.technical.string < y.notation.technical.string);
		});
		for (size_t i = 1; i < notes.size();i++) {
			if(notes[i].pos == notes[i-1].pos){
				notes[i].chord = true;
			}
		}
	}
public:
	unsigned int number;						//小节数
	Time time;
	vector<note> notes;
	key key;
	measure(cv::Mat img,vector<cv::Vec4i> rows) {
		recNum(img, rows);
	}
	
};
