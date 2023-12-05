/*
卡通化效果是创建一个看起来像卡通的图像，分为两部分
1.边缘检测：进行降噪处理,检测出图像的边缘,检测到边缘后进行膨胀操作连接断续的边缘;
2.颜色过滤：实现目标效果边缘图像与彩色图像进行multiply操作实现效果
*/

#include <iostream>
#include <vector>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

void cartoon(Mat& img_Lomo,Mat& result)
{
	// 第一步检测图像最重要的边缘
	// 检测边缘之前需要从输入图像中去除噪声（中值滤波或者高斯滤波来消除所有可能小的噪声）
	Mat imgMedian;
	medianBlur(img_Lomo, imgMedian, 7);
	imshow("cartoon1", imgMedian);

	// 检测强边缘,第一阈值与第二阈值之间的最小值用于边缘连接，最大值用于查找强边缘的初始段
	Mat imgCanny;
	Canny(imgMedian, imgCanny, 50, 150);
	imshow("cartoon2", imgCanny);

	// 检测到边缘后，用一个小的扩张来连接断开的边缘
	Mat kernel = getStructuringElement(MORPH_RECT, Size(2, 2));
	dilate(imgCanny, imgCanny, kernel);
	imshow("cartoon3", imgCanny);

	// 如果需要将边缘的结果图像与彩色图像相乘,则需要将像素值处在 0-1 的范围内
	// 因此将 canny 边缘检测结果除以 256, 并将边缘反转为黑色
	imgCanny = imgCanny / 255;
	imgCanny = 1 - imgCanny;
	imshow("cartoon4", imgCanny);

	// 将 canny 8位无符号像素格式转换为浮点矩阵
	Mat imgCannyf;
	imgCanny.convertTo(imgCannyf, CV_32FC3);

	// 平滑边缘
	blur(imgCannyf, imgCannyf, Size(5, 5));
	imshow("cartoon5", imgCannyf);

	// 为了获得卡通外观，使用 bilateral 滤镜（一种滤波器，可以在保持边缘的同时降低图像的噪声，通过适当的参数，能够获得卡通效果）
	// 当像素邻域直径大于5时，bilateral 滤镜开始变慢。当 Sigma 坐标空间值大于 150 时，会出现卡通效果
	Mat imgBF;
	bilateralFilter(img_Lomo, imgBF, 9, 150.0, 150.0);
	imshow("cartoon6", imgBF);

	// 创建更强大的卡通效果，通过乘以和除以像素值将可能的颜色值截断为10
	result = imgBF / 25;
	result = result * 25;
	
	// 合并颜色与边缘结果,并创建一个三通道图像
	Mat imgCanny3c;
	Mat cannyChannels[] = { imgCannyf, imgCannyf, imgCannyf };
	merge(cannyChannels, 3, imgCanny3c);
	imshow("cartoon7", imgCanny3c);

	// 将颜色结果图像转换为 32 位浮点图像
	Mat resultf;
	result.convertTo(resultf, CV_32FC3);

	//将两个图像的每个元素相乘
	multiply(resultf, imgCanny3c, resultf);
	
	// 将图像转换为8位
	resultf.convertTo(result, CV_8UC3);

	imshow("cartoon Effect", result);
}

int main()
{
	Mat image = imread("E:/Picture/6.png");
	if (image.empty())
	{
		return -1;
	}
	Mat result;
	cartoon(image,result);
	waitKey(0);
	return 0;
}