#include "Frame.h"

Frame::Frame(const Timeline &timeline, ImGuiIntegration::Context &guiContext) : _timeline{timeline}, _imgui{guiContext} {}

void Frame::draw3D() {}

void Frame::setupGUI() {}

void Frame::Enter() {}
void Frame::Leave() {}

void Frame::viewportEvent(SDLApp::ViewportEvent &event) {}
void Frame::mousePressEvent(SDLApp::MouseEvent &event) {}
void Frame::mouseReleaseEvent(SDLApp::MouseEvent &event) {}
void Frame::mouseMoveEvent(SDLApp::MouseMoveEvent &event) {}
void Frame::mouseScrollEvent(SDLApp::MouseScrollEvent &event) {}
void Frame::keyPressEvent(SDLApp::KeyEvent &event) {}
void Frame::keyReleaseEvent(SDLApp::KeyEvent &event) {}

Frame::~Frame() = default;