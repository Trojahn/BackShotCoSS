#ifndef FEATUREEXTRACTION_H
#define FEATUREEXTRACTION_H

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
#include <thread>
#include <mutex>

#include "Utils.hpp"
#include "Shot.hpp"
#include "Parameters.hpp"

using namespace std;

class FeatureExtraction {
	private:
		void processOpticalFlow(vector<Mat> frames, pair<int,double> &opticalFlow);
		vector<pair<int,int>> keyframes;
		vector<pair<int,int>> shots;
		string videoPath;
	
	public:	
		FeatureExtraction(vector< pair<int,int> > keyframes, vector<pair<int,int>> shots, string videoPath);
		vector<Shot> extractFeatures(bool enableOpticalFlow);
		
};

#endif