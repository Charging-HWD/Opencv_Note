/*
ͼ����⣨��ֱ��ͼ���⻯����ͼ��þ��о��ȷֲ�ֵ��ֱ��ͼ������Ľ����ͼ��Աȶ����ӣ�
�����ܹ�ʹ�ԱȶȽϵ͵ľֲ������ø߶Աȶȣ��Ӷ���ɢ��Ƶ����ǿ��
��ͼ��ǳ�����ǳ���ʱ�����ұ�����ǰ��֮����ڷǳ�С�Ĳ���ʱ���˷����ǳ�����
ͨ��ʹ��ֱ��ͼ���⻯���������ӶԱȶȣ���������¶���Ȼ�¶�����ϸ��
ȱ�㣺
1.�������������ӣ�
2.�����źŵļ���
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

	//�������κ�������ɫͼ���ʽ������ HSV �� YCrCb�������뵥��ͨ���е����ȷ���
	Mat ycrcb;
	cvtColor(img, ycrcb, COLOR_BGR2YCrCb);

	//�� YCrCb ͼ����Ϊ��ͬ��ͨ������
	vector<Mat> channels;
	split(ycrcb,channels);
	//ֻ������Yͨ���е�ֱ��ͼ
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