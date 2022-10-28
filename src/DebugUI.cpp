/*
	A decent part of this file is sourced from Magnum.
*/

#include "DebugUI.h"

#include <Corrade/Containers/ArrayViewStl.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/SceneGraph/Camera.h>

#include "ShadowLight.h"

DebugUI::DebugUI() : _mesh{GL::MeshPrimitive::Lines}
{
	_mesh.addVertexBuffer(_buffer, 0,
						  Shaders::VertexColorGL3D::Position{},
						  Shaders::VertexColorGL3D::Color3{});

	_font = manager.loadAndInstantiate("StbTrueTypeFont").release();

	if (!_font || !_font->openFile("res/Fonts/Roboto-Bold.ttf", 256.0f))
		Fatal{} << "Can't open res/Fonts/Roboto-Regular.ttf with StbTrueTypeFont";

	_font->fillGlyphCache(_cache, "abcdefghijklmnopqrstuvwxyz"
								  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
								  "0123456789?!:;,. ");

	distanceField.setWrapping(GL::SamplerWrapping::ClampToEdge).setStorage(1, GL::TextureFormat::RGBA8, _cache.texture().imageSize(0));
	TextureTools::DistanceField(7)(_cache.texture(), distanceField, Range2Di{{}, _cache.texture().imageSize(0)});

	Debug{} << _cache.texture().imageSize(0);

	_textRenderer = new Text::Renderer2D{*_font, _cache, 0.025f, Text::Alignment::MiddleCenter};
	_textRenderer->reserve(32, GL::BufferUsage::DynamicDraw, GL::BufferUsage::StaticDraw);
}

void DebugUI::reset()
{
	_lines[0].clear();
	_lines[1].clear();
	_labels.clear();
	_buffer.invalidateData();
}

void DebugUI::draw(SceneGraph::Camera3D &camera)
{
	auto transformationProjectionMatrix = camera.projectionMatrix() * camera.cameraMatrix();
	// draw lines with depth testing
	_buffer.setData(_lines[0], GL::BufferUsage::StreamDraw);
	_mesh.setCount(_lines[0].size());
	_shader
		.setTransformationProjectionMatrix(transformationProjectionMatrix)
		.draw(_mesh);

	// draw lines without depth testing
	GL::Renderer::disable(GL::Renderer::Feature::DepthTest);
	_buffer.setData(_lines[0], GL::BufferUsage::StreamDraw);
	_mesh.setCount(_lines[0].size());
	_shader
		.setTransformationProjectionMatrix(transformationProjectionMatrix)
		.draw(_mesh);
	GL::Renderer::enable(GL::Renderer::Feature::DepthTest);

	for (int i = 0; i < _labels.size(); i++)
	{

		Vector4 screenA = transformationProjectionMatrix * Vector4(_lines[0][i * 2].position, 1);
		Vector4 screenB = transformationProjectionMatrix * Vector4(_lines[0][i * 2 + 1].position, 1);

		auto clampAxis = [](Vector4 &a, Vector4 &b, float da, float db, float min = -1, float max = 1)
		{
			if (da < min)
				if (db >= min)
					a = Math::lerp(a, b, (da - min) / (da - db));
				else
					return false;

			if (da > max)
				if (db <= max)
					a = Math::lerp(a, b, (da - max) / (da - db));
				else
					return false;

			return true;
		};

		if (!clampAxis(screenA, screenB, screenA.z(), screenB.z(), 0, 1000) || !clampAxis(screenB, screenA, screenB.z(), screenA.z(), 0, 1000))
			continue;

		screenA.xy() /= screenA.w();
		screenB.xy() /= screenB.w();

		if (!clampAxis(screenA, screenB, screenA.x(), screenB.x()) ||
			!clampAxis(screenB, screenA, screenB.x(), screenA.x()) ||
			!clampAxis(screenA, screenB, screenA.y(), screenB.y()) ||
			!clampAxis(screenB, screenA, screenB.y(), screenA.y()))
			continue;

		if (_labels[i].size())
		{
			/* Update the text occasionally */
			_textRenderer->render(_labels[i]);

			GL::Renderer::disable(GL::Renderer::Feature::DepthTest);
			GL::Renderer::enable(GL::Renderer::Feature::Blending);

			Vector2 aspect = Vector2(camera.viewport()) / float(camera.viewport().x());

			/* Draw the text on the screen */
			_textShader.setTransformationProjectionMatrix(Matrix3::projection(aspect) * Matrix3::translation((screenA + screenB).xy() / 2.f * (aspect / 2.f)))
				.setColor((_lines[0][i * 2].color + _lines[0][i * 2 + 1].color) / 2.f)
				.setOutlineColor(Color4(1, 1, 1, 1))
				.setOutlineRange(0.7, 0.1)
				.bindVectorTexture(distanceField)
				.draw(_textRenderer->mesh());

			GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
			GL::Renderer::enable(GL::Renderer::Feature::Blending);
		}
	}
}

void DebugUI::addFrustum(const Matrix4 &imvp, const Color3 &col)
{
	addFrustum(imvp, col, 1.0f, -1.0f);
}

void DebugUI::addFrustum(const Matrix4 &imvp, const Color3 &col, const Float z0, const Float z1)
{
	auto worldPointsToCover = ShadowLight::frustumCorners(imvp, z0, z1);

	auto nearMid = (worldPointsToCover[0] +
					worldPointsToCover[1] +
					worldPointsToCover[3] +
					worldPointsToCover[2]) *
				   0.25f;

	addLine(nearMid, worldPointsToCover[1], col);
	addLine(nearMid, worldPointsToCover[3], col);
	addLine(nearMid, worldPointsToCover[2], col);
	addLine(nearMid, worldPointsToCover[0], col);

	addLine(worldPointsToCover[0], worldPointsToCover[1], col);
	addLine(worldPointsToCover[1], worldPointsToCover[3], col);
	addLine(worldPointsToCover[3], worldPointsToCover[2], col);
	addLine(worldPointsToCover[2], worldPointsToCover[0], col);

	addLine(worldPointsToCover[0], worldPointsToCover[4], col);
	addLine(worldPointsToCover[1], worldPointsToCover[5], col);
	addLine(worldPointsToCover[2], worldPointsToCover[6], col);
	addLine(worldPointsToCover[3], worldPointsToCover[7], col);

	addLine(worldPointsToCover[4], worldPointsToCover[5], col);
	addLine(worldPointsToCover[5], worldPointsToCover[7], col);
	addLine(worldPointsToCover[7], worldPointsToCover[6], col);
	addLine(worldPointsToCover[6], worldPointsToCover[4], col);
}
