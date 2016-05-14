#include "InputOutput.hpp"

bool InputOutput::checkFile(string name) {
  	if(FILE *file = fopen(name.c_str(),"r")) {
		fclose(file);
		return true;
	}
	return false;
}

vector<pair<int,int>> InputOutput::parseCSV(string filePath) {
	ifstream file(filePath.c_str());
	string line;
	string token;
	vector<pair<int,int>> ret;
	
	if(file.is_open()) {
		while(getline(file,line)) {
			istringstream str(line);
			vector<int> temp;
			while(getline(str,token,',')) {
				temp.push_back(atoi(token.c_str()));
			}
			ret.push_back(make_pair(temp[0],temp[1]));
			temp.clear();
		}
	}	
    file.close();
	return ret;
}

vector<pair<int,int>> InputOutput::extractKeyframes(string kfPath) {
	vector<pair<int,int>> keyframes = InputOutput::parseCSV(kfPath);
	sort(keyframes.begin(),keyframes.end(),Utils::pairCompare);
	//Check if the first shot is "0". If it's not, make it so...
	if(keyframes[0].first > 0) {
		Utils::normalizePairs(keyframes,-(keyframes[0].first));
	}
	
	return keyframes;
}

vector<pair<int,int>> InputOutput::extractShots(string shotsPath) {
	vector<pair<int,int>> shots = InputOutput::parseCSV(shotsPath);

	if(shots[0].first > 0) {
		Utils::normalizePairs(shots,-(shots[0].first));
	}
	
	return shots;
}

void InputOutput::writeCSV(string file, vector<pair<int,int>> data) {
	ofstream f (file.c_str());
	f << data[0].first << "," << data[0].second;
	for(int i = 1; i < data.size(); i++) {
		f << endl << data[i].first << "," << data[i].second;
	}
	f.close();
}