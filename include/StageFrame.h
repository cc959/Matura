#pragma once

#include "GlobalIncludes.h"

#include "Joystick.h"
#include "Frame.h"
#include "Stage.h"
#include "ShadowLight.h"
#include "DebugUI.h"

class StageFrame : public Frame, public Stage
{
	string _path;

	void setupShadows();

	Object3D _shadowCameraIndicator;

	DebugUI _debug;

	void addDebugLines();
	void addDebugLines(const Matrix4 &transformation);
	bool _setTarget = true;

	Joystick &_joystick;

public:
	Object3D _chacheCameraObject{&_scene};

	SceneGraph::Camera3D _cacheCamera{_chacheCameraObject};

	StageFrame(const Timeline &timeline, ImGuiIntegration::Context &guiContext, Joystick &joystick);

	StageFrame(string path, const Timeline &timeline, ImGuiIntegration::Context &guiContext, Joystick &joystick);

	void draw3D() override;

	bool my_tool_active = true;
	float my_color[4] = {1, 1, 1, 1};

	void setupGUI() override;

	// resize viewport event
	void viewportEvent(SDLApp::ViewportEvent &event) override;

	Vector3 _previousPosition;

	void mousePressEvent(SDLApp::MouseEvent &event) override;

	void mouseReleaseEvent(SDLApp::MouseEvent &event) override;

	void mouseMoveEvent(SDLApp::MouseMoveEvent &event) override;

	void mouseScrollEvent(SDLApp::MouseScrollEvent &event) override;

	void applyJoystick();
};