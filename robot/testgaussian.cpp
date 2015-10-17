#include"myhead.h"
//#include<opencv2/opencv.hpp>

int main(int argc, char** argv)
{
	
	IplImage* motion = 0;
	CvCapture* capture = 0; //��Ƶ��ȡ�ṹ
	if( argc == 1 || (argc == 2 && strlen(argv[1]) == 1 && isdigit(argv[1][0])))
		//ԭ�ͣ�extern int isdigit(char c);  //l�÷���#include <ctype.h>   ���ܣ��ж��ַ�c�Ƿ�Ϊ����    ˵������cΪ����0-9ʱ�����ط���ֵ�����򷵻��㡣
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
			
			if( !cvGrabFrame( capture )) //������ͷ������Ƶ�ļ���ץȡ֡
				break;
			image = cvRetrieveFrame( capture );
			//ȡ���ɺ���cvGrabFrameץȡ��ͼ��,�����ɺ���cvGrabFrame ץȡ��ͼ���ָ��
			if( image )
			{
				if( !motion )
				{
					 motion =  cvCreateImage( cvSize(image->width,image->height), 8, 1 );
					 cvZero( motion );
					 motion->origin = image->origin;
					 ///* 0 - ������ṹ, 1 - �ס���ṹ (Windows bitmaps ���) */
				}
			}
			myupdate1.init( image, motion, 60 );//����һ������
            
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