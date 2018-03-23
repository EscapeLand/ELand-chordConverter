#include "cv.h"                             
#include "highgui.h"

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"

#define len(x1,y1,x2,y2) ((x1)-(x2))*((x1)-(x2)) + ((y1)-(y2))*((y1)-(y2))

using namespace cv;
using namespace std;

typedef struct _space {
	int start;
	int length;
}space;

cv::Mat perspect(cv::Mat img, int width, int height);

Mat threshold(string picPath)
{
	//希望以 adaptiveThreshold 替代
	Mat arigin, r;
	arigin = imread(picPath.c_str(), 0);
	r = arigin.clone();
#define adaptive 1
#if adaptive
	adaptiveThreshold(arigin, r, 255, ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 3,15);
#else
	threshold(arigin, r, 210, 255, CV_THRESH_BINARY);				//希望尽量小的参数3
#endif
	//imshow("2", r); cvWaitKey();
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
	if (img.rows == ROI.rows && upper == 0 && lower == img.cols)
		return trim(255 - img);
	return ROI(Range(0, ROI.rows), Range(upper, lower)).clone();
}

bool* KClassify(vector<space> collection) {
	//函数名：LSMClassify
	//功能：分类对象是空白区域 struct _space，分类依据为起始和截止坐标
	//输出：算法将各区域分入两类，一类为false，一类为true；输出一个bool数组，依序对应各个区域的类别
	//		！！！切记用 delete[] 释放返回值！！！
	bool* r = new bool[collection.size()];
	int k1 = 15,k2 = 5;
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

void interCheck(vector<space> *collection) {
	size_t n = collection->size();
	if (n <= 1) return;
	int **pool = new int*[n];
	//初始化截止
	for (int i = 0; i < n;i++) pool[i] = new int[n]();
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (i == j) continue;
			pool[i][j] = abs((*collection)[i].length - (*collection)[j].length);
		}
	}
	//初值设置完毕
	//校验开始
	for (int i = 0; i < n; i++) {
		int min = 99999,max = 0;
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
			(*collection).erase((*collection).begin() + i);
			for (int k = 0; k < n; k++) delete[] pool[k];
			delete[] pool;
			return;
		}
	}
	for (int k = 0; k < n; k++) delete[] pool[k];
	delete[] pool;
	return;
}

int interCheck(vector<space> *collection,bool reserved) {
	size_t n = collection->size();
	if (n <= 1) return -1;
	int **pool = new int*[n];
	//初始化截止
	for (int i = 0; i < n; i++) pool[i] = new int[n]();
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (i == j) continue;
			pool[i][j] = abs((*collection)[i].length - (*collection)[j].length);
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
			(*collection).erase((*collection).begin() + i);
			for (int k = 0; k < n; k++) delete[] pool[k];
			delete[] pool;
			return i;
		}
	}
	for (int k = 0; k < n; k++) delete[] pool[k];
	delete[] pool;
	return -1;
}

void findRow(Mat img,double rangeTheta,vector<Vec4i> &lines) {
	//imshow("2", img); cvWaitKey();
	HoughLinesP(255-img, lines, 1, CV_PI / 180, 8, img.cols * 2 / 3,8);
Li:
	for (size_t i = 0; i < lines.size(); i++) {
		if (lines[i][2] == lines[i][0]) lines.erase(lines.begin() + i--);
		else if (abs(((float)lines[i][3] - lines[i][1]) / (lines[i][2] - lines[i][0])) > tan(rangeTheta)) lines.erase(lines.begin() + i--);
	}
	/*Mat color;
	cvtColor(img, color, CV_GRAY2BGR);
	for (size_t i = 0; i < lines.size(); i++) {
		line(color,Point(lines[i][0], lines[i][1]),Point(lines[i][2], lines[i][3]),Scalar(0,0,255));
	}
	imshow("2", color); waitKey();*/
	
	sort(lines.begin(), lines.end(), [](const Vec4i x, const Vec4i y) ->bool {
		return x[1] < y[1];
	});
	if (!lines.size()) return;
	//去除连续
	for (size_t i = 0; i < lines.size() - 1; i++) {
		if (lines[i][1] + 1 == lines[i + 1][1]) {
			size_t st = i, ed = i+1;
			for (size_t j = i + 1; j < lines.size() - 1; j++) {
				if (lines[j][1] + 1 == lines[j + 1][1]) {
					ed = j + 1;
				}
				else {
					break;
				}
			}
			size_t d = (st + ed) / 2;
			for (size_t i = st; i < d; i++) {
				lines.erase(lines.begin()+i);
			}
			for (size_t i = st + 1; i < ed - d + st + 1;i++) {
				lines.erase(lines.begin() + i);
			}
		}
	}
#define permissive 1												//是否宽容
#if permissive
	if (lines.size() > 3 && lines.size() < 6) {
		static bool foolish = false;
		if (foolish) {
			foolish = false;
			return;
		}
		lines.clear();
		//imshow("2", img); cvWaitKey();
		HoughLinesP(255 - img, lines, 1, CV_PI / 180, 3, img.cols * 2 / 3, 16);
		foolish = true;
		goto Li;
	}
#endif
}

void findCol(Mat img, double rangeTheta, int upper,int lower, vector<Vec4i> &lines) {
	//特殊参数解释：e_length，长度误差，即[length - e_length, length + e_length]
	int length = upper - lower;
	
#define isDilated 1																					//膨胀开关
#if isDilated
	Mat dilated, eroded;
	Mat vline = getStructuringElement(MORPH_RECT, Size(1, 3));										//竖直结构
	erode(img, eroded, vline);																		//腐蚀
	dilate(eroded, dilated, vline);																	//膨胀
	Mat inv = 255 - dilated;
#else
	Mat inv = 255 - img;
#endif
	HoughLinesP(inv, lines, 1, CV_PI / 180, 8, length-3 ,6);
	for (size_t i = 0; i < lines.size(); i++) {
		if (lines[i][2] == lines[i][0])
			continue;
		else if (abs(((float)lines[i][3] - lines[i][1]) / (lines[i][2] - lines[i][0])) < tan(rangeTheta))
			lines.erase(lines.begin() + i--);
	}
	for (size_t i = 0; i < lines.size(); i++) {
		for (int j = 1; j < 4; j+=2 )if (lines[i][j] > upper+3 || lines[i][j] < lower-3) {
			lines.erase(lines.begin() + i--);
			break;
		}
	}
	
	/*Mat color;
	cvtColor(img, color, CV_GRAY2BGR);
	for (size_t i = 0; i < lines.size(); i++) {
		line(color, Point(lines[i][0], lines[i][1]), Point(lines[i][2], lines[i][3]), Scalar(0, 0, 255));
	}
	imshow("2", color); waitKey();*/
	
	sort(lines.begin(), lines.end(), [](const Vec4i x,const Vec4i y) ->bool {
		return x[0] < y[0];
	});

	if (lines.size() > 1) {
		if (2 * lines[0][0] > lines[1][0] - lines[0][0]) { 
			Vec4i tmp = { 0,lines[0][1],0,lines[0][3] };
			lines.insert(lines.begin(),tmp);
		}
	}
}

void savePic(string folder,Mat pic) {
	static int num = 0;
	char c[8];
	_itoa_s(num++, c, 10);
	imwrite(folder + c + ".jpg",pic);
}

void saveNums(string folder, vector<Mat> nums){
	int cb = 0, rb = 0;
	for (Mat& i : nums) {
		assert(!i.empty());
		cb = max(cb,i.cols);
		rb = max(rb,i.rows);
	}
	for (Mat& i : nums) {
		//if (i.cols != cb || i.rows != rb) i = perspect(i,cb,rb);
		savePic(folder,i);
	}
}

Mat perspect(Mat img, int width, int height){
	cv::Mat r;

	std::vector<cv::Point2f> scorners(4);
	std::vector<cv::Point2f> dcorners(4);

	scorners[0] = cv::Point2f(0, 0);
	scorners[1] = cv::Point2f((float)img.cols - 1, 0);
	scorners[2] = cv::Point2f(0, (float)img.rows - 1);
	scorners[3] = cv::Point2f((float)img.cols - 1, (float)img.rows);

	dcorners[0] = cv::Point2f(0, 0);dcorners[1] = cv::Point2f((float)width, 0);
	dcorners[2] = cv::Point2f(0, (float)height);dcorners[3] = cv::Point2f((float)width, (float)height);

	cv::Mat tmp = cv::getPerspectiveTransform(scorners, dcorners);
	cv::warpPerspective(img, r, tmp, cv::Size(width, height));

	return r;
}