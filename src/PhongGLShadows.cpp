// A large part of the code sourced from Magnum: github.com/mosra/Magnum
// The PhongGL Shader with added shadows

#include "PhongGLShadows.h"

#if defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_BUILD_DEPRECATED)
#include <Corrade/Containers/Array.h>
#endif
#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Containers/Iterable.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/Containers/StringStl.h>
#include <Corrade/Utility/FormatStl.h>
#include <Corrade/Utility/Resource.h>

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Shader.h"
#include "Magnum/GL/Texture.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"

#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/TextureArray.h"
#endif

#include "Magnum/Shaders/Implementation/CreateCompatibilityShader.h"

namespace Magnum
{
	namespace Shaders
	{

		namespace
		{
			enum : Int
			{
				AmbientTextureUnit = 0,
				DiffuseTextureUnit = 1,
				SpecularTextureUnit = 2,
				NormalTextureUnit = 3,
				/* 4 taken by MeshVisualizer colormap */
				ObjectIdTextureUnit = 5 /* shared with Flat and MeshVisualizer */
			};

#ifndef MAGNUM_TARGET_GLES2
			enum : Int
			{
				ProjectionBufferBinding = 0,
				TransformationBufferBinding = 1,
				DrawBufferBinding = 2,
				TextureTransformationBufferBinding = 3,
				MaterialBufferBinding = 4,
				LightBufferBinding = 5
			};
#endif
		}

		PhongGLShadows::CompileState PhongGLShadows::compile(const Flags flags, const UnsignedInt lightCount
#ifndef MAGNUM_TARGET_GLES2
															 , const UnsignedInt numShadowLevels,
															 const UnsignedInt materialCount, const UnsignedInt drawCount
#endif
		)
		{
#ifndef CORRADE_NO_ASSERT
			{
				const bool textureTransformationNotEnabledOrTextured = !(flags & Flag::TextureTransformation) || (flags & (Flag::AmbientTexture | Flag::DiffuseTexture | Flag::SpecularTexture | Flag::NormalTexture))
#ifndef MAGNUM_TARGET_GLES2
																	   || flags >= Flag::ObjectIdTexture
#endif
					;
				CORRADE_ASSERT(textureTransformationNotEnabledOrTextured,
							   "Shaders::PhongGL: texture transformation enabled but the shader is not textured", CompileState{NoCreate});
			}
#endif

#ifndef MAGNUM_TARGET_GLES2
			CORRADE_ASSERT(!(flags >= Flag::InstancedObjectId) || !(flags & Flag::Bitangent),
						   "Shaders::PhongGL: Bitangent attribute binding conflicts with the ObjectId attribute, use a Tangent4 attribute with instanced object ID rendering instead", CompileState{NoCreate});
#endif

#ifndef MAGNUM_TARGET_GLES2
			CORRADE_ASSERT(!(flags >= Flag::UniformBuffers) || materialCount,
						   "Shaders::PhongGL: material count can't be zero", CompileState{NoCreate});
			CORRADE_ASSERT(!(flags >= Flag::UniformBuffers) || drawCount,
						   "Shaders::PhongGL: draw count can't be zero", CompileState{NoCreate});
#endif

#ifndef MAGNUM_TARGET_GLES2
			CORRADE_ASSERT(!(flags & Flag::TextureArrays) || (flags & (Flag::AmbientTexture | Flag::DiffuseTexture | Flag::SpecularTexture | Flag::NormalTexture)) || flags >= Flag::ObjectIdTexture,
						   "Shaders::PhongGL: texture arrays enabled but the shader is not textured", CompileState{NoCreate});
			CORRADE_ASSERT(!(flags & Flag::UniformBuffers) || !(flags & Flag::TextureArrays) || flags >= (Flag::TextureArrays | Flag::TextureTransformation),
						   "Shaders::PhongGL: texture arrays require texture transformation enabled as well if uniform buffers are used", CompileState{NoCreate});
			CORRADE_ASSERT(!(flags & Flag::LightCulling) || (flags & Flag::UniformBuffers),
						   "Shaders::PhongGL: light culling requires uniform buffers to be enabled", CompileState{NoCreate});
#endif

			CORRADE_ASSERT(!(flags & Flag::SpecularTexture) || !(flags & (Flag::NoSpecular)),
						   "Shaders::PhongGL: specular texture requires the shader to not have specular disabled", CompileState{NoCreate});

#ifndef MAGNUM_TARGET_GLES
			if (flags >= Flag::UniformBuffers)
				MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::ARB::uniform_buffer_object);
#endif
#ifndef MAGNUM_TARGET_GLES2
			if (flags >= Flag::MultiDraw)
			{
#ifndef MAGNUM_TARGET_GLES
				MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::ARB::shader_draw_parameters);
#elif !defined(MAGNUM_TARGET_WEBGL)
				MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::ANGLE::multi_draw);
#else
				MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::WEBGL::multi_draw);
#endif
			}
#endif
#ifndef MAGNUM_TARGET_GLES
			if (flags >= Flag::TextureArrays)
				MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::EXT::texture_array);
#endif

#ifdef MAGNUM_BUILD_STATIC
			/* Import resources on static build, if not already */
			if (!Utility::Resource::hasGroup("MagnumShadersGL"))
				importShaderResources();
#endif
			Utility::Resource magnumRS("MagnumShadersGL");
			Utility::Resource localRS("Shaders");

			const GL::Context &context = GL::Context::current();

#ifndef MAGNUM_TARGET_GLES
			CORRADE_ASSERT(!(flags >= Flag::UniformBuffers) || context.isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>(),
						   "Shaders::PhongGL: uniform buffers require" << GL::Extensions::ARB::uniform_buffer_object::string(), CompileState{NoCreate});
#endif

#ifndef MAGNUM_TARGET_GLES
			const GL::Version version = context.supportedVersion({GL::Version::GL320, GL::Version::GL310, GL::Version::GL300, GL::Version::GL210});
#else
			const GL::Version version = context.supportedVersion({GL::Version::GLES300, GL::Version::GLES200});
#endif

			PhongGLShadows out{NoInit};
			out._flags = flags;
			out._lightCount = lightCount;
			out._lightColorsUniform = out._lightPositionsUniform + Int(lightCount);
			out._lightSpecularColorsUniform = out._lightPositionsUniform + 2 * Int(lightCount);
			out._lightRangesUniform = out._lightPositionsUniform + 3 * Int(lightCount);
#ifndef MAGNUM_TARGET_GLES2
			out._materialCount = materialCount;
			out._drawCount = drawCount;
#endif

			GL::Shader vert = Implementation::createCompatibilityShader(magnumRS, version, GL::Shader::Type::Vertex);
			GL::Shader frag = Implementation::createCompatibilityShader(magnumRS, version, GL::Shader::Type::Fragment);

#ifndef MAGNUM_TARGET_GLES
			std::string lightInitializer;
			if (!(flags >= Flag::UniformBuffers) && lightCount)
			{
				using namespace Containers::Literals;

				/* Initializer for the light color / position / range arrays -- we need
				   a list of initializers joined by commas. For GLES we'll simply
				   upload the values directly. */
				constexpr Containers::StringView lightPositionInitializerPreamble = "#define LIGHT_POSITION_INITIALIZER "_s;
				constexpr Containers::StringView lightColorInitializerPreamble = "#define LIGHT_COLOR_INITIALIZER "_s;
				constexpr Containers::StringView lightRangeInitializerPreamble = "#define LIGHT_RANGE_INITIALIZER "_s;
				constexpr Containers::StringView lightPositionInitializerItem = "vec4(0.0, 0.0, 1.0, 0.0), "_s;
				constexpr Containers::StringView lightColorInitializerItem = "vec3(1.0), "_s;
				constexpr Containers::StringView lightRangeInitializerItem = "1.0/0.0, "_s;

				lightInitializer.reserve(
					lightPositionInitializerPreamble.size() +
					lightColorInitializerPreamble.size() +
					lightRangeInitializerPreamble.size() +
					lightCount * (lightPositionInitializerItem.size() +
								  lightColorInitializerItem.size() +
								  lightRangeInitializerItem.size()));

				lightInitializer.append(lightPositionInitializerPreamble);
				for (std::size_t i = 0; i != lightCount; ++i)
					lightInitializer.append(lightPositionInitializerItem);

				/* Drop the last comma and add a newline at the end */
				lightInitializer[lightInitializer.size() - 2] = '\n';
				lightInitializer.resize(lightInitializer.size() - 1);

				lightInitializer.append(lightColorInitializerPreamble);
				for (std::size_t i = 0; i != lightCount; ++i)
					lightInitializer.append(lightColorInitializerItem);

				/* Drop the last comma and add a newline at the end */
				lightInitializer[lightInitializer.size() - 2] = '\n';
				lightInitializer.resize(lightInitializer.size() - 1);

				lightInitializer.append(lightRangeInitializerPreamble);
				for (std::size_t i = 0; i != lightCount; ++i)
					lightInitializer.append(lightRangeInitializerItem);

				/* Drop the last comma and add a newline at the end */
				lightInitializer[lightInitializer.size() - 2] = '\n';
				lightInitializer.resize(lightInitializer.size() - 1);
			}
#endif

			vert.addSource((flags & (Flag::AmbientTexture | Flag::DiffuseTexture | Flag::SpecularTexture | Flag::NormalTexture)
#ifndef MAGNUM_TARGET_GLES2
							|| flags >= Flag::ObjectIdTexture
#endif
							)
							   ? "#define TEXTURED\n"
							   : "")
				.addSource(flags & Flag::NormalTexture ? "#define NORMAL_TEXTURE\n" : "")
				.addSource(flags & Flag::Bitangent ? "#define BITANGENT\n" : "")
				.addSource(flags & Flag::VertexColor ? "#define VERTEX_COLOR\n" : "")
				.addSource(flags & Flag::TextureTransformation ? "#define TEXTURE_TRANSFORMATION\n" : "")
#ifndef MAGNUM_TARGET_GLES2
				.addSource(flags & Flag::TextureArrays ? "#define TEXTURE_ARRAYS\n" : "")
#endif
				.addSource(lightCount ? "#define HAS_LIGHTS\n" : "")
#ifndef MAGNUM_TARGET_GLES2
				.addSource(flags >= Flag::InstancedObjectId ? "#define INSTANCED_OBJECT_ID\n" : "")
#endif
				.addSource(flags & Flag::InstancedTransformation ? "#define INSTANCED_TRANSFORMATION\n" : "")
				.addSource(flags >= Flag::InstancedTextureOffset ? "#define INSTANCED_TEXTURE_OFFSET\n" : "");

			if (flags & Flag::Shadows)
			{
				vert.addSource(flags & Flag::Shadows ? "#define HAS_SHADOWS\n" : "")
					.addSource("#define NUM_SHADOW_MAP_LEVELS " + std::to_string(numShadowLevels) + "\n");
			}

#ifndef MAGNUM_TARGET_GLES2
			if (flags >= Flag::UniformBuffers)
			{
				vert.addSource(Utility::formatString(
					"#define UNIFORM_BUFFERS\n"
					"#define DRAW_COUNT {}\n",
					drawCount,
					lightCount));
				vert.addSource(flags >= Flag::MultiDraw ? "#define MULTI_DRAW\n" : "");
			}
#endif
			vert.addSource(magnumRS.getString("generic.glsl"))
				.addSource(localRS.getString("PhongShadow.vert"));

			frag.addSource(flags & Flag::AmbientTexture ? "#define AMBIENT_TEXTURE\n" : "")
				.addSource(flags & Flag::DiffuseTexture ? "#define DIFFUSE_TEXTURE\n" : "")
				.addSource(flags & Flag::SpecularTexture ? "#define SPECULAR_TEXTURE\n" : "")
				.addSource(flags & Flag::NormalTexture ? "#define NORMAL_TEXTURE\n" : "")
#ifndef MAGNUM_TARGET_GLES2
				.addSource(flags & Flag::TextureArrays ? "#define TEXTURE_ARRAYS\n" : "")
#endif
				.addSource(flags & Flag::Bitangent ? "#define BITANGENT\n" : "")
				.addSource(flags & Flag::VertexColor ? "#define VERTEX_COLOR\n" : "")
				.addSource(flags & Flag::AlphaMask ? "#define ALPHA_MASK\n" : "")
#ifndef MAGNUM_TARGET_GLES2
				.addSource(flags & Flag::ObjectId ? "#define OBJECT_ID\n" : "")
				.addSource(flags >= Flag::InstancedObjectId ? "#define INSTANCED_OBJECT_ID\n" : "")
				.addSource(flags >= Flag::ObjectIdTexture ? "#define OBJECT_ID_TEXTURE\n" : "")
#endif
				.addSource(flags & Flag::NoSpecular ? "#define NO_SPECULAR\n" : "");

			if (flags & Flag::Shadows)
			{
				vert.addSource(flags & Flag::Shadows ? "#define HAS_SHADOWS\n" : "")
					.addSource("#define NUM_SHADOW_MAP_LEVELS " + std::to_string(numShadowLevels) + "\n");
			}

#ifndef MAGNUM_TARGET_GLES2
			if (flags >= Flag::UniformBuffers)
			{
				frag.addSource(Utility::formatString(
					"#define UNIFORM_BUFFERS\n"
					"#define DRAW_COUNT {}\n"
					"#define MATERIAL_COUNT {}\n"
					"#define LIGHT_COUNT {}\n",
					drawCount,
					materialCount,
					lightCount));
				frag.addSource(flags >= Flag::MultiDraw ? "#define MULTI_DRAW\n" : "")
					.addSource(flags >= Flag::LightCulling ? "#define LIGHT_CULLING\n" : "");
			}
			else
#endif
			{
				frag.addSource(Utility::formatString(
					"#define LIGHT_COUNT {}\n"
					"#define LIGHT_COLORS_LOCATION {}\n"
					"#define LIGHT_SPECULAR_COLORS_LOCATION {}\n"
					"#define LIGHT_RANGES_LOCATION {}\n",
					lightCount,
					out._lightPositionsUniform + lightCount,
					out._lightPositionsUniform + 2 * lightCount,
					out._lightPositionsUniform + 3 * lightCount));
			}
#ifndef MAGNUM_TARGET_GLES
			if (!(flags >= Flag::UniformBuffers) && lightCount)
				frag.addSource(std::move(lightInitializer));
#endif
			frag.addSource(magnumRS.getString("generic.glsl"))
				.addSource(localRS.getString("PhongShadow.frag"));

			vert.submitCompile();
			frag.submitCompile();

			out.attachShaders({vert, frag});

/* ES3 has this done in the shader directly and doesn't even provide
   bindFragmentDataLocation() */
#if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
#ifndef MAGNUM_TARGET_GLES
			if (!context.isExtensionSupported<GL::Extensions::ARB::explicit_attrib_location>(version))
#endif
			{
				out.bindAttributeLocation(Position::Location, "position");
				if (lightCount)
					out.bindAttributeLocation(Normal::Location, "normal");
				if ((flags & Flag::NormalTexture) && lightCount)
				{
					out.bindAttributeLocation(Tangent::Location, "tangent");
					if (flags & Flag::Bitangent)
						out.bindAttributeLocation(Bitangent::Location, "bitangent");
				}
				if (flags & Flag::VertexColor)
					out.bindAttributeLocation(Color3::Location, "vertexColor"); /* Color4 is the same */
				if (flags & (Flag::AmbientTexture | Flag::DiffuseTexture | Flag::SpecularTexture)
#ifndef MAGNUM_TARGET_GLES2
					|| flags >= Flag::ObjectIdTexture
#endif
				)
					out.bindAttributeLocation(TextureCoordinates::Location, "textureCoordinates");
#ifndef MAGNUM_TARGET_GLES2
				if (flags & Flag::ObjectId)
				{
					out.bindFragmentDataLocation(ColorOutput, "color");
					out.bindFragmentDataLocation(ObjectIdOutput, "objectId");
				}
				if (flags >= Flag::InstancedObjectId)
					out.bindAttributeLocation(ObjectId::Location, "instanceObjectId");
#endif
				if (flags & Flag::InstancedTransformation)
				{
					out.bindAttributeLocation(TransformationMatrix::Location, "instancedTransformationMatrix");
					if (lightCount)
						out.bindAttributeLocation(NormalMatrix::Location, "instancedNormalMatrix");
				}
				if (flags >= Flag::InstancedTextureOffset)
					out.bindAttributeLocation(TextureOffset::Location, "instancedTextureOffset");
			}
#endif

			out.submitLink();

			return CompileState{std::move(out), std::move(vert), std::move(frag), version};
		}

		PhongGLShadows::PhongGLShadows(CompileState &&state) : PhongGLShadows{static_cast<PhongGLShadows &&>(std::move(state))}
		{
#ifdef CORRADE_GRACEFUL_ASSERT
			/* When graceful assertions fire from within compile(), we get a NoCreate'd
			   CompileState. Exiting makes it possible to test the assert. */
			if (!id())
				return;
#endif

			CORRADE_INTERNAL_ASSERT_OUTPUT(checkLink({GL::Shader(state._vert), GL::Shader(state._frag)}));

			const GL::Context &context = GL::Context::current();
			const GL::Version version = state._version;

#ifndef MAGNUM_TARGET_GLES
			if (!context.isExtensionSupported<GL::Extensions::ARB::explicit_uniform_location>(version))
#endif
			{
#ifndef MAGNUM_TARGET_GLES2
				if (_flags >= Flag::UniformBuffers)
				{
					if (_drawCount > 1)
						_drawOffsetUniform = uniformLocation("drawOffset");
				}
				else
#endif
				{
					_transformationMatrixUniform = uniformLocation("transformationMatrix");
					if (_flags & Flag::TextureTransformation)
						_textureMatrixUniform = uniformLocation("textureMatrix");
#ifndef MAGNUM_TARGET_GLES2
					if (_flags & Flag::TextureArrays)
						_textureLayerUniform = uniformLocation("textureLayer");
#endif
					_projectionMatrixUniform = uniformLocation("projectionMatrix");
					_ambientColorUniform = uniformLocation("ambientColor");
					if (_lightCount)
					{
						_normalMatrixUniform = uniformLocation("normalMatrix");
						_diffuseColorUniform = uniformLocation("diffuseColor");
						if (!(_flags & Flag::NoSpecular))
						{
							_specularColorUniform = uniformLocation("specularColor");
							_shininessUniform = uniformLocation("shininess");
						}
						if (_flags & Flag::NormalTexture)
							_normalTextureScaleUniform = uniformLocation("normalTextureScale");
						_lightPositionsUniform = uniformLocation("lightPositions");
						_lightColorsUniform = uniformLocation("lightColors");
						if (!(_flags & Flag::NoSpecular))
							_lightSpecularColorsUniform = uniformLocation("lightSpecularColors");
						_lightRangesUniform = uniformLocation("lightRanges");
					}
					if (_flags & Flag::AlphaMask)
						_alphaMaskUniform = uniformLocation("alphaMask");
#ifndef MAGNUM_TARGET_GLES2
					if (_flags & Flag::ObjectId)
						_objectIdUniform = uniformLocation("objectId");
#endif
				}
			}

#ifndef MAGNUM_TARGET_GLES
			if (_flags && !context.isExtensionSupported<GL::Extensions::ARB::shading_language_420pack>(version))
#endif
			{
				if (_flags & Flag::AmbientTexture)
					setUniform(uniformLocation("ambientTexture"), AmbientTextureUnit);
				if (_lightCount)
				{
					if (_flags & Flag::DiffuseTexture)
						setUniform(uniformLocation("diffuseTexture"), DiffuseTextureUnit);
					if (_flags & Flag::SpecularTexture)
						setUniform(uniformLocation("specularTexture"), SpecularTextureUnit);
					if (_flags & Flag::NormalTexture)
						setUniform(uniformLocation("normalTexture"), NormalTextureUnit);
				}
#ifndef MAGNUM_TARGET_GLES2
				if (_flags >= Flag::ObjectIdTexture)
					setUniform(uniformLocation("objectIdTextureData"), ObjectIdTextureUnit);
				if (_flags >= Flag::UniformBuffers)
				{
					setUniformBlockBinding(uniformBlockIndex("Projection"), ProjectionBufferBinding);
					setUniformBlockBinding(uniformBlockIndex("Transformation"), TransformationBufferBinding);
					setUniformBlockBinding(uniformBlockIndex("Draw"), DrawBufferBinding);
					setUniformBlockBinding(uniformBlockIndex("Material"), MaterialBufferBinding);
					if (_flags & Flag::TextureTransformation)
						setUniformBlockBinding(uniformBlockIndex("TextureTransformation"), TextureTransformationBufferBinding);
					if (_lightCount)
						setUniformBlockBinding(uniformBlockIndex("Light"), LightBufferBinding);
				}
#endif
			}

/* Set defaults in OpenGL ES (for desktop they are set in shader code itself) */
#ifdef MAGNUM_TARGET_GLES
#ifndef MAGNUM_TARGET_GLES2
			if (_flags >= Flag::UniformBuffers)
			{
				/* Draw offset is zero by default */
			}
			else
#endif
			{
				/* Default to fully opaque white so we can see the textures */
				if (_flags & Flag::AmbientTexture)
					setAmbientColor(Magnum::Color4{1.0f});
				else
					setAmbientColor(Magnum::Color4{0.0f});
				setTransformationMatrix(Matrix4{Math::IdentityInit});
				setProjectionMatrix(Matrix4{Math::IdentityInit});
				if (_lightCount)
				{
					setDiffuseColor(Magnum::Color4{1.0f});
					if (!(_flags & Flag::NoSpecular))
					{
						setSpecularColor(Magnum::Color4{1.0f, 0.0f});
						setShininess(80.0f);
					}
					if (_flags & Flag::NormalTexture)
						setNormalTextureScale(1.0f);
					setLightPositions(Containers::Array<Vector4>{DirectInit, _lightCount, Vector4{0.0f, 0.0f, 1.0f, 0.0f}});
					Containers::Array<Magnum::Color3> colors{DirectInit, _lightCount, Magnum::Color3{1.0f}};
					setLightColors(colors);
					if (!(_flags & Flag::NoSpecular))
						setLightSpecularColors(colors);
					setLightRanges(Containers::Array<Float>{DirectInit, _lightCount, Constants::inf()});
					/* Light position is zero by default */
					setNormalMatrix(Matrix3x3{Math::IdentityInit});
				}
				if (_flags & Flag::TextureTransformation)
					setTextureMatrix(Matrix3{Math::IdentityInit});
				/* Texture layer is zero by default */
				if (_flags & Flag::AlphaMask)
					setAlphaMask(0.5f);
				/* Object ID is zero by default */
			}
#endif

			static_cast<void>(context);
			static_cast<void>(version);

			setUniform(uniformLocation("shadowmapTexture"), ShadowmapTextureLayer);
			_shadowmapMatrixUniform = uniformLocation("shadowmapMatrix");
			_shadowBiasUniform = uniformLocation("shadowBias");
		}

		PhongGLShadows::PhongGLShadows(const Flags flags, const UnsignedInt lightCount, const UnsignedInt numShadowLevels) : PhongGLShadows{compile(flags, lightCount, numShadowLevels)} {}

#ifndef MAGNUM_TARGET_GLES2
		PhongGLShadows::CompileState PhongGLShadows::compile(const Flags flags, const UnsignedInt lightCount, const UnsignedInt numShadowLevels)
		{
			return compile(flags, lightCount, numShadowLevels, 1, 1);
		}

		PhongGLShadows::PhongGLShadows(const Flags flags, const UnsignedInt lightCount, const UnsignedInt materialCount, const UnsignedInt drawCount, const UnsignedInt numShadowLevels) : PhongGLShadows{compile(flags, lightCount, numShadowLevels, materialCount, drawCount)} {}
#endif

		PhongGLShadows &PhongGLShadows::setAmbientColor(const Magnum::Color4 &color)
		{
#ifndef MAGNUM_TARGET_GLES2
			CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
						   "Shaders::PhongGL::setAmbientColor(): the shader was created with uniform buffers enabled", *this);
#endif
			setUniform(_ambientColorUniform, color);
			return *this;
		}

		PhongGLShadows &PhongGLShadows::setDiffuseColor(const Magnum::Color4 &color)
		{
#ifndef MAGNUM_TARGET_GLES2
			CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
						   "Shaders::PhongGL::setDiffuseColor(): the shader was created with uniform buffers enabled", *this);
#endif
			if (_lightCount)
				setUniform(_diffuseColorUniform, color);
			return *this;
		}

		PhongGLShadows &PhongGLShadows::setSpecularColor(const Magnum::Color4 &color)
		{
#ifndef MAGNUM_TARGET_GLES2
			CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
						   "Shaders::PhongGL::setSpecularColor(): the shader was created with uniform buffers enabled", *this);
#endif
			CORRADE_ASSERT(!(_flags >= Flag::NoSpecular),
						   "Shaders::PhongGL::setSpecularColor(): the shader was created with specular disabled", *this);
			if (_lightCount)
				setUniform(_specularColorUniform, color);
			return *this;
		}

		PhongGLShadows &PhongGLShadows::setShininess(Float shininess)
		{
#ifndef MAGNUM_TARGET_GLES2
			CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
						   "Shaders::PhongGL::setShininess(): the shader was created with uniform buffers enabled", *this);
#endif
			CORRADE_ASSERT(!(_flags >= Flag::NoSpecular),
						   "Shaders::PhongGL::setShininess(): the shader was created with specular disabled", *this);
			if (_lightCount)
				setUniform(_shininessUniform, shininess);
			return *this;
		}

		PhongGLShadows &PhongGLShadows::setNormalTextureScale(const Float scale)
		{
#ifndef MAGNUM_TARGET_GLES2
			CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
						   "Shaders::PhongGL::setNormalTextureScale(): the shader was created with uniform buffers enabled", *this);
#endif
			CORRADE_ASSERT(_flags & Flag::NormalTexture,
						   "Shaders::PhongGL::setNormalTextureScale(): the shader was not created with normal texture enabled", *this);
			if (_lightCount)
				setUniform(_normalTextureScaleUniform, scale);
			return *this;
		}

		PhongGLShadows &PhongGLShadows::setAlphaMask(Float mask)
		{
#ifndef MAGNUM_TARGET_GLES2
			CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
						   "Shaders::PhongGL::setAlphaMask(): the shader was created with uniform buffers enabled", *this);
#endif
			CORRADE_ASSERT(_flags & Flag::AlphaMask,
						   "Shaders::PhongGL::setAlphaMask(): the shader was not created with alpha mask enabled", *this);
			setUniform(_alphaMaskUniform, mask);
			return *this;
		}

#ifndef MAGNUM_TARGET_GLES2
		PhongGLShadows &PhongGLShadows::setObjectId(UnsignedInt id)
		{
			CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
						   "Shaders::PhongGL::setObjectId(): the shader was created with uniform buffers enabled", *this);
			CORRADE_ASSERT(_flags & Flag::ObjectId,
						   "Shaders::PhongGL::setObjectId(): the shader was not created with object ID enabled", *this);
			setUniform(_objectIdUniform, id);
			return *this;
		}
#endif

		PhongGLShadows &PhongGLShadows::setTransformationMatrix(const Matrix4 &matrix)
		{
#ifndef MAGNUM_TARGET_GLES2
			CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
						   "Shaders::PhongGL::setTransformationMatrix(): the shader was created with uniform buffers enabled", *this);
#endif
			setUniform(_transformationMatrixUniform, matrix);
			return *this;
		}

		PhongGLShadows &PhongGLShadows::setNormalMatrix(const Matrix3x3 &matrix)
		{
#ifndef MAGNUM_TARGET_GLES2
			CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
						   "Shaders::PhongGL::setNormalMatrix(): the shader was created with uniform buffers enabled", *this);
#endif
			if (_lightCount)
				setUniform(_normalMatrixUniform, matrix);
			return *this;
		}

		PhongGLShadows &PhongGLShadows::setProjectionMatrix(const Matrix4 &matrix)
		{
#ifndef MAGNUM_TARGET_GLES2
			CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
						   "Shaders::PhongGL::setProjectionMatrix(): the shader was created with uniform buffers enabled", *this);
#endif
			setUniform(_projectionMatrixUniform, matrix);
			return *this;
		}

		PhongGLShadows &PhongGLShadows::setTextureMatrix(const Matrix3 &matrix)
		{
#ifndef MAGNUM_TARGET_GLES2
			CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
						   "Shaders::PhongGL::setTextureMatrix(): the shader was created with uniform buffers enabled", *this);
#endif
			CORRADE_ASSERT(_flags & Flag::TextureTransformation,
						   "Shaders::PhongGL::setTextureMatrix(): the shader was not created with texture transformation enabled", *this);
			setUniform(_textureMatrixUniform, matrix);
			return *this;
		}

#ifndef MAGNUM_TARGET_GLES2
		PhongGLShadows &PhongGLShadows::setTextureLayer(UnsignedInt id)
		{
			CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
						   "Shaders::PhongGL::setTextureLayer(): the shader was created with uniform buffers enabled", *this);
			CORRADE_ASSERT(_flags & Flag::TextureArrays,
						   "Shaders::PhongGL::setTextureLayer(): the shader was not created with texture arrays enabled", *this);
			setUniform(_textureLayerUniform, id);
			return *this;
		}
#endif

		PhongGLShadows &PhongGLShadows::setLightPositions(const Containers::ArrayView<const Vector4> positions)
		{
#ifndef MAGNUM_TARGET_GLES2
			CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
						   "Shaders::PhongGL::setLightPositions(): the shader was created with uniform buffers enabled", *this);
#endif
			CORRADE_ASSERT(_lightCount == positions.size(),
						   "Shaders::PhongGL::setLightPositions(): expected" << _lightCount << "items but got" << positions.size(), *this);
			if (_lightCount)
				setUniform(_lightPositionsUniform, positions);
			return *this;
		}

		/* It's light, but can't be in the header because MSVC needs to know the size
		   of Vector3 for the initializer list use */
		PhongGLShadows &PhongGLShadows::setLightPositions(const std::initializer_list<Vector4> positions)
		{
			return setLightPositions(Containers::arrayView(positions));
		}

#ifdef MAGNUM_BUILD_DEPRECATED
		PhongGLShadows &PhongGLShadows::setLightPositions(const Containers::ArrayView<const Vector3> positions)
		{
			Containers::Array<Vector4> fourComponent{NoInit, positions.size()};
			for (std::size_t i = 0; i != positions.size(); ++i)
				fourComponent[i] = Vector4{positions[i], 0.0f};
			setLightPositions(fourComponent);
			return *this;
		}

		PhongGLShadows &PhongGLShadows::setLightPositions(const std::initializer_list<Vector3> positions)
		{
			CORRADE_IGNORE_DEPRECATED_PUSH
			return setLightPositions(Containers::arrayView(positions));
			CORRADE_IGNORE_DEPRECATED_POP
		}
#endif

		PhongGLShadows &PhongGLShadows::setLightPosition(const UnsignedInt id, const Vector4 &position)
		{
#ifndef MAGNUM_TARGET_GLES2
			CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
						   "Shaders::PhongGL::setLightPosition(): the shader was created with uniform buffers enabled", *this);
#endif
			CORRADE_ASSERT(id < _lightCount,
						   "Shaders::PhongGL::setLightPosition(): light ID" << id << "is out of bounds for" << _lightCount << "lights", *this);
			setUniform(_lightPositionsUniform + id, position);
			return *this;
		}

#ifdef MAGNUM_BUILD_DEPRECATED
		PhongGLShadows &PhongGLShadows::setLightPosition(UnsignedInt id, const Vector3 &position)
		{
			return setLightPosition(id, Vector4{position, 0.0f});
		}

		PhongGLShadows &PhongGLShadows::setLightPosition(const Vector3 &position)
		{
			/* Use the list variant to check the shader really has just one light */
			return setLightPositions({Vector4{position, 0.0f}});
		}
#endif

		PhongGLShadows &PhongGLShadows::setLightColors(const Containers::ArrayView<const Magnum::Color3> colors)
		{
#ifndef MAGNUM_TARGET_GLES2
			CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
						   "Shaders::PhongGL::setLightColors(): the shader was created with uniform buffers enabled", *this);
#endif
			CORRADE_ASSERT(_lightCount == colors.size(),
						   "Shaders::PhongGL::setLightColors(): expected" << _lightCount << "items but got" << colors.size(), *this);
			if (_lightCount)
				setUniform(_lightColorsUniform, colors);
			return *this;
		}

#ifdef MAGNUM_BUILD_DEPRECATED
		PhongGLShadows &PhongGLShadows::setLightColors(const Containers::ArrayView<const Magnum::Color4> colors)
		{
			Containers::Array<Magnum::Color3> threeComponent{NoInit, colors.size()};
			for (std::size_t i = 0; i != colors.size(); ++i)
				threeComponent[i] = colors[i].rgb();
			setLightColors(threeComponent);
			return *this;
		}

		PhongGLShadows &PhongGLShadows::setLightColors(const std::initializer_list<Magnum::Color4> colors)
		{
			CORRADE_IGNORE_DEPRECATED_PUSH
			return setLightColors(Containers::arrayView(colors));
			CORRADE_IGNORE_DEPRECATED_POP
		}
#endif

		PhongGLShadows &PhongGLShadows::setLightColors(const std::initializer_list<Magnum::Color3> colors)
		{
			return setLightColors(Containers::arrayView(colors));
		}

		PhongGLShadows &PhongGLShadows::setLightColor(const UnsignedInt id, const Magnum::Color3 &color)
		{
#ifndef MAGNUM_TARGET_GLES2
			CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
						   "Shaders::PhongGL::setLightColor(): the shader was created with uniform buffers enabled", *this);
#endif
			CORRADE_ASSERT(id < _lightCount,
						   "Shaders::PhongGL::setLightColor(): light ID" << id << "is out of bounds for" << _lightCount << "lights", *this);
			setUniform(_lightColorsUniform + id, color);
			return *this;
		}

#ifdef MAGNUM_BUILD_DEPRECATED
		PhongGLShadows &PhongGLShadows::setLightColor(UnsignedInt id, const Magnum::Color4 &color)
		{
			return setLightColor(id, color.rgb());
		}

		PhongGLShadows &PhongGLShadows::setLightColor(const Magnum::Color4 &color)
		{
			/* Use the list variant to check the shader really has just one light */
			return setLightColors({color.rgb()});
		}
#endif

		PhongGLShadows &PhongGLShadows::setLightSpecularColors(const Containers::ArrayView<const Magnum::Color3> colors)
		{
#ifndef MAGNUM_TARGET_GLES2
			CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
						   "Shaders::PhongGL::setLightSpecularColors(): the shader was created with uniform buffers enabled", *this);
#endif
			CORRADE_ASSERT(_lightCount == colors.size(),
						   "Shaders::PhongGL::setLightSpecularColors(): expected" << _lightCount << "items but got" << colors.size(), *this);
			CORRADE_ASSERT(!(_flags >= Flag::NoSpecular),
						   "Shaders::PhongGL::setLightSpecularColors(): the shader was created with specular disabled", *this);
			if (_lightCount)
				setUniform(_lightSpecularColorsUniform, colors);
			return *this;
		}

		PhongGLShadows &PhongGLShadows::setLightSpecularColors(const std::initializer_list<Magnum::Color3> colors)
		{
			return setLightSpecularColors(Containers::arrayView(colors));
		}

		PhongGLShadows &PhongGLShadows::setLightSpecularColor(const UnsignedInt id, const Magnum::Color3 &color)
		{
#ifndef MAGNUM_TARGET_GLES2
			CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
						   "Shaders::PhongGL::setLightSpecularColor(): the shader was created with uniform buffers enabled", *this);
#endif
			CORRADE_ASSERT(id < _lightCount,
						   "Shaders::PhongGL::setLightSpecularColor(): light ID" << id << "is out of bounds for" << _lightCount << "lights", *this);
			CORRADE_ASSERT(!(_flags >= Flag::NoSpecular),
						   "Shaders::PhongGL::setLightSpecularColor(): the shader was created with specular disabled", *this);
			setUniform(_lightSpecularColorsUniform + id, color);
			return *this;
		}

		PhongGLShadows &PhongGLShadows::setLightRanges(const Containers::ArrayView<const Float> ranges)
		{
#ifndef MAGNUM_TARGET_GLES2
			CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
						   "Shaders::PhongGL::setLightRanges(): the shader was created with uniform buffers enabled", *this);
#endif
			CORRADE_ASSERT(_lightCount == ranges.size(),
						   "Shaders::PhongGL::setLightRanges(): expected" << _lightCount << "items but got" << ranges.size(), *this);
			if (_lightCount)
				setUniform(_lightRangesUniform, ranges);
			return *this;
		}

		PhongGLShadows &PhongGLShadows::setLightRanges(const std::initializer_list<Float> ranges)
		{
			return setLightRanges(Containers::arrayView(ranges));
		}

		PhongGLShadows &PhongGLShadows::setLightRange(const UnsignedInt id, const Float range)
		{
#ifndef MAGNUM_TARGET_GLES2
			CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
						   "Shaders::PhongGL::setLightRange(): the shader was created with uniform buffers enabled", *this);
#endif
			CORRADE_ASSERT(id < _lightCount,
						   "Shaders::PhongGL::setLightRange(): light ID" << id << "is out of bounds for" << _lightCount << "lights", *this);
			setUniform(_lightRangesUniform + id, range);
			return *this;
		}

#ifndef MAGNUM_TARGET_GLES2
		PhongGLShadows &PhongGLShadows::setDrawOffset(const UnsignedInt offset)
		{
			CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
						   "Shaders::PhongGL::setDrawOffset(): the shader was not created with uniform buffers enabled", *this);
			CORRADE_ASSERT(offset < _drawCount,
						   "Shaders::PhongGL::setDrawOffset(): draw offset" << offset << "is out of bounds for" << _drawCount << "draws", *this);
			if (_drawCount > 1)
				setUniform(_drawOffsetUniform, offset);
			return *this;
		}

		PhongGLShadows &PhongGLShadows::bindProjectionBuffer(GL::Buffer &buffer)
		{
			CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
						   "Shaders::PhongGL::bindProjectionBuffer(): the shader was not created with uniform buffers enabled", *this);
			buffer.bind(GL::Buffer::Target::Uniform, ProjectionBufferBinding);
			return *this;
		}

		PhongGLShadows &PhongGLShadows::bindProjectionBuffer(GL::Buffer &buffer, const GLintptr offset, const GLsizeiptr size)
		{
			CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
						   "Shaders::PhongGL::bindProjectionBuffer(): the shader was not created with uniform buffers enabled", *this);
			buffer.bind(GL::Buffer::Target::Uniform, ProjectionBufferBinding, offset, size);
			return *this;
		}

		PhongGLShadows &PhongGLShadows::bindTransformationBuffer(GL::Buffer &buffer)
		{
			CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
						   "Shaders::PhongGL::bindTransformationBuffer(): the shader was not created with uniform buffers enabled", *this);
			buffer.bind(GL::Buffer::Target::Uniform, TransformationBufferBinding);
			return *this;
		}

		PhongGLShadows &PhongGLShadows::bindTransformationBuffer(GL::Buffer &buffer, const GLintptr offset, const GLsizeiptr size)
		{
			CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
						   "Shaders::PhongGL::bindTransformationBuffer(): the shader was not created with uniform buffers enabled", *this);
			buffer.bind(GL::Buffer::Target::Uniform, TransformationBufferBinding, offset, size);
			return *this;
		}

		PhongGLShadows &PhongGLShadows::bindDrawBuffer(GL::Buffer &buffer)
		{
			CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
						   "Shaders::PhongGL::bindDrawBuffer(): the shader was not created with uniform buffers enabled", *this);
			buffer.bind(GL::Buffer::Target::Uniform, DrawBufferBinding);
			return *this;
		}

		PhongGLShadows &PhongGLShadows::bindDrawBuffer(GL::Buffer &buffer, const GLintptr offset, const GLsizeiptr size)
		{
			CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
						   "Shaders::PhongGL::bindDrawBuffer(): the shader was not created with uniform buffers enabled", *this);
			buffer.bind(GL::Buffer::Target::Uniform, DrawBufferBinding, offset, size);
			return *this;
		}

		PhongGLShadows &PhongGLShadows::bindTextureTransformationBuffer(GL::Buffer &buffer)
		{
			CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
						   "Shaders::PhongGL::bindTextureTransformationBuffer(): the shader was not created with uniform buffers enabled", *this);
			CORRADE_ASSERT(_flags & Flag::TextureTransformation,
						   "Shaders::PhongGL::bindTextureTransformationBuffer(): the shader was not created with texture transformation enabled", *this);
			buffer.bind(GL::Buffer::Target::Uniform, TextureTransformationBufferBinding);
			return *this;
		}

		PhongGLShadows &PhongGLShadows::bindTextureTransformationBuffer(GL::Buffer &buffer, const GLintptr offset, const GLsizeiptr size)
		{
			CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
						   "Shaders::PhongGL::bindTextureTransformationBuffer(): the shader was not created with uniform buffers enabled", *this);
			CORRADE_ASSERT(_flags & Flag::TextureTransformation,
						   "Shaders::PhongGL::bindTextureTransformationBuffer(): the shader was not created with texture transformation enabled", *this);
			buffer.bind(GL::Buffer::Target::Uniform, TextureTransformationBufferBinding, offset, size);
			return *this;
		}

		PhongGLShadows &PhongGLShadows::bindMaterialBuffer(GL::Buffer &buffer)
		{
			CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
						   "Shaders::PhongGL::bindMaterialBuffer(): the shader was not created with uniform buffers enabled", *this);
			buffer.bind(GL::Buffer::Target::Uniform, MaterialBufferBinding);
			return *this;
		}

		PhongGLShadows &PhongGLShadows::bindMaterialBuffer(GL::Buffer &buffer, const GLintptr offset, const GLsizeiptr size)
		{
			CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
						   "Shaders::PhongGL::bindMaterialBuffer(): the shader was not created with uniform buffers enabled", *this);
			buffer.bind(GL::Buffer::Target::Uniform, MaterialBufferBinding, offset, size);
			return *this;
		}

		PhongGLShadows &PhongGLShadows::bindLightBuffer(GL::Buffer &buffer)
		{
			CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
						   "Shaders::PhongGL::bindLightBuffer(): the shader was not created with uniform buffers enabled", *this);
			buffer.bind(GL::Buffer::Target::Uniform, LightBufferBinding);
			return *this;
		}

		PhongGLShadows &PhongGLShadows::bindLightBuffer(GL::Buffer &buffer, const GLintptr offset, const GLsizeiptr size)
		{
			CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
						   "Shaders::PhongGL::bindLightBuffer(): the shader was not created with uniform buffers enabled", *this);
			buffer.bind(GL::Buffer::Target::Uniform, LightBufferBinding, offset, size);
			return *this;
		}
#endif

		PhongGLShadows &PhongGLShadows::bindAmbientTexture(GL::Texture2D &texture)
		{
			CORRADE_ASSERT(_flags & Flag::AmbientTexture,
						   "Shaders::PhongGL::bindAmbientTexture(): the shader was not created with ambient texture enabled", *this);
#ifndef MAGNUM_TARGET_GLES2
			CORRADE_ASSERT(!(_flags & Flag::TextureArrays),
						   "Shaders::PhongGL::bindAmbientTexture(): the shader was created with texture arrays enabled, use a Texture2DArray instead", *this);
#endif
			texture.bind(AmbientTextureUnit);
			return *this;
		}

#ifndef MAGNUM_TARGET_GLES2
		PhongGLShadows &PhongGLShadows::bindAmbientTexture(GL::Texture2DArray &texture)
		{
			CORRADE_ASSERT(_flags & Flag::AmbientTexture,
						   "Shaders::PhongGL::bindAmbientTexture(): the shader was not created with ambient texture enabled", *this);
			CORRADE_ASSERT(_flags & Flag::TextureArrays,
						   "Shaders::PhongGL::bindAmbientTexture(): the shader was not created with texture arrays enabled, use a Texture2D instead", *this);
			texture.bind(AmbientTextureUnit);
			return *this;
		}
#endif

		PhongGLShadows &PhongGLShadows::bindDiffuseTexture(GL::Texture2D &texture)
		{
			CORRADE_ASSERT(_flags & Flag::DiffuseTexture,
						   "Shaders::PhongGL::bindDiffuseTexture(): the shader was not created with diffuse texture enabled", *this);
#ifndef MAGNUM_TARGET_GLES2
			CORRADE_ASSERT(!(_flags & Flag::TextureArrays),
						   "Shaders::PhongGL::bindDiffuseTexture(): the shader was created with texture arrays enabled, use a Texture2DArray instead", *this);
#endif
			if (_lightCount)
				texture.bind(DiffuseTextureUnit);
			return *this;
		}

#ifndef MAGNUM_TARGET_GLES2
		PhongGLShadows &PhongGLShadows::bindDiffuseTexture(GL::Texture2DArray &texture)
		{
			CORRADE_ASSERT(_flags & Flag::DiffuseTexture,
						   "Shaders::PhongGL::bindDiffuseTexture(): the shader was not created with diffuse texture enabled", *this);
			CORRADE_ASSERT(_flags & Flag::TextureArrays,
						   "Shaders::PhongGL::bindDiffuseTexture(): the shader was not created with texture arrays enabled, use a Texture2D instead", *this);
			if (_lightCount)
				texture.bind(DiffuseTextureUnit);
			return *this;
		}
#endif

		PhongGLShadows &PhongGLShadows::bindSpecularTexture(GL::Texture2D &texture)
		{
			CORRADE_ASSERT(_flags & Flag::SpecularTexture,
						   "Shaders::PhongGL::bindSpecularTexture(): the shader was not created with specular texture enabled", *this);
#ifndef MAGNUM_TARGET_GLES2
			CORRADE_ASSERT(!(_flags & Flag::TextureArrays),
						   "Shaders::PhongGL::bindSpecularTexture(): the shader was created with texture arrays enabled, use a Texture2DArray instead", *this);
#endif
			if (_lightCount)
				texture.bind(SpecularTextureUnit);
			return *this;
		}

#ifndef MAGNUM_TARGET_GLES2
		PhongGLShadows &PhongGLShadows::bindSpecularTexture(GL::Texture2DArray &texture)
		{
			CORRADE_ASSERT(_flags & Flag::SpecularTexture,
						   "Shaders::PhongGL::bindSpecularTexture(): the shader was not created with specular texture enabled", *this);
			CORRADE_ASSERT(_flags & Flag::TextureArrays,
						   "Shaders::PhongGL::bindSpecularTexture(): the shader was not created with texture arrays enabled, use a Texture2D instead", *this);
			if (_lightCount)
				texture.bind(SpecularTextureUnit);
			return *this;
		}
#endif

		PhongGLShadows &PhongGLShadows::bindNormalTexture(GL::Texture2D &texture)
		{
			CORRADE_ASSERT(_flags & Flag::NormalTexture,
						   "Shaders::PhongGL::bindNormalTexture(): the shader was not created with normal texture enabled", *this);
#ifndef MAGNUM_TARGET_GLES2
			CORRADE_ASSERT(!(_flags & Flag::TextureArrays),
						   "Shaders::PhongGL::bindNormalTexture(): the shader was created with texture arrays enabled, use a Texture2DArray instead", *this);
#endif
			if (_lightCount)
				texture.bind(NormalTextureUnit);
			return *this;
		}

#ifndef MAGNUM_TARGET_GLES2
		PhongGLShadows &PhongGLShadows::bindNormalTexture(GL::Texture2DArray &texture)
		{
			CORRADE_ASSERT(_flags & Flag::NormalTexture,
						   "Shaders::PhongGL::bindNormalTexture(): the shader was not created with normal texture enabled", *this);
			CORRADE_ASSERT(_flags & Flag::TextureArrays,
						   "Shaders::PhongGL::bindNormalTexture(): the shader was not created with texture arrays enabled, use a Texture2D instead", *this);
			if (_lightCount)
				texture.bind(NormalTextureUnit);
			return *this;
		}
#endif

#ifndef MAGNUM_TARGET_GLES2
		PhongGLShadows &PhongGLShadows::bindObjectIdTexture(GL::Texture2D &texture)
		{
			CORRADE_ASSERT(_flags >= Flag::ObjectIdTexture,
						   "Shaders::PhongGL::bindObjectIdTexture(): the shader was not created with object ID texture enabled", *this);
#ifndef MAGNUM_TARGET_GLES2
			CORRADE_ASSERT(!(_flags & Flag::TextureArrays),
						   "Shaders::PhongGL::bindObjectIdTexture(): the shader was created with texture arrays enabled, use a Texture2DArray instead", *this);
#endif
			texture.bind(ObjectIdTextureUnit);
			return *this;
		}

		PhongGLShadows &PhongGLShadows::bindObjectIdTexture(GL::Texture2DArray &texture)
		{
			CORRADE_ASSERT(_flags >= Flag::ObjectIdTexture,
						   "Shaders::PhongGL::bindObjectIdTexture(): the shader was not created with object ID texture enabled", *this);
			CORRADE_ASSERT(_flags & Flag::TextureArrays,
						   "Shaders::PhongGL::bindObjectIdTexture(): the shader was not created with texture arrays enabled, use a Texture2D instead", *this);
			texture.bind(ObjectIdTextureUnit);
			return *this;
		}
#endif

		PhongGLShadows &PhongGLShadows::bindTextures(GL::Texture2D *ambient, GL::Texture2D *diffuse, GL::Texture2D *specular, GL::Texture2D *normal)
		{
			CORRADE_ASSERT(_flags & (Flag::AmbientTexture | Flag::DiffuseTexture | Flag::SpecularTexture | Flag::NormalTexture),
						   "Shaders::PhongGL::bindTextures(): the shader was not created with any textures enabled", *this);
#ifndef MAGNUM_TARGET_GLES2
			CORRADE_ASSERT(!(_flags & Flag::TextureArrays),
						   "Shaders::PhongGL::bindTextures(): the shader was created with texture arrays enabled, use a Texture2DArray instead", *this);
#endif
			GL::AbstractTexture::bind(AmbientTextureUnit, {ambient, diffuse, specular, normal});
			return *this;
		}

		Debug &operator<<(Debug &debug, const PhongGLShadows::Flag value)
		{
#ifndef MAGNUM_TARGET_GLES2
			/* Special case coming from the Flags printer. As both flags are a superset
			   of ObjectId, printing just one would result in
			   `Flag::InstancedObjectId|Flag(0x20000)` in the output. */
			if (value == PhongGLShadows::Flag(UnsignedInt(PhongGLShadows::Flag::InstancedObjectId | PhongGLShadows::Flag::ObjectIdTexture)))
				return debug << PhongGLShadows::Flag::InstancedObjectId << Debug::nospace << "|" << Debug::nospace << PhongGLShadows::Flag::ObjectIdTexture;
#endif

			debug << "Shaders::PhongGL::Flag" << Debug::nospace;

			switch (value)
			{
/* LCOV_EXCL_START */
#define _c(v)                     \
	case PhongGLShadows::Flag::v: \
		return debug << "::" #v;
				_c(AmbientTexture)
					_c(DiffuseTexture)
						_c(SpecularTexture)
							_c(NormalTexture)
								_c(Bitangent)
									_c(AlphaMask)
										_c(VertexColor)
											_c(TextureTransformation)
#ifndef MAGNUM_TARGET_GLES2
												_c(ObjectId)
													_c(InstancedObjectId)
														_c(ObjectIdTexture)
#endif
															_c(InstancedTransformation)
																_c(InstancedTextureOffset)
#ifndef MAGNUM_TARGET_GLES2
																	_c(UniformBuffers)
																		_c(MultiDraw)
																			_c(TextureArrays)
																				_c(LightCulling)
#endif
																					_c(NoSpecular)
#undef _c
				/* LCOV_EXCL_STOP */
			}

			return debug << "(" << Debug::nospace << reinterpret_cast<void *>(UnsignedInt(value)) << Debug::nospace << ")";
		}

		Debug &operator<<(Debug &debug, const PhongGLShadows::Flags value)
		{
			return Containers::enumSetDebugOutput(debug, value, "Shaders::PhongGL::Flags{}", {PhongGLShadows::Flag::AmbientTexture, PhongGLShadows::Flag::DiffuseTexture, PhongGLShadows::Flag::SpecularTexture, PhongGLShadows::Flag::NormalTexture, PhongGLShadows::Flag::Bitangent, PhongGLShadows::Flag::AlphaMask, PhongGLShadows::Flag::VertexColor, PhongGLShadows::Flag::InstancedTextureOffset, /* Superset of TextureTransformation */
																							  PhongGLShadows::Flag::TextureTransformation,
#ifndef MAGNUM_TARGET_GLES2
																							  /* Both are a superset of ObjectId, meaning printing just one would
																								 result in `Flag::InstancedObjectId|Flag(0x20000)` in the output. So
																								 we pass both and let the Flag printer deal with that. */
																							  PhongGLShadows::Flag(UnsignedInt(PhongGLShadows::Flag::InstancedObjectId | PhongGLShadows::Flag::ObjectIdTexture)), PhongGLShadows::Flag::InstancedObjectId, /* Superset of ObjectId */
																							  PhongGLShadows::Flag::ObjectIdTexture,																													   /* Superset of ObjectId */
																							  PhongGLShadows::Flag::ObjectId,
#endif
																							  PhongGLShadows::Flag::InstancedTransformation,
#ifndef MAGNUM_TARGET_GLES2
																							  PhongGLShadows::Flag::MultiDraw, /* Superset of UniformBuffers */
																							  PhongGLShadows::Flag::UniformBuffers, PhongGLShadows::Flag::TextureArrays, PhongGLShadows::Flag::LightCulling,
#endif
																							  PhongGLShadows::Flag::NoSpecular});
		}

		PhongGLShadows &PhongGLShadows::setShadowmapTexture(GL::Texture2DArray &texture)
		{
			texture.bind(ShadowmapTextureLayer);
			return *this;
		}

		PhongGLShadows &PhongGLShadows::setShadowBias(const Float bias)
		{
			setUniform(_shadowBiasUniform, bias);
			return *this;
		}

		PhongGLShadows &PhongGLShadows::setShadowmapMatrices(const Containers::ArrayView<const Matrix4> matrices)
		{
			setUniform(_shadowmapMatrixUniform, matrices);
			return *this;
		}
	}
}