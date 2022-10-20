#include "CameraLocator.h"


Matrix4 getMatrix(const apriltag_pose_t& pose) {
    const Vector3 mapping = {1, -1, -1};

    // transform OpenCV matrix to Magnum and transform coordinate system -> must transform because of different storage format
    auto translation = Vector3(Vector3d::from(pose.t->data)) * mapping;

    auto rotationMatrix = Matrix3(Matrix3d::from(pose.R->data).transposed());
    auto rotation = Quaternion::fromMatrix(rotationMatrix);
    auto eulerAngles = rotation.toEuler();
    rotation =  Quaternion::rotation(eulerAngles.z() * mapping.z(), Vector3::zAxis()) *
                Quaternion::rotation(eulerAngles.y() * mapping.y(), Vector3::yAxis()) *
                Quaternion::rotation(eulerAngles.x() * mapping.x(), Vector3::xAxis());

    return Matrix4::from(rotation.toMatrix(), translation);
}


void CameraLocator::processFrame()
{
	lock_guard<mutex> guard(_cameraMutex);

	_cap >> _rawFrame;
	undistort(_rawFrame, _undistortedFrame, _cameraIntrinsicMatrix, _cameraDistortion);

	cvtColor(_undistortedFrame, _grayscaleFrame, COLOR_BGR2GRAY);

	// Make an image_u8_t header for the Mat data
	image_u8_t im = {.width = _grayscaleFrame.cols,
					 .height = _grayscaleFrame.rows,
					 .stride = _grayscaleFrame.cols,
					 .buf = _grayscaleFrame.data};

	zarray_t *detections = apriltag_detector_detect(_td, &im);
	cout << zarray_size(detections) << " tags detected" << endl;

	// Draw detection outlines
	for (int i = 0; i < zarray_size(detections); i++)
	{
		apriltag_detection_t *det;
		zarray_get(detections, i, &det);

		apriltag_detection_info_t info;
		info.det = det;
		info.tagsize = _tagsize;
		info.fx = 522.12285297286087;
		info.fy = 531.89050725948039;
		info.cx = 320;
		info.cy = 240;

		apriltag_pose_t pose;
		estimate_tag_pose(&info, &pose);

		Matrix4 transformationTag = (det->id >= 0 && det->id < _tagTransformations.size()) ? _tagTransformations[det->id] : Matrix4::translation({});

		_cameraTransformation =getMatrix(pose).inverted();

		line(_undistortedFrame, Point(det->p[0][0], det->p[0][1]),
			 Point(det->p[1][0], det->p[1][1]),
			 Scalar(0, 0, 0xff), 2);
		line(_undistortedFrame, Point(det->p[0][0], det->p[0][1]),
			 Point(det->p[3][0], det->p[3][1]),
			 Scalar(0, 0xff, 0), 2);
		line(_undistortedFrame, Point(det->p[1][0], det->p[1][1]),
			 Point(det->p[2][0], det->p[2][1]),
			 Scalar(0xff, 0, 0), 2);
		line(_undistortedFrame, Point(det->p[2][0], det->p[2][1]),
			 Point(det->p[3][0], det->p[3][1]),
			 Scalar(0xff, 0, 0), 2);

		string text = to_string(det->id);
		int fontface = FONT_HERSHEY_SCRIPT_SIMPLEX;
		double fontscale = 1.0;
		int baseline;
		Size textsize = getTextSize(text, fontface, fontscale, 2,
									&baseline);
		putText(_undistortedFrame, text, Point(det->c[0] - textsize.width / 2, det->c[1] + textsize.height / 2),
				fontface, fontscale, Scalar(0xff, 0x99, 0), 2);
	}

	apriltag_detections_destroy(detections);
}

void CameraLocator::mainLoop(int captureDevice)
{
	_cap.open(captureDevice);

	if (!_cap.isOpened())
	{
		cerr << "Could not open capture device " << captureDevice << "!\n";
		return;
	}

	while (_running)
	{
		processFrame();
		usleep(1000000 / 30);
	}

	_cap.release();
}

CameraLocator::CameraLocator(vector<Matrix4> tagTransformations, double tagsize) : _tagsize{tagsize}, _tagTransformations{tagTransformations}
{
	_td = apriltag_detector_create();
	apriltag_detector_add_family(_td, tag36h11_create());
	_td->quad_decimate = 1.0;   // decimate factor
	_td->quad_sigma = 0.0;	    // apply this much low-pass blur to input
	_td->nthreads = 1;		    // use this many cpu threads
	_td->debug = false;	        // print debug output
	_td->refine_edges = true;   // refine tag edges

	_cameraIntrinsicMatrix = Mat(3, 3, CV_64F, intrinsicData);
	_cameraDistortion = Mat(1, 4, CV_64F, distortionCoefficients);
}

void CameraLocator::Start(int captureDevice)
{
	if (!_running)
	{
		_running = true;
		_mainThread = thread(&CameraLocator::mainLoop, this, captureDevice);
	}
	else
	{
		cerr << "Is already running!\n";
	}
}

void CameraLocator::Stop()
{
	if (_running)
	{
		_running = false;
		_mainThread.join();
	}
}

Matrix4 CameraLocator::getTransformation()
{
	lock_guard<mutex> guard(_cameraMutex);
    if (_undistortedFrame.size().width > 0) {
        imshow("Tag Detections", _undistortedFrame);
        waitKey(1);
    }

	return _cameraTransformation;
}

CameraLocator::~CameraLocator()
{
	// destroy tag families
	zarray_t *families;
	families = _td->tag_families;
	for (int i = 0; i < zarray_size(families); i++)
	{
		apriltag_family_t *tf;
		zarray_get(families, i, &tf);

		if (!strcmp(tf->name, "tag36h11"))
		{
			tag36h11_destroy(tf);
		}
		else if (!strcmp(tf->name, "tag25h9"))
		{
			tag25h9_destroy(tf);
		}
		else if (!strcmp(tf->name, "tag16h5"))
		{
			tag16h5_destroy(tf);
		}
		else if (!strcmp(tf->name, "tagCircle21h7"))
		{
			tagCircle21h7_destroy(tf);
		}
		else if (!strcmp(tf->name, "tagCircle49h12"))
		{
			tagCircle49h12_destroy(tf);
		}
		else if (!strcmp(tf->name, "tagStandard41h12"))
		{
			tagStandard41h12_destroy(tf);
		}
		else if (!strcmp(tf->name, "tagStandard52h13"))
		{
			tagStandard52h13_destroy(tf);
		}
		else if (!strcmp(tf->name, "tagCustom48h12"))
		{
			tagCustom48h12_destroy(tf);
		}
	}
	apriltag_detector_destroy(_td);
}