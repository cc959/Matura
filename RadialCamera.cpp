#include "RadialCamera.h"

RadialCamera::RadialCamera(double width, double height, double focalLen, double distortion)
{
	params = {width, height, focalLen, distortion};
}

vector<RadialCamera> RadialCamera::loadCameras(string path)
{
	ifstream cameraData(path);

	char trash[10000];

	vector<RadialCamera> cameras;
	while (!cameraData.eof())
	{
		if (cameraData.peek() == '#') // comment
		{
			cameraData.getline(trash, 10000);
			continue;
		}

		int number;
		string type;
		cameraData >> number >> type;

		if (type != "SIMPLE_RADIAL")
		{
			cameraData.getline(trash, 10000);
			continue;
		}

		double width, height;
		vector<double> params(4);
		cameraData >> width >> height;
		cameraData >> params[0] >> params[1] >> params[2] >> params[3];

		cameras.push_back({params[0], params[1], params[2], params[3]});
	}

	return cameras;
}