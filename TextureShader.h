#pragma once

#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Resource.h>
#include <Magnum/ImageView.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/ImageData.h>
#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Color.h>

using namespace Magnum;

class TextureShader : public GL::AbstractShaderProgram
{
public:
	typedef GL::Attribute<0, Vector2> Position;
	typedef GL::Attribute<1, Vector2> TextureCoordinates;

	explicit TextureShader()
	{
		MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL330);

		const Utility::Resource rs{"texturedquad-data"};

		GL::Shader vert(GL::Version::GL330, GL::Shader::Type::Vertex);
		GL::Shader frag{GL::Version::GL330, GL::Shader::Type::Fragment};

		vert.addSource(rs.getString("res/vert.glsl"));
		frag.addSource(rs.getString("res/frag.glsl"));

		CORRADE_INTERNAL_ASSERT_OUTPUT(vert.compile() && frag.compile());

		attachShaders({vert, frag});

		CORRADE_INTERNAL_ASSERT_OUTPUT(link());

		_colorUniform = uniformLocation("color");
		setUniform(uniformLocation("textureData"), TextureUnit);
	}

	TextureShader &setColor(const Color3 &color)
	{
		setUniform(_colorUniform, color);

		return *this;
	}

	TextureShader &bindTexture(GL::Texture2D &texture)
	{
		texture.bind(TextureUnit);
		return *this;
	}

private:
	enum : Int
	{
		TextureUnit = 0
	};

	Int _colorUniform;
};