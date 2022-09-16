#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/GL/PixelFormat.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/TranslationRotationScalingTransformation3D.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/SceneGraph/Object.h>
#include <Magnum/Trade/PhongMaterialData.h>
#include <Corrade/Containers/Array.h>
#include <Magnum/Image.h>
#include <Magnum/Shaders/PhongGL.h>
#include <Magnum/Math/Color.h>
#include <Magnum/PixelFormat.h>

#include <bits/stdc++.h>

using namespace std;

using namespace Magnum;
using namespace Containers;

typedef SceneGraph::Object<SceneGraph::TranslationRotationScalingTransformation3D> Object3D;

class PhongDrawable : public SceneGraph::Drawable3D
{
public:
	explicit PhongDrawable(Object3D &object, GL::Mesh &mesh, Containers::Array<Optional<GL::Texture2D>> &textures, Trade::PhongMaterialData &material, SceneGraph::DrawableGroup3D &group) : SceneGraph::Drawable3D{object, &group}, _mesh(mesh), _textures(textures), _material{material}
	{
		auto diffuseData = Containers::array<char>({-1, -1, -1, -1});
		Image2D diffuseImage(PixelFormat::RGBA8Unorm, {1, 1}, move(diffuseData));

		_defaultDiffuse
			.setMagnificationFilter(SamplerFilter::Linear)
			.setMinificationFilter(SamplerFilter::Linear, SamplerMipmap::Linear)
			.setWrapping(GL::SamplerWrapping::Repeat)
			.setStorage(1, GL::textureFormat(Magnum::PixelFormat::RGBA8Unorm), Vector2i(1, 1))
			.setSubImage(0, {}, diffuseImage)
			.generateMipmap();

		_hasNormals = _material.hasAttribute(Trade::MaterialAttribute::NormalTexture) && _material.normalTexture() < _textures.size() && _textures[_material.normalTexture()];

		if (_hasNormals)
			_shader = Shaders::PhongGL{Shaders::PhongGL::Flag::DiffuseTexture | Shaders::PhongGL::Flag::NormalTexture | Shaders::PhongGL::Flag::Bitangent};
		else
			_shader = Shaders::PhongGL{Shaders::PhongGL::Flag::DiffuseTexture};

		_shader.setAmbientColor(Color4(0.1, 0.1, 0.1, 1))
			.setSpecularColor(Color4(1, 1, 1, 1))
			.setShininess(80.f);
	}

private:
	void draw(const Matrix4 &transformationMatrix, SceneGraph::Camera3D &camera) override
	{

		if (_material.hasAttribute(Trade::MaterialAttribute::DiffuseTexture) && _textures[_material.diffuseTexture()])
			_shader.bindDiffuseTexture(*_textures[_material.diffuseTexture()]);
		else
			_shader.bindDiffuseTexture(_defaultDiffuse);

		if (_hasNormals)
			_shader.bindNormalTexture(*_textures[_material.normalTexture()]);

		_shader
			.setShininess(max(_material.shininess(), 1.f)) // 0.01f because shader acts wierd at  0
			.setSpecularColor({_material.shininess() / 100.f, _material.shininess() / 100.f, _material.shininess() / 100.f, 1})
			.setDiffuseColor(_material.diffuseColor())
			.setLightPositions({{camera.cameraMatrix().transformPoint({-3.0f, 10.0f, 10.0f}), 0.0f}})
			.setTransformationMatrix(transformationMatrix)
			.setNormalMatrix(transformationMatrix.normalMatrix())
			.setProjectionMatrix(camera.projectionMatrix())
			.draw(_mesh);
	}

	Shaders::PhongGL _shader;
	GL::Mesh &_mesh;
	Containers::Array<Optional<GL::Texture2D>> &_textures;

	Trade::PhongMaterialData &_material;

	GL::Texture2D _defaultDiffuse;

	bool _hasNormals;
};
