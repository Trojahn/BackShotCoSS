# BackShotCoSS
BackShotCoSS (**Back**ward **Shot** **Co**herence **S**cene **S**egmentation) is a video scene segmentation technique based on visual backward shot coherence as described by Trojahn and Goularte in 2013.

# Requirements
*	A C++11 compatible compiler, like gcc
*	A functional OpenCV installation (version 3.x and above)
*   An appropriate set of CODECs (for video decoding)

# Arguments
	./BackShotCoSS <videoFile> <keyframesCSV> <shotSegmentationCSV> <outputCSV> [<disableOpticalFlow>]
1. **videoFile** - A video file which you want to segment.
2. **keyframesCSV** - A CSV file which describes the selected keyframes for each shot of the video. The first value must be the shot number and the second the (global) frame number. There needs to have at least one keyframe for each shot. If in doubt, check the FAST (https://github.com/Trojahn/FAST) for more details.
3. **shotSegmentationCSV** - A CSV file with the frame-based shot segmentation of the input file.
4. **outputCSV** - The CSV output file with the shot number which composes each scene. The first shot is "1".
5. (optional) **disableOpticalFlow** - A (YES/NO) optional parameter which may disables the optical flow heuristic.

# Example
	$ cat keyframes.csv
		1,100
		1,95
		2,241
		3,654
		3,701
	$ cat shotSegmentation.csv
		1,200
		201,574
		590,889
		890,1017		
	$ ./BackShotCoSS video.avi keyframes.csv shotSegmentation.csv output.csv yes
	$ cat output.csv
		1,3
		4,26
		27,56
		57,88
		89,91
		92,107

# Caveats
At least a single keyframe frame is required for each shot identified in the shotSegmentation file.

The application, while being fully automatic, depends on a number of predefined heuristics and thresholds, whose values where heuristically estimated in the movies domain. Some of the most important thresholds are defined in the *Parameters* (.cpp and .hpp), along with a simple explanation of each one. These values may be manually tunned to achieve better results.

The application extracts HSV histograms of the video keyframes' for the scene segmentation procedures and its related heuristics. A gray-scale video input may return undesirable results.

A particular heuristic is based on optical flow motion estimation of all adjacent frames. The heuristic, which was developed to minimize oversegmentation due to high motion ("action scenes"), does requires the analysis of all video frames. This heuristic makes the application considerably slower. If you do not wanna use it, just disable it using the "disableOpticalFlow" flag.

All the applied heuristics does try to improve the overall precision by merging adjacent scenes by some criteria (i.e. it tries to remove false positives transitions). This could hurt the result overall recall (i.e. by eliminating true positive transitions). To disable this, remove/comment the related lines on the *getScenes* method of the *SceneSegmentation* class.   

# Related applications
FAST (https://github.com/Trojahn/FAST) - An application developed to achieve a fast video shot segmentation. The output shot segmentation file is supported by the BackShotCoSS.
VKFrameS (https://github.com/Trojahn/VKFrameS) - An application which tries to extract a set of different keyframes for each input shot. The keyframes output file is supported by the BackShotCoSS. 

# Related publications
Trojahn, T. H., & Goularte, R. (2013). Video scene segmentation by improved visual shot coherence. In Proceedings of the 19th Brazilian symposium on Multimedia and the web - WebMedia ’13 (pp. 23–30). New York, New York, USA: ACM Press. http://doi.org/10.1145/2526188.2526206

Trojahn, T. H., 2014. *Automatic video scene segmentation based on shot coherence*. M.S. thesis. Universidade de São Paulo. Avaliable in http://www.teses.usp.br/teses/disponiveis/55/55134/tde-20052014-152446/. (In portuguese).


# About
BackShotCoSS is an implementation of the M.S. thesis "Segmentação automática de vídeo em cenas baseada em coerência entre tomadas" (*Automatic video scene segmentation based on shot coherence* - in Portuguese), authored by Tiago H. Trojahn under the coordination of Rudinei Goularte at Instituto de Computação e Matemática Computacional (ICMC) of Universidade de São Paulo (USP) in São Carlos, São Paulo, Brazil.
