#pragma once

#include "GlobalIncludes.h"

class PostProcess : public GL::AbstractShaderProgram
{
public:
	typedef GL::Attribute<0, Vector4> Position;
	typedef GL::Attribute<1, Vector2> TextureCoordinates;

	PostProcess()
	{
		MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL330);

		const Utility::Resource rs{"Shaders"};

		GL::Shader vert{GL::Version::GL330, GL::Shader::Type::Vertex};
		GL::Shader frag{GL::Version::GL330, GL::Shader::Type::Fragment};

		vert.addSource(rs.getString("PostProcess.vert"));
		frag.addSource(rs.getString("PostProcess.frag"));

		CORRADE_INTERNAL_ASSERT_OUTPUT(vert.compile() && frag.compile());

		attachShaders({vert, frag});

		CORRADE_INTERNAL_ASSERT_OUTPUT(link());
		setUniform(uniformLocation("_texture"), TextureUnit);
		_modeLocation = uniformLocation("mode");

		_quad = MeshTools::compile(Primitives::planeSolid(Primitives::PlaneFlag::TextureCoordinates));
	}

	void setupFramebuffers(Vector2i frameSize, int blurLevels)
	{
		_frameSize = {frameSize};
		_blurLevels = {blurLevels};

		_color.resize(blurLevels + 1);
		_raw.clear();

		for (int i = 0; i <= blurLevels; i++)
		{
			_color[i].setWrapping(GL::SamplerWrapping::ClampToEdge).setStorage(1, GL::TextureFormat::RGBA8, frameSize);
			Range2Di screenRect{{}, frameSize};

			_raw.push_back(GL::Framebuffer{screenRect});
			_raw.back().attachTexture(GL::Framebuffer::ColorAttachment{0}, _color[i], 0);

			frameSize /= 2;
		}
	}

	PostProcess &bloom(GL::AbstractFramebuffer &source)
	{

		GL::AbstractFramebuffer::blit(source, _raw[0], {{}, _frameSize}, {{}, _frameSize}, GL::FramebufferBlitMask(GL::FramebufferBlit::Color), GL::FramebufferBlitFilter::Linear);

		setUniform(_modeLocation, 0);

		for (int i = 1; i <= _blurLevels; i++)
		{
			_raw[i].bind();
			_color[i - 1].bind(TextureUnit);
			draw(_quad);
		}
		setUniform(_modeLocation, 1);
		for (int i = _blurLevels - 1; i >= 0; i--)
		{
			_raw[i].bind();
			_color[i + 1].bind(TextureUnit);
			draw(_quad);
		}

		source.bind();

		_color[0].bind(TextureUnit);
		setUniform(_modeLocation, 2);
		draw(_quad);

		return *this;
	}

	PostProcess &vignette(GL::AbstractFramebuffer &source)
	{
		GL::AbstractFramebuffer::blit(source, _raw[0], {{}, _frameSize}, {{}, _frameSize}, GL::FramebufferBlitMask(GL::FramebufferBlit::Color), GL::FramebufferBlitFilter::Linear);

		setUniform(_modeLocation, 3);
		source.bind();
		_color[0].bind(TextureUnit);
		draw(_quad);

		return *this;
	}

private:
	GL::Mesh _quad;

	using GL::AbstractShaderProgram::draw;

	enum : Int
	{
		TextureUnit = 0
	};

	int _modeLocation;
	vector<GL::Texture2D> _color;
	vector<GL::Framebuffer> _raw;

	Vector2i _frameSize;
	int _blurLevels;
};