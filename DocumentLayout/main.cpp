#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <Windows.h>
#include "HeuristicLayout.h"

using namespace cv;
using namespace std;

void DetectLayout01(Mat src);

int main()
{
	Mat src = imread("pic.png");
	
	if (src.empty())
	{
		cout << "Can't find image";
		return -1;
	}

	imshow("Original", src);
	//waitKey(0);

	//HeuristicLayout::DetectLayout(src);
	//waitKey(0);

	DetectLayout01(src);

	destroyAllWindows();
	return 0;
}

int Distance(int x1, int y1, int x2, int y2)
{
	return abs(x1 - x2) + abs(y1 - y2);
}

int RectDistance2(Rect a, Rect b);

int RectDistance(Rect a, Rect b)
{
	int x1 = a.x;
	int x1b = a.x + a.width;
	int y1 = a.y;
	int y1b = a.y + a.height;

	int x2 = b.x;
	int x2b = b.x + b.width;
	int y2 = b.y;
	int y2b = b.y + b.height;

	bool left = x2b < x1;
	bool right = x1b < x2;
	bool bottom = y2b < y1;
	bool top = y1b < y2;

	if (top && left)
		return Distance(x1, y1b, x2b, y2);
	else if (left && bottom)
		return Distance(x1, y1, x2b, y2b);
	else if (bottom && right)
		return Distance(x1b, y1, x2, y2b);
	else if (right && top)
		return Distance(x1b, y1b, x2, y2);
	else if (left)
		return abs(x1 - x2b);
	else if (right)
		return abs(x2 - x1b);
	else if (bottom)
		return abs(y1 - y2b);
	else if (top)
		return abs(y2 - y1b);
	else
		return 0;
		//return RectDistance2(a,b);
}

int RectDistance2(Rect a, Rect b)
{
	int x1 = a.x;
	int x1b = a.x + a.width;
	int y1 = a.y;
	int y1b = a.y + a.height;

	int x2 = b.x;
	int x2b = b.x + b.width;
	int y2 = b.y;
	int y2b = b.y + b.height;

	bool left = x2b < x1;
	bool right = x1b < x2;
	bool bottom = y2b < y1;
	bool top = y1b < y2;

	int min_distance = 99999999999;

	int dist = Distance(x1, y1, x2, y2);
	if (dist < min_distance)
		min_distance = dist;

	dist = Distance(x1, y1b, x2, y2);
	if (dist < min_distance)
		min_distance = dist;

	dist = Distance(x1b, y1, x2, y2);
	if (dist < min_distance)
		min_distance = dist;

	dist = Distance(x1b, y1b, x2, y2);
	if (dist < min_distance)
		min_distance = dist;
	////
	dist = Distance(x1, y1b, x2b, y2b);
	if (dist < min_distance)
		min_distance = dist;

	dist = Distance(x1, y1b, x2b, y2b);
	if (dist < min_distance)
		min_distance = dist;

	dist = Distance(x1b, y1, x2b, y2b);
	if (dist < min_distance)
		min_distance = dist;

	dist = Distance(x1b, y1b, x2b, y2b);
	if (dist < min_distance)
		min_distance = dist;
	////
	dist = Distance(x1, y1b, x2, y2b);
	if (dist < min_distance)
		min_distance = dist;

	dist = Distance(x1, y1b, x2, y2b);
	if (dist < min_distance)
		min_distance = dist;

	dist = Distance(x1b, y1, x2, y2b);
	if (dist < min_distance)
		min_distance = dist;

	dist = Distance(x1b, y1b, x2, y2b);
	if (dist < min_distance)
		min_distance = dist;
	////
	dist = Distance(x1, y1b, x2b, y2);
	if (dist < min_distance)
		min_distance = dist;

	dist = Distance(x1, y1b, x2b, y2);
	if (dist < min_distance)
		min_distance = dist;

	dist = Distance(x1b, y1, x2b, y2);
	if (dist < min_distance)
		min_distance = dist;

	dist = Distance(x1b, y1b, x2b, y2);
	if (dist < min_distance)
		min_distance = dist;

	return min_distance;
}

bool UpdateBoundingRects(vector<Rect>& rects, vector<Rect>& finalRects, int D)
{
	bool done = true;
	
	vector<Rect> newRects;

	Rect newRect;

	vector<Rect> nextSearchRect;
	vector<Rect> searchRect = rects;
	bool findMore = false;
	
	newRect = searchRect[0];

	do
	{
		findMore = false;
		nextSearchRect.clear();
		for (int i = 1; i < searchRect.size(); i++)
		{
			int dist = RectDistance(newRect, searchRect[i]);
			if (dist <= D)
			{
				done = false;
				newRect |= searchRect[i];
				findMore = true;
			}
			else
			{
				nextSearchRect.push_back(searchRect[i]);
			}
		}
		searchRect = nextSearchRect;
	} while (findMore);

	finalRects.push_back(newRect);

	rects = searchRect;
	
	return rects.size() == 0;
}

void DrawBoundingRects(Mat src, vector<Rect> rects, Scalar color)
{
	for (int i = 0; i < rects.size(); i++)
	{
		Rect brect = rects[i];
		rectangle(src, brect, color, 1);
	}
}

void DetectLayout01(Mat src)
{
	Mat cannyOutput;
	Canny(src, cannyOutput, 100, 200);

	imshow("Canny", cannyOutput);
	//waitKey(0);

	/*Mat element = getStructuringElement(MORPH_RECT, Size(3, 3), Point(1, 1));
	dilate(cannyOutput, cannyOutput, element);*/


	Mat blurOutput;
	blur(cannyOutput, blurOutput, Size(5, 5));

	/*imshow("Blur", blurOutput);
	waitKey(0);*/

	//Contours
	vector <vector<Point>> contours;
	vector<Vec4i> hierarchy;

	Mat contoursImg;
	src.copyTo(contoursImg);

	findContours(cannyOutput, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);


	Scalar incolor = Scalar(0, 0, 255);
	Scalar outcolor = Scalar(255, 0, 0);
	vector<Rect> boundingRects;
	for (int i = 0; i < contours.size(); i++)
	{
		Scalar color = outcolor;
		Rect brect = boundingRect(contours[i]);
		boundingRects.push_back(brect);
		rectangle(contoursImg, brect, color, 1);
	}

	imshow("Contours", contoursImg);
	Mat layoutImg;
	vector<Rect> newBoundingRects;
	while (!UpdateBoundingRects(boundingRects, newBoundingRects, 10))
	{
		waitKey(10);
		src.copyTo(layoutImg);
		DrawBoundingRects(layoutImg, newBoundingRects, incolor);
		imshow("Contours", layoutImg);
	}

	waitKey(500);
	src.copyTo(layoutImg);
	DrawBoundingRects(layoutImg, newBoundingRects, incolor);
	imshow("Contours", layoutImg);

	waitKey(0);
}