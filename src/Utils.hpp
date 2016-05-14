#ifndef UTILS_H
#define UTILS_H

#include "opencv2/video/tracking.hpp"
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

#include "Shot.hpp"
#include "Parameters.hpp"

using namespace std;
using namespace cv;

class Utils {
	private:
		static std::mutex mutex;
		
	public:
		/* Pair utils */
		static void normalizePairs(vector<pair<int,int>> &pairs, int val);
		static bool pairCompare(const pair<int,int> &a, const pair<int,int> &b);
		static bool isKeyFramesValid(vector<pair<int,int>> kf);
		static void extractHistogram(Mat frame, int num, vector<pair<int,Mat>> &hsvHistograms);
		static pair<int,double> extractOpticalFlow(Mat prevFrame, vector<Point2f> prevFramePoints, Mat actualFrame, vector<Point2f> actualFramePoints);
		static vector<Point2f> extractFeaturesToTrack(Mat frame);
		static double histogramIntersection(Mat hist1, Mat hist2);
		static vector<pair<int,int>> convertFramesToShots(vector<pair<int,int>> frames, vector<pair<int,int>> shots);
		static double calculateMotion(vector<Shot> scene);
};

#endif