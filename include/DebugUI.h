#pragma once

#include "GlobalIncludes.h"

class DebugUI
{
	struct Point
	{
		Vector3 position;
		Color3 color;
	};

public:
	DebugUI();

	void reset();

	void addLine(const Vector3 &p0, const Color3 &col0, const Vector3 &p1, const Color3 &col1, bool depth = false, string text = "")
	{
		_lines[depth].push_back({p0, col0});
		_lines[depth].push_back({p1, col1});

		_labels.push_back(text);
	}

	void addLine(const Vector4 &p0, const Color3 &col0, const Vector4 &p1, const Color3 &col1, bool depth = false, string text = "")
	{
		addLine(p0.xyz(), col0, p1.xyz(), col1, depth, text);
	}

	void addLine(const Vector4 &p0, const Vector4 &p1, const Color3 &col, bool depth = false, string text = "")
	{
		addLine(p0.xyz(), p1.xyz(), col, depth, text);
	}

	void addLine(const Vector3 &p0, const Vector3 &p1, const Color3 &col, bool depth = false, string text = "")
	{
		addLine(p0, col, p1, col, depth, text);
	}

	void addPoint(const Vector3 &p0, const Color3 &col, bool depth = false, string text = "")
	{
		addLine(p0, p0, col, depth, text);
	}

	void addPoint(const Vector4 &p0, const Color3 &col, bool depth = false, string text = "")
	{
		addLine(p0.xyz(), p0.xyz(), col, depth, text);
	}

	void addFrustum(const Matrix4 &imvp, const Color3 &col);
	void addFrustum(const Matrix4 &imvp, const Color3 &col, Float z0, Float z1);

	void draw(SceneGraph::Camera3D &camera);

protected:
	vector<Point> _lines[2];
	vector<string> _labels;

	GL::Buffer _buffer;
	GL::Mesh _mesh;
	Shaders::VertexColorGL3D _shader;

	PluginManager::Manager<Text::AbstractFont> manager;
	Text::Renderer2D *_textRenderer;
	Text::GlyphCache _cache{Vector2i(2048 * 2)};
	Text::AbstractFont *_font;
	Shaders::VectorGL2D _textShader;
};
