/*
图像均衡（即直方图均衡化）试图获得具有均匀分布值的直方图，均衡的结果是图像对比度增加；
均衡能够使对比度较低的局部区域获得高对比度，从而分散最频繁的强度
当图像非常亮或非常暗时，并且背景和前景之间存在非常小的差异时，此方法非常有用
通过使用直方图均衡化，可以增加对比度，并提升暴露过度或暴露不足的细节
缺点：
1.背景噪声的增加；
2.有用信号的减少
*/

#include <iostream>
#include <vector>
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

int Equalize()
{
	Mat img;
	img = imread("E:/Picture/3.png");
	if (img.empty())
		return 0;
	imshow("img", img);

	//可以用任何其他彩色图像格式（例如 HSV 或 YCrCb）来分离单个通道中的亮度分量
	Mat ycrcb;
	cvtColor(img, ycrcb, COLOR_BGR2YCrCb);

	//将 YCrCb 图像拆分为不同的通道矩阵
	vector<Mat> channels;
	split(ycrcb,channels);
	//只均衡在Y通道中的直方图
	equalizeHist(channels[0], channels[0]);
	imshow("Y", channels[0]);
	imshow("Cr", channels[1]);
	imshow("Cb", channels[2]);

	merge(channels, ycrcb);

	Mat result;
	cvtColor(ycrcb, result, COLOR_YCrCb2BGR);

	imshow("result", result);
	waitKey(0);
	return 0;

}