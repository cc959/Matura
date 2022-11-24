#pragma once

#include "GlobalIncludes.h"

#include "PhongGLShadows.h"

// forward declaration
class Stage;

#include "Stage.h"

class PhongDrawable : public SceneGraph::Drawable3D {
public:
    explicit PhongDrawable(Object3D &object, int objectID, Stage &stage, GL::Mesh &mesh, Trade::PhongMaterialData &material,
                           SceneGraph::DrawableGroup3D &group);

private:
    void draw(const Matrix4 &transformationMatrix, SceneGraph::Camera3D &camera) override;

    static Shaders::PhongGLShadows *_shaderStandard;
    static Shaders::PhongGLShadows *_shaderNormalMap;
    GL::Mesh &_mesh;
    Stage &_stage;

    Trade::PhongMaterialData &_material;

    GL::Texture2D _defaultDiffuse;

    Object3D &_object;

    bool _hasNormalMap;

    int _objectID;

};
