#include<opencv2/opencv.hpp>
#include<iostream>

using namespace std;
using namespace cv;

VideoCapture g_VideoCapture;
vector<Point> initialPoints, docPoints;

//ͼ��Ԥ����ת��Ϊ�Ҷȡ����ģ����ʹ��Canny��Ե������ҵ���Ե��֪��ֽ�������������ֽ�ŵ�������ȡ�ĸ��ǵõ�����ͼ
Mat preProcessing(Mat img) {

	/*�Ҷ�*/
	Mat imgGray;
	cvtColor(img, imgGray, COLOR_BGR2GRAY);

	/*��˹ģ��*/
	Mat imgBlur;
	GaussianBlur(imgGray, imgBlur, Size(7, 7), 5, 0);

	/*��ֵ��*/
	Mat imgThreshold;
	threshold(imgBlur, imgThreshold, 200, 255, THRESH_BINARY | THRESH_OTSU);

	/*��Ե*/
	Mat imgCanny;
	Canny(imgThreshold, imgCanny, 25, 75);

	/*����*/
	Mat imgDil;
	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));//����һ���ˣ�����Size��ֻ����������������/��ʴ����
	dilate(imgCanny, imgDil, kernel);//���ű�Ե�����ӱ�Ե��ȣ�
	return imgDil;
}
 
//����ֽ�ŵ�4���ǵ�
vector<Point> getContours(Mat imgDil) {//imgDil�Ǵ�������ű�Ե��ͼ����������������img��Ҫ�����ϻ���������ͼ��
	vector<vector<Point>> contours;//������⵽��������ÿ�������ߴ洢Ϊһ���������
	vector<Vec4i> hierarchy;//��������ӳ�����˵���Ϣ  typedef Vec<int, 4> Vec4i;����4������ֵ
	findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	//drawContours(img, contours, -1, Scalar(255, 0, 255), 2);//img��Ҫ����������ʲôͼƬ�ϣ�contours��Ҫ���Ƶ�������-1����Ҫ���Ƶ������ţ�-1��ʾ������������Saclar��ʾ������ɫ��2��ʾ���
	vector<vector<Point>> conPoly(contours.size());//conploy������ӦС��contours
	vector<Rect> boundRect(contours.size());
	vector<Point> biggest;
	int maxArea = 0;

	//��������ͨ�������������������
	for (int i = 0; i < contours.size(); i++) {//������⵽������
		int area = contourArea(contours[i]);
		string objectType;
		if (area > 1000) {//���������1000�Ż���
			//���������ܳ��򰼿ӳ��ȡ��ú������������߳��Ⱥͷ�յ��ܳ���
			float peri = arcLength(contours[i], true);//�����������ܳ�
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);//��ָ���ľ��Ƚ��ƶ�������ߡ��ڶ�������conPloy[i]�洢���ƵĽ�����������

			if (area > maxArea && conPoly[i].size() == 4) {
				biggest = { conPoly[i][0],conPoly[i][1],conPoly[i][2],conPoly[i][3] };
				maxArea = area;
			}
			//rectangle/*���Ʊ߽����*/(imgOriginal, boundRect[i].tl()/*tl()��topleft�������Ͻ�����*/, boundRect[i].br()/*br()��bottom right�������½�����*/, Scalar(0, 255, 0), 5);
			//drawContours(imgOriginal, conPoly, i, Scalar(255, 0, 255), 2);
		}
	}
	return biggest;
}

void drawPoints(Mat& img,vector<Point> points, Scalar color) {
	for (int i = 0; i < points.size(); i++) {
		circle(img, points[i], 10, color, FILLED);
		putText(img, to_string(i), points[i], FONT_HERSHEY_PLAIN, 4, color, 4);
	}
}

vector<Point> reorder(vector<Point> points) {//��ǵ��˳���䣬Ҫȷ��һ��˳�� 0 1 2 3							 
	vector<Point> newPoints;
	vector<int>  sumPoints, subPoints;
	for (int i = 0; i < 4; i++) {
		sumPoints.push_back(points[i].x + points[i].y);
		subPoints.push_back(points[i].x - points[i].y);
	}
	newPoints.push_back(points[min_element/* find smallest element*/(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]);
	newPoints.push_back(points[max_element/* find largest element*/(subPoints.begin(), subPoints.end()) - subPoints.begin()]);
	newPoints.push_back(points[min_element/* find smallest element*/(subPoints.begin(), subPoints.end()) - subPoints.begin()]);
	newPoints.push_back(points[max_element/* find largest element*/(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]);
	return newPoints;
}

Mat getWarp(Mat img, vector<Point> points, float w, float h) {
	Mat imgWarp;
	Point2f src[4] = { points[0],points[1],points[2],points[3] };//Point2f��ʾ������
	Point2f dst[4] = { {0.0f,0.0f},{w,0.0f},{0.0f,h},{w,h} };//Point2f��ʾ������
	Mat matrix = getPerspectiveTransform(src, dst);
	warpPerspective(img, imgWarp, matrix, Point(w, h));
	return imgWarp;
}

float getDistance(Point pointO, Point pointA)
{
	float distance;
	distance = powf((pointO.x - pointA.x), 2) + powf((pointO.y - pointA.y), 2);
	distance = sqrtf(distance);
	return distance;
}

void Draw(Mat img, vector<Point> points)
{
	vector<Point> docpoints;
	docpoints.push_back(points[0]);
	docpoints.push_back(points[1]);
	docpoints.push_back(points[3]);
	docpoints.push_back(points[2]);
	polylines(img, docpoints, true, Scalar(0, 255, 0), 3);
}

void AutomaticDeviationCorrection() {
	g_VideoCapture.open(1, cv::CAP_DSHOW);
	Mat imgOriginal;
	while(g_VideoCapture.isOpened())
	{
		g_VideoCapture.read(imgOriginal);
		Mat imgThre = preProcessing(imgOriginal);//Ԥ����		
		initialPoints = getContours(imgThre);//�������--���������
		if (initialPoints.size() != 0)
		{
			docPoints = reorder(initialPoints);
			drawPoints(imgOriginal,docPoints, Scalar(0, 255, 0));
			int width = getDistance(docPoints[0], docPoints[1]);
			int height = getDistance(docPoints[0], docPoints[2]);
			Mat imgWarp = getWarp(imgOriginal, docPoints, width, height);
			Draw(imgOriginal, docPoints);
			imshow("Image", imgOriginal);
			imshow("Image Dilation", imgThre);
			imshow("Image Warp", imgWarp);
			waitKey(1);
		}
	}
}