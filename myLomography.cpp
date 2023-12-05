/*
lomography效果:
首先对red通道进行 LUT 操作实现曲线变换;
对图像增加暗晕（dark halo）,最后multiply操作实现效果

LUT是向量或表，它返回给定值的预处理值，以便在存储器中执行计算
不对每个像素调用 exponential/divide 函数，只对每个可能的像素点执行一次，并将结果存储在表中，以少量内存为代价节省了 CPU 的时间
对图像中的每个像素调用一次函数，那么必须执行次数为 宽度×高度 的操作
例如 100*100 像素中，将有 10000次计算。如果可以预先计算所有可能输入的所有可能结果，就能创建 LUT 表
*/

#include <iostream>
#include <vector>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

void lomo(Mat& img_Lomo,Mat& result)
{
	const double exponential_e = std::exp(1.0);
	//建立一个256个元素的映射表  
	Mat lut(1, 256, CV_8UC1);
	for (int i = 0; i < 256; i++)
	{
		float x = (float)i / 256.0;
		lut.at<uchar>(i) = cvRound(256 * (1 / (1 + pow(exponential_e, -((x - 0.5) / 0.1)))));//pow(InputArray src, double p, OutputArray dst)
	}
	// 分离图像通道
	vector<Mat> bgr;
	split(img_Lomo, bgr);

	//将 LUT 表变量用于红色通道 
	LUT(bgr[2], lut, bgr[2]);

	// 合并通道
	merge(bgr, result);

	// 创建黑暗光环
	Mat halo(img_Lomo.rows, img_Lomo.cols, CV_32FC3, Scalar(0.3, 0.3, 0.3));
	imshow("lomo1", halo);
	// 创建一个内部带有白色圆圈的灰色图像
	circle(halo, Point(img_Lomo.cols / 2, img_Lomo.rows / 2), img_Lomo.cols / 3, Scalar(1, 1, 1), -1);
	imshow("lomo2", halo);

	//直接应用于输入图像，将得到从黑暗变为白色的强烈变化，因此使用 blur 滤镜函数对圆光晕图像应用大模糊，以获得平滑效果
	blur(halo, halo, Size(img_Lomo.cols / 3, img_Lomo.cols / 3));
	imshow("lomo3", halo);

	// 必须把输入图像从8位图像转换为32位浮点数，因为需要把具有 0-1 范围值的模糊图像与具有整数值的输入图像相乘
	Mat resultf;
	result.convertTo(resultf, CV_32FC3);
	imshow("lomo4", resultf);

	// 将每个元素的每个矩阵相乘
	multiply(resultf, halo, resultf);
	imshow("lomo5", resultf);

	// 将浮点图像矩阵结果转换为 8位图像矩阵
	resultf.convertTo(result, CV_8UC3);
	
	imshow("Lomo Effect", result);
}

int myLomography()
{
	Mat image = imread("E:/Picture/6.png");
	if (image.empty())
	{
		return -1;
	}
	Mat result;
	lomo(image, result);
	waitKey(0);
	return 0;
}