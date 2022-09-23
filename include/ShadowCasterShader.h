// part of the code sourced from Magnum: github.com/mosra/magnum

#pragma once

#include <Magnum/GL/AbstractShaderProgram.h>
#include "GlobalIncludes.h"

class ShadowCasterShader : public GL::AbstractShaderProgram
{
public:
	explicit ShadowCasterShader();

	ShadowCasterShader &setTransformationMatrix(const Matrix4 &matrix);

private:
	Int _transformationMatrixUniform;
};
