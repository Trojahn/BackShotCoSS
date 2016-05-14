#include "Parameters.hpp"

/*
	The number of backward shots thats going to be analyzed. As specified by "Rasheed, Z., & Shah, M. (2003). Scene detection in Hollywood movies and TV shows. In 2003 IEEE Computer Society Conference on Computer Vision and Pattern Recognition, 2003. Proceedings. (Vol. 2, pp. II–343–8). Vancouver, Canada: IEEE Comput. Soc. http://doi.org/10.1109/CVPR.2003.1211489", the value 10 seems to be adequate.
*/
int Parameters::BSCWWindowsSize = 10;

/*
	This value is used to improve the BSC value among closer shots than the far ones. As "Trojahn, T. H., & Goularte, R. (2013). Video scene segmentation by improved visual shot coherence. In Proceedings of the 19th Brazilian symposium on Multimedia and the web - WebMedia ’13 (pp. 23–30). New York, New York, USA: ACM Press. http://doi.org/10.1145/2526188.2526206", the default value is 0.05.
*/
double Parameters::BSCWIncrement = 0.05;

/*
	A potential scene boundary (PSB) is detected when the BSCW value decreases between two adjacent values. The value, as specified by Trojahn and Goularte, is at least 15%.
*/
double Parameters::PSBLargeDecrementDifference = 0.15;

/*
	The PSBSmallDecrementDifference describes the variation among 3 consecutie scenes. The value, as specified by Trojahn and Goularte, is at least 5%.
*/
double Parameters::PSBSmallDecrementDifference = 0.05;

/*
	The maximum number of points which are going to be analyzed by the optical flow procedure. May be increased if the video shows a expressive amount of borders with relevant motion and/or the video resolution is HUGE!
	
	Most of the time, 1000 seems to be enough.
*/
int Parameters::MaxOpticalFlowFeatures = 1000;

/*
	The SimilarOpticalFlowMotionValue is used by the optical flow heuristic to determine if at most 3 consecutive "scenes" does have "similar motion". If true, the scenes will be merged into a single scene.
	
	This value has HIGH impact on the heuristic itself. A smaller value does merge only very similar scenes, while large values merges almost anything. The best way to determine an adequate value are by try and error.
	
	The default value, 25%, seems to be adequate to movies with almost static scenes followed be high motion content (like action movies).
*/
double Parameters::SimilarOpticalFlowMotionValue = 0.25;

/*
	The video resolution, which are determined at runtime, is used by the opticalflow heuristic to merge consecutive "high motion" scenes.
*/
int Parameters::VideoResolution = -1;

/*
	Specify the windows size to extract potential features for the optical flow procedure. Also used in the opticalflow heuristic. A larger number may decrease the overall performance of the algorithm.
*/
int Parameters::OpticalFlowWindowSize = 15;