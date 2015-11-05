#include "gaussian.h"
myupdate_mhi::myupdate_mhi()//构造函数
{
	MHI_DURATION = 0.5;
	MAX_TIME_DELTA = 0.5;
	MIN_TIME_DELTA = 0.05;
	N = 3;

	CONTOUR_MAX_AERA = 10000;
	// ring image buffer
	buf = 0;
	last = 0;
	// temporary images
	mhi = 0;
	//MHI: motion history image
	filter = CV_GAUSSIAN_5x5;

}

void myupdate_mhi::init(IplImage* im,IplImage* ds,int di)//构造函数
{
	img=im;
	dst=ds;
	diff_threshold=di;


};

int myupdate_mhi:: update_mhi(Rect &object)
{
	double timestamp = clock()/100.;
	// get current time in seconds
	CvSize size = cvSize(img->width,img->height);
	// get current frame size
	int i, j, idx1, idx2;

	int myarea=0;//用来找最大面积
	CvPoint points1=cvPoint(0,0),points2=cvPoint(0,0);//用来存储最大矩形的端点

	IplImage* silh;
	uchar val;
	float temp;
	IplImage* pyr = cvCreateImage( cvSize((size.width & -2)/2, (size.height & -2)/2), 8, 1 );
	CvMemStorage *stor;
	CvSeq *cont, *result, *squares;
	CvSeqReader reader;
	if( !mhi || mhi->width != size.width || mhi->height != size.height )
	{
		if( buf == 0 )
		{
			buf = (IplImage**)malloc(N*sizeof(buf[0]));
			memset( buf, 0, N*sizeof(buf[0]));
		}
		for( i = 0; i < N; i++ )
		{
			cvReleaseImage( &buf[i] );
			buf[i] = cvCreateImage( size, IPL_DEPTH_8U, 1 );
			cvZero( buf[i] );
		}
		cvReleaseImage( &mhi );
		mhi = cvCreateImage( size, IPL_DEPTH_32F, 1 );
		cvZero( mhi );
		// clear MHI at the beginning
	}
	// end of if(mhi)
	cvCvtColor( img, buf[last], CV_BGR2GRAY );
	// convert frame to grayscale
	idx1 = last;
	idx2 = (last + 1) % N;
	// index of (last - (N-1))th frame
	last = idx2;
	// 做帧差
	silh = buf[idx2];
	cvAbsDiff( buf[idx1], buf[idx2], silh );
	// get difference between frames
	// 对差图像做二值化
	cvThreshold( silh, silh, 30, 255, CV_THRESH_BINARY );//cvShowImage("Bin",silh);
	// and threshold it
	cvUpdateMotionHistory( silh, mhi, timestamp, MHI_DURATION );
	// update MHI
	cvCvtScale( mhi, dst, 255./MHI_DURATION,
		(MHI_DURATION - timestamp)*255./MHI_DURATION );
	cvCvtScale( mhi, dst, 255./MHI_DURATION, 0 );//cvShowImage("MHI",dst);
	// 中值滤波，消除小的噪声
	cvSmooth( dst, dst, CV_MEDIAN, 3, 0, 0, 0 ); //cvShowImage("Bin",dst);
	// 向下采样，去掉噪声
	cvPyrDown( dst, pyr, 7 );//函数cvPyrDown使用Gaussian金字塔分解对输入图像向下采样。对图像进行缩放处理

	Mat mtpyr = pyr;//把IplImage转化为mat
	dilate(mtpyr,mtpyr,Mat(10,10,CV_8U),Point(-1,-1),1);


	//cvDilate( pyr, pyr, 0, 1 );// 做膨胀操作，消除目标的不连续空洞
	cvPyrUp( pyr, dst, 7 ); //opencv库中函数用于图像的上采样 输出图像, 宽度和高度应是输入图像的2倍
	//
	// 下面的程序段用来找到轮廓
	//
	// Create dynamic structure and sequence.
	stor = cvCreateMemStorage(0);
	cont = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint) , stor);


	// 找到所有轮廓
	cvFindContours( dst, stor, &cont, sizeof(CvContour),CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));// 如果轮廓不为NULL,则用多边形来拟合找到的轮廓,以减少轮郭的"点数".
	// 直接使用CONTOUR中的矩形来画轮廓

	//CvPoint* points1;
	bool output=false;
	for(;cont;cont = cont->h_next)
	{
		CvRect r = ((CvContour*)cont)->rect;
		if(r.height * r.width > CONTOUR_MAX_AERA) // 面积小的方形抛弃掉
		{
			if((myarea<(r.height*r.width))&&(r.x>100)&&((r.x+r.width)<500))
			{
				points1=cvPoint(r.x,r.y);
				points2=cvPoint(r.x + r.width, r.y + r.height);
				myarea=r.height*r.width;
				//cout<<"points1="<<r.x<<","<<r.y<<" and "<<"points2="<<(r.x + r.width)<<","<<(r.y + r.height)<<endl;
				output=true;

			}



		}
	}      // free memory

	//cvRectangle( img, points1,points2,CV_RGB(255,0,0), 1, CV_AA,0);
	if(output==true)
	{
		Point center((points2.x+points1.x)/2,(points2.y+points1.y)/2);
		object.width=(points2.x-points1.x)/2;
		object.height=(points2.y-points1.y)/2;
		object.x=center.x-object.width/2;
		object.y=center.y-object.height/2;
		//object.x=points1.x;
		//object.y=points1.y;

		//object.width=points2.x-points1.x;
		//object.height=points2.y-points1.y;

	}

	cvReleaseMemStorage(&stor);
	cvReleaseImage( &pyr );
	return output==true?0:-1;
}//成员函数

