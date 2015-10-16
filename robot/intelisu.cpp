#include <opencv2/opencv.hpp>
#include <tld_utils.h>
#include <iostream>
#include <sstream>
#include <TLD.h>
#include <stdio.h>
#include "serial.h"
using namespace cv;
using namespace std;

#define SERIAL_PORT

//Global variables
Rect box;
bool drawing_box = false;
bool gotBB = false;
bool tl = true;//track or not
bool rep = false;
bool fromfile=false;
string video;

//read the imformation of initial box
void readBB(char* file){
  ifstream bb_file (file);
  string line;
  getline(bb_file,line);
  istringstream linestream(line);
  string x1,y1,x2,y2;
  getline (linestream,x1, ',');
  getline (linestream,y1, ',');
  getline (linestream,x2, ',');
  getline (linestream,y2, ',');
  int x = atoi(x1.c_str());// = (int)file["bb_x"];
  int y = atoi(y1.c_str());// = (int)file["bb_y"];
  int w = atoi(x2.c_str())-x;// = (int)file["bb_w"];
  int h = atoi(y2.c_str())-y;// = (int)file["bb_h"];
  box = Rect(x,y,w,h);
}

//bounding box mouse callback
void mouseHandler(int event, int x, int y, int flags, void *param){
  switch( event ){
  case CV_EVENT_MOUSEMOVE:
    if (drawing_box){
        box.width = x-box.x;
        box.height = y-box.y;
    }
    break;
  case CV_EVENT_LBUTTONDOWN:
    drawing_box = true;
    box = Rect( x, y, 0, 0 );
    break;
  case CV_EVENT_LBUTTONUP:
    drawing_box = false;
    if( box.width < 0 ){
        box.x += box.width;
        box.width *= -1;
    }
    if( box.height < 0 ){
        box.y += box.height;
        box.height *= -1;
    }
    gotBB = true;
    break;
  }
}


int main(int argc, char * argv[])
{
    VideoCapture capture;
    if(argc>1)
    {
        capture.open(argv[1][0]-'0');
    }
    else
    {
        capture.open(0);
    }

    FileStorage fs;
    fs.open("parameters.yml", FileStorage::READ);

    if(!capture.isOpened())
    {
        cout<<"Cam Invalid"<<endl;
        return -1;
    }


    //serial initialize
    #ifdef SERIAL_PORT
    Serial serial;
    unsigned char sendBuff[10];
    if(serial.init()!=-1)
    {
        cout<<"serial init success"<<endl;
    }
    else
    {
        cout<<"serial init failed,please check the interface"<<endl;
        return -1;
    }
    #endif

    //set capture size
    capture.set(CV_CAP_PROP_FRAME_WIDTH,340);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT,240);

    namedWindow("Intelisu",CV_WINDOW_AUTOSIZE);
    setMouseCallback( "Intelisu", mouseHandler, NULL );

    TLD tld;
    tld.read(fs.getFirstTopLevelNode());
    Mat frame;
    Mat last_gray;
    Mat first;

    //aquire the object patch
    while(!gotBB)
    {
        if (!fromfile){
            capture >> frame;
        }
        else
            first.copyTo(frame);
        cvtColor(frame, last_gray, CV_RGB2GRAY);
        drawBox(frame,box);
        putText(frame,"Wating for the object...",Point(20,20),FONT_HERSHEY_SIMPLEX,0.6,10,2);
        //Mat scaleImage;
        //resize(frame,scaleImage,Size(2*round(frame.cols),2*round(frame.rows)));
        imshow("Intelisu", frame);
        if (cvWaitKey(33) == 'q')
            return 0;
    }


    //Remove callback
    cvSetMouseCallback( "Intelisu", NULL, NULL );

    //open the output file
    FILE  *bb_file = fopen("bounding_boxes.txt","w");


    //TLD initialization
    tld.init(last_gray,box,bb_file);

    //runtime variable
    Mat current_gray;
    BoundingBox pbox;
    vector<Point2f> pts1;
    vector<Point2f> pts2;
    bool status=true;
    int frames = 1;
    int detections = 1;

    while(capture.read(frame)){

        //get frame
        cvtColor(frame, current_gray, CV_RGB2GRAY);
        //Process Frame
        tld.processFrame(last_gray,current_gray,pts1,pts2,pbox,status,tl,bb_file);
        //Draw Points
        if (status){
          drawPoints(frame,pts1);
          drawPoints(frame,pts2,Scalar(0,255,0));
          drawBox(frame,pbox);
          detections++;
        }
        //Display
        ostringstream printText;
        printText<<"X:"<<pbox.x+pbox.width/2<<"   "<<"Y:"<<pbox.y+pbox.height/2;
        putText(frame,printText.str(),Point(40,40),FONT_HERSHEY_SIMPLEX,0.7,CV_RGB(100,255,0),2);
        imshow("Intelisu", frame);

        //Super Display
//        Mat scaleImage;
//        resize(frame,scaleImage,Size(2*round(frame.cols),2*round(frame.rows)));
//        ostringstream printText;
//        printText<<"X:"<<pbox.x+pbox.width/2<<"   "<<"Y:"<<pbox.y+pbox.height/2;
//        putText(scaleImage,printText.str(),Point(40,40),FONT_HERSHEY_SIMPLEX,1,CV_RGB(100,255,0),2);
//        imshow("Intelisu", scaleImage);

        //swap points and images
        swap(last_gray,current_gray);

        //clean the track
        pts1.clear();//clean the
        pts2.clear();
        frames++;
        //printf("Detection rate: %d/%d\n",detections,frames);
        if ((char)waitKey(1) == 'q')
          break;

        //send data through serial port
        #ifdef SERIAL_PORT
            sendBuff[0]=0xf2;
			sendBuff[1]=pbox.x+pbox.width/2;
			sendBuff[2]=pbox.y+pbox.height/2;
			serial.send_data_tty(sendBuff,3);
        #endif
  }
    fclose(bb_file);
    return 0;
}
