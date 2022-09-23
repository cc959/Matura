// part of the code sourced from Magnum: github.com/mosra/magnum

#include "ShadowCasterDrawable.h"
#include <Magnum/SceneGraph/Camera.h>

ShadowCasterDrawable::ShadowCasterDrawable(Object3D &object, GL::Mesh &mesh, SceneGraph::DrawableGroup3D &group) : Magnum::SceneGraph::Drawable3D{object, &group}, _mesh{mesh} {}

void ShadowCasterDrawable::draw(const Matrix4 &transformationMatrix, SceneGraph::Camera3D &shadowCamera)
{
	_shader
		.setTransformationMatrix(shadowCamera.projectionMatrix() * transformationMatrix)
		.draw(_mesh);
}
Float ShadowCasterDrawable::radius() const { return _radius; }
