#include "cv.h"
#include "ml.hpp"
#include "opencv.hpp"
#include <fstream>
#include <Windows.h>  
#include <iterator>  
#include <string> 
#include <iostream>

#define defaultCSV "tData.csv"

using namespace cv;
using namespace cv::ml;

cv::Ptr<cv::ml::KNearest> &load(std::string csv, Ptr<KNearest> &knn);

char rec(Mat character) {
	Mat res;
	Ptr<KNearest> knn = KNearest::create();
	knn = load(defaultCSV, knn);
	knn->findNearest(character, 5, res);
	return NULL;
}

void train(std::string save) {
	//trainData 个数*大小
	//Labels 个数*10
	Mat trainData,Label;
	//录入训练样本和标记
	int n,num;														//num 是样本是什么数字
	std::vector<char[MAX_PATH]> fileList;
	std::string path = "";
	for (num = 0; num < 9; num++) {
		char c = num + '0';
		ls((path + c).c_str(), fileList);
		for (int i = 0; i < n; i++) {
			Mat tmp = imread(std::string(fileList[i]));
			trainData.push_back(tmp.reshape(0, 1));
			Label.push_back(num);										//与trainData对应的标记
		}
		fileList.clear();
	}
	trainData.convertTo(trainData,CV_32F);
	Ptr<TrainData> tData = TrainData::create(trainData, ROW_SAMPLE, Label);
	std::ofstream file(save);
	file << format(tData->getTrainSamples(), Formatter::FMT_CSV);
	file.close();
	
}

Ptr<KNearest> &load(std::string csv, Ptr<KNearest> &knn) {
	int K = 5;
	Ptr<TrainData> tData = TrainData::loadFromCSV(csv, 0);
	knn->setDefaultK(K);
	knn->setIsClassifier(true);
	knn->train(tData);
	return knn;
}

void ls(const char* lpPath, std::vector<char[MAX_PATH]> &fileList)
{
	char szFind[MAX_PATH];
	WIN32_FIND_DATA FindFileData;

	strcpy(szFind, lpPath);
	strcat(szFind, "\\*.*");

	HANDLE hFind = ::FindFirstFile(szFind, &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind)    return;

	while (true)
	{
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (FindFileData.cFileName[0] != '.')
			{
				char szFile[MAX_PATH];
				strcpy(szFile, lpPath);
				strcat(szFile, "\\");
				strcat(szFile, (char*)(FindFileData.cFileName));
				ls(szFile, fileList);
			}
		}
		else
		{
			//std::cout << FindFileData.cFileName << std::endl;  
			fileList.push_back(FindFileData.cFileName);
		}
		if (!FindNextFile(hFind, &FindFileData))    break;
	}
	FindClose(hFind);
}