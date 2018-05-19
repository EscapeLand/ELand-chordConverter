#include"myheader.h"

#define mode 2  //0-main program  1-train  2-test

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
	std::cout << rec(cv::imread(f)) << std::endl;
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
	//imshow("2", trimmed); cvWaitKey();
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
		std::cout << "�����㷨����" << std::endl;
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
	std::vector<cv::Mat> section;
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
		std::cout << "�����޲��㷨" << std::endl;
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
				//��piece[i]��piece[i-1]ƴ����һ��
				vconcat(piece[i-1],piece[i], piece[i - 1]);
				piece[i].release();
			}
		}
		delete[] b;
	}
	
	

	for (int i = 0; i <= n; i++) {
		if (piece[i].empty()) continue;
		std::vector<cv::Vec4i> rows;
		int thick = 0;
		findRow(piece[i], CV_PI / 18, rows,thick);
		
		if (rows.size() > 5) {
			flag = true;
			chords.push_back(piece[i]);
			//ΪOCRȥ������
			//����̬ѧ��ʴ�õ�mask ��mask�ϵĵ���0
			cv::Mat eroded, dilated;
			cv::Mat hline = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(piece[i].cols / 30, 1));			//ˮƽ�ṹ
			erode(255 - piece[i], eroded, hline);																//��ʴ
			dilate(eroded, dilated, hline);																		//����
			toOCR = cv::max(dilated, piece[i]);

			std::vector<cv::Vec4i> lines;
			int max = std::min(rows[5][1], rows[5][3]);
			int min = std::max(rows[0][1], rows[0][3]);

			findCol(piece[i], CV_PI / 18 * 8, max, min, thick, lines);
			int range = 0;
			int bottom;
			if(lines.size()) range = cut(toOCR, lines, 0, section, true);
			extractNum(pos, nums, section, rows, bottom, range);
			std::vector<cv::Mat> tmpNotes;
			cut(piece[i], lines, 0,tmpNotes , true);
			for (size_t i = 0; i < tmpNotes.size(); i++) {
				cv::Mat tmp;
				tmpNotes[i](cv::Range(bottom, tmpNotes[i].rows), cv::Range::all()).copyTo(tmp);
				timeValue.push_back(tmp);
			}
		}
		else {
			if(flag) notes.push_back(piece[i]);
			else info.push_back(piece[i]);
		}
	}
	piece.clear();

	for (int i = 0; i < pos.size(); i++) {

	}
	/*for (cv::Mat& i : info) {
		imshow("info",i);
		cvWaitKey();
	}
	cvDestroyWindow("info");*/
	/*for (cv::Mat& i : section) {
		if(!i.empty()) imshow("section",i);
		else std::cerr << "empty image in section" << std::endl;
		cvWaitKey(0);
	}
	cvDestroyWindow("section");*/
	/*for (cv::Mat& i : notes) {
		imshow("notes",i);
		cvWaitKey();
	}*/
	/*cvWaitKey(0);
	cvDestroyAllWindows();*/

	
	/*saveNums("C:\\Users\\Administrator\\Desktop\\oh\\", nums);
	system("pause");*/

	/*for (cv::Mat& i : timeValue) {
		std::vector<space> spaces;
		duration(i, spaces);
	}*/
}

