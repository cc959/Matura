#pragma once

#include <bits/stdc++.h>
#include <colmap/base/camera_models.h>

using namespace std;
using namespace colmap;

class RadialCamera
{
public:
	vector<double> params;

	RadialCamera(double width, double height, double focalLen, double distortion);

	static vector<RadialCamera> loadCameras(string path);
};