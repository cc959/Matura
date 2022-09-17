#include "Frame.h"

Frame::Frame(const Timeline &timeline, ImGuiIntegration::Context &guiContext, Joystick &joystick) : _timeline{timeline}, _imgui{guiContext}, _joystick{joystick} {}

void Frame::draw3D() {}

void Frame::setupGUI() {}

void Frame::viewportEvent(SDLApp::ViewportEvent &event) {}
void Frame::mousePressEvent(SDLApp::MouseEvent &event) {}
void Frame::mouseReleaseEvent(SDLApp::MouseEvent &event) {}
void Frame::mouseMoveEvent(SDLApp::MouseMoveEvent &event) {}
void Frame::mouseScrollEvent(SDLApp::MouseScrollEvent &event) {}

Frame::~Frame() = default;