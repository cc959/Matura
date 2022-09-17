#include "PhongDrawable.h"

PhongDrawable::PhongDrawable(Object3D &object, Stage &stage, GL::Mesh &mesh, Trade::PhongMaterialData &material, SceneGraph::DrawableGroup3D &group) : SceneGraph::Drawable3D{object, &group}, _mesh(mesh), _stage(stage), _material{material}
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

	_hasNormals = _material.hasAttribute(Trade::MaterialAttribute::NormalTexture) && _material.normalTexture() < _stage._textures.size() && _stage._textures[_material.normalTexture()];

	if (_hasNormals)
		_shader = Shaders::PhongGL{Shaders::PhongGL::Flag::DiffuseTexture | Shaders::PhongGL::Flag::NormalTexture | Shaders::PhongGL::Flag::Bitangent};
	else
		_shader = Shaders::PhongGL{Shaders::PhongGL::Flag::DiffuseTexture};

	_shader.setAmbientColor(Color4(0.1, 0.1, 0.1, 1))
		.setSpecularColor(Color4(1, 1, 1, 1))
		.setShininess(80.f);
}

void PhongDrawable::draw(const Matrix4 &transformationMatrix, SceneGraph::Camera3D &camera)
{

	if (_material.hasAttribute(Trade::MaterialAttribute::DiffuseTexture) && _stage._textures[_material.diffuseTexture()])
		_shader.bindDiffuseTexture(*_stage._textures[_material.diffuseTexture()]);
	else
		_shader.bindDiffuseTexture(_defaultDiffuse);

	if (_hasNormals)
		_shader.bindNormalTexture(*_stage._textures[_material.normalTexture()]);


	_shader
		.setShininess(max(_material.shininess(), 1.f)) // 0.01f because shader acts wierd at  0
		.setSpecularColor({_material.shininess() / 100.f, _material.shininess() / 100.f, _material.shininess() / 100.f, 1})
		.setDiffuseColor(_material.diffuseColor())
		.setLightPositions({{camera.cameraMatrix().transformPoint({0.f, 2.f, 0.f}), 1.0f}})
		.setLightColors() _stage._lightColors)
		.setTransformationMatrix(transformationMatrix)
		.setNormalMatrix(transformationMatrix.normalMatrix())
		.setProjectionMatrix(camera.projectionMatrix())
		.draw(_mesh);
}
