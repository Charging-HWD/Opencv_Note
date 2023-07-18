#include "iostream" 
#include "opencv2/opencv.hpp"
using namespace cv;
using namespace std;

Mat img, imgPoint;			//ȫ��ͼ��
Point prePoint;				//ǰһʱ���������꣬���ڻ���ֱ��
Point newPoint;				//����ʵʱ���꣬���ڻ��ƾ���
bool LDOWN_flag = false;	//��������־�����ڻ���������

void mouse_Line_Image(int event, int x, int y, int flags, void*)
{
	if (event == EVENT_RBUTTONDOWN)//�����Ҽ�
	{
		cout << "����������ſ��Ի���ͼ��" << endl;
	}
	if (event == EVENT_LBUTTONDOWN)//����������������
	{
		prePoint = Point(x, y);
		cout << "�켣��ʼ������" << prePoint << endl;
	}
	if (event == EVENT_MOUSEMOVE && (flags & EVENT_FLAG_LBUTTON))//��ס�������ƶ�
	{
		//ͨ���ı�ͼ��������ʾ����ƶ��켣
		imgPoint.at<Vec3b>(y, x) = Vec3b(0, 0, 255);
		imgPoint.at<Vec3b>(y, x-1) = Vec3b(0, 0, 255);
		imgPoint.at<Vec3b>(y, x+1) = Vec3b(0, 0, 255);
		imgPoint.at<Vec3b>(y-1, x) = Vec3b(0, 0, 255);
		imgPoint.at<Vec3b>(y+1, x) = Vec3b(0, 0, 255);
		imshow("ͼ�񴰿�2", imgPoint);

		//ͨ������ֱ����ʾ����ƶ��켣
		Point pt(x, y);
		line(img, prePoint, pt, Scalar(0, 0, 255), 2, 5, 0);
		prePoint = pt;
		imshow("ͼ�񴰿�1", img);
	}
}

void mouse_Rect_Image(int event, int x, int y, int flags, void*)
{
	if (event == EVENT_RBUTTONDOWN)//�����Ҽ�
	{
		cout << "����������ſ��Ի���ͼ��" << endl;
	}
	if (event == EVENT_LBUTTONDOWN)//����������������
	{
		prePoint = Point(x, y);
		cout << "�켣��ʼ������" << prePoint << endl;
	}
	if(event == EVENT_MOUSEMOVE && (flags & EVENT_FLAG_LBUTTON))//��ס�������ƶ�
	{
		LDOWN_flag = true;
		newPoint = Point(x, y);
	}
	if (LDOWN_flag == true && event != EVENT_MOUSEMOVE)//����Ѿ�ֹͣ
	{
		LDOWN_flag = false;
		cout << "�켣��ֹ������" << newPoint << endl;
		rectangle(img, prePoint, newPoint, Scalar(0, 0, 255), 2, 5, 0);//
		imshow("ͼ�񴰿�1", img);
	}
}

void mouse_Rect_Video(int event, int x, int y, int flags, void*)
{
	if (event == EVENT_RBUTTONDOWN)//�����Ҽ�
	{
		cout << "����������ſ��Ի���ͼ��" << endl;
	}
	if (event == EVENT_LBUTTONDOWN)//����������������
	{
		LDOWN_flag = false;
		prePoint = Point(x, y);
		cout << "�켣��ʼ������" << prePoint << endl;
	}
	if (event == EVENT_MOUSEMOVE && (flags & EVENT_FLAG_LBUTTON))//��ס�������ƶ�
	{
		LDOWN_flag = true;
		newPoint = Point(x, y);
		cout << "�켣��ֹ������" << newPoint << endl;
	}
}

void Image()
{
	img = imread("E:/1.jpg");
	if (img.empty())
	{
		cout << "ͼ����ȷ" << endl;
		exit(0);
	}
	img.copyTo(imgPoint);
	imshow("ͼ�񴰿�1", img);
	imshow("ͼ�񴰿�2", imgPoint);
	setMouseCallback("ͼ�񴰿�1", mouse_Rect_Image, 0);//��̬ͼ�񻭾���
	//setMouseCallback("ͼ�񴰿�1", mouse_Line_Image, 0);//��̬ͼ���ƶ��켣
	waitKey(0);
}

int main()
{
	VideoCapture Camera;
	Camera.open(0);
	while (Camera.isOpened())
	{
		Camera.read(img);
		if (!img.empty())
		{
			setMouseCallback("ͼ�񴰿�1", mouse_Rect_Video, 0);
			if (LDOWN_flag == true)
			{
				rectangle(img, prePoint, newPoint, Scalar(0, 0, 255), 2, 5, 0);
			}
			imshow("ͼ�񴰿�1", img);
			if (waitKey(1) == '27')
				break;
		}
	}
	return 0;
}