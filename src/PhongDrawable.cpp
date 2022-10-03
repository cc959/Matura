#include "PhongDrawable.h"

#include "Magnum/Shaders/PhongGL.h"

#define _shader (*(_hasNormalMap ? _shaderNormalMap : _shaderStandard))

Shaders::PhongGLShadows *PhongDrawable::_shaderStandard = nullptr;
Shaders::PhongGLShadows *PhongDrawable::_shaderNormalMap = nullptr;

PhongDrawable::PhongDrawable(Object3D &object, Stage &stage, GL::Mesh &mesh, Trade::PhongMaterialData &material, SceneGraph::DrawableGroup3D &group) : SceneGraph::Drawable3D{object, &group}, _object(object), _mesh(mesh), _stage(stage), _material{material}
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

	_hasNormalMap = _material.hasAttribute(Trade::MaterialAttribute::NormalTexture) && _material.normalTexture() < _stage._textures.size() && _stage._textures[_material.normalTexture()];

	if (!_hasNormalMap)
	{
		if (!_shaderStandard)
			_shaderStandard = new Shaders::PhongGLShadows{Shaders::PhongGLShadows::Flag::DiffuseTexture | Shaders::PhongGLShadows::Flag::Shadows};
	}
	else
	{
		if (!_shaderNormalMap)
			_shaderNormalMap = new Shaders::PhongGLShadows{Shaders::PhongGLShadows::Flag::DiffuseTexture | Shaders::PhongGLShadows::Flag::NormalTexture | Shaders::PhongGLShadows::Flag::Bitangent | Shaders::PhongGLShadows::Flag::Shadows};
	}

	_shader.setAmbientColor(Color4(0.1, 0.1, 0.1, 1))
		.setSpecularColor(Color4(1, 1, 1, 1))
		.setShininess(80.f);
}

// Shaders::PhongGLShadows PhongDrawable::_shaderNormalMap;
// Shaders::PhongGLShadows PhongDrawable::_shaderStandard;

void PhongDrawable::draw(const Matrix4 &transformationMatrix, SceneGraph::Camera3D &camera)
{

	if (_material.hasAttribute(Trade::MaterialAttribute::DiffuseTexture) && _stage._textures[_material.diffuseTexture()])
		_shader.bindDiffuseTexture(*_stage._textures[_material.diffuseTexture()]);
	else
		_shader.bindDiffuseTexture(_defaultDiffuse);

	if (_hasNormalMap)
		_shader.bindNormalTexture(*_stage._textures[_material.normalTexture()]);

	Containers::ArrayView<Color3> colors{&_stage._lightColors[0], _stage._lightColors.size()};
	Containers::Array<Vector4> positions{_stage._lights.size()};

	Matrix4 worldToCamera = camera.cameraMatrix();

	for (int i = 0; i < int(_stage._lights.size()); i++)
	{
		if (_stage._lights[i].w() == 0)
			positions[i] = Vector4(worldToCamera.transformVector(_stage._lights[i].xyz()), 0);
		else
			positions[i] = Vector4(worldToCamera.transformPoint(_stage._lights[i].xyz()), 1);
	}

	_shader.setShadowmapMatrices(_stage.shadowMatrices)
		.setShadowmapTexture(_stage._shadows._shadowLight.shadowTexture())
		.setModelMatrix(_object.absoluteTransformationMatrix())
		.setShadowBias(_stage._shadows._shadowBias)
		.setDebug(_stage._shadows._debug);

	if (positions.size() && colors.size())
	{
		_shader
			.setLightPositions(positions)
			.setLightColors(colors);
	}
	else
	{
		_shader
			.setLightPositions({Vector4{worldToCamera.transformVector({0.7071, 0.7071, 0}), 0}})
			.setLightColors({Color3{1, 1, 1}});
	}

	_shader
		.setShininess(max(_material.shininess(), 1.f)) // 0.01f because shader acts wierd at  0
		.setSpecularColor({_material.shininess() / 100.f, _material.shininess() / 100.f, _material.shininess() / 100.f, 1})
		.setDiffuseColor(_material.diffuseColor())

		.setTransformationMatrix(transformationMatrix)
		.setNormalMatrix(transformationMatrix.normalMatrix())
		.setProjectionMatrix(camera.projectionMatrix())
		.draw(_mesh);
}
