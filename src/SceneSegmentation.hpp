#ifndef SCENESEGMENTATION_H
#define SCENESEGMENTATION_H

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
#include <limits>
#include <algorithm>

#include "Shot.hpp"
#include "Utils.hpp"
#include "Parameters.hpp"

using namespace std;
using namespace cv;

class SceneSegmentation {
	private:
		vector<Shot> shots;
		vector<double> calculateBSCWMT();
		double maxIntersect(vector<Mat> s1, vector<Mat> s2);
		double avgIntersect(vector<Mat> s1, vector<Mat> s2);
		double minIntersect(vector<Mat> s1, vector<Mat> s2);
		vector<vector<Shot>> smallScenesHeuristic(vector<vector<Shot>> scenes);
		vector<vector<Shot>> similarAdjacentScenesHeuristic(vector<vector<Shot>> scenes);
		vector<vector<Shot>> similarWindowedAdjacentScenesHeuristic(vector<vector<Shot>> scenes);
		vector<vector<Shot>> opticalFlowHeuristic(vector<vector<Shot>> scenes);
		/* 0 = MAX, 1 = MIN, 2 = AVG */ 
		double compareIntersecScenes(vector<Shot> sce1, vector<Shot> sce2, int op);
		void calculateBSCW(int index, vector<Shot> &shotTemp, double &result);
		
	public:
		SceneSegmentation(vector<Shot> shots);
		vector<pair<int,int>> getScenes(bool enableOpticalFlow);
};

#endif