#include "FeatureExtraction.hpp"


FeatureExtraction::FeatureExtraction(vector<pair<int,int>> keyframes, vector<pair<int,int>> shots, string videoPath) {
	this->keyframes = keyframes;
	this->shots = shots;
	this->videoPath = videoPath;
}

vector<Shot> FeatureExtraction::extractFeatures(bool enableOpticalFlow) {
	VideoCapture video(this->videoPath);
	if(!video.isOpened()) {
		cout << "SceneSegmentation: Error opening the video file" << endl;
		exit(1);
	}
	
	vector<pair<int,Mat>> tempKeyframes;
	vector<pair<int,double>> tempOpticalFlow(shots.size());
	
	int frameIndex = 0;
	int shotIndex = 0;
	int keyframeIndex = 0;
	
	vector<thread> poolKeyframes;
	vector<thread> poolOpticalFlow;
	
	unsigned numThreads = thread::hardware_concurrency() * 3;
	
	Mat frame;
	vector<Mat> tempFrames;
	
	while(true) {		
		bool status = video.read(frame);
		if(!status || frameIndex > shots.back().second) {
			break;
		}
		
		if(poolKeyframes.size() >= numThreads) {
			for(auto &t : poolKeyframes) {
				t.join();
			}
			poolKeyframes.clear();
		}		
		
		if(frameIndex > keyframes[keyframeIndex].second) {
			keyframeIndex++;
		}
		
		/* Check if its a keyframe */
		if(keyframeIndex < keyframes.size() && keyframes[keyframeIndex].second == frameIndex) {
			Mat hsv;
			cvtColor(frame,hsv,CV_BGR2HSV);
			poolKeyframes.push_back(thread(&Utils::extractHistogram, hsv, frameIndex, std::ref(tempKeyframes)));
		}
		
		if(enableOpticalFlow) {	
			if(Parameters::VideoResolution == -1) {
				Parameters::VideoResolution = frame.rows * frame.cols;
			}
			if(poolOpticalFlow.size() >= numThreads) {
				for(auto &t : poolOpticalFlow) {
					t.join();
				}
				poolOpticalFlow.clear();
			}
					
			if(frameIndex > shots[shotIndex].second) {
				if(tempFrames.size() > 0) {
					poolOpticalFlow.push_back(thread(&FeatureExtraction::processOpticalFlow, this, tempFrames, std::ref(tempOpticalFlow[shotIndex])));				
				}
				tempFrames.clear();
				shotIndex++;
			}		
			/* The frame does belong to a shot? */ 
			if(shotIndex < shots.size() && shots[shotIndex].first <= frameIndex && shots[shotIndex].second >= frameIndex) {
				Mat gray;
				cvtColor(frame,gray,CV_BGR2GRAY);
				tempFrames.push_back(gray);
			}
		}	

		frameIndex++;
	}
	
	if(enableOpticalFlow) {
		if(tempFrames.size() > 0) {
			poolOpticalFlow.push_back(thread(&FeatureExtraction::processOpticalFlow, this, tempFrames, std::ref(tempOpticalFlow[shotIndex])));				
		}
		for(auto &t : poolOpticalFlow) {
			t.join();
		}
	}	
	
	for(auto &t : poolKeyframes) {
		t.join();
	}
		
	poolOpticalFlow.clear();	
	poolKeyframes.clear();
	frame.release();
	video.release();
	
	vector<Shot> ret;
	
	for(int i = 0; i < shots.size(); i++) {
		vector<Mat> kfTemp;
		for(int j = 0; j < tempKeyframes.size(); j++) {
			if(tempKeyframes[j].first >= shots[i].first && tempKeyframes[j].first <= shots[i].second) {
				kfTemp.push_back(tempKeyframes[j].second);
			}
		}
		Shot shot(shots[i], kfTemp, tempOpticalFlow[i]);
		ret.push_back(shot);
	}
		
	return ret;	
}


void FeatureExtraction::processOpticalFlow(vector<Mat> frames, pair<int,double> &opticalFlow) {
	pair<int,double> result(0,0.0);
	vector<vector<Point2f>> points;
	
	for(int i = 0; i < frames.size(); i++) {
		points.push_back(Utils::extractFeaturesToTrack(frames[i]));
	}
	
	for(int i = 1; i < frames.size(); i++) {
		pair<int,double> ret = Utils::extractOpticalFlow(frames[i-1], points[i-1], frames[i], points[i]);
		result.first = result.first + ret.first;
		result.second = result.second + ret.second;
	}
	frames.clear();
	points.clear();
	opticalFlow = result;
}