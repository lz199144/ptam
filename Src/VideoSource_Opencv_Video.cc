/*
* Autor : Arnaud GROSJEAN (VIDE SARL)
* This implementation of VideoSource allows to use OpenCV as a source for the video input
* I did so because libCVD failed getting my V4L2 device
*
* INSTALLATION :
* - Copy the VideoSource_Linux_OpenCV.cc file in your PTAM directory
* - In the Makefile:
*	- set the linkflags to
	LINKFLAGS = -L MY_CUSTOM_LINK_PATH -lblas -llapack -lGVars3 -lcvd -lcv -lcxcore -lhighgui
*	- set the videosource to 
	VIDEOSOURCE = VideoSource_Linux_OpenCV.o
* - Compile the project
* - Enjoy !
* 
* Notice this code define two constants for the image width and height (OPENCV_VIDEO_W and OPENCV_VIDEO_H)
*/

#include "VideoSource.h"
#include <cvd/Linux/v4lbuffer.h>
#include <cvd/colourspace_convert.h>
#include <cvd/colourspaces.h>
#include <gvars3/instances.h>
#include <opencv2/opencv.hpp>

#include <iostream>
#include <sstream>

using namespace CVD;
using namespace std;
using namespace GVars3;
using namespace cv;
using namespace std;

VideoSource::VideoSource()
{
    cout << "  VideoSource_Linux_OpenCV: Opening video source..." << endl;

    mptr = new VideoCapture(0); // first try the camera

    string PathCam="/home/lz/Document/video/WJL.mp4";//= "/home/bingxiong/Desktop/in vivo videos/04022013_075544/ch1_video_01.mpg"; //for calibration, PathCam can't be read in, manually put the video here if needed.

    if(!((VideoCapture*) mptr)->isOpened())
    { 
        PathCam = GV3::get<string>("PathCam");//put the video file directory in camera.cfg, which will be loaded by settings.cfg
        mptr = new VideoCapture(PathCam); // by url/video-file  then try the video file
    }

    VideoCapture* cap = (VideoCapture*)mptr;
    if(!cap->isOpened() ){
      cerr << "Unable to get video source"<< endl;
      exit(-1);
    }

    cout << "  ... got video sources." << endl;
    int img_width = 0, img_height = 0;
    img_width = cap->get(CV_CAP_PROP_FRAME_WIDTH);
    img_height = cap->get(CV_CAP_PROP_FRAME_HEIGHT);
    if(img_width != 0 && img_height != 0)
    {
      cout << "width: "<<img_width<<", height: "<< img_height <<endl;
      mirSize = ImageRef(img_width, img_height);
    }
    else
    {
      cout<<"error in the video resolution, please check"<<endl;
      mirSize = ImageRef(640, 480);//in case there is an error
    }
};

ImageRef VideoSource::Size()
{ 
    return mirSize;
};

void conversionNB(Mat frame, Image<byte> &imBW)
{
  	Mat clone = frame.clone();
  	Mat_<Vec3b>& frame_p = (Mat_<Vec3b>&)clone;
    int img_width = frame.size().width;
    int img_height = frame.size().height;
  	for (int i = 0; i < img_height; i++){
  		  for (int j = 0; j < img_width; j++){	
  		  imBW[i][j] = (frame_p(i,j)[0] + frame_p(i,j)[1] + frame_p(i,j)[2]) / 3;
  		  }
  	}

}
void conversionGray2NB(Mat frame, Image<byte> &imBW){
  	Mat clone = frame.clone();
  	Mat_<char>& frame_p = (Mat_<char>&)clone;
    int img_width = frame.size().width;
    int img_height = frame.size().height;
  	for (int i = 0; i < img_height; i++){
  	    for (int j = 0; j < img_width; j++){	
  		  imBW[i][j] = frame_p(i,j);
  		  }
  	}
}

void conversionRGB(Mat frame, Image<Rgb<byte> > &imRGB){
  	Mat clone = frame.clone();
  	Mat_<Vec3b>& frame_p = (Mat_<Vec3b>&)clone;
    int img_width = frame.size().width;
    int img_height = frame.size().height;
  	for (int i = 0; i < img_height; i++){
  	   	for (int j = 0; j < img_width; j++){	
  		  imRGB[i][j].red = frame_p(i,j)[2];
  		  imRGB[i][j].green = frame_p(i,j)[1];
  		  imRGB[i][j].blue = frame_p(i,j)[0];
  		  }
  	}
}

void VideoSource::GetAndFillFrameBWandRGB(Image<byte> &imBW, Image<Rgb<byte> > &imRGB)
{
    Mat frame;// frame_gray, frame_canny;
    VideoCapture* cap = (VideoCapture*)mptr;
    *cap >> frame;

    //cvtColor(frame, frame_gray, CV_BGR2GRAY);
    //Canny(frame_gray, frame_canny, 60, 40, 3);

    conversionNB(frame, imBW);
    conversionRGB(frame, imRGB);
}