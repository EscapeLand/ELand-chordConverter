#include "cv.h"                        
#include "highgui.h"
#include "global.h"
#include <io.h>
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"

#define adaptive 1
#define dododebug 0									//0-draw nothing    1-draw rows    2-draw cols    3-draw rows and cols

#define len(x1,y1,x2,y2) ((x1)-(x2))*((x1)-(x2)) + ((y1)-(y2))*((y1)-(y2))
#define inALine(x1,y1,x2,y2,x3,y3) (((x1) - (x2)) * ((y3) - (y2)) == ((x3) - (x2)) * ((y1) - (y2)))

using namespace cv;
using namespace std;

typedef struct space {
	int start;
	int length;
}space;
template<class T>
void NormalDistribution(vector<T> &collection, int(*p)(const T x), bool* exep = NULL);

int lineThickness = 0;
int colLenth = 0;
int rowLenth = 0;

Mat Morphology(Mat img, int len, bool horizontal, bool open) {
	Mat dilated, eroded;
	Mat kernel = getStructuringElement(MORPH_RECT, horizontal
			? Size(max(len, 1), 1)													//水平结构
			: Size(1, max(len, 1))); 												//竖直结构
	if (open) {
		dilate(img, dilated, kernel);												//膨胀
		erode(dilated, eroded, kernel);												//腐蚀
		return eroded;
	}
	else {
		erode(img, eroded, kernel);													//腐蚀
		dilate(eroded, dilated, kernel);											//膨胀
		return dilated;
	}
}

Mat threshold(string picPath)
{
	/*
	 * 函数：threshold
	 * 功能：读取指定路径的图片，并将其二值化
	*/
	Mat arigin, r;
	arigin = imread(picPath.c_str(), 0);
	r = arigin.clone();
#if adaptive
	//blocksize-5:960 5:1860 7:2480
	int blocksize = 2 * (r.cols / 2000 + 2) + 1;
	adaptiveThreshold(arigin, r, 255, ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, blocksize,17);
#else
	threshold(arigin, r, 210, 255, CV_THRESH_BINARY);				//希望尽量小的参数3
#endif
	return r;
}

bool isEmptyLine(Mat img, int y,double rate) {
	//函数名 isEmptyLine 
	//功能 测试一行是否是空白行,返回布尔值
	//特殊参数：
	//	thrshold 越大，对“白”要求越严格
	//	rate 越小，对白色范围要求越严格
	int sum = 0;
	if (y >= img.rows) return true;
	uchar *ptr = img.ptr<uchar>(y);
	for (int w = 0; w < img.cols; w++)
	{
		
		if (!ptr[w]) sum++;
	}
	if ((double)sum / img.cols > rate) return false;
	else return true;
}

bool isEmptyLine(Mat img, int y,int from,int to ,double rate) {
	//函数名 isEmptyLine 
	//功能 测试一行是否是空白行,返回布尔值
	//特殊参数：
	//	thrshold 越大，对“白”要求越严格
	//	rate 越小，对白色范围要求越严格
	int sum = 0,n = min(to,img.cols);
	if (y >= img.rows) return true;
	uchar *ptr = img.ptr<uchar>(y);
	for (int w = from; w < n; w++) {
		if (!ptr[w]) sum++;
	}
	if ((double)sum / img.cols > rate) return false;
	else return true;
}

bool isEmptyCol(Mat img, int x, double rate) {
	//函数名 isEmptyCol
	//功能 测试一列是否是空白列,返回布尔值
	//特殊参数：
	//	thrshold 越大，对“白”要求越严格
	//	rate 越小，对白色范围要求越严格
	int sum = 0;
	if (x >= img.cols) return true;
	for (int y = 0; y < img.rows; y++)
	{
		uchar *ptr = img.ptr<uchar>(y);
		if (!ptr[x]) sum++;
	}
	if ((double)sum / img.rows > rate) return false;
	else return true;
}

Mat trim(Mat img, double threshold = 0.01) {
	int i;
	int upper;
	for (i = 0; i < img.rows; i++) if (!isEmptyLine(img, i, threshold)) {
		upper = i;
		break;
	}
	int lower = upper;
	for (i = img.rows-1; i >upper ; i--) if (!isEmptyLine(img, i, threshold)) {
		lower = i+1;
		break;
	}
	if (lower == upper) {
		Mat r;
		return r;
	}
	Mat ROI = img(Range(upper, lower),Range(0, img.cols)).clone();
	for (i = 0; i < img.cols; i++) if (!isEmptyCol(ROI, i, threshold)) {
		upper = i;
		break;
	}
	lower = upper;
	for (i = img.cols - 1; i >upper; i--) if (!isEmptyCol(ROI, i, threshold)) {
		lower = i + 1;
		break;
	}
	/*if (img.rows == ROI.rows && upper == 0 && lower == img.cols)
		return trim(255 - img);*/
	return ROI(Range(0, ROI.rows), Range(upper, lower)).clone();
}

bool* KClassify(vector<space> collection) {
	//函数名：LSMClassify
	//功能：分类对象是空白区域 struct _space，分类依据为起始和截止坐标
	//输出：算法将各区域分入两类，一类为false，一类为true；输出一个bool数组，依序对应各个区域的类别
	//		！！！切记用 delete[] 释放返回值！！！

	//k1 15:1800    k2 5:1800
	bool* r = new bool[collection.size()];
	int k1 = 18 ,k2 = 5;
	int min = collection[0].length, max = min;
	for (int i = 0; i < collection.size(); i++) {
		if (collection[i].length > max) {
			max = collection[i].length;
		}
		else if(collection[i].length < min){
			min = collection[i].length;
		}
	}
	for (int i = 0; i < collection.size(); i++) {
		if (min + k1 > collection[i].length) {
			r[i] = false;
		}
		else if (max - k2 < collection[i].length) {
			r[i] = true;
		}
		else {
			if (collection[i].length - min - k1 < max - k2 - collection[i].length) {
				k1 = collection[i].length - min + 1;
				r[i] = false;
			}
			else if (collection[i].length - min - k1 > max - k2 - collection[i].length) {
				k2 = max - collection[i].length + 1;
				r[i] = true;
			}
			else {
				//要是等于就不要他了hhh
				r[i] = false;
			}
		}
	}
	return r;
}

int interCheck(vector<space> &collection) {
	size_t n = collection.size();

	if (n <= 1) return -1;
	int **pool = new int*[n];
	//初始化截止
	for (int i = 0; i < n; i++) pool[i] = new int[n]();
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (i == j) continue;
			pool[i][j] = abs(collection[i].length - collection[j].length);
		}
	}
	//初值设置完毕
	//校验开始
	for (int i = 0; i < n; i++) {
		int min = 99999, max = 0;
		for (int j = 0; j < n; j++) {
			if (j == i) continue;
			if (min > pool[i][j]) {
				min = pool[i][j];
			}
			for (int q = 0; q < n; q++) {
				if (q == i) continue;
				if (max < pool[q][j]) {
					max = pool[q][j];
				}
			}
		}
		if (max < min) {
			collection.erase(collection.begin() + i);
			for (int k = 0; k < n; k++) delete[] pool[k];
			delete[] pool;
			return i;
		}
	}
	for (int k = 0; k < n; k++) delete[] pool[k];
	delete[] pool;
	return -1;
}

void findRow(Mat img,double rangeTheta,vector<Vec4i> &lines,int &thickness) {
	/*
	* 函数：findRow
	* 功能：概率霍夫变换提取横线
	* 特殊参数：rangeTheta，直线偏离水平方向的最大角度
	* thickness，算得直线的最大宽度
	* lines，筛选后的结果
	*/

	//Mat dilated, eroded;
	//Mat hline = getStructuringElement(MORPH_RECT, Size(max(img.cols / 120, 1),1));						//竖直结构
	//erode(img, eroded, hline);																			//腐蚀
	//dilate(eroded, dilated, hline);																		//膨胀
	//Mat inv = 255 - dilated;
	HoughLinesP(255-img, lines, 1, CV_PI / 180, 100, img.cols * 2 / 3, img.cols/50);
	  //角度筛选
	for (size_t i = 0; i < lines.size(); i++) {
		if (lines[i][2] == lines[i][0]) lines.erase(lines.begin() + i--);
		else if (abs(((float)lines[i][3] - lines[i][1]) / (lines[i][2] - lines[i][0])) > tan(rangeTheta)) {
			lines.erase(lines.begin() + i--);
			continue;
		}
	}
	sort(lines.begin(), lines.end(), [](const Vec4i x, const Vec4i y) ->bool {
		return x[1] < y[1];
	});
	if (!lines.size()) return;
	//去除连续
	for (int i = 0; i < (int)lines.size() - 1; i++) {
		if (lines[i][1] + 1 >= lines[i + 1][1]) {
			size_t st = i, ed = i+1;
			for (size_t j = i + 1; j < lines.size() - 1; j++) {
				if (lines[j][1] + 1 >= lines[j + 1][1]) {
					ed = j + 1;
				}
				else {
					break;
				}
			}
			size_t d = (st + ed) / 2;
			thickness = max((int)(ed - st + 1),thickness);
			for (int i = (int)ed; i >= (int)st; i--) {
				if (i == d) continue;
				lines.erase(lines.begin()+i);
			}
		}
	}

	if (lines.size() < 3) return;
	vector<int> tmp;
	for (int i = 0; i < lines.size() - 1; i++) {
		tmp.push_back(lines[i+1][1] - lines[i][1]);
	}
	bool* classifier = new bool[tmp.size()];
	NormalDistribution<int>(tmp, [](const int x) -> int{return x; },classifier);
	for (int k = 0,i = 0; i < tmp.size()-1; i++) {
		if (classifier[i] && classifier[i + 1]) {
			lines.erase(lines.begin() + i + 1 + k++);
		}
	}
	if (classifier[0]) lines.erase(lines.begin());
	if (classifier[tmp.size() - 1]) lines.erase(lines.begin() + lines.size() - 1);
	delete[] classifier;
#if dododebug % 2
	Mat color;
	cvtColor(img, color, CV_GRAY2BGR);
	for (size_t i = 0; i < lines.size(); i++) {
		line(color,Point(lines[i][0], lines[i][1]),Point(lines[i][2], lines[i][3]),Scalar(0,0,255));
	}
	imshow("2", color); cvWaitKey();
#endif
}

void findCol(Mat img, double rangeTheta, int upper,int lower, int thickness, vector<Vec4i> &lines) {
	/*
	 * 函数：findCol
	 * 功能：概率霍夫变换提取竖线
	 * 特殊参数：rangeTheta，直线偏离竖直方向的最大角度
	 * upper，检测到的横线的上界；lower，检测到的横线的下界
	 * thickness，直线的宽度
	 * lines，筛选后的结果
	*/
	int length = upper - lower;
	thickness = max(1, thickness);
	Mat toScan = img(Range(lower,upper), Range::all()).clone();
	Mat inv = 255 - Morphology(toScan, max(toScan.rows / 2, 1), false, true);
	//imshow("2", inv); cvWaitKey();
	//80:140
	HoughLinesP(inv, lines, 1, CV_PI / 180, (toScan.rows - 2 * thickness) / 2, toScan.rows - 2 * thickness, thickness + 2);
	for (size_t i = 0; i < lines.size(); i++) {
		if (lines[i][2] == lines[i][0]) continue;
		else if (abs(((float)lines[i][3] - lines[i][1]) / (lines[i][2] - lines[i][0])) < tan(rangeTheta))
			lines.erase(lines.begin() + i--);
	}
	sort(lines.begin(), lines.end(), [](const Vec4i x,const Vec4i y) ->bool {
		return x[0] < y[0];
	});
	//去除连续
	for (int i = 0; i < (int)lines.size() - 1; i++) {
		if (lines[i][0] + 1 >= lines[i + 1][0]) {
			size_t st = i, ed = i + 1;
			for (int j = i + 1; (int)j < lines.size() - 1; j++) {
				if (lines[j][0] + 1 >= lines[j + 1][0]) {
					ed = j + 1;
				}
				else {
					break;
				}
			}
			size_t d = (st + ed) / 2;
			Vec4i tmp = lines[d];
			lines.erase(lines.begin() + st, lines.begin() + ed + 1);
			lines.insert(lines.begin() + st, tmp);
		}
	}
	for (size_t i = 1; i < lines.size(); i++) {
		if (!colLenth) {
			if(lines[i][0] - lines[i - 1][0] > 16) colLenth = lines[i][0] - lines[i - 1][0];
		}
		else {
			if (lines[i][0] - lines[i - 1][0] >= colLenth / 5) {
				colLenth = (colLenth + lines[i][0] - lines[i - 1][0]) / 2;
			}
		}
	}
#if dododebug == 2
	Mat color;
	cvtColor(img, color, CV_GRAY2BGR);
	for (size_t i = 0; i < lines.size(); i++) {
		line(color, Point(lines[i][0], 0), Point(lines[i][2], img.rows), Scalar(0, 0, 255));
	}
	imshow("2", color); waitKey(); cvWaitKey();
#endif
}

void savePic(string folder,Mat pic) {
	/*
	 * 函数：sacePic
	 * 功能：将一个矩阵保存到硬盘
	*/

	if (_access(folder.c_str(), 0) == -1) {
		system(("md " + folder).c_str());
	}
	static int num = 0;
	char c[8];
	_itoa_s(num++, c, 10);
	imwrite(folder + "\\" + c + ".jpg",pic);
}

Mat perspect(Mat img, int width, int height) {
	/*
	* 函数：perspect
	* 功能：借助透视变换拉伸图片
	* 特殊参数：width, 目标宽度；height，目标高度
	*/
	cv::Mat r;

	std::vector<cv::Point2f> scorners(4);
	std::vector<cv::Point2f> dcorners(4);

	scorners[0] = cv::Point2f(0, 0);
	scorners[1] = cv::Point2f((float)img.cols - 1, 0);
	scorners[2] = cv::Point2f(0, (float)img.rows - 1);
	scorners[3] = cv::Point2f((float)img.cols - 1, (float)img.rows);

	dcorners[0] = cv::Point2f(0, 0); dcorners[1] = cv::Point2f((float)width, 0);
	dcorners[2] = cv::Point2f(0, (float)height); dcorners[3] = cv::Point2f((float)width, (float)height);

	cv::Mat tmp = cv::getPerspectiveTransform(scorners, dcorners);
	cv::warpPerspective(img, r, tmp, cv::Size(width, height));

	return r;
}

void saveNums(string folder, vector<Mat> nums) {
	/*
	 * 函数：saveNums
	 * 功能：将nums中的图片保存到folder
	*/
	cout << "开始保存图片" << endl;
	for (Mat& i : nums) {
		if (i.cols != 8 || i.rows != 10) i = perspect(i, 8, 10);
		int sum = 0;
		for (int j = 0; j < i.rows; j++) {
			uchar *ptr = i.ptr<uchar>(j);
			for (int w = 0; w < i.cols; w++)
			{
				if (!ptr[w]) sum++;
			}
		}
		if (sum > 8 && sum < 48) savePic(folder, i);
	}
	cout << "图片保存完成" << endl;
}



template<class T>
void NormalDistribution(vector<T> &collection, int(*p)(const T x), bool* exep) {
	/*
	 * 函数：NormalDistribution
	 * 功能：利用正态分布判断数据是否相近
	 * 输入：collection，输入数据集；
	 * p，函数指针，对collection中的任意元素，如何访问其用于分类的特征；
	 * exep，长度与collection相同的bool数组，标明对应元素是否聚类
	*/
	float sigma, mu;
	size_t sum = 0, sum2 = 0;
	for (T& i : collection) {
		sum += p(i);
		sum2 += p(i) * p(i);
	}
	mu = sum / (float)collection.size();
	sigma = sqrt(sum2 / (float)collection.size() - mu * mu);

	for (int i = 0; i < (int)collection.size(); i++) {
		if (abs(p(collection[i]) - mu) > ceil(2 * sigma)) {

			if (!exep) collection.erase(collection.begin() + i);
			exep[i] = true;
			continue;
		}
		exep[i] = false;
	}
}

inline bool through(Vec4i character, int row) {
	if (character[1] >= character[3]) {
		return false;
	}
	if (character[1] < row && row < character[3]) {
		return true;
	}
	return false;
}

int whichLine(Vec4i character, vector<Vec4i> rows) {
	for (int i = 0; i < 6; i++) {
		if (through(character, rows[i][1])) return i + 1;
	}
	return 0;
}

bool isDotLine(Mat img, Vec4i line) {
	auto y = [line](int x) -> int {
		return line[1] + (line[3] - line[1]) / (line[2] - line[0]) * (x - line[0]);
	};
	int sum = 0;
	for (int i = line[0]; i <= line[2]; i++) {
		if (!img.at<int>(i, y(i))) {
			sum++;
		}
	}
	if (sum < 0.7 * (line[2] - line[0])) {
		return false;
	}
	return true;
}