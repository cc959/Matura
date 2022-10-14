#pragma once

#include "GlobalIncludes.h"

#include "Joystick.h"
#include "PhongDrawable.h"
#include "Frame.h"
#include "StageFrame.h"
#include "ImageFrame.h"
#include "PostProcess.h"

#define FL if (_currentFrame)

class Viewer : public Platform::Application
{
public:
	Frame *_currentFrame = nullptr;
	int _currentFrameIndex;
	vector<Frame *> _frames;

	PostProcess _postProcesser;

	explicit Viewer(const Arguments &arguments) : Platform::Application{arguments, Configuration{}.setTitle("Robot Arm").setSize(Vector2i(1280, 720), Vector2(1, 1)).addWindowFlags(Configuration::WindowFlag::Resizable), GLConfiguration{}.setSampleCount(4).setSrgbCapable(true)}
	{
		_imgui = ImGuiIntegration::Context(Vector2{windowSize()} / dpiScaling(),
										   windowSize(), framebufferSize());

		joystick.Load("/dev/input/js0");

		// addFrame(new StageFrame("/home/elias/Downloads/untitled.fbx", timeline, _imgui, joystick));
		// addFrame(new StageFrame("/home/elias/Downloads/untitled2.fbx", timeline, _imgui, joystick));
		// addFrame(new StageFrame("/home/elias/Downloads/untitled3.fbx", timeline, _imgui, joystick));
		// addFrame(new StageFrame("/home/elias/Downloads/untitled4.fbx", timeline, _imgui, joystick));
		// addFrame(new StageFrame("/home/elias/Downloads/autumn tree001-coronafbx.FBX", timeline, _imgui, joystick));
		// addFrame(new StageFrame("/home/elias/Downloads/audi_r8/scene.gltf", timeline, _imgui, joystick));
		addFrame(new StageFrame("/home/elias/Downloads/untitled.fbx", timeline, _imgui, joystick));
		addFrame(new ImageFrame(Utility::Resource("Images").getRaw("Shang.png"), timeline, _imgui));

		GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add,
									   GL::Renderer::BlendEquation::Add);
		GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::SourceAlpha,
									   GL::Renderer::BlendFunction::OneMinusSourceAlpha);

		// GL::Renderer::setClearColor(Color4(1, 0, 0, 1));

		timeline.start();

		// setSwapInterval(0);
	}

private:
	int addFrame(Frame *newFrame)
	{
		_frames.push_back(newFrame);
		if (!_currentFrame)
		{
			_currentFrame = newFrame;
			_currentFrameIndex = _frames.size() - 1;
		}
		return _frames.size() - 1;
	}

	void setFrame(int frameID)
	{
		_currentFrame = _frames[frameID];
		_currentFrameIndex = frameID;
	}

	void nextFrame()
	{
		setFrame((_currentFrameIndex + 1) % _frames.size());
	}

	void prevFrame()
	{
		setFrame((_currentFrameIndex - 1 + _frames.size()) % _frames.size());
	}

	void drawEvent() override
	{

		GL::defaultFramebuffer.clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth);

		_imgui.newFrame();

		FL _currentFrame->setupGUI();

		FL _currentFrame->draw3D();


		GL::Renderer::enable(GL::Renderer::Feature::Blending);
		GL::Renderer::enable(GL::Renderer::Feature::ScissorTest);
		GL::Renderer::disable(GL::Renderer::Feature::DepthTest);
		GL::Renderer::disable(GL::Renderer::Feature::FaceCulling);

		_postProcesser.draw();

		_imgui.updateApplicationCursor(*this);

		_imgui.drawFrame();

		GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
		GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
		GL::Renderer::disable(GL::Renderer::Feature::ScissorTest);
		GL::Renderer::disable(GL::Renderer::Feature::Blending);

		swapBuffers();

		redraw();

		timeline.nextFrame();
	}

	// resize viewport event
	void viewportEvent(ViewportEvent &event) override
	{
		GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});

		_imgui.relayout(Vector2{event.windowSize()} / event.dpiScaling(),
						event.windowSize(), event.framebufferSize());

		FL _currentFrame->viewportEvent(event);
	}

	void mousePressEvent(MouseEvent &event) override
	{
		if (_imgui.handleMousePressEvent(event))
			return;

		FL _currentFrame->mousePressEvent(event);
	}

	void mouseReleaseEvent(MouseEvent &event) override
	{
		if (_imgui.handleMouseReleaseEvent(event))
			return;

		FL _currentFrame->mouseReleaseEvent(event);
	}

	void mouseMoveEvent(MouseMoveEvent &event) override
	{
		if (_imgui.handleMouseMoveEvent(event))
			return;

		FL _currentFrame->mouseMoveEvent(event);
	}

	void mouseScrollEvent(MouseScrollEvent &event) override
	{
		if (_imgui.handleMouseScrollEvent(event))
			return;

		FL _currentFrame->mouseScrollEvent(event);
	}

	void keyPressEvent(KeyEvent &event) override
	{
		if (event.key() == SDLApp::KeyEvent::Key::PageUp)
			nextFrame();
		if (event.key() == SDLApp::KeyEvent::Key::PageDown)
			prevFrame();

		if (_imgui.handleKeyPressEvent(event))
			return;

		FL _currentFrame->keyPressEvent(event);
	}

	void keyReleaseEvent(KeyEvent &event) override
	{
		if (_imgui.handleKeyReleaseEvent(event))
			return;

		FL _currentFrame->keyReleaseEvent(event);
	}

	ImGuiIntegration::Context _imgui{Magnum::NoCreate};

	Timeline timeline;
	Joystick joystick;
};