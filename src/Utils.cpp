#include "Utils.hpp"

std::mutex Utils::mutex;

bool Utils::pairCompare(const pair<int,int> &a, const pair<int,int> &b) {
	return (a.first < b.first || (a.first == b.first && a.second < b.second));
}

void Utils::normalizePairs(vector< pair<int,int> > &pairs, int val) {
	for(int i = 0; i < pairs.size(); i++) {
		pairs[i].first = pairs[i].first + val;
		pairs[i].second = pairs[i].second + val;
	}
}

bool Utils::isKeyFramesValid(vector< pair<int,int> > kf) {
	int shots = kf.back().first;
	bool temp;
	
	for(int i = 0; i < shots; i++) {
		bool temp = false;
		for(pair<int, int> p : kf) {
			if(p.first == i) {
				temp = true;
				break;
			}
		}
		if(!temp) {
			return false;
		}
	}
	return true;
}

void Utils::extractHistogram(Mat frame, int num, vector<pair<int, Mat>> &hsvHistograms) {
	float Hsize[] = {0,180};
	float Ssize[] = {0,256};
	float Vsize[] = {0,256};
	const float* space[] = {Hsize, Ssize, Vsize};
	int channels[] = {0,1,2};
	int Hbins = 8;
	int Sbins = 4;
	int Vbins = 4;
	int histogramBins[] = {Hbins, Sbins, Vbins};
	Mat histogram;


	calcHist(&frame,1,channels,Mat(),histogram,3,histogramBins,space);
	frame.release();
	
	histogram = histogram / cv::sum(histogram)[0];
	
	Utils::mutex.lock();
	hsvHistograms.push_back(make_pair(num,histogram));
	Utils::mutex.unlock();
}

vector<Point2f> Utils::extractFeaturesToTrack(Mat frame) {
	TermCriteria termcrit(TermCriteria::COUNT|TermCriteria::EPS, 20, 0.03);	
	vector<Point2f> points;
		
	goodFeaturesToTrack(frame, points, Parameters::MaxOpticalFlowFeatures, 0.05, 5, Mat(), 3, false, 0.04);
	if(points.size() > 0) {
		cornerSubPix(frame, points, Size(10,10), Size(-1,-1), termcrit);
	}
		
	return points;
}

pair<int,double> Utils::extractOpticalFlow(Mat prevFrame, vector<Point2f> prevFramePoints, Mat actualFrame, vector<Point2f> actualFramePoints) {
	if(prevFramePoints.size() == 0 || actualFramePoints.size() == 0) {
		return make_pair(0,0.0);
	}
	
	Size winSize(Parameters::OpticalFlowWindowSize, Parameters::OpticalFlowWindowSize);
	TermCriteria termcrit(TermCriteria::COUNT|TermCriteria::EPS, 20, 0.03);	

	vector<uchar> status;
    vector<float> err;
	calcOpticalFlowPyrLK(prevFrame, actualFrame, prevFramePoints, actualFramePoints, status, err, winSize, 3, termcrit, 0, 0.01);
	
	int quant = 0;
	double totaldist = 0.0;
	
	double maxError = (double) (Parameters::MaxOpticalFlowFeatures / 2.0);
	for(int i = 0; i < status.size(); i++) {
		if(status[i] == false || err[i] >= maxError) {
			continue;
		}
		double dist = sqrt(pow(prevFramePoints[i].x - actualFramePoints[i].x,2) + pow(prevFramePoints[i].y - actualFramePoints[i].y, 2));
		if(dist < 1.0) {
			continue;
		}
		quant++;
		totaldist = totaldist + dist;
	}	

	return make_pair(quant, totaldist);
}

double Utils::histogramIntersection(Mat hist1, Mat hist2) {
	return compareHist(hist1, hist2, HISTCMP_INTERSECT);
}

vector<pair<int,int>> Utils::convertFramesToShots(vector<pair<int,int>> frames, vector<pair<int,int>> shots) {
	vector<pair<int,int>> ret;
	
	for(auto &f : frames) {
		int min = 0;
		int max = 0;
		
		for(int i = 0; i < shots.size(); i++) {
			if(shots[i].first == f.first) {
				min = i;
			}
			if(shots[i].second == f.second) {
				max = i;
			}
		}
		ret.push_back(make_pair(min,max));
	}
	if(ret[0].first == 0) {
		Utils::normalizePairs(ret,+1);
	}
	return ret;
}

double Utils::calculateMotion(vector<Shot> scene) {
	int num = 0;
	double val = 0.0;
	
	for(auto &s : scene) {
		num = num + s.getOpticalFlow().first;
		val = val + s.getOpticalFlow().second;
	}
	return (double) ((double)val / (double)num);
}