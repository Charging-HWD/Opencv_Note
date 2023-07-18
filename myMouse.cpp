#include "iostream" 
#include "opencv2/opencv.hpp"
using namespace cv;
using namespace std;

Mat img, imgPoint;			//全局图像
Point prePoint;				//前一时刻鼠标的坐标，用于绘制直线
Point newPoint;				//鼠标的实时坐标，用于绘制矩形
bool LDOWN_flag = false;	//鼠标左键标志，用于绘制最大矩形

void mouse_Line_Image(int event, int x, int y, int flags, void*)
{
	if (event == EVENT_RBUTTONDOWN)//单击右键
	{
		cout << "点击鼠标左键才可以绘制图迹" << endl;
	}
	if (event == EVENT_LBUTTONDOWN)//单击左键，输出坐标
	{
		prePoint = Point(x, y);
		cout << "轨迹起始点坐标" << prePoint << endl;
	}
	if (event == EVENT_MOUSEMOVE && (flags & EVENT_FLAG_LBUTTON))//按住鼠标左键移动
	{
		//通过改变图像像素显示鼠标移动轨迹
		imgPoint.at<Vec3b>(y, x) = Vec3b(0, 0, 255);
		imgPoint.at<Vec3b>(y, x-1) = Vec3b(0, 0, 255);
		imgPoint.at<Vec3b>(y, x+1) = Vec3b(0, 0, 255);
		imgPoint.at<Vec3b>(y-1, x) = Vec3b(0, 0, 255);
		imgPoint.at<Vec3b>(y+1, x) = Vec3b(0, 0, 255);
		imshow("图像窗口2", imgPoint);

		//通过绘制直线显示鼠标移动轨迹
		Point pt(x, y);
		line(img, prePoint, pt, Scalar(0, 0, 255), 2, 5, 0);
		prePoint = pt;
		imshow("图像窗口1", img);
	}
}

void mouse_Rect_Image(int event, int x, int y, int flags, void*)
{
	if (event == EVENT_RBUTTONDOWN)//单击右键
	{
		cout << "点击鼠标左键才可以绘制图迹" << endl;
	}
	if (event == EVENT_LBUTTONDOWN)//单击左键，输出坐标
	{
		prePoint = Point(x, y);
		cout << "轨迹起始点坐标" << prePoint << endl;
	}
	if(event == EVENT_MOUSEMOVE && (flags & EVENT_FLAG_LBUTTON))//按住鼠标左键移动
	{
		LDOWN_flag = true;
		newPoint = Point(x, y);
	}
	if (LDOWN_flag == true && event != EVENT_MOUSEMOVE)//鼠标已经停止
	{
		LDOWN_flag = false;
		cout << "轨迹终止点坐标" << newPoint << endl;
		rectangle(img, prePoint, newPoint, Scalar(0, 0, 255), 2, 5, 0);//
		imshow("图像窗口1", img);
	}
}

void mouse_Rect_Video(int event, int x, int y, int flags, void*)
{
	if (event == EVENT_RBUTTONDOWN)//单击右键
	{
		cout << "点击鼠标左键才可以绘制图迹" << endl;
	}
	if (event == EVENT_LBUTTONDOWN)//单击左键，输出坐标
	{
		LDOWN_flag = false;
		prePoint = Point(x, y);
		cout << "轨迹起始点坐标" << prePoint << endl;
	}
	if (event == EVENT_MOUSEMOVE && (flags & EVENT_FLAG_LBUTTON))//按住鼠标左键移动
	{
		LDOWN_flag = true;
		newPoint = Point(x, y);
		cout << "轨迹终止点坐标" << newPoint << endl;
	}
}

void Image()
{
	img = imread("E:/1.jpg");
	if (img.empty())
	{
		cout << "图像不正确" << endl;
		exit(0);
	}
	img.copyTo(imgPoint);
	imshow("图像窗口1", img);
	imshow("图像窗口2", imgPoint);
	setMouseCallback("图像窗口1", mouse_Rect_Image, 0);//静态图像画矩形
	//setMouseCallback("图像窗口1", mouse_Line_Image, 0);//静态图像画移动轨迹
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
			setMouseCallback("图像窗口1", mouse_Rect_Video, 0);
			if (LDOWN_flag == true)
			{
				rectangle(img, prePoint, newPoint, Scalar(0, 0, 255), 2, 5, 0);
			}
			imshow("图像窗口1", img);
			if (waitKey(1) == '27')
				break;
		}
	}
	return 0;
}