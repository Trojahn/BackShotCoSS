#include "SceneSegmentation.hpp"

SceneSegmentation::SceneSegmentation(vector<Shot> shots) {
	this->shots = shots;
}

double SceneSegmentation::maxIntersect(vector<Mat> s1, vector<Mat> s2) {
	double val = numeric_limits<double>::min();
	
	for(Mat hist1 : s1) {
		for(Mat hist2 : s2) {
			val = std::max(val, Utils::histogramIntersection(hist1,hist2));
		}
	}
	
	return val;
}

double SceneSegmentation::avgIntersect(vector<Mat> s1, vector<Mat> s2) {
	double val = 0.0;
	
	for(Mat hist1 : s1) {
		for(Mat hist2 : s2) {
			val = val + Utils::histogramIntersection(hist1,hist2);
		}
	}
		
	return val / (s1.size() + s2.size());
}

double SceneSegmentation::minIntersect(vector<Mat> s1, vector<Mat> s2) {
	double val = numeric_limits<double>::max();
	
	for(Mat hist1 : s1) {
		for(Mat hist2 : s2) {
			val = std::min(val, Utils::histogramIntersection(hist1,hist2));
		}
	}
	return val;
}
/* op: 0 = MAX, 1 = MIN, 2 = AVG */ 
double SceneSegmentation::compareIntersecScenes(vector<Shot> sce1, vector<Shot> sce2, int op) {
	double ret = 0.0;
	
	switch(op) {
		case 0:
			for(Shot s1 : sce1) {
				for(Shot s2 : sce2) {
					ret = std::max(ret, this->maxIntersect(s1.getKeyframes(), s2.getKeyframes()));
				}
			}
			break;
		case 1:
			ret = 1.0;
			for(Shot s1 : sce1) {
				for(Shot s2 : sce2) {
					ret = std::min(ret, this->maxIntersect(s1.getKeyframes(), s2.getKeyframes()));
				}
			}
			break;
		case 2:
		
			vector<Mat> keyframesAnt;
			vector<Mat> keyframesPos;
			
			for(Shot s : sce1) {
				for(Mat kf : s.getKeyframes()) {
					keyframesAnt.push_back(kf);
				}
			}			
			for(Shot s : sce2) {
				for(Mat kf : s.getKeyframes()) {
					keyframesPos.push_back(kf);
				}
			}
			ret = this->avgIntersect(keyframesAnt, keyframesPos);
			break;
	}
	return ret;
}

void SceneSegmentation::calculateBSCW(int index, vector<Shot> &shotTemp, double &result) {
	double val = numeric_limits<double>::min();
	double mult = 1.0;
	
	for(int i = index - Parameters::BSCWWindowsSize; i < index; i++) {
		if(i >= 0) {
			val = std::max(val, this->maxIntersect(shotTemp[i].getKeyframes(), shotTemp[index].getKeyframes()) * mult);
			mult = mult + Parameters::BSCWIncrement;
		}
	}	
	result = std::min(1.0, val);
}

vector<double> SceneSegmentation::calculateBSCWMT() {
	
	vector<double> val(shots.size(), 0.0);
	val[0] = 0.0;
	
	vector<thread> pool;
	unsigned numThreads = thread::hardware_concurrency() * 3;
	
	for(int i = 1; i < this->shots.size(); i++) {
		if(pool.size() >= numThreads) {
			for(auto &t : pool) {
				t.join();
			}
			pool.clear();
		}
		pool.push_back(thread(&SceneSegmentation::calculateBSCW, this, i, std::ref(this->shots), std::ref(val[i])));		
	}
	for(auto &t : pool) {
		t.join();
	}
	pool.clear();
	return val;
}

vector<pair<int,int>> SceneSegmentation::getScenes(bool enableOpticalFlow) {
	vector<double> bscw = this->calculateBSCWMT();
	int prev = 0;
	vector<vector<Shot>> scenes;
	
	double small = 1 - Parameters::PSBSmallDecrementDifference;
	double large = 1 - Parameters::PSBLargeDecrementDifference;
	
	for(int i = 1; i < bscw.size()-1; i++) {		
		if(bscw[i-1]*large >= bscw[i] ||
		 (bscw[i-1]*small >= bscw[i] && bscw[i]*small >= bscw[i+1])) {
			vector<Shot> temp;
			while(prev < i) {
				temp.push_back(shots[prev]);
				prev++;
			}
			scenes.push_back(temp);
			prev = i;
		}
	}
	
	/* The following lines contains the application heuristics, developed to remove false positive transitions. If neeeded, just remove the following lines */
	
	cout << "Scenes before small scenes heuristic: " << scenes.size() << endl;
	scenes = this->smallScenesHeuristic(scenes);
	
	cout << "Scenes before similar adjacent scenes heuristic: " << scenes.size() << endl;
	scenes = similarAdjacentScenesHeuristic(scenes);
	
	cout << "Scenes before windowed adjacent scenes heuristic: " << scenes.size() << endl;
	scenes = similarWindowedAdjacentScenesHeuristic(scenes);
	
	if(enableOpticalFlow) {
		cout << "Scenes before windowed optical flow heuristic: " << scenes.size() << endl;
		scenes = opticalFlowHeuristic(scenes);
	}
	
	cout << "Total number of detected scenes: " << scenes.size() << endl;		
	/* End of the heuristics block */
	
	vector<pair<int,int>> ret;
	
	for(int i = 0; i < scenes.size(); i++) {
		int min = numeric_limits<int>::max();
		int max = numeric_limits<int>::min();  
		
		for(Shot s : scenes[i]) {
			min = std::min(min,s.getFrameNumber().first);
			max = std::max(max,s.getFrameNumber().second);
		}
		ret.push_back(make_pair(min,max));
	}
	return ret;
}

vector<vector<Shot>> SceneSegmentation::opticalFlowHeuristic(vector<vector<Shot>> scenes) {
	
	vector<double> motion;
	vector<int> mergedScenes;
	
	for(auto &s : scenes) {
		motion.push_back(Utils::calculateMotion(s));
	}
	
	double small = 1 - Parameters::SimilarOpticalFlowMotionValue;
	double large = 1 + Parameters::SimilarOpticalFlowMotionValue;
	
	/* Remove adjacent scenes with a similar "motion" content */
	for(int i = 1; i < motion.size()-1; i++) {
		bool d12 = motion[i] >= motion[i-1] * small && motion[i] <= motion[i-1] * large ? true : false;
		bool d23 = motion[i] >= motion[i+1] * small && motion[i] <= motion[i+1] * large ? true : false;
		bool d13 = motion[i-1] >= motion[i+1] * small && motion[i-1] <= motion[i+1] * large ? true : false;
		
		if(d12 && d23 && d13) {
			for(Shot s : scenes[i]) {
				scenes[i-1].push_back(s);
			}
			for(Shot s : scenes[i+1]) {
				scenes[i-1].push_back(s);
			}
			motion.erase(motion.begin() + i + 1);
			motion.erase(motion.begin() + i );
			scenes.erase(scenes.begin() + i + 1);
			scenes.erase(scenes.begin() + i);
			i++;
			continue;
		}
		
		if(d12) {
			for(Shot s : scenes[i]) {
				scenes[i-1].push_back(s);
			}
			motion.erase(motion.begin() + i);
			scenes.erase(scenes.begin() + i);
			i++;
			continue;
		}
		
		if(d23) {
			for(Shot s : scenes[i+1]) {
				scenes[i].push_back(s);
			}
			motion.erase(motion.begin() + i + 1);
			scenes.erase(scenes.begin() + i + 1);
			i++;
			continue;	
		}		
	}
	
	motion.clear();
	for(auto &s : scenes) {
		motion.push_back(Utils::calculateMotion(s));
	}	
	double threshold = Parameters::VideoResolution / pow(Parameters::OpticalFlowWindowSize, 2);
		
	/* Remove adjacent scenes with, both, high motion content */
	for(int i = 1; i < motion.size(); i++) {
		if(motion[i-1] >= threshold && motion[i] >= threshold) {
			for(auto &s : scenes[i]) {
				scenes[i-1].push_back(s);
			}			
			motion.erase(motion.begin() + i);
			scenes.erase(scenes.begin() + i);
		}
	}
	return scenes;
	
}

vector<vector<Shot>> SceneSegmentation::similarAdjacentScenesHeuristic(vector<vector<Shot>> scenes) {
	bool control = true;
	while(control) {
		control = false;
		
		for(int i = 1; i < scenes.size(); i++) {
			double max = this->compareIntersecScenes(scenes[i], scenes[i-1], 0);
			double min = this->compareIntersecScenes(scenes[i], scenes[i-1], 1);
			
			if(max >= 0.8 || (max >= 0.6 && min >= 0.2)) {
				for(Shot s : scenes[i]) {
					scenes[i-1].push_back(s);
				}
				scenes.erase(scenes.begin() + i);
				control = true;
				break;
			}
		}
	}
	return scenes;
	
}

vector<vector<Shot>> SceneSegmentation::smallScenesHeuristic(vector<vector<Shot>> scenes) {
	bool control = true;
	double minVal, maxVal, avgVal;
	
	while(control) {
		control = false;
		
		for(int i = 0; i < scenes.size(); i++) {
			/* Does it have a single shot? */
			if (scenes[i].size() == 1) {
				if((scenes[i][0].getFrameNumber().second - scenes[i][0].getFrameNumber().first) > 150) {
					/* The shot is large enougth to be a scene itself */
					continue;
				}
				
				/* Its the first scene */
				if(i == 0) {
					if(this->compareIntersecScenes(scenes[i], scenes[i+1], 1) >= 0.2) {
						scenes[i+1].push_back(scenes[i][0]);
						control = true;
						scenes.erase(scenes.begin() + i);
						break;
					}					
				} else {
					/* Its the last scene */
					if (i == scenes.size()-1) {						
						if(this->compareIntersecScenes(scenes[i], scenes[i-1], 1) >= 0.2) {
							scenes[i-1].push_back(scenes[i][0]);
							control = true;
							scenes.erase(scenes.begin() + i);
							break;
						}	
					} else {
						/* Its a middle scene */
						double maxPos = this->compareIntersecScenes(scenes[i], scenes[i+1], 0);
						double maxPrev = this->compareIntersecScenes(scenes[i], scenes[i-1], 0);
						
						double minPos = this->compareIntersecScenes(scenes[i],scenes[i+1],1);
						double minPrev = this->compareIntersecScenes(scenes[i],scenes[i-1],1);
						
						double avgPos = this->compareIntersecScenes(scenes[i],scenes[i+1], 2);
						double avgPrev = this->compareIntersecScenes(scenes[i],scenes[i-1], 2);					
						
						if(maxPos >= maxPrev * 1.2 || (maxPos >= maxPrev && avgPos >= avgPrev) || minPos >= minPrev * 1.1) {
							scenes[i+1].push_back(scenes[i][0]);
							scenes.erase(scenes.begin() + i);
							control = true;
							break;
						}
						if(maxPrev >= maxPos * 1.2 || (maxPrev >= maxPos && avgPrev >= avgPos) || minPrev >= minPos * 1.1) {
							scenes[i-1].push_back(scenes[i][0]);
							scenes.erase(scenes.begin() + i);
							control = true;
							break;
						}
					}
				}
			}
		}		
	}
	
	return scenes;
}

vector<vector<Shot>> SceneSegmentation::similarWindowedAdjacentScenesHeuristic(vector<vector<Shot>> scenes) {
	for(int i = 1; i < scenes.size()-1; i++) {
		double max12 = this->compareIntersecScenes(scenes[i-1],scenes[i], 0);
		double min12 = this->compareIntersecScenes(scenes[i-1],scenes[i], 1);
		double avg12 = this->compareIntersecScenes(scenes[i-1],scenes[i], 2);
		
		double max23 = this->compareIntersecScenes(scenes[i],scenes[i+1], 0);
		double min23 = this->compareIntersecScenes(scenes[i],scenes[i+1], 1);
		double avg23 = this->compareIntersecScenes(scenes[i],scenes[i+1], 2);
		
		double max13 = this->compareIntersecScenes(scenes[i-1],scenes[i+1], 0);
		double min13 = this->compareIntersecScenes(scenes[i-1],scenes[i+1], 1);
		double avg13 = this->compareIntersecScenes(scenes[i-1],scenes[i+1], 2);
			
		if(min13 >= 0.5) {
			if(max13 >= 1.8 || avg13 >= 0.3) {					
				for(Shot s : scenes[i]) {
					scenes[i-1].push_back(s);
				}
				for(Shot s : scenes[i+1]) {
					scenes[i-1].push_back(s);
				}
				scenes.erase(scenes.begin() + i + 1);
				scenes.erase(scenes.begin() + i);
				continue;
			}					
		}
		if(min12 >= 0.25) {
			if(max12 >= 0.8 || avg12 >= 0.3) {
				for(Shot s : scenes[i]) {
					scenes[i-1].push_back(s);
				}
				scenes.erase(scenes.begin() + i);
				continue;
			}
		}
		
		if(min23 >= 0.25) {
			if(max23 >= 0.8 || avg23 >= 0.3) {
				for(Shot s : scenes[i+1]) {
					scenes[i].push_back(s);
				}
				scenes.erase(scenes.begin() + i + 1);
				i++;
				continue;
			}
		}
	}
	return scenes;
}