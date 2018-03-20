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
		if (isEmptyLine(trimmed, i, 0.001)) {
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
	if (coll.size() < 2) {
		cat = false;
		std::cout << "裁剪失败，等待二次裁剪" << std::endl;
		//二次裁剪为缩减判断空行的范围，从之前的从像素x=0 至x=col到检测到的横线的x1至x2
		std::vector<cv::Vec4i> rows;
		findRow(trimmed,CV_PI/18,rows);
		if (rows.size() > 5) {
			std::cout << "二次裁剪开始." << std::endl;
			flag = false;
			for (int st = trimmed.rows, i = 0; i < trimmed.rows; i++) {
				if (isEmptyLine(trimmed, i,std::max(std::min(rows[0][0],rows[0][2]),std::min(rows[5][0],rows[5][2])),
					std::min(std::max(rows[0][0], rows[0][2]), std::max(rows[5][0], rows[5][2])), 0.01)) {
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
		if (coll.size()<2) {
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
			
			/*cv::Mat ccolor;
			cvtColor(trimmed, ccolor, CV_GRAY2BGR);
			line(ccolor, CvPoint(0, coll[i].start), CvPoint(trimmed.cols, coll[i].start), CvScalar(0, 0, 255));
			line(ccolor, CvPoint(0, coll[i].start + coll[i].length), CvPoint(trimmed.cols, coll[i].start + coll[i].length), CvScalar(0, 0, 255));
			imshow("2", ccolor); cvWaitKey();
			*/

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
	n = toCut.size();
	delete[] r;
	cv::Mat *piece = new cv::Mat[n+1];
	piece[0] = trimmed(cv::Range(0, toCut[0].start), cv::Range(0, trimmed.cols));
	for (int i = 1; i < n; i++) {
		//bug
		piece[i] = trimmed(cv::Range(toCut[i - 1].start + toCut[i - 1].length + 1, toCut[i].start),
			cv::Range(0, trimmed.cols));
	}
	piece[n] = trimmed(cv::Range(toCut[n - 1].start + toCut[n - 1].length + 1, trimmed.rows), cv::Range(0, trimmed.cols));
	
	if (dog && cat) {
		std::cout << "运行修补算法" << std::endl;
		std::vector<space> toJoin;
		for (int i = 1; i <= n; i++) toJoin.push_back({0,piece[i].rows});
		bool* b = new bool[toCut.size()+1]();
		do{
			int a = interCheck(&toJoin, true);
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
#define forOCR 0
	std::vector<cv::Mat> chords;
	std::vector<cv::Mat> section;
	std::vector<cv::Mat> info;
	std::vector<cv::Mat> notes;
#if forOCR
	std::vector<cv::Mat> forOCR;
#endif
	flag = false;
	for (int i = 0; i <= n; i++) {
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
			for (int j = 0; j < (int)lines.size() - 1; j++) {
				section.push_back(piece[i](cv::Range(0, piece[i].rows), cv::Range(lines[j][0]+1, lines[j + 1][0])).clone());
			}
#if forOCR
			//为OCR去掉横线
			//用形态学腐蚀得到mask 将mask上的点置0
			cv::Mat eroded, dilated;
			cv::Mat hline = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 1));			//水平结构
			erode(255 - piece[i], eroded, hline);												//腐蚀
			dilate(eroded, dilated, hline);														//膨胀
			forOCR.push_back(cv::max(dilated, piece[i]));
#endif
		}
		else {
			if(flag) notes.push_back(piece[i]);
			else info.push_back(piece[i]);
		}
	}
	delete[] piece;
	for (cv::Mat& i : info) {
		imshow("info",i);
		cvWaitKey();
	}
	cvDestroyWindow("info");
	for (int i = 0; i < section.size();i++) {
		imshow("section",section[i]);
		cvWaitKey();
	}
	cvDestroyWindow("section");
	for (cv::Mat& i : notes) {
		imshow("notes",i);
		cvWaitKey();
	}
	cvWaitKey();
	cvDestroyAllWindows();
}

