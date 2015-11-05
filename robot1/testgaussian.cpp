#include"myhead.h"
//#include<opencv2/opencv.hpp>

int main(int argc, char** argv)
{
	
	IplImage* motion = 0;
	CvCapture* capture = 0; //视频获取结构
	if( argc == 1 || (argc == 2 && strlen(argv[1]) == 1 && isdigit(argv[1][0])))
		//原型：extern int isdigit(char c);  //l用法：#include <ctype.h>   功能：判断字符c是否为数字    说明：当c为数字0-9时，返回非零值，否则返回零。
		  capture = cvCaptureFromCAM( argc == 2 ? argv[1][0] - '0' : 0 );
	else if( argc == 2 )
		capture = cvCaptureFromAVI( argv[1] );
	if( capture )
	{ 
		cvNamedWindow( "Motion", 1 );
		myupdate_mhi myupdate1;
		Rect result;
		IplImage* image; 
		for(;;)
		{
			
			if( !cvGrabFrame( capture )) //从摄像头或者视频文件中抓取帧
				break;
			image = cvRetrieveFrame( capture );
			//取回由函数cvGrabFrame抓取的图像,返回由函数cvGrabFrame 抓取的图像的指针
			if( image )
			{
				if( !motion )
				{
					 motion =  cvCreateImage( cvSize(image->width,image->height), 8, 1 );
					 cvZero( motion );
					 motion->origin = image->origin;
					 ///* 0 - 顶—左结构, 1 - 底—左结构 (Windows bitmaps 风格) */
				}
			}
			myupdate1.init( image, motion, 60 );//定义一个对象
            
			if(myupdate1.update_mhi(result)==0)
			{
				cout<<"x:"<<result.x+result.width/2<<"    "<<"y:"<<result.y+result.height/2<<endl;
				rectangle(Mat(image,0),result, Scalar(255,0,0));
			}

			
			cvShowImage( "Motion", image );

			

			if( cvWaitKey(10) >= 0 )
				break; 
		}
		rectangle(Mat(image,0),result, Scalar(255,0,0));
		cvShowImage( "Motion", image );
		waitKey(-1);
		cvReleaseCapture( &capture );
		cvDestroyWindow( "Motion" );
	}
	return 0;
}