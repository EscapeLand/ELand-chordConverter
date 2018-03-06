#include"myheader.h"

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
	bool cat = true;
	if (!coll.size()) {
		cat = false;
		std::cout << "裁剪失败，等待二次裁剪" << std::endl;
		//TODO
		std::vector<cv::Vec4i> rows;
		findRow(trimmed,CV_PI/18,rows);
		if (rows.size() > 5) {
			std::cout << "二次裁剪开始." << std::endl;
			flag = false;
			for (int st = trimmed.rows, i = 0; i < trimmed.rows; i++) {
				if (isEmptyLine(trimmed, i,std::max(std::min(rows[0][0],rows[0][2]),std::min(rows[5][0],rows[5][2])),
					std::min(std::max(rows[0][0], rows[0][2]), std::max(rows[5][0], rows[5][2])), 0)) {
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
			
		}
		if (!coll.size()) {
			std::cout << "二次裁剪失败，请手动处理" << std::endl;
			imshow("WTF", trimmed);
			cvWaitKey();
			return 1;
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
			cv::Mat ccolor;
			cvtColor(img, ccolor, CV_GRAY2BGR);
			line(ccolor, CvPoint(0, coll[i].start), CvPoint(trimmed.cols, coll[i].start), CvScalar(0, 0, 255));
			line(ccolor, CvPoint(0, coll[i].start + coll[i].length), CvPoint(trimmed.cols, coll[i].start + coll[i].length), CvScalar(0, 0, 255));
			imshow("2", ccolor); cvWaitKey(); 
			
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
	
	if (dog && cat) {
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
		for (size_t i = toCut.size(); i > 0; i--) {
			if (!b[i]) {
				//将piece[i]与piece[i-1]拼接在一起
				vconcat(piece[i-1],piece[i], piece[i - 1]);
				piece[i].release();
			}
		}
		delete[] b;
	}
	std::vector<cv::Mat> chords;
	std::vector<cv::Mat> section;
	std::vector<cv::Mat> info;
	std::vector<cv::Mat> notes;
	flag = false;
	for (int i = 0; i <= toCut.size(); i++) {
		if (piece[i].empty()) continue;
		std::vector<cv::Vec4i> rows;
		findRow(piece[i], CV_PI / 18, rows);
		
		if (rows.size() > 5) {
			flag = true;
			chords.push_back(piece[i]);

			std::vector<cv::Vec4i> lines;
			int max = std::min(rows[5][1],rows[5][3]);
			int min = std::max(rows[0][1],rows[0][3]);
			findCol(piece[i], CV_PI / 18 * 8, max , min , lines);
			for (int j = 0; j < lines.size() - 1; j++) {
				section.push_back(piece[i](cv::Range(0, piece[i].rows), cv::Range(lines[j][0]+1, lines[j + 1][0])).clone());
			}
			
		}
		else {
			if(flag) notes.push_back(piece[i]);
			else info.push_back(piece[i]);
		}
	}
	delete[] piece;
	for (int i = 0; i < section.size();i++) {
		imshow("section",section[i]);
		cvWaitKey(1000);
	}
	cvDestroyWindow("section");
	for (cv::Mat& i : notes) {
		imshow("notes",i);
		cvWaitKey(1000);
	}
	cvWaitKey();
	cvDestroyAllWindows();
	
}

