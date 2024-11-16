#include <opencv2/opencv.hpp>
#include <iostream>
#include "zbar.h" 
#include <string>
#include <time.h>
#include <thread>
#include <fstream>
#include <Windows.h>

int getImagePathList(std::string folder, std::vector<cv::String>& imagePathList)
{
	//search all the image in a folder
	cv::glob(folder, imagePathList);
	return 0;
}



int main()
{
	std::string log_txt = "result.txt";
	std::fstream f;
	f.open(log_txt, std::ios::out);
	HMODULE hDll = LoadLibraryA("scanner.dll");
	if (hDll == NULL) {
		std::cerr << "无法加载DLL：" << GetLastError() << std::endl;
		return 1;
	}

	typedef int(*MYFUNCTION)(unsigned char* , int , int , int , char* , int* );
	MYFUNCTION scan = (MYFUNCTION)GetProcAddress(hDll, "scan");
	if (scan == NULL) {
		std::cerr << "无法获取函数地址：" << GetLastError() << std::endl;
		FreeLibrary(hDll);
		return 1;
	}

	// 获取输入输出文件夹
	std::string result_folder = "choose_test_result";  
	cv::String folder = "test/images";
	std::vector<cv::String> imagePathList;
	getImagePathList(folder, imagePathList);

	for (int i = 0; i < imagePathList.size(); i++)
	{
		// 读取
		std::cout << imagePathList[i] << std::endl;
		
		cv::Mat img = cv::imread(imagePathList[i]);
		// 计时
		clock_t start, end;
		start = clock();
		// 检测
		unsigned char* buffer = img.data;

		//char* preresultdata = (char*)malloc(2048 * sizeof(char));
		char preresultdata[2048];
		int lenth = 10;
		int res = scan(buffer, img.cols, img.rows, img.channels(), preresultdata, &lenth);
		std::cout << "res:" << res << "\t resultdata:" << preresultdata << std::endl;
		std::cout << "lenth:" << lenth << std::endl;

		// 终止
		end = clock();
		// 输出结果
		double spend_time = double(end - start) / CLOCKS_PER_SEC;
		std::cout << "spend time:" << spend_time << "s" << std::endl;
		f << imagePathList[i] << " " << preresultdata <<" "<<spend_time<<"s"<< "\n";
		/*std::cout << res << std::endl;*/
		//imwrite(result_folder + "/" + std::to_string(i + 1) + ".jpg", img);
	}
	f.close();

	std::cout << "end" << std::endl;

	return 0;
}