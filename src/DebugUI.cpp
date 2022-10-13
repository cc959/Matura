/*
	This file is part of Magnum.

	Original authors — credit is appreciated but not required:

		2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
		2020, 2021, 2022 — Vladimír Vondruš <mosra@centrum.cz>
		2016 — Bill Robinson <airbaggins@gmail.com>

	This is free and unencumbered software released into the public domain.

	Anyone is free to copy, modify, publish, use, compile, sell, or distribute
	this software, either in source code form or as a compiled binary, for any
	purpose, commercial or non-commercial, and by any means.

	In jurisdictions that recognize copyright laws, the author or authors of
	this software dedicate any and all copyright interest in the software to
	the public domain. We make this dedication for the benefit of the public
	at large and to the detriment of our heirs and successors. We intend this
	dedication to be an overt act of relinquishment in perpetuity of all
	present and future rights to this software under copyright law.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
	THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
	IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
	CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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

	if (!_font || !_font->openFile("res/Fonts/Roboto-Regular.ttf", 256.0f))
		Fatal{} << "Can't open res/Fonts/Roboto-Regular.ttf with StbTrueTypeFont";

	_font->fillGlyphCache(_cache, "abcdefghijklmnopqrstuvwxyz"
								  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
								  "0123456789?!:;,. ");

	_textRenderer = new Text::Renderer2D{*_font, _cache, 0.05f, Text::Alignment::LineCenter};
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

		/* Update the text occasionally */
		_textRenderer->render(_labels[i]);

		GL::Renderer::disable(GL::Renderer::Feature::DepthTest);
		GL::Renderer::enable(GL::Renderer::Feature::Blending);

		/* Draw the text on the screen */
		_textShader.setTransformationProjectionMatrix(Matrix3::translation((screenA + screenB).xy() / 2.f))
			.setColor((_lines[0][i * 2].color + _lines[0][i * 2 + 1].color) / 2.f)
			.bindVectorTexture(_cache.texture())
			.draw(_textRenderer->mesh());

		GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
		GL::Renderer::enable(GL::Renderer::Feature::Blending);
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
