#pragma once

#include "GlobalIncludes.h"

// forward declaration
class Stage;

#include "Stage.h"

class PhongDrawable : public SceneGraph::Drawable3D
{
public:
	explicit PhongDrawable(Object3D &object, Stage &stage, GL::Mesh &mesh, Trade::PhongMaterialData &material, SceneGraph::DrawableGroup3D &group);

private:
	void draw(const Matrix4 &transformationMatrix, SceneGraph::Camera3D &camera) override;

	Shaders::PhongGL _shader;
	GL::Mesh &_mesh;
	Stage& _stage;

	Trade::PhongMaterialData &_material;

	GL::Texture2D _defaultDiffuse;

	bool _hasNormals;
};
