#include "GlobalIncludes.h"
#include "opencv2/opencv.hpp"

extern "C"
{
#include "apriltag.h"
#include "apriltag_pose.h"
#include "tag36h11.h"
#include "tag25h9.h"
#include "tag16h5.h"
#include "tagCircle21h7.h"
#include "tagCircle49h12.h"
#include "tagCustom48h12.h"
#include "tagStandard41h12.h"
#include "tagStandard52h13.h"
#include "common/getopt.h"
}

using namespace cv;

class CameraLocator
{
	vector<Matrix4> _tagTransformations;
	double _tagsize;

	VideoCapture _cap;

	apriltag_detector *_td;

	Mat _cameraIntrinsicMatrix, _cameraDistortion;
	Mat _rawFrame, _undistortedFrame, _grayscaleFrame;

	thread _mainThread;
	bool _running = false;

	mutex _cameraMutex;
	Matrix4 _cameraTransformation;

	void processFrame();

	void mainLoop(int captureDevice);

	double intrinsicData[9] = {522.12285297286087, 0, 320,
							   0, 531.89050725948039, 240,
							   0, 0, 1};

	double distortionCoefficients[4] = {0.090356664956049604, -0.070991149612318186, 0.0045443094634366422, 0.0055652050488018776};

public:
	explicit CameraLocator(vector<Matrix4> tagTransformations, double tagsize = 0.1);

	void Start(int captureDevice);

	void Stop();

	Matrix4 getTransformation();

	~CameraLocator();
};