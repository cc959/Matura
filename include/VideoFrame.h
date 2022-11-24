#pragma once

#include "GlobalIncludes.h"
#include "Joystick.h"
#include "Frame.h"

#include "opencv2/core.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/imgproc.hpp"

using namespace cv;

class VideoFrame : public Frame
{
    struct Fr {
        GL::Texture2D image;
        int index;
    };

public:
	VideoFrame(string path, const Timeline &timeline, ImGuiIntegration::Context &guiContext);

private:
	void draw3D() override;
	void setupGUI() override;
    void Enter() override;
    void Leave() override;

    void loadToRam(int count = 1);
    void loadToVRam();

	static GL::Mesh *_quad;
	static Shaders::FlatGL2D *_shader;
	map<int, GL::Texture2D> _frames;
    map<int, Mat> _ramFrames;

    float _startTime = 0;
    float _fps;
    int _loadedFrameAt = 0;
    VideoCapture _cap;

    static const int _bufferFrames = 25;

    Mutex _bufferMutex, _ramBufferMutex, _captureMutex;
};