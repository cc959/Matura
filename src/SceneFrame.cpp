#include "SceneFrame.h"

SceneFrame::SceneFrame(const Timeline &timeline, ImGuiIntegration::Context &guiContext, Joystick &joystick) : Frame{timeline, guiContext, joystick}, Stage{}
{
}

SceneFrame::SceneFrame(string path, const Timeline &timeline, ImGuiIntegration::Context &guiContext, Joystick &joystick) : Frame{timeline, guiContext, joystick}, Stage{path}, _path{path}
{
	_player.play(_timeline.previousFrameTime());
	_player.setPlayCount(3);
}

void SceneFrame::draw3D()
{
	_player.advance(_timeline.previousFrameTime());
	if (_player.state() != Animation::State::Playing)
		applyJoystick();
	_camera->draw(_drawables);
}

bool my_tool_active = true;
float my_color[4] = {1, 1, 1, 1};

void SceneFrame::setupGUI()
{
	if (my_tool_active)
	{
		// Create a window called "My First Tool", with a menu bar.
		ImGui::Begin(_path.c_str(), &my_tool_active, ImGuiWindowFlags_MenuBar);
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open..", "Ctrl+O"))
				{ /* Do stuff */
				}
				if (ImGui::MenuItem("Save", "Ctrl+S"))
				{ /* Do stuff */
				}
				if (ImGui::MenuItem("Close", "Ctrl+W"))
				{
					my_tool_active = false;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		// Edit a color (stored as ~4 floats)
		ImGui::ColorEdit4("Color", my_color);

		// Plot some values
		const float my_values[] = {0.2f, 0.1f, 1.0f, 0.5f, 0.9f, 2.2f};
		ImGui::PlotLines("Frame Times", my_values, IM_ARRAYSIZE(my_values));

		// Display contents in a scrolling region
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "Important Stuff");
		ImGui::BeginChild("Scrolling");
		for (int n = 0; n < 50; n++)
			ImGui::Text("%04d: Some text", n);
		ImGui::EndChild();
		ImGui::End();
	}
}

// resize viewport event
void SceneFrame::viewportEvent(SDLApp::ViewportEvent &event)
{
	_camera->setViewport(event.windowSize());
}

Vector3 _previousPosition;

void SceneFrame::mousePressEvent(SDLApp::MouseEvent &event)
{
	if (event.button() == SDLApp::MouseEvent::Button::Left)
		_previousPosition = {event.position().x(), event.position().y(), 0};
}

void SceneFrame::mouseReleaseEvent(SDLApp::MouseEvent &event)
{
	if (event.button() == SDLApp::MouseEvent::Button::Left)
		_previousPosition = Vector3();
}

void SceneFrame::mouseMoveEvent(SDLApp::MouseMoveEvent &event)
{
	if (!(event.buttons() & SDLApp::MouseMoveEvent::Button::Left))
		return;

	const Vector3 currentPosition = {event.position().x(), event.position().y(), 0};
	const Vector3 diff = currentPosition - _previousPosition;

	if (_previousPosition.length() < 0.001f)
		return;

	_cameraRoot.rotateXLocal(-Math::Rad(diff.y() / 100.f));
	_cameraRoot.rotateY(-Math::Rad(diff.x() / 100.f));
	_previousPosition = currentPosition;
}

void SceneFrame::mouseScrollEvent(SDLApp::MouseScrollEvent &event)
{
	if (!event.offset().y())
		return;
	/* Move 15% of the distance back or forward */
	// if (!joystick.IsLoaded())
	_cameraObject.setTranslation(_cameraObject.translation() * (event.offset().y() > 0 ? 1 / 0.85f : 0.85f));
}

void SceneFrame::applyJoystick()
{
	if (_joystick.IsLoaded())
	{
		// _manipulator.rotateYLocal(-Math::Rad(joystick.GetAxisValue(2) * _deltaTime));
		// _manipulator.rotateX(-Math::Rad(joystick.GetAxisValue(0) * _deltaTime));

		if (_objectsByID.size() >= 15)
		{
			_objectsByID[14]->rotateX(Math::Rad(_joystick.GetAxisValue(1) * _timeline.previousFrameDuration()));
			_objectsByID[19]->rotateX(Math::Rad(_joystick.GetAxisValue(5) * _timeline.previousFrameDuration()));
			_objectsByID[6]->rotateZ(-Math::Rad(_joystick.GetAxisValue(2) * _timeline.previousFrameDuration()));
		}
		// float factor = (joystick.GetAxisValue(3) + 1.f) / 2.f;
		// _cameraObject.setTranslation(_cameraObject.translation().normalized() * pow(1.15f, factor * 10.f));
	}
}
