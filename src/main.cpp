// #include "opencv2/imgproc.hpp"
// #include "opencv2/highgui.hpp"
// #include "opencv2/videoio.hpp"
// #include "opencv2/features2d.hpp"
// #include "glm/glm.hpp"

// #include <bits/stdc++.h>
// using namespace cv;
// const int max_value_H = 360 / 2;
// const int max_value = 255;
// const String window_capture_name = "Video Capture";
// const String window_detection_name = "Object Detection";
// int low_H = 110, low_S = 148, low_V = 139;
// int high_H = 130, high_S = max_value, high_V = max_value;
// static void on_low_H_thresh_trackbar(int, void *)
// {
// 	low_H = min(high_H - 1, low_H);
// 	setTrackbarPos("Low H", window_detection_name, low_H);
// }
// static void on_high_H_thresh_trackbar(int, void *)
// {
// 	high_H = max(high_H, low_H + 1);
// 	setTrackbarPos("High H", window_detection_name, high_H);
// }
// static void on_low_S_thresh_trackbar(int, void *)
// {
// 	low_S = min(high_S - 1, low_S);
// 	setTrackbarPos("Low S", window_detection_name, low_S);
// }
// static void on_high_S_thresh_trackbar(int, void *)
// {
// 	high_S = max(high_S, low_S + 1);
// 	setTrackbarPos("High S", window_detection_name, high_S);
// }
// static void on_low_V_thresh_trackbar(int, void *)
// {
// 	low_V = min(high_V - 1, low_V);
// 	setTrackbarPos("Low V", window_detection_name, low_V);
// }
// static void on_high_V_thresh_trackbar(int, void *)
// {
// 	high_V = max(high_V, low_V + 1);
// 	setTrackbarPos("High V", window_detection_name, high_V);
// }
// int main()
// {
// 	VideoCapture cap("/dev/video2");

// 	namedWindow(window_capture_name);
// 	namedWindow(window_detection_name);
// 	// Trackbars to set thresholds for HSV values
// 	createTrackbar("Low H", window_detection_name, &low_H, max_value_H, on_low_H_thresh_trackbar);
// 	createTrackbar("High H", window_detection_name, &high_H, max_value_H, on_high_H_thresh_trackbar);
// 	createTrackbar("Low S", window_detection_name, &low_S, max_value, on_low_S_thresh_trackbar);
// 	createTrackbar("High S", window_detection_name, &high_S, max_value, on_high_S_thresh_trackbar);
// 	createTrackbar("Low V", window_detection_name, &low_V, max_value, on_low_V_thresh_trackbar);
// 	createTrackbar("High V", window_detection_name, &high_V, max_value, on_high_V_thresh_trackbar);

// 	SimpleBlobDetector::Params params;
// 	memset(&params, 0, sizeof(SimpleBlobDetector::Params));

// 	params.minThreshold = 250;
// 	params.maxThreshold = 256;
// 	params.thresholdStep = 1;
// 	params.minRepeatability = 1;

// 	params.filterByArea = true;
// 	params.minArea = 100;
// 	params.maxArea = 1000000;

// 	params.filterByColor = true;
// 	params.blobColor = 255;

// 	params.filterByCircularity = true;
// 	params.minCircularity = 0.3;
// 	params.maxCircularity = 1;

// 	// params.filterByCircularity = true;
// 	// params.minCircularity = 0.8f;

// 	Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);

// 	Mat frame, frame_HSV, frame_threshold;
// 	while (true)
// 	{
// 		cap >> frame;
// 		if (frame.empty())
// 		{
// 			break;
// 		}
// 		// Convert from BGR to HSV colorspace
// 		cvtColor(frame, frame_HSV, COLOR_RGB2HSV);
// 		// Detect the object based on HSV Range Values
// 		inRange(frame_HSV, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), frame_threshold);
// 		// Show the frames

// 		std::vector<KeyPoint> points;
// 		detector->detect(frame_threshold, points);

// 		for (auto pnt : points)
// 		{
// 			std::cout << pnt.pt.x << " " << pnt.pt.y << " " << pnt.size << "\n";
// 		}

// 		Mat im_with_keypoints;
// 		drawKeypoints(frame_threshold, points, frame_threshold, Scalar(255, 0, 0), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
// 		drawKeypoints(frame, points, im_with_keypoints, Scalar(255, 0, 0), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

// 		imshow(window_capture_name, im_with_keypoints);
// 		imshow(window_detection_name, frame_threshold);

// 		char key = (char)waitKey(1);
// 		if (key == 'q' || key == 27)
// 		{
// 			break;
// 		}
// 	}
// 	return 0;
// }

// #include <bits/stdc++.h>

// using namespace std;

// int main()
// {

// 	fstream file;

// 	file.open("/dev/ttyACM0", fstream::out);
// 	sleep(5);

// 	while (true)
// 	{
// 		int32_t speed, shoulder, elbow;
// 		cin >> speed >> shoulder >> elbow;

// 		unsigned char out[3][4];
// 		(*(int *)out[0]) = speed;
// 		(*(int *)out[1]) = shoulder;
// 		(*(int *)out[2]) = elbow;
// 		file.write((char *)out, 12);

// 		file.flush();
// 	}

// 	sleep(5);

// 	file.close();
// 	return 0;
// }

#include "Viewer.h"

int main(int argc, char **argv)
{
	Viewer app({argc, argv});
	return app.exec();
}

/* Copyright (C) 2013-2016, The Regents of The University of Michigan.
All rights reserved.
This software was developed in the APRIL Robotics Lab under the
direction of Edwin Olson, ebolson@umich.edu. This software may be
available under alternative licensing terms; contact the address above.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the Regents of The University of Michigan.
*/

// #CAMERA_ID, MODEL, WIDTH, HEIGHT, PARAMS[] < -fx, fy, cx, cy, k1, k2, p1, p2
// 1 OPENCV 640 480 522.12285297286087 531.89050725948039 320 240 0.090356664956049604 - 0.070991149612318186 0.0045443094634366422 0.0055652050488018776

// #include <iostream>
// #include <bits/stdc++.h>

// #include "opencv2/opencv.hpp"

// extern "C"
// {
// #include "apriltag.h"
// #include "apriltag_pose.h"
// #include "tag36h11.h"
// #include "tag25h9.h"
// #include "tag16h5.h"
// #include "tagCircle21h7.h"
// #include "tagCircle49h12.h"
// #include "tagCustom48h12.h"
// #include "tagStandard41h12.h"
// #include "tagStandard52h13.h"
// #include "common/getopt.h"
// }

// #include <colmap/base/camera_models.h>

// using namespace std;
// using namespace cv;

// #define NOW chrono::high_resolution_clock::now().time_since_epoch().count()

// void printMat(Mat data)
// {
// 	for (int i = 0; i < data.size().height; i++)
// 	{
// 		for (int j = 0; j < data.size().width; j++)
// 		{
// 			cerr << data.at<double>(i, j) << " ";
// 		}
// 		cerr << "\n";
// 	}
// }

// int main()
// {
// 	// Initialize camera
// 	VideoCapture cap(0);
// 	if (!cap.isOpened())
// 	{
// 		cerr << "Couldn't open video capture device" << endl;
// 		return -1;
// 	}

// 	cerr << cap.get(CAP_PROP_FRAME_WIDTH) << "x" << cap.get(CAP_PROP_FRAME_HEIGHT) << " at " << cap.get(CAP_PROP_FPS) << " fps"
// 		 << "\n";

// 	// Initialize tag detector with options
// 	apriltag_detector_t *td = apriltag_detector_create();
// 	apriltag_detector_add_family(td, tag36h11_create());
// 	td->quad_decimate = 1.0; // decimate factor
// 	td->quad_sigma = 0.0;	 // apply this much low-pass blur to input
// 	td->nthreads = 1;		 // use this many cpu threads
// 	td->debug = 0;			 // print debug output
// 	td->refine_edges = 1;	 // refine tag edges

// 	auto lastFrame = NOW;

// 	double intrinsicData[9] = {522.12285297286087, 0, 320,
// 							   0, 531.89050725948039, 240,
// 							   0, 0, 1};

// 	double distortionCoefficients[4] = {0.090356664956049604, -0.070991149612318186, 0.0045443094634366422, 0.0055652050488018776};

// 	Mat cameraIntrinsicMatrix(3, 3, CV_64F, intrinsicData);
// 	Mat cameraDistortion(1, 4, CV_64F, distortionCoefficients);

// 	Mat frameDistorted, frame, frameWarped, gray;

// 	int frameID = 0;

// 	while (true)
// 	{
// 		cap >> frameDistorted;
// 		undistort(frameDistorted, frame, cameraIntrinsicMatrix, cameraDistortion);

// 		cvtColor(frame, gray, COLOR_BGR2GRAY);

// 		// Make an image_u8_t header for the Mat data
// 		image_u8_t im = {.width = gray.cols,
// 						 .height = gray.rows,
// 						 .stride = gray.cols,
// 						 .buf = gray.data};

// 		zarray_t *detections = apriltag_detector_detect(td, &im);
// 		cout << zarray_size(detections) << " tags detected" << endl;

// 		// Draw detection outlines
// 		for (int i = 0; i < zarray_size(detections); i++)
// 		{
// 			apriltag_detection_t *det;
// 			zarray_get(detections, i, &det);

// 			apriltag_detection_info_t info;
// 			info.det = det;
// 			info.tagsize = 0.1;
// 			info.fx = 522.12285297286087;
// 			info.fy = 531.89050725948039;
// 			info.cx = 320;
// 			info.cy = 240;

// 			apriltag_pose_t pose;
// 			estimate_tag_pose(&info, &pose);

// 			Mat t(3, 1, CV_64FC1, pose.t->data);
// 			Mat r(3, 3, CV_64FC1, pose.R->data);

// 			Mat blub;

// 			hconcat(r, t, blub);

// 			double data[4] = {0, 0, 0, 1};

// 			Mat b(1, 4, CV_64FC1, data);

// 			Mat blub2;

// 			vconcat(blub, b, blub2);

// 			printMat(blub2.inv().col(3));

// 			// double data[9] = {1, 0, -640 / 2, 0, 1, -380 / 2, 0, 0, 1};

// 			// H = Mat(3, 3, CV_64F, det->H->data);

// 			// H = cameraIntrinsicMatrix * H.inv();

// 			// // double norm = sqrt(H.at<double>(0, 0) * H.at<double>(0, 0) +
// 			// // 				   H.at<double>(1, 0) * H.at<double>(1, 0) +
// 			// // 				   H.at<double>(2, 0) * H.at<double>(2, 0));

// 			// warpPerspective(frame, frameWarped, H, frame.size());

// 			// vector<Mat> Rs;
// 			// vector<Mat> Ts;

// 			// decomposeHomographyMat(H.inv(), cameraIntrinsicMatrix, Rs, Ts, noArray());

// 			// for (auto mat : Ts)
// 			// {
// 			// 	printMat(mat);
// 			// 	cerr << "\n";
// 			// }
// 			// cerr << "\n";

// 			// cerr << Rs.size() << " " << Ts.size() << "\n";

// 			// Mat c1 = H.col(0);
// 			// Mat c2 = H.col(1);
// 			// Mat c3 = c1.cross(c2);
// 			// Mat tvec = H.col(2);
// 			// Mat R(3, 3, CV_64F);
// 			// for (int i = 0; i < 3; i++)
// 			// {
// 			// 	R.at<double>(i, 0) = c1.at<double>(i, 0);
// 			// 	R.at<double>(i, 1) = c2.at<double>(i, 0);
// 			// 	R.at<double>(i, 2) = c3.at<double>(i, 0);
// 			// }

// 			// printMat(tvec);

// 			line(frame, Point(det->p[0][0], det->p[0][1]),
// 				 Point(det->p[1][0], det->p[1][1]),
// 				 Scalar(0, 0xff, 0), 2);
// 			line(frame, Point(det->p[0][0], det->p[0][1]),
// 				 Point(det->p[3][0], det->p[3][1]),
// 				 Scalar(0, 0, 0xff), 2);
// 			line(frame, Point(det->p[1][0], det->p[1][1]),
// 				 Point(det->p[2][0], det->p[2][1]),
// 				 Scalar(0xff, 0, 0), 2);
// 			line(frame, Point(det->p[2][0], det->p[2][1]),
// 				 Point(det->p[3][0], det->p[3][1]),
// 				 Scalar(0xff, 0, 0), 2);

// 			stringstream ss;
// 			ss << det->id;
// 			String text = ss.str();
// 			int fontface = FONT_HERSHEY_SCRIPT_SIMPLEX;
// 			double fontscale = 1.0;
// 			int baseline;
// 			Size textsize = getTextSize(text, fontface, fontscale, 2,
// 										&baseline);
// 			putText(frame, text, Point(det->c[0] - textsize.width / 2, det->c[1] + textsize.height / 2),
// 					fontface, fontscale, Scalar(0xff, 0x99, 0), 2);
// 		}
// 		apriltag_detections_destroy(detections);

// 		// imwrite("./Colmap/images/frame" + to_string(frameID++) + ".png", frame);

// 		auto currentFrame = NOW;

// 		float delta = float(currentFrame - lastFrame) / 1e9;
// 		float fps = 1 / delta;

// 		// putText(frame, to_string(fps), Point(0, 50),
// 		// 		6, 0.5, Scalar(0xff, 0x99, 0), 2);

// 		lastFrame = currentFrame;

// 		imshow("Tag Detections", frame);
// 		if (frameWarped.size().width > 0)
// 			imshow("Blub", frameWarped);
// 		if (waitKey(30) == 113)
// 			break;
// 	}

// 	// destroy tag families
// 	zarray_t *families;
// 	families = td->tag_families;
// 	for (int i = 0; i < zarray_size(families); i++)
// 	{
// 		apriltag_family_t *tf;
// 		zarray_get(families, i, &tf);

// 		if (!strcmp(tf->name, "tag36h11"))
// 		{
// 			tag36h11_destroy(tf);
// 		}
// 		else if (!strcmp(tf->name, "tag25h9"))
// 		{
// 			tag25h9_destroy(tf);
// 		}
// 		else if (!strcmp(tf->name, "tag16h5"))
// 		{
// 			tag16h5_destroy(tf);
// 		}
// 		else if (!strcmp(tf->name, "tagCircle21h7"))
// 		{
// 			tagCircle21h7_destroy(tf);
// 		}
// 		else if (!strcmp(tf->name, "tagCircle49h12"))
// 		{
// 			tagCircle49h12_destroy(tf);
// 		}
// 		else if (!strcmp(tf->name, "tagStandard41h12"))
// 		{
// 			tagStandard41h12_destroy(tf);
// 		}
// 		else if (!strcmp(tf->name, "tagStandard52h13"))
// 		{
// 			tagStandard52h13_destroy(tf);
// 		}
// 		else if (!strcmp(tf->name, "tagCustom48h12"))
// 		{
// 			tagCustom48h12_destroy(tf);
// 		}
// 	}
// 	apriltag_detector_destroy(td);

// 	return 0;
// }
