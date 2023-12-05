/*
��ͨ��Ч���Ǵ���һ����������ͨ��ͼ�񣬷�Ϊ������
1.��Ե��⣺���н��봦��,����ͼ��ı�Ե,��⵽��Ե��������Ͳ������Ӷ����ı�Ե;
2.��ɫ���ˣ�ʵ��Ŀ��Ч����Եͼ�����ɫͼ�����multiply����ʵ��Ч��
*/

#include <iostream>
#include <vector>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

void cartoon(Mat& img_Lomo,Mat& result)
{
	// ��һ�����ͼ������Ҫ�ı�Ե
	// ����Ե֮ǰ��Ҫ������ͼ����ȥ����������ֵ�˲����߸�˹�˲����������п���С��������
	Mat imgMedian;
	medianBlur(img_Lomo, imgMedian, 7);
	imshow("cartoon1", imgMedian);

	// ���ǿ��Ե,��һ��ֵ��ڶ���ֵ֮�����Сֵ���ڱ�Ե���ӣ����ֵ���ڲ���ǿ��Ե�ĳ�ʼ��
	Mat imgCanny;
	Canny(imgMedian, imgCanny, 50, 150);
	imshow("cartoon2", imgCanny);

	// ��⵽��Ե����һ��С�����������ӶϿ��ı�Ե
	Mat kernel = getStructuringElement(MORPH_RECT, Size(2, 2));
	dilate(imgCanny, imgCanny, kernel);
	imshow("cartoon3", imgCanny);

	// �����Ҫ����Ե�Ľ��ͼ�����ɫͼ�����,����Ҫ������ֵ���� 0-1 �ķ�Χ��
	// ��˽� canny ��Ե��������� 256, ������Ե��תΪ��ɫ
	imgCanny = imgCanny / 255;
	imgCanny = 1 - imgCanny;
	imshow("cartoon4", imgCanny);

	// �� canny 8λ�޷������ظ�ʽת��Ϊ�������
	Mat imgCannyf;
	imgCanny.convertTo(imgCannyf, CV_32FC3);

	// ƽ����Ե
	blur(imgCannyf, imgCannyf, Size(5, 5));
	imshow("cartoon5", imgCannyf);

	// Ϊ�˻�ÿ�ͨ��ۣ�ʹ�� bilateral �˾���һ���˲����������ڱ��ֱ�Ե��ͬʱ����ͼ���������ͨ���ʵ��Ĳ������ܹ���ÿ�ͨЧ����
	// ����������ֱ������5ʱ��bilateral �˾���ʼ�������� Sigma ����ռ�ֵ���� 150 ʱ������ֿ�ͨЧ��
	Mat imgBF;
	bilateralFilter(img_Lomo, imgBF, 9, 150.0, 150.0);
	imshow("cartoon6", imgBF);

	// ������ǿ��Ŀ�ͨЧ����ͨ�����Ժͳ�������ֵ�����ܵ���ɫֵ�ض�Ϊ10
	result = imgBF / 25;
	result = result * 25;
	
	// �ϲ���ɫ���Ե���,������һ����ͨ��ͼ��
	Mat imgCanny3c;
	Mat cannyChannels[] = { imgCannyf, imgCannyf, imgCannyf };
	merge(cannyChannels, 3, imgCanny3c);
	imshow("cartoon7", imgCanny3c);

	// ����ɫ���ͼ��ת��Ϊ 32 λ����ͼ��
	Mat resultf;
	result.convertTo(resultf, CV_32FC3);

	//������ͼ���ÿ��Ԫ�����
	multiply(resultf, imgCanny3c, resultf);
	
	// ��ͼ��ת��Ϊ8λ
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