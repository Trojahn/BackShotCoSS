#ifndef SHOT_H
#define SHOT_H

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <iomanip>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cmath>

using namespace std;
using namespace cv;

class Shot {
	private:
		pair<int,int> framesNumber;
		vector<Mat> keyframes;
		pair<int,double> opticalFlow;
		
		
	public:
		Shot(pair<int,int> framesNumber, vector<Mat> keyframes, pair<int,double> opticalFlow);
		vector<Mat> getKeyframes();		
		pair<int,double> getOpticalFlow();
		pair<int,int> getFrameNumber();
		
};

#endif