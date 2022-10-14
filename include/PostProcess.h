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
		_srcResolutionLocation = uniformLocation("_srcResolution");
		_destResolutionLocation = uniformLocation("_destResolution");

		_quad = MeshTools::compile(Primitives::planeSolid(Primitives::PlaneFlag::TextureCoordinates));
	}

	PostProcess &draw()
	{

		vector<GL::Texture2D> color(2);
		color[0].setWrapping(GL::SamplerWrapping::ClampToEdge).setStorage(1, GL::TextureFormat::RGBA8, {1280, 720});
		color[1].setWrapping(GL::SamplerWrapping::ClampToEdge).setStorage(1, GL::TextureFormat::RGBA8, {1280, 720});

		Range2Di screenRect{{}, {1280, 720}};

		vector<GL::Framebuffer> raw;
		raw.push_back(GL::Framebuffer{screenRect});
		raw.push_back(GL::Framebuffer{screenRect});
		raw[0].attachTexture(GL::Framebuffer::ColorAttachment{0}, color[0], 0);
		raw[1].attachTexture(GL::Framebuffer::ColorAttachment{0}, color[1], 0);

		GL::AbstractFramebuffer::blit(GL::defaultFramebuffer, raw[0], screenRect, screenRect, GL::FramebufferBlitMask(GL::FramebufferBlit::Color), GL::FramebufferBlitFilter::Linear);

		Vector2i size = color[0].imageSize(0);

		setUniform(_modeLocation, 0);

		for (int i = 1; i <= 4; i++)
		{
			raw[i % 2].bind();
			Vector2i newSize = size / 2;
			color[(i - 1) % 2].bind(TextureUnit);
			setUniform(_srcResolutionLocation, size);
			setUniform(_destResolutionLocation, newSize);
			draw(_quad);
			size = newSize;
		}
		setUniform(_modeLocation, 1);
		for (int i = 3; i >= 0; i--)
		{
			raw[i % 2].bind();
			Vector2i newSize = size * 2;
			color[(i + 1) % 2].bind(TextureUnit);
			setUniform(_srcResolutionLocation, size);
			setUniform(_destResolutionLocation, newSize);
			draw(_quad);
			size = newSize;
		}

		GL::defaultFramebuffer.bind();

		color[0].bind(TextureUnit);
		setUniform(_modeLocation, 2);
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

	int _modeLocation, _srcResolutionLocation, _destResolutionLocation;
};