#pragma once

#include "GlobalIncludes.h"
#include "Joystick.h"
#include "PhongDrawable.h"

class Frame
{
public:
	Frame(const Timeline &timeline, ImGuiIntegration::Context &guiContext, Joystick &joystick);

	const Timeline &_timeline;
	ImGuiIntegration::Context &_imgui;

	Joystick &_joystick;

	virtual void draw3D();

	virtual void setupGUI();

	// resize viewport event
	virtual void viewportEvent(SDLApp::ViewportEvent &event);
	virtual void mousePressEvent(SDLApp::MouseEvent &event);
	virtual void mouseReleaseEvent(SDLApp::MouseEvent &event);
	virtual void mouseMoveEvent(SDLApp::MouseMoveEvent &event);
	virtual void mouseScrollEvent(SDLApp::MouseScrollEvent &event);

	virtual ~Frame();
};