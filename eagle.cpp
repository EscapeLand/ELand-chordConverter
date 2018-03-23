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
void ls(const char* lpPath, std::vector<std::string> &fileList);

char rec(Mat character) {
	Mat res;
	Ptr<KNearest> knn = KNearest::create();
	knn = load(defaultCSV, knn);
	knn->findNearest(character, 5, res);
	return NULL;
}

void train(std::string save = defaultCSV) {
	//trainData ����*��С
	//Labels ����*10
	Mat trainData,Label;
	//¼��ѵ�������ͱ��
	int num;														//num ��������ʲô����
	std::vector<std::string> fileList;
	std::string path = "C:\\Users\\Administrator\\Desktop\\";
	for (num = 0; num < 9; num++) {
		char c = num + '0';
		ls((path + c).c_str(), fileList);
		for (int i = 0; i < fileList.size(); i++) {
			Mat tmp = imread(std::string(fileList[i]));
			//std::cout << tmp.type() << std::endl;
			trainData.push_back(tmp.reshape(1, 1));
			Label.push_back(num);										//��trainData��Ӧ�ı��
		}
		fileList.clear();
	}
		
		trainData.convertTo(trainData, CV_32FC1, 1.0 / 255.0);
	
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

void ls(const char* lpPath, std::vector<std::string> &fileList)
{
	char szFind[MAX_PATH];
	WIN32_FIND_DATA FindFileData;

	strcpy_s(szFind, lpPath);
	strcat_s(szFind, "\\*.*");

	HANDLE hFind = ::FindFirstFile(szFind, &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind)    return;

	while (true)
	{
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (FindFileData.cFileName[0] != '.')
			{
				char szFile[MAX_PATH];
				strcpy_s(szFile, lpPath);
				strcat_s(szFile, "\\");
				strcat_s(szFile, (char*)(FindFileData.cFileName));
				ls(szFile, fileList);
			}
		}
		else
		{
			//std::cout << FindFileData.cFileName << std::endl;  
			char szFile[MAX_PATH];
			strcpy_s(szFile,lpPath);
			strcat_s(szFile, "\\");
			strcat_s(szFile, FindFileData.cFileName);
			fileList.push_back(std::string(szFile));
		}
		if (!FindNextFile(hFind, &FindFileData))    break;
	}
	FindClose(hFind);
}