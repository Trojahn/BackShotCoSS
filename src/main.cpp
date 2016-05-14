#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <utility>
#include <algorithm>

#include "Utils.hpp"
#include "InputOutput.hpp"
#include "FeatureExtraction.hpp"
#include "SceneSegmentation.hpp"

using namespace std;
using namespace cv;

int main(int argc, char* argv[]) {
	
	if(argc < 5) {
		cout << "Incorrect parameter count." << endl;
		cout << "./BackShotCoSS <videoFile> <keyframesCSV> <shotSegmentationCSV> <outputCSV> [<disableOpticalFlow>]" << endl;
		cout << "Example: " << endl;
		cout << "./BackShotCoSS video.avi keyframes.csv videoShotSegmentation.csv scenesOutput.csv yes" << endl;
		return 1;
	}
	
	string vPath = string(argv[1]);
	if(!InputOutput::checkFile(vPath)) {
		cout << "The videoFilePath seems to be invalid or cannot be read" << endl;
		return 1;
	}
	
	string kfPath = string(argv[2]);
	if(!InputOutput::checkFile(kfPath)) {
		cout << "The keyframesFilePath seems to be invalid or cannot be read" << endl;
		return 1;
	}
	
	string shotsPath = string(argv[3]);
	if(!InputOutput::checkFile(shotsPath)) {
		cout << "The shotSegmentationFilePath seems to be invalid or cannot be read" << endl;
		return 1;
	}
	
	string output = string(argv[4]);
	if(InputOutput::checkFile(output)) {		
		while(true) {
			string in;
			cout << "File '" << output << "' already exists. Overwrite ? [y/N]" << endl;
			getline(std::cin,in);
			if(in == "Y" || in == "y") {
				break;
			}
			if(in == "" || in == "N" || in == "n") {
				return 1;
			}
		}
	}
	bool enableOpticalFlow = true;
	if(argc > 5) {
		string temp = string(argv[5]);
		if(temp == "YES" || temp == "yes" || temp == "Yes") {
			cout << "Optical flow heuristic DISABLED!" << endl;			
			enableOpticalFlow = false;
		}
	}
		
	vector< pair<int,int> > keyframes = InputOutput::extractKeyframes(kfPath);
	if(!Utils::isKeyFramesValid(keyframes)) {
		cout << "There seems to be some shots without a selected keyframe!" << endl;
		return 1;
	}

	vector<pair<int,int>> shots = InputOutput::extractShots(shotsPath);
		
	FeatureExtraction featureExtract(keyframes, shots, vPath);
	vector<Shot> shotFeatures = featureExtract.extractFeatures(enableOpticalFlow);

	SceneSegmentation sceSeg(shotFeatures);
	vector<pair<int,int>> scenes = sceSeg.getScenes(enableOpticalFlow);

	scenes = Utils::convertFramesToShots(scenes, shots);

	InputOutput::writeCSV(output, scenes);
	return 0;
}