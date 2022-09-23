// part of the code sourced from Magnum: github.com/mosra/magnum

#include "ShadowCasterShader.h"

#include "GlobalIncludes.h"

ShadowCasterShader::ShadowCasterShader()
{
	MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL330);

	const Utility::Resource rs{"Shaders"};

	GL::Shader vert{GL::Version::GL330, GL::Shader::Type::Vertex};
	GL::Shader frag{GL::Version::GL330, GL::Shader::Type::Fragment};

	vert.addSource(rs.getString("ShadowCaster.vert"));
	frag.addSource(rs.getString("ShadowCaster.frag"));

	CORRADE_INTERNAL_ASSERT_OUTPUT(vert.compile() && frag.compile());

	attachShaders({vert, frag});

	CORRADE_INTERNAL_ASSERT_OUTPUT(link());

	_transformationMatrixUniform = uniformLocation("transformationMatrix");
}

ShadowCasterShader &ShadowCasterShader::setTransformationMatrix(const Matrix4 &matrix)
{
	setUniform(_transformationMatrixUniform, matrix);
	return *this;
}
