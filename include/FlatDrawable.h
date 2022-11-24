#pragma once

#include "GlobalIncludes.h"

class Stage;
#include "Stage.h"

class FlatDrawable : public SceneGraph::Drawable3D {
public:
    explicit FlatDrawable(Object3D &object, int objectID, Stage &stage, GL::Mesh &mesh,
                           SceneGraph::DrawableGroup3D &group);

private:
    void draw(const Matrix4 &transformationMatrix, SceneGraph::Camera3D &camera) override;

    static Shaders::FlatGL3D *_shaderStandard;
    GL::Mesh &_mesh;
    Stage &_stage;

    Object3D &_object;

    int _objectID;
};
