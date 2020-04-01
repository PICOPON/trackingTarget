#include<iostream>
#include<opencv2/opencv.hpp>
#include<opencv2/tracking/tracker.hpp>
#include<string>
#include<fstream>
#include<time.h>
#include "fdssttracker.hpp"

using namespace std;
using namespace cv;


Point previousPoint, currentPoint;
Rect2d bbox;
Point center;
//��̬����洢�����
vector<Point2d> points;

void draw_rectangle(int event, int x, int y, int flags, void*);
Mat tmp,frame,dst;


int main(int argc,char *argv[])
{
	VideoCapture cap;
	int count = 1;
	//������Ƶʵ��·������
	//����ͼƬ���У������޸����ļ���������ʽ
	//���磺ͼ��·��imgPath
	//char name[10];
	//sprintf_s(name, "%04d", count);
	//std::string imgPath = "F:\\Code\\trackingAlgorithm\\fDSST_cpp-master\\src\\Car1\\img\\";
	//std::string imgFinalPath = imgPath + std::string(name) + ".jpg";

	string filename = "F:\\Code\\trajectory_recognition\\VID1AN.mp4";	
	cap.open(filename);
	if (!cap.isOpened())
	{
		cout << "###############��Ƶ��ʧ��###############" << endl;
		return -1;
	}
	cap.read(frame);
	cvtColor(frame,dst, CV_BGR2GRAY);
	if (!frame.empty())
	{
		namedWindow("output", 0);
		imshow("output", dst);
		setMouseCallback("output", draw_rectangle, 0);
		waitKey();
	}

	/*********************OpencvĿ��׷���㷨ģ�庯��***************************/
	//Ptr<TrackerMIL> tracker=TrackerMIL::create();
	//Ptr<TrackerTLD> tracker=TrackerTLD::create();
	//Ptr<TrackerMedianFlow> tracker=TrackerMedianFlow::create();
	//Ptr<TrackerKCF> tracker=TrackerKCF::create();
	//Ptr<TrackerBoosting> tracker=TrackerBoosting::create();
	//Ptr<TrackerCSRT> tracker = TrackerCSRT::create();
	//Ptr<TrackerGOTURN> tracker = TrackerGOTURN::create();
	/***********************************************************************/

	/************************FDSSTĿ������㷨*********************************/
	/************************************************************************/

	bool HOG = true;
	bool FIXEDWINDOW = false;
	bool MULTISCALE = true;
	bool SILENT = true;
	bool LAB = false;
	// Create DSSTTracker tracker object
	FDSSTTracker tracker(HOG, FIXEDWINDOW, MULTISCALE, LAB);

	//ofstream infile;
	//infile.open("./coordxy.txt");
	double duration = 0;
	for(;;)
	{	
		cap.read(frame);
		if (!cap.read(frame))
		{
			break;
		}
		cvtColor(frame, dst, CV_BGR2GRAY);//fdsstԴ����fhogֻ�ܴ���Ҷ�ͼƬ����˶�ͼƬ��һ���Ҷ�ת��

		//��ʱ��㿪ʼ
		auto t_start = clock();
		//�ַ�����洢������������
		//char target[30];
		if (frame.empty())
		{
			break;
		}
		if (count==1)
		{
			//������֡����ѡ��ʼ���ٿ�bbox
		
			/*opencv�Դ��㷨*/
			//tracker->init(frame, bbox);
			/*�Ľ��㷨*/
			//FDSST
			tracker.init(bbox, dst);
		}
		else {
			/*opencv�Դ��㷨*/
			//tracker->update(frame, bbox);
			/*�Ľ��㷨*/
			//FDSST
			bbox = tracker.update(dst);
			
		}
		//��ʱ������ʱ������
		auto t_end = clock();
		duration += (double)(t_end - t_start) / CLOCKS_PER_SEC;
		//��FPS:
		cout << "FPS: " << count / duration << "\n";
		count++;
		rectangle(frame, bbox, Scalar(255, 255, 0), 2, 1);
		center.x = bbox.x + bbox.width / 2;
		center.y = bbox.y + bbox.height / 2;
		points.push_back(center);
		//���ƹ켣���ٿ����ĵ�ͼ��
		for (int j = 1; j < points.size(); j++)
		{
			Point pre, last;
			if (points.size() < 3)
			{
				pre = points[j];
				last = points[j];
			}
			else
			{
				pre = points[j - 1];
				last = points[j];
				line(frame, pre, last, Scalar(0, 255, 0), 1, 8);
			}
		}
		circle(frame, center, 4, Scalar(0, 255, 0), -1);
		/*��ʾ���������ĵ�λ������*/
		//sprintf(target, "��������λ��(%d,%d)", center.x, center.y);
		//cout << target << endl;
		/********************����������д��txt�ļ���*************************/
		/*
		infile << center.x << "\t" << center.y << endl;
		*/

		namedWindow("tracking", 0);
		imshow("tracking", frame);
		int delayms = 1;
		if (waitKey(delayms) == 27)
			break;
		

	}
	//infile.close();
	return 0;
}
void draw_rectangle(int event, int x, int y, int flags, void*)
{
	if (event == EVENT_LBUTTONDOWN)
	{
		previousPoint = Point(x, y);
		cout << "(" << previousPoint.x << "," << previousPoint.y << ")" << endl;
	}
	else if (event == EVENT_MOUSEMOVE && (flags&EVENT_FLAG_LBUTTON))
	{
		Mat tmp;
		char str[20];
		frame.copyTo(tmp);
		currentPoint = Point(x, y);
		sprintf(str, "(%d,%d)", currentPoint.x, currentPoint.y);
		putText(tmp, str, currentPoint, FONT_HERSHEY_PLAIN, 1, Scalar(255, 0, 255), 2, 8);
		rectangle(tmp, previousPoint, currentPoint, Scalar(0, 255, 0),2, 1, 0);
		imshow("output", tmp);
	}
	else if (event == EVENT_LBUTTONUP)
	{
		bbox.x = previousPoint.x;
		bbox.y = previousPoint.y;
		bbox.width = abs(previousPoint.x - currentPoint.x);
		bbox.height = abs(previousPoint.y - currentPoint.y);

	}
	else if (event == EVENT_RBUTTONUP)
		destroyWindow("output");
}
