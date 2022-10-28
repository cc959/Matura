#include "StageFrame.h"

StageFrame::StageFrame(const Timeline &timeline, ImGuiIntegration::Context &guiContext, Joystick &joystick) : Frame{timeline, guiContext}, Stage{}, _joystick{joystick}, _fb{GL::defaultFramebuffer.viewport()}
{
	setup();
}

StageFrame::StageFrame(string path, const Timeline &timeline, ImGuiIntegration::Context &guiContext, Joystick &joystick) : Frame{timeline, guiContext}, Stage{path}, _path{path}, _joystick{joystick}, _fb{GL::defaultFramebuffer.viewport()}
{
	_player.play(_timeline.previousFrameTime());
	pauseTime = _player.elapsed(_timeline.previousFrameTime()).second;

	_player.setPlayCount(0);

	setup();
}

void StageFrame::Enter()
{
	_player.play(_timeline.previousFrameTime() - pauseTime);
	locator.Start(0);
}

void StageFrame::Leave()
{
	pauseTime = _player.elapsed(_timeline.previousFrameTime()).second;

	locator.Stop();
}

void StageFrame::setup()
{
	_cacheCamera.setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
		.setProjectionMatrix(Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.01f, 100.0f))
		.setViewport(GL::defaultFramebuffer.viewport().size());
	_shadows._shadowLight.setupShadowmaps(3, _shadows._shadowMapSize);
	_shadows._shadowLight.setupSplitDistances(0.01f, 100.f, _shadows._layerSplitExponent);

	_cameraRoot = new Object3D();
	_cameraObject = new Object3D();
	_cameraRoot->setParent(&_scene);
	_cameraObject->setParent(_cameraRoot);
	_cameraObject->setTranslation(Vector3(0, 3, 5));
	_cameraObject->setRotation(Quaternion::fromMatrix(Matrix4::lookAt(Vector3(0, 3, 5), Vector3(0, 0, 0), Vector3::yAxis(1)).rotation()));

    addObject(_cameraObject, "Turntable camera");
    addObject(_cameraRoot, "Turntable camera root");


    (*(_activeCamera = new SceneGraph::Camera3D{*_cameraObject}))
		.setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
		.setProjectionMatrix(Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.01f, 1000.0f))
		.setViewport(GL::defaultFramebuffer.viewport().size());

	_cameras.push_back(_activeCamera);

    setupRenderBuffers();

}

void StageFrame::setupRenderBuffers() {
    _fb = GL::Framebuffer{GL::defaultFramebuffer.viewport()};

    _color.setStorage(GL::RenderbufferFormat::RGBA8, GL::defaultFramebuffer.viewport().size());
    _objectId.setStorage(GL::RenderbufferFormat::R32UI, GL::defaultFramebuffer.viewport().size());
    _depth.setStorage(GL::RenderbufferFormat::DepthComponent24, GL::defaultFramebuffer.viewport().size());
    _fb.attachRenderbuffer(GL::Framebuffer::ColorAttachment{0}, _color)
            .attachRenderbuffer(GL::Framebuffer::ColorAttachment{1}, _objectId)
            .attachRenderbuffer(GL::Framebuffer::BufferAttachment::Depth, _depth)
            .mapForDraw({{Shaders::PhongGL::ColorOutput, GL::Framebuffer::ColorAttachment{0}},
                         {Shaders::PhongGL::ObjectIdOutput, GL::Framebuffer::ColorAttachment{1}}});
    CORRADE_INTERNAL_ASSERT(_fb.checkStatus(GL::FramebufferTarget::Draw) == GL::Framebuffer::Status::Complete);
}

void StageFrame::addDebugLines()
{

	constexpr const Matrix4 unbiasMatrix{{2.0f, 0.0f, 0.0f, 0.0f},
										 {0.0f, 2.0f, 0.0f, 0.0f},
										 {0.0f, 0.0f, 2.0f, 0.0f},
										 {-1.0f, -1.0f, -1.0f, 1.0f}};
	_debug.reset();
	const Matrix4 imvp = (_cacheCamera.projectionMatrix() * _cacheCamera.cameraMatrix()).inverted();
	for (std::size_t layerIndex = 0; layerIndex != _shadows._shadowLight.layerCount(); ++layerIndex)
	{
		const Matrix4 layerMatrix = _shadows._shadowLight.layerMatrix(layerIndex);
		const Deg hue = layerIndex * 360.0_degf / _shadows._shadowLight.layerCount();
		_debug.addFrustum((unbiasMatrix * layerMatrix).inverted(),
						  Color3::fromHsv({hue, 1.0f, 0.5f}));
		_debug.addFrustum(imvp,
						  Color3::fromHsv({hue, 1.0f, 1.0f}),
						  layerIndex == 0 ? 0 : _shadows._shadowLight.cutZ(layerIndex - 1), _shadows._shadowLight.cutZ(layerIndex));
	}
}

void StageFrame::addDebugLines(const Matrix4 &transformation)
{
	_debug.addLine(transformation * Vector4{0, 0, 0, 1}, transformation * Vector4{1, 0, 0, 1}, {1, 0, 0});
	_debug.addLine(transformation * Vector4{0, 0, 0, 1}, transformation * Vector4{0, 1, 0, 1}, {0, 1, 0});
	_debug.addLine(transformation * Vector4{0, 0, 0, 1}, transformation * Vector4{0, 0, 1, 1}, {0, 0, 1});
}

void StageFrame::draw3D()
{
    _fb.clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth);


    _player.advance(_timeline.previousFrameTime());

	if (_player.state() != Animation::State::Playing)
		applyJoystick();

    _debug.reset();

	_shadows._shadowLight.setupSplitDistances(0.01f, 100.f, _shadows._layerSplitExponent);

	Matrix4 transformation = locator.getTransformation();
    addDebugLines(transformation);

    getObject("Camera")->setTransformation(transformation);

	if (_setTarget)
	{
		_chacheCameraObject.setTransformation(_activeCamera->object().absoluteTransformationMatrix());

		_debug.addLine({10, 0, 0}, Color3{0, 0, 1}, {0, 20, 0}, Color3{1, 0, 0}, false, "20m");

		for (auto camera : _cameras)
		{
			if (camera == _activeCamera)
				continue;
			_debug.addFrustum((camera->projectionMatrix() * camera->cameraMatrix()).inverted(), Color3{0, 1, 0});
			_debug.addPoint(camera->object().absoluteTransformationMatrix() * Vector4(0, 0, 0, 1), Color3{0, 1, 0}, false, "Camera");
		}
	}
	else
	{
		addDebugLines();
	}

	// for (Object3D* object : _objectByID) {
	// 	addDebugLines(object->absoluteTransformationMatrix());
	// }

	// addDebugLines(_objectByName["Camera manipulator"]->absoluteTransformationMatrix());

	// addDebugLines(Matrix4());

	const Vector3 screenDirection = _shadows._shadowStaticAlignment ? Vector3::zAxis() : _chacheCameraObject.transformation()[2].xyz();
	_shadows._shadowLight.setTarget(_lights.size() ? _lights[0].xyz() : Vector3{3, 2, 1}, screenDirection, _cacheCamera);
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

    _fb.bind();
    _activeCamera->draw(_selectables);

    GL::defaultFramebuffer.bind();
	_activeCamera->draw(_shadowReceivers);
	_debug.draw(*_activeCamera);
}

bool my_tool_active = true;
float my_color[4] = {1, 1, 1, 1};

void StageFrame::setupGUI()
{
	if (my_tool_active)
	{
		// Create a window called "My First Tool", with a menu bar.
		ImGui::Begin(_path.c_str(), &my_tool_active, 0);

		ImGui::SliderFloat("Bias", &_shadows._shadowBias, 0, 0.01, "%.5f");
		ImGui::SliderFloat("Power", &_shadows._layerSplitExponent, 0, 10);

		ImGui::Checkbox("Debug", &_shadows._debug);

		ImGui::Text(("Fps: " + to_string(1 / _timeline.previousFrameDuration())).c_str());

        std::function<void(Object3D*)> recursiveSceneTree;

        recursiveSceneTree = [&](Object3D* object) {
            string name = getName(object);
            if (name.size() && ImGui::TreeNode(name.c_str())) {
                if (ImGui::TreeNode("Transform")) {
                    Vector3 translation = object->translation();
                    Vector3 rotation = Vector3(object->rotation().toEuler());
                    Vector3 scale = object->scaling();

                    rotation *= 180.0 / M_PI;

                    ImGui::DragFloat3("Translation", translation.data(), 0.1);
                    ImGui::DragFloat3("Rotation", rotation.data());
                    ImGui::DragFloat3("Scale", scale.data(), 0.1);

                    rotation *= M_PI / 180.0;


                    object->setTranslation(translation);
                    object->setRotation(
                            Quaternion::rotation(Rad(rotation.z()), Vector3::zAxis()) *
                            Quaternion::rotation(Rad(rotation.y()), Vector3::yAxis()) *
                            Quaternion::rotation(Rad(rotation.x()), Vector3::xAxis())
                            );
                    object->setScaling(scale);
                }

                for (auto& child : object->children()) {
                    recursiveSceneTree(&child);
                }
                ImGui::TreePop();
            }
        };

        if (ImGui::TreeNode("Scene")) {
            for (auto& child : _scene.children()) {
                recursiveSceneTree(&child);
            }
            ImGui::TreePop();
        }


		ImGui::End();
	}
}

// resize viewport eventg
void StageFrame::viewportEvent(SDLApp::ViewportEvent &event)
{
	for (SceneGraph::Camera3D *_camera : _cameras)
		_camera->setViewport(event.windowSize());

    setupRenderBuffers();
}

Vector3 _previousPosition;

void StageFrame::mousePressEvent(SDLApp::MouseEvent &event)
{
	if (event.button() == SDLApp::MouseEvent::Button::Left)
		_previousPosition = {event.position().x(), event.position().y(), 0};
	if (event.button() == SDLApp::MouseEvent::Button::Right)
	{
		if (_setTarget)
		{
			_setTarget = false;
		}
		else
		{
			_debug.reset();
			_setTarget = true;
		}
	}
}

void StageFrame::mouseReleaseEvent(SDLApp::MouseEvent &event)
{
	if (event.button() == SDLApp::MouseEvent::Button::Left) {
        const Vector2i position = event.position();

        if (!_moved) {

            const Vector2i fbPosition{position.x(), GL::defaultFramebuffer.viewport().sizeY() - position.y() - 1};

            /* Read object ID at given click position, and then switch to the color
               attachment again so drawEvent() blits correct buffer */
            _fb.mapForRead(GL::Framebuffer::ColorAttachment{1});
            Image2D data = _fb.read(
                    Range2Di::fromSize(fbPosition, {1, 1}),
                    {PixelFormat::R32UI});
            _fb.mapForRead(GL::Framebuffer::ColorAttachment{0});

            /* Highlight object under mouse and deselect all other */
            UnsignedInt id = data.pixels<UnsignedInt>()[0][0];

            if (_selected.count(id)) {
                if (_shift)
                    _selected.erase(id);
                else {
                    bool addAgain = _selected.size() > 1;
                    _selected.clear();
                    if (addAgain)
                        _selected.insert(id);
                }
            } else {
                if (!_shift)
                    _selected.clear();
                _selected.insert(id);
            }
        }

        _previousPosition = Vector3();

    }

    _moved = false;
}

void StageFrame::mouseMoveEvent(SDLApp::MouseMoveEvent &event)
{
	if (!(event.buttons() & SDLApp::MouseMoveEvent::Button::Left))
		return;

	const Vector3 currentPosition = {event.position().x(), event.position().y(), 0};
	const Vector3 diff = currentPosition - _previousPosition;

	if (_previousPosition.length() < 0.001f)
		return;

	_cameraRoot->rotateXLocal(-Math::Rad(diff.y() / 100.f));
	_cameraRoot->rotateY(-Math::Rad(diff.x() / 100.f));
	_previousPosition = currentPosition;

    _moved = true;
}

void StageFrame::mouseScrollEvent(SDLApp::MouseScrollEvent &event)
{
	if (!event.offset().y())
		return;
	/* Move 15% of the distance back or forward */
	// if (!joystick.IsLoaded())
	_cameraObject->setTranslation(_cameraObject->translation() * (event.offset().y() > 0 ? 1 / 0.85f : 0.85f));
}

void StageFrame::keyPressEvent(SDLApp::KeyEvent &event)
{
	if (event.key() == SDLApp::KeyEvent::Key::Left)
	{
		_activeCamera = _cameras[(getCameraID(_activeCamera) - 1 + _cameras.size()) % _cameras.size()];
	}
	if (event.key() == SDLApp::KeyEvent::Key::Right)
	{
		_activeCamera = _cameras[(getCameraID(_activeCamera) + 1 + _cameras.size()) % _cameras.size()];
	}
    if (event.key() == SDLApp::KeyEvent::Key::LeftShift || event.key() == SDLApp::KeyEvent::Key::RightShift) {
        _shift = true;
    }
}
void StageFrame::keyReleaseEvent(SDLApp::KeyEvent &event) {
    if (event.key() == SDLApp::KeyEvent::Key::LeftShift || event.key() == SDLApp::KeyEvent::Key::RightShift) {
        _shift = false;
    }
}

void StageFrame::applyJoystick()
{
	if (_joystick.IsLoaded())
	{
		// _manipulator.rotateYLocal(-Math::Rad(joystick.GetAxisValue(2) * _deltaTime));
		// _manipulator.rotateX(-Math::Rad(joystick.GetAxisValue(0) * _deltaTime));

		if (objectCount() >= 15)
		{
			getObject(14)->rotateX(Math::Rad(_joystick.GetAxisValue(1) * _timeline.previousFrameDuration()));
			getObject(19)->rotateX(Math::Rad(_joystick.GetAxisValue(5) * _timeline.previousFrameDuration()));
			getObject(6)->rotateZ(-Math::Rad(_joystick.GetAxisValue(2) * _timeline.previousFrameDuration()));
		}
		// float factor = (joystick.GetAxisValue(3) + 1.f) / 2.f;
		// _cameraObject.setTranslation(_cameraObject.translation().normalized() * pow(1.15f, factor * 10.f));
	}
}
