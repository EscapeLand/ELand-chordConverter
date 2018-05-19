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
	int string;								//��
	int fret;									//Ʒ
}technical;

typedef struct notations {
	int dynamics;								//����
	technical technical;
}notations;

typedef struct note {
	bool chord = false;							//������ǣ�Ϊtrue����һ������ͬʱ����
	vector<int> possible;						//�������ܵ�Ʒ��
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
	int mode = 1;								//1-��׼��������
}key;

class measure {
private:
	void recNum(cv::Mat section, std::vector<cv::Vec4i> rows) {
		/*
		* ������Dsection::recNum
		* ���ܣ��Ӵ���ͼ������ȡ���ֵ�
		* ������
		section��Mat������ͼ��
		rows��Vec4i�������������Ϣ�����ߣ�
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
			//�޶�ɸѡ
			if (tmp[3] - tmp[1] < rows[1][1] - rows[0][1]					//�����޶�
				&& tmp[3] - tmp[1] > tmp[2] - tmp[0])						//��״�޶�
			{
				note newNote;
				cv::Mat number = section(cv::Range(tmp[1], tmp[3] + 1), cv::Range(tmp[0], tmp[2] + 1)).clone();

				//savePic("C:\\Users\\Administrator\\Desktop\\oh\\", number);

				cvtColor(number, number, CV_GRAY2BGR);
				if (number.cols != 8 || number.rows != 10) number = perspect(number, 8, 10);
				newNote.notation.technical.string = whichLine(tmp, rows);										//���ι�ϵ�ж�string
				newNote.notation.technical.fret = rec(number, newNote.possible);										//ʶ������fret
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
	unsigned int number;						//С����
	Time time;
	vector<note> notes;
	key key;
	measure(cv::Mat img,vector<cv::Vec4i> rows) {
		recNum(img, rows);
	}
	
};
