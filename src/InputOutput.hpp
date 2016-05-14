#ifndef INPUTOUTPUT_H
#define INPUTOUTPUT_H

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <iomanip>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "Utils.hpp"

using namespace std;

class InputOutput {
	public:	
		static bool checkFile(string name);
		static vector<pair<int,int>> parseCSV(string filePath);
		static vector<pair<int,int>> extractKeyframes(string kfPath);
		static vector<pair<int,int>> extractShots(string shotsPath);
		static void writeCSV(string file, vector<pair<int,int>> data);
};

#endif