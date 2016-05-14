#include "Shot.hpp"

Shot::Shot(pair<int,int> framesNumber, vector<Mat> keyframes, pair<int,double> opticalFlow) {
	this->framesNumber = framesNumber;
	this->keyframes = keyframes;
	this->opticalFlow = opticalFlow;
}

vector<Mat> Shot::getKeyframes() {
	return this->keyframes;
}

pair<int,double> Shot::getOpticalFlow() {
	return this->opticalFlow;
}

pair<int,int> Shot::getFrameNumber() {
	return this->framesNumber;
}