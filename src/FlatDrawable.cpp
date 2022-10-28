#include "PhongDrawable.h"

#include "Magnum/Shaders/PhongGL.h"
#include "FlatDrawable.h"


#define shader (*_shaderStandard)

Shaders::FlatGL3D *FlatDrawable::_shaderStandard = nullptr;


void FlatDrawable::draw(const Matrix4 &transformationMatrix, SceneGraph::Camera3D &camera) {


    shader
    .setColor({1, 0, 0, 1})
            .setObjectId(_objectID)
            .setTransformationProjectionMatrix(camera.projectionMatrix() * transformationMatrix)
            .draw(_mesh);

}

FlatDrawable::FlatDrawable(Object3D &object, int objectID, Stage &stage, GL::Mesh &mesh,
                           SceneGraph::DrawableGroup3D &group) : SceneGraph::Drawable3D{object, &group}, _mesh(mesh), _stage(stage), _object(object), _objectID(objectID) {
    if (!_shaderStandard)
        _shaderStandard = new Shaders::FlatGL3D{Shaders::FlatGL3D::Flag::ObjectId};
}
