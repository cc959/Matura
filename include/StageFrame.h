#pragma once

#include "GlobalIncludes.h"

#include "Joystick.h"
#include "Frame.h"
#include "Stage.h"

class StageFrame : public Frame, public Stage
{
	Stage _scene;

	string _path;

public:
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