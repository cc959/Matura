#pragma once

#include "GlobalIncludes.h"

#include "PhongDrawable.h"

class Stage
{
public:
	vector<Optional<GL::Mesh>> _meshes;
	vector<Optional<GL::Texture2D>> _textures;
	vector<Optional<Trade::PhongMaterialData>> _materials;
	vector<Object3D *> _objectsByID;
	map<string, Object3D *> _objectByName;

	vector<Vector4> _lights;
	vector<Color3> _lightColors;

	Color3 _ambientLight;

	Animation::Player<Float> _player;
	vector<Optional<Trade::AnimationData>> _animationData;

	Scene3D _scene;
	Object3D _manipulator, _cameraRoot, _cameraObject;
	SceneGraph::Camera3D *_camera;
	SceneGraph::DrawableGroup3D _drawables;

	Stage();

	Stage(string path);

	Stage(Trade::AnySceneImporter &importer);

	void Import(Trade::AnySceneImporter &importer);

	static Animation::Player<Float> ImportAnimations(Trade::AnySceneImporter &importer, vector<Optional<Trade::AnimationData>> &animationData, vector<Object3D *> &objects);
	static vector<Optional<GL::Texture2D>> ImportTextures(Trade::AnySceneImporter &importer);
	static vector<Optional<Trade::PhongMaterialData>> ImportMaterials(Trade::AnySceneImporter &importer);
	static vector<Optional<GL::Mesh>> ImportMeshes(Trade::AnySceneImporter &importer);
};