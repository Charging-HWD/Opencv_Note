#include<opencv2/opencv.hpp>
#include<iostream>

using namespace std;
using namespace cv;

VideoCapture g_VideoCapture;
vector<Point> initialPoints, docPoints;

//图像预处理：转换为灰度、添加模糊、使用Canny边缘检测器找到边缘（知道纸张在哪里）、基于纸张的坐标提取四个角得到顶视图
Mat preProcessing(Mat img) {

	/*灰度*/
	Mat imgGray;
	cvtColor(img, imgGray, COLOR_BGR2GRAY);

	/*高斯模糊*/
	Mat imgBlur;
	GaussianBlur(imgGray, imgBlur, Size(7, 7), 5, 0);

	/*二值化*/
	Mat imgThreshold;
	threshold(imgBlur, imgThreshold, 200, 255, THRESH_BINARY | THRESH_OTSU);

	/*边缘*/
	Mat imgCanny;
	Canny(imgThreshold, imgCanny, 25, 75);

	/*膨胀*/
	Mat imgDil;
	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));//创建一个核，增加Size（只能是奇数）会扩张/侵蚀更多
	dilate(imgCanny, imgDil, kernel);//扩张边缘（增加边缘厚度）
	return imgDil;
}
 
//返回纸张的4个角点
vector<Point> getContours(Mat imgDil) {//imgDil是传入的扩张边缘的图像用来查找轮廓，img是要在其上绘制轮廓的图像
	vector<vector<Point>> contours;//轮廓检测到的轮廓。每个轮廓线存储为一个点的向量
	vector<Vec4i> hierarchy;//包含关于映像拓扑的信息  typedef Vec<int, 4> Vec4i;具有4个整数值
	findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	//drawContours(img, contours, -1, Scalar(255, 0, 255), 2);//img：要绘制轮廓在什么图片上，contours：要绘制的轮廓，-1定义要绘制的轮廓号（-1表示所有轮廓），Saclar表示轮廓颜色，2表示厚度
	vector<vector<Point>> conPoly(contours.size());//conploy的数量应小于contours
	vector<Rect> boundRect(contours.size());
	vector<Point> biggest;
	int maxArea = 0;

	//过滤器：通过轮廓面积来过滤噪声
	for (int i = 0; i < contours.size(); i++) {//遍历检测到的轮廓
		int area = contourArea(contours[i]);
		string objectType;
		if (area > 1000) {//轮廓面积＞1000才绘制
			//计算轮廓周长或凹坑长度。该函数计算了曲线长度和封闭的周长。
			float peri = arcLength(contours[i], true);//计算封闭轮廓周长
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);//以指定的精度近似多边形曲线。第二个参数conPloy[i]存储近似的结果，是输出。

			if (area > maxArea && conPoly[i].size() == 4) {
				biggest = { conPoly[i][0],conPoly[i][1],conPoly[i][2],conPoly[i][3] };
				maxArea = area;
			}
			//rectangle/*绘制边界矩形*/(imgOriginal, boundRect[i].tl()/*tl()：topleft矩形左上角坐标*/, boundRect[i].br()/*br()：bottom right矩形右下角坐标*/, Scalar(0, 255, 0), 5);
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

vector<Point> reorder(vector<Point> points) {//标记点的顺序会变，要确定一个顺序 0 1 2 3							 
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
	Point2f src[4] = { points[0],points[1],points[2],points[3] };//Point2f表示浮点数
	Point2f dst[4] = { {0.0f,0.0f},{w,0.0f},{0.0f,h},{w,h} };//Point2f表示浮点数
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
		Mat imgThre = preProcessing(imgOriginal);//预处理		
		initialPoints = getContours(imgThre);//获得轮廓--获得最大矩形
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