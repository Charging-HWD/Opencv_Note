/*
lomographyЧ��:
���ȶ�redͨ������ LUT ����ʵ�����߱任;
��ͼ�����Ӱ��Σ�dark halo��,���multiply����ʵ��Ч��

LUT��������������ظ���ֵ��Ԥ����ֵ���Ա��ڴ洢����ִ�м���
����ÿ�����ص��� exponential/divide ������ֻ��ÿ�����ܵ����ص�ִ��һ�Σ���������洢�ڱ��У��������ڴ�Ϊ���۽�ʡ�� CPU ��ʱ��
��ͼ���е�ÿ�����ص���һ�κ�������ô����ִ�д���Ϊ ��ȡ��߶� �Ĳ���
���� 100*100 �����У����� 10000�μ��㡣�������Ԥ�ȼ������п�����������п��ܽ�������ܴ��� LUT ��
*/

#include <iostream>
#include <vector>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

void lomo(Mat& img_Lomo,Mat& result)
{
	const double exponential_e = std::exp(1.0);
	//����һ��256��Ԫ�ص�ӳ���  
	Mat lut(1, 256, CV_8UC1);
	for (int i = 0; i < 256; i++)
	{
		float x = (float)i / 256.0;
		lut.at<uchar>(i) = cvRound(256 * (1 / (1 + pow(exponential_e, -((x - 0.5) / 0.1)))));//pow(InputArray src, double p, OutputArray dst)
	}
	// ����ͼ��ͨ��
	vector<Mat> bgr;
	split(img_Lomo, bgr);

	//�� LUT ��������ں�ɫͨ�� 
	LUT(bgr[2], lut, bgr[2]);

	// �ϲ�ͨ��
	merge(bgr, result);

	// �����ڰ��⻷
	Mat halo(img_Lomo.rows, img_Lomo.cols, CV_32FC3, Scalar(0.3, 0.3, 0.3));
	imshow("lomo1", halo);
	// ����һ���ڲ����а�ɫԲȦ�Ļ�ɫͼ��
	circle(halo, Point(img_Lomo.cols / 2, img_Lomo.rows / 2), img_Lomo.cols / 3, Scalar(1, 1, 1), -1);
	imshow("lomo2", halo);

	//ֱ��Ӧ��������ͼ�񣬽��õ��Ӻڰ���Ϊ��ɫ��ǿ�ұ仯�����ʹ�� blur �˾�������Բ����ͼ��Ӧ�ô�ģ�����Ի��ƽ��Ч��
	blur(halo, halo, Size(img_Lomo.cols / 3, img_Lomo.cols / 3));
	imshow("lomo3", halo);

	// ���������ͼ���8λͼ��ת��Ϊ32λ����������Ϊ��Ҫ�Ѿ��� 0-1 ��Χֵ��ģ��ͼ�����������ֵ������ͼ�����
	Mat resultf;
	result.convertTo(resultf, CV_32FC3);
	imshow("lomo4", resultf);

	// ��ÿ��Ԫ�ص�ÿ���������
	multiply(resultf, halo, resultf);
	imshow("lomo5", resultf);

	// ������ͼ�������ת��Ϊ 8λͼ�����
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