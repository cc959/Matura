// part of the code sourced from Magnum: github.com/mosra/magnum

#pragma once

#include "GlobalIncludes.h"

#include "ShadowCasterShader.h"

class ShadowCasterShader;

class ShadowCasterDrawable : public SceneGraph::Drawable3D
{
public:
	explicit ShadowCasterDrawable(Object3D &object, GL::Mesh &mesh, SceneGraph::DrawableGroup3D &group);

	Float radius() const;

	void draw(const Matrix4 &transformationMatrix, SceneGraph::Camera3D &shadowCamera) override;

private:
	GL::Mesh &_mesh;
	ShadowCasterShader _shader;
	Float _radius;
};
