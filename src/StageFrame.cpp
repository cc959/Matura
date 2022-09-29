#include "StageFrame.h"

StageFrame::StageFrame(const Timeline &timeline, ImGuiIntegration::Context &guiContext, Joystick &joystick) : Frame{timeline, guiContext, joystick}, Stage{}
{
	setupShadows();
}

StageFrame::StageFrame(string path, const Timeline &timeline, ImGuiIntegration::Context &guiContext, Joystick &joystick) : Frame{timeline, guiContext, joystick}, Stage{path}, _path{path}
{
	_player.play(_timeline.previousFrameTime());
	_player.setPlayCount(3);

	setupShadows();
}

void StageFrame::setupShadows()
{
	_shadows._shadowLight.setupShadowmaps(3, _shadows._shadowMapSize);
	_shadows._shadowLight.setupSplitDistances(0.01f, 100.f, _shadows._layerSplitExponent);
	//_shadows._layerSplitExponent = 2.f;

	//_shadowCameraIndicator.setParent(&_scene);
	// new PhongDrawable(_shadowCameraIndicator, *(Stage*)this, *_meshes[0], *_materials[0], _shadowReceivers);
}
void StageFrame::addDebugLines()
{

	constexpr const Matrix4 unbiasMatrix{{2.0f, 0.0f, 0.0f, 0.0f},
										 {0.0f, 2.0f, 0.0f, 0.0f},
										 {0.0f, 0.0f, 2.0f, 0.0f},
										 {-1.0f, -1.0f, -1.0f, 1.0f}};
	_debug.reset();
	const Matrix4 imvp = (_camera->projectionMatrix() * _camera->cameraMatrix()).inverted();
	for (std::size_t layerIndex = 0; layerIndex != _shadows._shadowLight.layerCount(); ++layerIndex)
	{
		const Matrix4 layerMatrix = _shadows._shadowLight.layerMatrix(layerIndex);
		const Deg hue = layerIndex * 360.0_degf / _shadows._shadowLight.layerCount();
		_debug.addFrustum((unbiasMatrix * layerMatrix).inverted(),
						  Color3::fromHsv({hue, 1.0f, 0.5f}));
		// _debug.addFrustum(imvp,
		// 				  Color3::fromHsv({hue, 1.0f, 1.0f}),
		// 				  layerIndex == 0 ? 0 : _shadows._shadowLight.cutZ(layerIndex - 1), _shadows._shadowLight.cutZ(layerIndex));
	}
}

void StageFrame::draw3D()
{
	_player.advance(_timeline.previousFrameTime());
	if (_player.state() != Animation::State::Playing)
		applyJoystick();

	_shadows._shadowLight.setupSplitDistances(0.01f, 100.f, _shadows._layerSplitExponent);

	const Vector3 screenDirection = _shadows._shadowStaticAlignment ? Vector3::zAxis() : _cameraObject.transformation()[2].xyz();
	_shadows._shadowLight.setTarget(_lights[0].xyz(), screenDirection, *_camera);

	switch (_shadows._shadowMapFaceCullMode)
	{
	case 0:
		GL::Renderer::disable(GL::Renderer::Feature::FaceCulling);
		break;
	case 2:
		GL::Renderer::setFaceCullingMode(GL::Renderer::PolygonFacing::Front);
		break;
	}

	_shadows._shadowLight.render(_shadowCasters);

	switch (_shadows._shadowMapFaceCullMode)
	{
	case 0:
		GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
		break;
	case 2:
		GL::Renderer::setFaceCullingMode(GL::Renderer::PolygonFacing::Back);
		break;
	}

	shadowMatrices = Containers::Array<Matrix4>{Corrade::NoInit, _shadows._shadowLight.layerCount()};
	for (std::size_t layerIndex = 0; layerIndex != _shadows._shadowLight.layerCount(); ++layerIndex)
		shadowMatrices[layerIndex] = _shadows._shadowLight.layerMatrix(layerIndex);

	//_shadowCameraIndicator.setTransformation(_shadows._shadowLight._layers[0].shadowCameraMatrix);

	// _shadowReceiverShader.setShadowmapMatrices(shadowMatrices)
	// 	.setShadowmapTexture(_shadows._shadowLight.shadowTexture());

	_camera->draw(_shadowReceivers);

	_debug.draw(_camera->projectionMatrix() * _camera->cameraMatrix());
}

bool my_tool_active = true;
float my_color[4] = {1, 1, 1, 1};

void StageFrame::setupGUI()
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

		ImGui::DragFloat("Bias", &_shadows._shadowBias, 0.001, -0.01, 0.01);
		ImGui::DragFloat("Power", &_shadows._layerSplitExponent, 0.25, 0, 10);

		ImGui::Text(("Fps: " + to_string(1 / _timeline.previousFrameDuration())).c_str());

		ImGui::EndChild();
		ImGui::End();
	}
}

// resize viewport eventg
void StageFrame::viewportEvent(SDLApp::ViewportEvent &event)
{
	_camera->setViewport(event.windowSize());
}

Vector3 _previousPosition;

void StageFrame::mousePressEvent(SDLApp::MouseEvent &event)
{
	if (event.button() == SDLApp::MouseEvent::Button::Left)
		_previousPosition = {event.position().x(), event.position().y(), 0};
	if (event.button() == SDLApp::MouseEvent::Button::Right)
		addDebugLines();
}

void StageFrame::mouseReleaseEvent(SDLApp::MouseEvent &event)
{
	if (event.button() == SDLApp::MouseEvent::Button::Left)
		_previousPosition = Vector3();
}

void StageFrame::mouseMoveEvent(SDLApp::MouseMoveEvent &event)
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

void StageFrame::mouseScrollEvent(SDLApp::MouseScrollEvent &event)
{
	if (!event.offset().y())
		return;
	/* Move 15% of the distance back or forward */
	// if (!joystick.IsLoaded())
	_cameraObject.setTranslation(_cameraObject.translation() * (event.offset().y() > 0 ? 1 / 0.85f : 0.85f));
}

void StageFrame::applyJoystick()
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
