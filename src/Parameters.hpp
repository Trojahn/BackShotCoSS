#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <string>
#include <limits>
#include <algorithm>

using namespace std;

class Parameters {
	public:
		static int BSCWWindowsSize;
		static double BSCWIncrement;
		static double PSBLargeDecrementDifference;
		static double PSBSmallDecrementDifference;
		static double SimilarOpticalFlowMotionValue;
		static int MaxOpticalFlowFeatures;
		
		static int VideoResolution;
		static int OpticalFlowWindowSize;
			
};

#endif