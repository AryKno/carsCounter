//#pragma once
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/bgsegm.hpp>
//#include <opencv2/video/background_segm.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <tuple>

#define DIR "./images"
#define VIDEO "./images/output3-sample.mp4"
#define HEIGHT 720
#define WIDTH 1280

#define VHCL_MIN_WIDTH  35
#define VHCL_MIN_HEIGHT 35

#define VLIST_AREAX 75
#define VLIST_AREAY 75



using namespace cv;
using namespace std;

void writeText(Mat *, string , int x, int , Scalar , double );
int vehiclesAfterLimit(list<tuple<int,int,bool>> *vehiclesList);
void vehiclesListDump(list<tuple<int,int,bool>> *vehiclesList);
void trackCars(list<tuple<int,int,bool>> *vehiclesList, int x, int y, int Xwidth, int Ywidth, int yLimit, int *counter);
bool boundRectIsValid(Rect *rect,int minWidth,int minHeight);

