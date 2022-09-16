// #include "opencv2/imgproc.hpp"
// #include "opencv2/highgui.hpp"
// #include "opencv2/videoio.hpp"
// #include "opencv2/features2d.hpp"
// #include "glm/glm.hpp"

// #include <bits/stdc++.h>
// using namespace cv;
// const int max_value_H = 360 / 2;
// const int max_value = 255;
// const String window_capture_name = "Video Capture";
// const String window_detection_name = "Object Detection";
// int low_H = 110, low_S = 148, low_V = 139;
// int high_H = 130, high_S = max_value, high_V = max_value;
// static void on_low_H_thresh_trackbar(int, void *)
// {
// 	low_H = min(high_H - 1, low_H);
// 	setTrackbarPos("Low H", window_detection_name, low_H);
// }
// static void on_high_H_thresh_trackbar(int, void *)
// {
// 	high_H = max(high_H, low_H + 1);
// 	setTrackbarPos("High H", window_detection_name, high_H);
// }
// static void on_low_S_thresh_trackbar(int, void *)
// {
// 	low_S = min(high_S - 1, low_S);
// 	setTrackbarPos("Low S", window_detection_name, low_S);
// }
// static void on_high_S_thresh_trackbar(int, void *)
// {
// 	high_S = max(high_S, low_S + 1);
// 	setTrackbarPos("High S", window_detection_name, high_S);
// }
// static void on_low_V_thresh_trackbar(int, void *)
// {
// 	low_V = min(high_V - 1, low_V);
// 	setTrackbarPos("Low V", window_detection_name, low_V);
// }
// static void on_high_V_thresh_trackbar(int, void *)
// {
// 	high_V = max(high_V, low_V + 1);
// 	setTrackbarPos("High V", window_detection_name, high_V);
// }
// int main()
// {
// 	VideoCapture cap("/dev/video2");

// 	namedWindow(window_capture_name);
// 	namedWindow(window_detection_name);
// 	// Trackbars to set thresholds for HSV values
// 	createTrackbar("Low H", window_detection_name, &low_H, max_value_H, on_low_H_thresh_trackbar);
// 	createTrackbar("High H", window_detection_name, &high_H, max_value_H, on_high_H_thresh_trackbar);
// 	createTrackbar("Low S", window_detection_name, &low_S, max_value, on_low_S_thresh_trackbar);
// 	createTrackbar("High S", window_detection_name, &high_S, max_value, on_high_S_thresh_trackbar);
// 	createTrackbar("Low V", window_detection_name, &low_V, max_value, on_low_V_thresh_trackbar);
// 	createTrackbar("High V", window_detection_name, &high_V, max_value, on_high_V_thresh_trackbar);

// 	SimpleBlobDetector::Params params;
// 	memset(&params, 0, sizeof(SimpleBlobDetector::Params));

// 	params.minThreshold = 250;
// 	params.maxThreshold = 256;
// 	params.thresholdStep = 1;
// 	params.minRepeatability = 1;

// 	params.filterByArea = true;
// 	params.minArea = 100;
// 	params.maxArea = 1000000;

// 	params.filterByColor = true;
// 	params.blobColor = 255;

// 	params.filterByCircularity = true;
// 	params.minCircularity = 0.3;
// 	params.maxCircularity = 1;

// 	// params.filterByCircularity = true;
// 	// params.minCircularity = 0.8f;

// 	Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);

// 	Mat frame, frame_HSV, frame_threshold;
// 	while (true)
// 	{
// 		cap >> frame;
// 		if (frame.empty())
// 		{
// 			break;
// 		}
// 		// Convert from BGR to HSV colorspace
// 		cvtColor(frame, frame_HSV, COLOR_RGB2HSV);
// 		// Detect the object based on HSV Range Values
// 		inRange(frame_HSV, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), frame_threshold);
// 		// Show the frames

// 		std::vector<KeyPoint> points;
// 		detector->detect(frame_threshold, points);

// 		for (auto pnt : points)
// 		{
// 			std::cout << pnt.pt.x << " " << pnt.pt.y << " " << pnt.size << "\n";
// 		}

// 		Mat im_with_keypoints;
// 		drawKeypoints(frame_threshold, points, frame_threshold, Scalar(255, 0, 0), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
// 		drawKeypoints(frame, points, im_with_keypoints, Scalar(255, 0, 0), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

// 		imshow(window_capture_name, im_with_keypoints);
// 		imshow(window_detection_name, frame_threshold);

// 		char key = (char)waitKey(1);
// 		if (key == 'q' || key == 27)
// 		{
// 			break;
// 		}
// 	}
// 	return 0;
// }

// #include <bits/stdc++.h>

// using namespace std;

// int main()
// {

// 	fstream file;

// 	file.open("/dev/ttyACM0", fstream::out);
// 	sleep(5);

// 	while (true)
// 	{
// 		int32_t speed, shoulder, elbow;
// 		cin >> speed >> shoulder >> elbow;

// 		unsigned char out[3][4];
// 		(*(int *)out[0]) = speed;
// 		(*(int *)out[1]) = shoulder;
// 		(*(int *)out[2]) = elbow;
// 		file.write((char *)out, 12);

// 		file.flush();
// 	}

// 	sleep(5);

// 	file.close();
// 	return 0;
// }

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/DebugStl.h>
#include <Magnum/ImageView.h>
#include <Magnum/Mesh.h>
#include <Magnum/PixelFormat.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/Math/Color.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/TranslationRotationScalingTransformation3D.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/SceneGraph/Object.h>
#include <Magnum/Shaders/PhongGL.h>
#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/ImageData.h>
#include <Magnum/Trade/MeshData.h>
#include <Magnum/Trade/PhongMaterialData.h>
#include <Magnum/Trade/SceneData.h>
#include <Magnum/Trade/TextureData.h>
#include <Magnum/Trade/AnimationData.h>
#include <Magnum/Timeline.h>
#include <Magnum/Image.h>
#include <Magnum/Animation/Animation.h>
#include <Magnum/Animation/Player.h>
#include <Magnum/GL/PixelFormat.h>
#include <Magnum/Math/Quaternion.h>
#include <MagnumPlugins/AnySceneImporter/AnySceneImporter.h>
#include <MagnumPlugins/GltfImporter/GltfImporter.h>
#include <Magnum/ImGuiIntegration/Context.hpp>

#include <bits/stdc++.h>
#include "Joystick.h"
#include <PhongDrawable.h>

//#include <TextureShader.h>

using namespace Magnum;

using namespace Containers;
using namespace Math::Literals;

typedef SceneGraph::Object<SceneGraph::TranslationRotationScalingTransformation3D> Object3D;
typedef SceneGraph::Scene<SceneGraph::TranslationRotationScalingTransformation3D> Scene3D;

class ViewerExample : public Platform::Application
{
public:
	explicit ViewerExample(const Arguments &arguments) : Platform::Application{arguments, Configuration{}.setTitle("Robot Arm").setSize(Vector2i(1280, 720), Vector2(1, 1)).addWindowFlags(Configuration::WindowFlag::Resizable), GLConfiguration{}.setSampleCount(4).setSrgbCapable(true)}
	{
		// Utility::Arguments args;
		// args.addArgument("file");
		// args.setHelp("file", "file to load");
		// args.addOption("importer", "AnySceneImporter");
		// args.setHelp("importer", "importer plugin to use");
		// args.addSkippedPrefix("magnum", "engine-specific options");
		// args.setGlobalHelp("Displays a 3D scene file provided on command line.");
		// args.parse(arguments.argc, arguments.argv);

		_imgui = ImGuiIntegration::Context(Vector2{windowSize()} / dpiScaling(),
										   windowSize(), framebufferSize());

		joystick.Load("/dev/input/js0");

		_cameraRoot.setParent(&_scene);
		_cameraObject.setParent(&_cameraRoot);
		_cameraObject.setTranslation(Vector3(0, 3, 5));
		_cameraObject.setRotation(Quaternion::fromMatrix(Matrix4::lookAt(Vector3(0, 3, 5), Vector3(0, 0, 0), Vector3::yAxis(1)).rotation()));

		(*(_camera = new SceneGraph::Camera3D{_cameraObject}))
			.setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
			.setProjectionMatrix(Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.01f, 1000.0f))
			.setViewport(GL::defaultFramebuffer.viewport().size());

		_manipulator.setParent(&_scene);

		GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add,
									   GL::Renderer::BlendEquation::Add);
		GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::SourceAlpha,
									   GL::Renderer::BlendFunction::OneMinusSourceAlpha);

		std::string path = "/home/elias/Downloads/untitled.fbx";
		// std::getline(std::cin, path);

		PluginManager::Manager<Trade::AbstractImporter> manager;
		Containers::Pointer<Trade::AbstractImporter> importer = manager.loadAndInstantiate("AnySceneImporter");
		// Magnum::Trade::AnySceneImporter importer(manager);
		//  manager.loadAndInstantiate(args.value("importer"));

		// if (!importer || !  importer->openFile(args.value("file")))
		if (!importer->openFile(path))
			std::exit(1);

		_textures = Containers::Array<Optional<GL::Texture2D>>{
			importer->textureCount()};
		for (UnsignedInt i = 0; i != importer->textureCount(); ++i)
		{
			Optional<Trade::TextureData> textureData =
				importer->texture(i);
			if (!textureData || textureData->type() != Trade::TextureType::Texture2D)
			{
				Warning{} << "Cannot load texture" << i
						  << importer->textureName(i);
				continue;
			}

			Optional<Trade::ImageData2D> imageData =
				importer->image2D(textureData->image());

			if (!imageData /*|| !imageData->isCompressed()*/)
			{
				Warning{} << "Cannot load image" << textureData->image()
						  << importer->image2DName(textureData->image());
				continue;
			}
			else
			{
				Debug{} << "Loaded image" << importer->image2DName(textureData->image());
			}

			(*(_textures[i] = GL::Texture2D{}))
				.setMagnificationFilter(textureData->magnificationFilter())
				.setMinificationFilter(textureData->minificationFilter(),
									   textureData->mipmapFilter())
				.setWrapping(GL::SamplerWrapping::Repeat)
				.setStorage(Math::log2(imageData->size().max()) + 1,
							GL::textureFormat(imageData->format()), imageData->size())
				.setSubImage(0, {}, *imageData)
				.generateMipmap();

			Debug{} << imageData->format();
		}

		_materials = Containers::Array<Optional<Trade::PhongMaterialData>>{importer->materialCount()};

		for (UnsignedInt i = 0; i != importer->materialCount(); ++i)
		{
			Optional<Trade::MaterialData> materialData;
			if (!(materialData = importer->material(i)))
			{
				Warning{} << "Cannot load material" << i
						  << importer->materialName(i);
				continue;
			}

			_materials[i] = std::move(*materialData).as<Trade::PhongMaterialData>();
		}

		_meshes = Containers::Array<Optional<GL::Mesh>>{
			importer->meshCount()};
		for (UnsignedInt i = 0; i != importer->meshCount(); ++i)
		{
			Optional<Trade::MeshData> meshData;
			if (!(meshData = importer->mesh(i)))
			{
				Warning{} << "Cannot load mesh" << i << importer->meshName(i);
				continue;
			}

			for (int i = 0; i < meshData->attributeCount(); i++)
				Debug{} << meshData->attributeName(i);

			MeshTools::CompileFlags flags;
			if (!meshData->hasAttribute(Trade::MeshAttribute::Normal))
				flags |= MeshTools::CompileFlag::GenerateFlatNormals;
			_meshes[i] = MeshTools::compile(*meshData, flags);
		}

		if (importer->defaultScene() == -1)
		{
			Debug{} << "File has no scene!";
			// if (!_meshes.isEmpty() && _meshes[0])
			// 	new ColoredDrawable{_manipulator, _coloredShader, *_meshes[0],
			// 						0xffffff_rgbf, _drawables};
		}
		else
		{
			Optional<Trade::SceneData> scene;
			if (!(scene = importer->scene(importer->defaultScene())) || !scene->is3D() || !scene->hasField(Trade::SceneField::Parent) || !scene->hasField(Trade::SceneField::Mesh))
			{
				Fatal{} << "Cannot load scene" << importer->defaultScene() << importer->sceneName(importer->defaultScene());
			}

			_objects = Containers::Array<Object3D *>{std::size_t(scene->mappingBound())};

			Containers::Array<Containers::Pair<UnsignedInt, Int>> parents = scene->parentsAsArray(); // object index, parent

			Debug{} << parents;

			for (const Containers::Pair<UnsignedInt, Int> &parent : parents) // instantiate objects
				_objects[parent.first()] = new Object3D{};

			for (const Containers::Pair<UnsignedInt, Int> &parent : parents) // set parents of objects
				_objects[parent.first()]->setParent(parent.second() == -1 ? &_manipulator : _objects[parent.second()]);

			for (const Containers::Pair<UnsignedInt, Matrix4> &transformation : scene->transformations3DAsArray()) // object index, transformations
			{
				if (Object3D *object = _objects[transformation.first()])
					object->setTransformation(transformation.second());
			}

			for (const Containers::Pair<UnsignedInt, Containers::Pair<UnsignedInt, Int>> &
					 meshMaterial : scene->meshesMaterialsAsArray())
			{
				Object3D *object = _objects[meshMaterial.first()];				   // object to assign material to
				Optional<GL::Mesh> &mesh = _meshes[meshMaterial.second().first()]; // mesh to assign material to
				if (!object || !mesh)
					continue;

				Int materialId = meshMaterial.second().second();

				/* Material not available / not loaded, use a default material */
				if (materialId == -1 || !_materials[materialId])
				{
					Debug{} << "Object has no material";
					// new ColoredDrawable{*object, _coloredShader, *mesh, 0xffffff_rgbf,
					// 					_drawables};
				}
				else
				{
					/* Color-only material */
					new PhongDrawable{*object, *mesh, _textures, (*_materials[materialId]), _drawables};
				}
			}
		}

		_animationData = Containers::Array<Optional<Trade::AnimationData>>(importer->animationCount());

		for (int j = 0; j < importer->animationCount(); j++)
		{
			Debug{} << importer->animationName(j);
			_animationData[j] = importer->animation(j);

			auto &data = _animationData[j];

			if (!data)
			{
				Warning{} << "Unable to import animation " << importer->animationName(j);
				continue;
			}

			for (UnsignedInt i = 0; i != data->trackCount(); ++i)
			{
				Object3D &animatedObject = *_objects[data->trackTarget(i)];
				Debug{} << data->trackTarget(i) << ":" << data->trackTargetType(i);
				if (data->trackTargetType(i) == Trade::AnimationTrackTargetType::Translation3D)
				{
					const auto callback = [](Float, const Vector3 &translation, Object3D &object)
					{
						object.setTranslation(translation);
					};
					CORRADE_INTERNAL_ASSERT(data->trackType(i) == Trade::AnimationTrackType::Vector3);
					player.addWithCallback(data->track<Vector3>(i), callback, animatedObject);
					// Debug{} << data->track<Vector3>(i).values();
				}

				if (data->trackTargetType(i) == Trade::AnimationTrackTargetType::Rotation3D)
				{
					const auto callback = [](Float, const Quaternion &rotation, Object3D &object)
					{
						object.setRotation(rotation);
					};
					assert(data->trackType(i) == Trade::AnimationTrackType::Quaternion);
					player.addWithCallback(data->track<Quaternion>(i), callback, animatedObject);

					// Debug{} << data->track<Quaternion>(i).values();
				}

				if (data->trackTargetType(i) == Trade::AnimationTrackTargetType::Scaling3D)
				{
					const auto callback = [](Float, const Vector3 &scaling, Object3D &object)
					{
						object.setScaling(scaling);
					};
					CORRADE_INTERNAL_ASSERT(data->trackType(i) ==
											Trade::AnimationTrackType::Vector3);
					player.addWithCallback(data->track<Vector3>(i), callback, animatedObject);
				}
			}
		}

		setSwapInterval(0);

		timeline.start();
		// player.setPlayCount(0);
		// player.play(timeline.previousFrameTime());
		// player.setDuration(Range1D{0, player.duration().max()});
	}

private:
	void drawEvent() override
	{

		_deltaTime = timeline.previousFrameDuration();

		GL::defaultFramebuffer.clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth);

		// player.advance(timeline.previousFrameTime());

		applyJoystick();

		_camera->draw(_drawables);

		_imgui.newFrame();

		ImGui::ShowDemoWindow();

		_imgui.updateApplicationCursor(*this);

		GL::Renderer::enable(GL::Renderer::Feature::Blending);
		GL::Renderer::enable(GL::Renderer::Feature::ScissorTest);
		GL::Renderer::disable(GL::Renderer::Feature::DepthTest);
		GL::Renderer::disable(GL::Renderer::Feature::FaceCulling);

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
		_camera->setViewport(event.windowSize());

		_imgui.relayout(Vector2{event.windowSize()} / event.dpiScaling(),
						event.windowSize(), event.framebufferSize());
	}

	void mousePressEvent(MouseEvent &event) override
	{
		if (_imgui.handleMousePressEvent(event))
			return;

		if (event.button() == MouseEvent::Button::Left)
			_previousPosition = {event.position().x(), event.position().y(), 0};
	}

	void mouseReleaseEvent(MouseEvent &event) override
	{
		if (_imgui.handleMouseReleaseEvent(event))
			return;

		if (event.button() == MouseEvent::Button::Left)
			_previousPosition = Vector3();
	}

	void mouseMoveEvent(MouseMoveEvent &event) override
	{
		if (_imgui.handleMouseMoveEvent(event))
			return;

		if (!(event.buttons() & MouseMoveEvent::Button::Left))
			return;

		const Vector3 currentPosition = {event.position().x(), event.position().y(), 0};
		const Vector3 diff = currentPosition - _previousPosition;

		if (_previousPosition.length() < 0.001f)
			return;

		// if (!joystick.IsLoaded())
		//{
		_cameraRoot.rotateXLocal(-Math::Rad(diff.y() / 100.f));
		_cameraRoot.rotateY(-Math::Rad(diff.x() / 100.f));
		//}
		_previousPosition = currentPosition;
	}

	void mouseScrollEvent(MouseScrollEvent &event) override
	{
		// if (_imgui.handleMouseScrollEvent(event))
		// 	return;

		if (!event.offset().y())
			return;
		/* Move 15% of the distance back or forward */
		// if (!joystick.IsLoaded())
		_cameraObject.setTranslation(_cameraObject.translation() * (event.offset().y() > 0 ? 1 / 0.85f : 0.85f));
	}

	Vector3 positionOnSphere(const Vector2i &position) const
	{
		const Vector2 positionNormalized = Vector2{position} / Vector2{_camera->viewport()} - Vector2{0.5f};
		const Float length = positionNormalized.length();
		const Vector3 result(length > 1.0f ? Vector3(positionNormalized, 0.0f) : Vector3(positionNormalized, 1.0f - length));
		return (result * Vector3::yScale(-1.0f)).normalized();
	}

	void applyJoystick()
	{
		if (joystick.IsLoaded())
		{
			// _manipulator.rotateYLocal(-Math::Rad(joystick.GetAxisValue(2) * _deltaTime));
			// _manipulator.rotateX(-Math::Rad(joystick.GetAxisValue(0) * _deltaTime));

			_objects[14]->rotateX(Math::Rad(joystick.GetAxisValue(1) * _deltaTime));
			_objects[19]->rotateX(Math::Rad(joystick.GetAxisValue(5) * _deltaTime));
			_objects[6]->rotateZ(-Math::Rad(joystick.GetAxisValue(2) * _deltaTime));

			// float factor = (joystick.GetAxisValue(3) + 1.f) / 2.f;
			// _cameraObject.setTranslation(_cameraObject.translation().normalized() * pow(1.15f, factor * 10.f));
		}
	}

	ImGuiIntegration::Context _imgui{Magnum::NoCreate};

	float _deltaTime = 0;

	Containers::Array<Optional<GL::Mesh>> _meshes;
	Containers::Array<Optional<GL::Texture2D>> _textures;
	Containers::Array<Optional<Trade::PhongMaterialData>> _materials;
	Containers::Array<Object3D *> _objects;

	Animation::Player<Float> player;
	Containers::Array<Optional<Trade::AnimationData>> _animationData;

	Scene3D _scene;
	Object3D _manipulator, _cameraRoot, _cameraObject;
	SceneGraph::Camera3D *_camera;
	SceneGraph::DrawableGroup3D _drawables;
	Vector3 _previousPosition;

	Timeline timeline;
	Joystick joystick;
};

MAGNUM_APPLICATION_MAIN(ViewerExample);
