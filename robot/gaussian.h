#ifndef MYHEAD_H 
#define MYHEAD_H

#include "cv.h"
#include <cxcore.h>
#include "highgui.h"
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

using namespace cv;
using namespace std;
// various tracking parameters (in seconds)

// 参数：
// img – 输入视频帧
// dst – 检测结果


class myupdate_mhi//用于检测运动画出矩形
{
private:
	IplImage* img;
	IplImage* dst;
	int diff_threshold;
	
	double MHI_DURATION;//= 0.5;
	double MAX_TIME_DELTA;// = 0.5;
	double MIN_TIME_DELTA;// = 0.05;
	int N;// = 3;
	//
	int CONTOUR_MAX_AERA;// = 20000;
	// ring image buffer
	IplImage **buf;// = 0;
	int last;// = 0;
	// temporary images
	IplImage *mhi;// = 0;
	// MHI: motion history image
	int filter;// = CV_GAUSSIAN_5x5;
	CvConnectedComp *cur_comp, min_comp;
	CvConnectedComp comp;
	CvMemStorage *storage; 
	CvPoint pt[4];

public:
	void init(IplImage* im,IplImage* ds,int di);//构造函数
	int update_mhi(Rect &result); //成员函数
	myupdate_mhi();
};

#endif

