#pragma once

#include "GlobalIncludes.h"

#include "PhongDrawable.h"
#include "ShadowCasterDrawable.h"
#include "ShadowLight.h"

class Stage
{
public:
	struct Shadows
	{
		Float _shadowBias;
		Float _layerSplitExponent;
		Vector2i _shadowMapSize;
		Object3D _shadowLightObject;
		ShadowLight _shadowLight;
		Int _shadowMapFaceCullMode;
		bool _shadowStaticAlignment;
		bool _debug;

		Shadows(Scene3D &scene, Float bias = 0.001f, Float layerSplitExponent = 3.f, Vector2i shadowMapSize = {1024 * 4, 1024 * 4}, Int shadowMapFaceCullMode = 0, bool shadowStaticAlignment = true, bool debug = false) : _shadowBias{bias},
																																																							_layerSplitExponent{layerSplitExponent},
																																																							_shadowMapSize{shadowMapSize},
																																																							_shadowLightObject(&scene),
																																																							_shadowLight{_shadowLightObject},
																																																							_shadowMapFaceCullMode{shadowMapFaceCullMode},
																																																							_shadowStaticAlignment{shadowStaticAlignment},
																																																							_debug{debug}

		{
		}
	};

	vector<std::pair<Optional<GL::Mesh>, float>> _meshes;
	vector<float> _radii;
	vector<Optional<GL::Texture2D>> _textures;
	vector<Optional<Trade::PhongMaterialData>> _materials;
	vector<Object3D *> _objectByID;
	map<string, Object3D *> _objectByName;

	Containers::Array<Matrix4> shadowMatrices;

	vector<Vector4> _lights;
	vector<Color3> _lightColors;

	Color3 _ambientLight;

	Animation::Player<Float> _player;
	vector<Optional<Trade::AnimationData>> _animationData;

	Scene3D _scene{};
	Object3D _manipulator;

	vector<SceneGraph::Camera3D *> _cameras;

	SceneGraph::Camera3D *_activeCamera = nullptr;
	SceneGraph::DrawableGroup3D _shadowReceivers;
	SceneGraph::DrawableGroup3D _shadowCasters;

	Shadows _shadows{_scene};

	Stage();

	Stage(string path);

	Stage(Trade::AnySceneImporter &importer);

	void Import(Trade::AnySceneImporter &importer);

	static Animation::Player<Float> ImportAnimations(Trade::AnySceneImporter &importer, vector<Optional<Trade::AnimationData>> &animationData, vector<Object3D *> &objects);
	static vector<Optional<GL::Texture2D>> ImportTextures(Trade::AnySceneImporter &importer);
	static vector<Optional<Trade::PhongMaterialData>> ImportMaterials(Trade::AnySceneImporter &importer);
	static vector<std::pair<Optional<GL::Mesh>, float>> ImportMeshes(Trade::AnySceneImporter &importer);

	int getCameraID(SceneGraph::Camera3D* camera);

    int addCamera(Deg fov, float near, float far, const string& name, Matrix4 baseTransformation = Matrix4::translation({}) ) {
        auto *cameraObejct = new Object3D{};
        cameraObejct->setParent(&_manipulator);

        auto *cameraManipulator = new Object3D{};
        cameraManipulator->setParent(cameraObejct);

        auto *camera = new SceneGraph::Camera3D(*cameraManipulator);

        (*camera)
                .setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
                .setProjectionMatrix(Matrix4::perspectiveProjection(fov, 1, near, far))
                .setViewport(GL::defaultFramebuffer.viewport().size());

        _objectByName[name]= cameraObejct;
        _objectByID.push_back(cameraObejct);

        _objectByName[name + " manipulator"] = cameraManipulator;
        _objectByID.push_back(cameraManipulator);

        _cameras.push_back(camera);

        return int(_cameras.size()) - 1;
    }
};