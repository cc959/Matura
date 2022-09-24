// A large part of the code sourced from Magnum: github.com/mosra/Magnum
// The PhongGL Shader with added shadows

#pragma once

#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/Shaders/GenericGL.h"
#include "Magnum/Shaders/glShaderWrapper.h"
#include "Magnum/Shaders/visibility.h"

namespace Magnum
{
	namespace Shaders
	{
		class MAGNUM_SHADERS_EXPORT PhongGLShadows : public GL::AbstractShaderProgram
		{
		public:
			class CompileState;

			/**
			 * @brief Vertex position
			 *
			 * @ref shaders-generic "Generic attribute",
			 * @ref Magnum::Vector3 "Vector3".
			 */
			typedef GenericGL3D::Position Position;

			/**
			 * @brief Normal direction
			 *
			 * @ref shaders-generic "Generic attribute",
			 * @ref Magnum::Vector3 "Vector3".
			 */
			typedef GenericGL3D::Normal Normal;

			/**
			 * @brief Tangent direction
			 * @m_since{2019,10}
			 *
			 * @ref shaders-generic "Generic attribute",
			 * @ref Magnum::Vector3 "Vector3". Use either this or the @ref Tangent4
			 * attribute. If only a three-component attribute is used and
			 * @ref Flag::Bitangent is not enabled, it's the same as if
			 * @ref Tangent4 was specified with the fourth component always being
			 * @cpp 1.0f @ce. Used only if @ref Flag::NormalTexture is set.
			 * @see @ref Shaders-PhongGL-normal-mapping
			 */
			typedef GenericGL3D::Tangent Tangent;

			/**
			 * @brief Tangent direction with a bitangent sign
			 * @m_since_latest
			 *
			 * @ref shaders-generic "Generic attribute",
			 * @ref Magnum::Vector4 "Vector4". Use either this or the @ref Tangent
			 * attribute. If @ref Flag::Bitangent is set, the fourth component is
			 * ignored and bitangents are taken from the @ref Bitangent attribute
			 * instead. Used only if @ref Flag::NormalTexture is set.
			 * @see @ref Shaders-PhongGL-normal-mapping
			 */
			typedef GenericGL3D::Tangent4 Tangent4;

			/**
			 * @brief Bitangent direction
			 * @m_since_latest
			 *
			 * @ref shaders-generic "Generic attribute",
			 * @ref Magnum::Vector3 "Vector3". Use either this or the @ref Tangent4
			 * attribute. Used only if both @ref Flag::NormalTexture and
			 * @ref Flag::Bitangent are set.
			 * @see @ref Shaders-PhongGL-normal-mapping
			 */
			typedef GenericGL3D::Bitangent Bitangent;

			/**
			 * @brief 2D texture coordinates
			 *
			 * @ref shaders-generic "Generic attribute",
			 * @ref Magnum::Vector2 "Vector2", used only if at least one of
			 * @ref Flag::AmbientTexture, @ref Flag::DiffuseTexture and
			 * @ref Flag::SpecularTexture is set.
			 */
			typedef GenericGL3D::TextureCoordinates TextureCoordinates;

			/**
			 * @brief Three-component vertex color
			 * @m_since{2019,10}
			 *
			 * @ref shaders-generic "Generic attribute", @ref Magnum::Color3. Use
			 * either this or the @ref Color4 attribute. Used only if
			 * @ref Flag::VertexColor is set.
			 */
			typedef GenericGL3D::Color3 Color3;

			/**
			 * @brief Four-component vertex color
			 * @m_since{2019,10}
			 *
			 * @ref shaders-generic "Generic attribute", @ref Magnum::Color4. Use
			 * either this or the @ref Color3 attribute. Used only if
			 * @ref Flag::VertexColor is set.
			 */
			typedef GenericGL3D::Color4 Color4;

#ifndef MAGNUM_TARGET_GLES2
			/**
			 * @brief (Instanced) object ID
			 * @m_since{2020,06}
			 *
			 * @ref shaders-generic "Generic attribute", @ref Magnum::UnsignedInt.
			 * Used only if @ref Flag::InstancedObjectId is set.
			 * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
			 * @requires_gles30 Object ID output requires integer support in
			 *      shaders, which is not available in OpenGL ES 2.0.
			 * @requires_webgl20 Object ID output requires integer support in
			 *      shaders, which is not available in WebGL 1.0.
			 */
			typedef GenericGL3D::ObjectId ObjectId;
#endif

			/**
			 * @brief (Instanced) transformation matrix
			 * @m_since{2020,06}
			 *
			 * @ref shaders-generic "Generic attribute", @ref Magnum::Matrix4.
			 * Used only if @ref Flag::InstancedTransformation is set.
			 * @requires_gl33 Extension @gl_extension{ARB,instanced_arrays}
			 * @requires_gles30 Extension @gl_extension{ANGLE,instanced_arrays},
			 *      @gl_extension{EXT,instanced_arrays} or
			 *      @gl_extension{NV,instanced_arrays} in OpenGL ES 2.0.
			 * @requires_webgl20 Extension @webgl_extension{ANGLE,instanced_arrays}
			 *      in WebGL 1.0.
			 */
			typedef GenericGL3D::TransformationMatrix TransformationMatrix;

			/**
			 * @brief (Instanced) normal matrix
			 * @m_since{2020,06}
			 *
			 * @ref shaders-generic "Generic attribute", @ref Magnum::Matrix3x3.
			 * Used only if @ref Flag::InstancedTransformation is set.
			 * @requires_gl33 Extension @gl_extension{ARB,instanced_arrays}
			 * @requires_gles30 Extension @gl_extension{ANGLE,instanced_arrays},
			 *      @gl_extension{EXT,instanced_arrays} or
			 *      @gl_extension{NV,instanced_arrays} in OpenGL ES 2.0.
			 * @requires_webgl20 Extension @webgl_extension{ANGLE,instanced_arrays}
			 *      in WebGL 1.0.
			 */
			typedef GenericGL3D::NormalMatrix NormalMatrix;

			/**
			 * @brief (Instanced) texture offset
			 * @m_since{2020,06}
			 *
			 * @ref shaders-generic "Generic attribute", @ref Magnum::Vector2. Used
			 * only if @ref Flag::InstancedTextureOffset is set.
			 * @requires_gl33 Extension @gl_extension{ARB,instanced_arrays}
			 * @requires_gles30 Extension @gl_extension{ANGLE,instanced_arrays},
			 *      @gl_extension{EXT,instanced_arrays} or
			 *      @gl_extension{NV,instanced_arrays} in OpenGL ES 2.0.
			 * @requires_webgl20 Extension @webgl_extension{ANGLE,instanced_arrays}
			 *      in WebGL 1.0.
			 */
			typedef typename GenericGL3D::TextureOffset TextureOffset;

#ifndef MAGNUM_TARGET_GLES2
			/**
			 * @brief (Instanced) texture offset and layer
			 * @m_since_latest
			 *
			 * @ref shaders-generic "Generic attribute", @ref Magnum::Vector3, with
			 * the last component interpreted as an integer. Use either this or the
			 * @ref TextureOffset attribute. First two components used only if
			 * @ref Flag::InstancedTextureOffset is set, third component only if
			 * @ref Flag::TextureArrays is set.
			 * @requires_gl33 Extension @gl_extension{EXT,texture_array} and
			 *      @gl_extension{ARB,instanced_arrays}
			 * @requires_gles30 Texture arrays are not available in OpenGL ES 2.0.
			 * @requires_webgl20 Texture arrays are not available in WebGL 1.0.
			 */
			typedef typename GenericGL3D::TextureOffsetLayer TextureOffsetLayer;
#endif

			enum : UnsignedInt
			{
				/**
				 * Color shader output. @ref shaders-generic "Generic output",
				 * present always. Expects three- or four-component floating-point
				 * or normalized buffer attachment.
				 * @m_since{2019,10}
				 */
				ColorOutput = GenericGL3D::ColorOutput,

#ifndef MAGNUM_TARGET_GLES2
				/**
				 * Object ID shader output. @ref shaders-generic "Generic output",
				 * present only if @ref Flag::ObjectId is set. Expects a
				 * single-component unsigned integral attachment. Writes the value
				 * set in @ref setObjectId() and possibly also a per-vertex ID and
				 * an ID fetched from a texture, see @ref Shaders-PhongGL-object-id
				 * for more information.
				 * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
				 * @requires_gles30 Object ID output requires integer support in
				 *      shaders, which is not available in OpenGL ES 2.0.
				 * @requires_webgl20 Object ID output requires integer support in
				 *      shaders, which is not available in WebGL 1.0.
				 * @m_since{2019,10}
				 */
				ObjectIdOutput = GenericGL3D::ObjectIdOutput
#endif
			};

			/**
			 * @brief Flag
			 *
			 * @see @ref Flags, @ref flags()
			 */
			enum class Flag : UnsignedInt
			{
				/**
				 * Multiply ambient color with a texture.
				 * @see @ref setAmbientColor(), @ref bindAmbientTexture()
				 */
				AmbientTexture = 1 << 0,

				/**
				 * Multiply diffuse color with a texture.
				 * @see @ref setDiffuseColor(), @ref bindDiffuseTexture()
				 */
				DiffuseTexture = 1 << 1,

				/**
				 * Multiply specular color with a texture.
				 * @see @ref setSpecularColor(), @ref bindSpecularTexture()
				 */
				SpecularTexture = 1 << 2,

				/**
				 * Modify normals according to a texture. Requires the
				 * @ref Tangent attribute to be present.
				 * @m_since{2019,10}
				 */
				NormalTexture = 1 << 4,

				/**
				 * Enable alpha masking. If the combined fragment color has an
				 * alpha less than the value specified with @ref setAlphaMask(),
				 * given fragment is discarded.
				 *
				 * This uses the @glsl discard @ce operation which is known to have
				 * considerable performance impact on some platforms. While useful
				 * for cheap alpha masking that doesn't require depth sorting,
				 * with proper depth sorting and blending you'll usually get much
				 * better performance and output quality.
				 */
				AlphaMask = 1 << 3,

				/**
				 * Multiply the diffuse and ambient color with a vertex color.
				 * Requires either the @ref Color3 or @ref Color4 attribute to be
				 * present.
				 * @m_since{2019,10}
				 */
				VertexColor = 1 << 5,

				/**
				 * Use the separate @ref Bitangent attribute for retrieving vertex
				 * bitangents. If this flag is not present, the last component of
				 * @ref Tangent4 is used to calculate bitangent direction. See
				 * @ref Shaders-PhongGL-normal-mapping for more information.
				 * @m_since_latest
				 */
				Bitangent = 1 << 11,

				/**
				 * Enable texture coordinate transformation. If this flag is set,
				 * the shader expects that at least one of
				 * @ref Flag::AmbientTexture, @ref Flag::DiffuseTexture,
				 * @ref Flag::SpecularTexture or @ref Flag::NormalTexture is
				 * enabled as well.
				 * @see @ref setTextureMatrix()
				 * @m_since{2020,06}
				 */
				TextureTransformation = 1 << 6,

#ifndef MAGNUM_TARGET_GLES2
				/**
				 * Enable object ID output. See @ref Shaders-PhongGL-object-id
				 * for more information.
				 * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
				 * @requires_gles30 Object ID output requires integer support in
				 *      shaders, which is not available in OpenGL ES 2.0.
				 * @requires_webgl20 Object ID output requires integer support in
				 *      shaders, which is not available in WebGL 1.0.
				 * @m_since{2019,10}
				 */
				ObjectId = 1 << 7,

				/**
				 * Instanced object ID. Retrieves a per-instance / per-vertex
				 * object ID from the @ref ObjectId attribute, outputting a sum of
				 * the per-vertex ID and ID coming from @ref setObjectId() or
				 * @ref PhongDrawUniform::objectId. Implicitly enables
				 * @ref Flag::ObjectId. See @ref Shaders-PhongGL-object-id for more
				 * information.
				 * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
				 * @requires_gles30 Object ID output requires integer support in
				 *      shaders, which is not available in OpenGL ES 2.0.
				 * @requires_webgl20 Object ID output requires integer support in
				 *      shaders, which is not available in WebGL 1.0.
				 * @m_since{2020,06}
				 */
				InstancedObjectId = (1 << 8) | ObjectId,

				/**
				 * Object ID texture. Retrieves object IDs from a texture bound
				 * with @ref bindObjectIdTexture(), outputting a sum of the object
				 * ID texture, the ID coming from @ref setObjectId() or
				 * @ref PhongDrawUniform::objectId and possibly also the per-vertex
				 * ID, if @ref Flag::InstancedObjectId is enabled as well.
				 * Implicitly enables @ref Flag::ObjectId. See
				 * @ref Shaders-PhongGL-object-id for more information.
				 * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
				 * @requires_gles30 Object ID output requires integer support in
				 *      shaders, which is not available in OpenGL ES 2.0.
				 * @requires_webgl20 Object ID output requires integer support in
				 *      shaders, which is not available in WebGL 1.0.
				 * @m_since_latest
				 */
				ObjectIdTexture = (1 << 17) | ObjectId,
#endif

				/**
				 * Instanced transformation. Retrieves a per-instance
				 * transformation and normal matrix from the
				 * @ref TransformationMatrix / @ref NormalMatrix attributes and
				 * uses them together with matrices coming from
				 * @ref setTransformationMatrix() and @ref setNormalMatrix() or
				 * @ref TransformationUniform3D::transformationMatrix and
				 * @ref PhongDrawUniform::normalMatrix (first the per-instance,
				 * then the uniform matrix). See @ref Shaders-PhongGL-instancing
				 * for more information.
				 * @requires_gl33 Extension @gl_extension{ARB,instanced_arrays}
				 * @requires_gles30 Extension @gl_extension{ANGLE,instanced_arrays},
				 *      @gl_extension{EXT,instanced_arrays} or
				 *      @gl_extension{NV,instanced_arrays} in OpenGL ES 2.0.
				 * @requires_webgl20 Extension @webgl_extension{ANGLE,instanced_arrays}
				 *      in WebGL 1.0.
				 * @m_since{2020,06}
				 */
				InstancedTransformation = 1 << 9,

				/**
				 * Instanced texture offset. Retrieves a per-instance offset vector
				 * from the @ref TextureOffset attribute and uses it together with
				 * the matrix coming from @ref setTextureMatrix() or
				 * @ref TextureTransformationUniform::rotationScaling and
				 * @ref TextureTransformationUniform::offset (first the
				 * per-instance vector, then the uniform matrix). Instanced texture
				 * scaling and rotation is not supported at the moment, you can
				 * specify that only via the uniform @ref setTextureMatrix().
				 * Implicitly enables @ref Flag::TextureTransformation. See
				 * @ref Shaders-PhongGL-instancing for more information.
				 *
				 * If @ref Flag::TextureArrays is set as well, a three-component
				 * @ref TextureOffsetLayer attribute can be used instead of
				 * @ref TextureOffset to specify per-instance texture layer, which
				 * gets added to the uniform layer numbers set by
				 * @ref setTextureLayer() or
				 * @ref TextureTransformationUniform::layer.
				 * @requires_gl33 Extension @gl_extension{ARB,instanced_arrays}
				 * @requires_gles30 Extension @gl_extension{ANGLE,instanced_arrays},
				 *      @gl_extension{EXT,instanced_arrays} or
				 *      @gl_extension{NV,instanced_arrays} in OpenGL ES 2.0.
				 * @requires_webgl20 Extension @webgl_extension{ANGLE,instanced_arrays}
				 *      in WebGL 1.0.
				 * @m_since{2020,06}
				 */
				InstancedTextureOffset = (1 << 10) | TextureTransformation,

#ifndef MAGNUM_TARGET_GLES2
				/**
				 * Use uniform buffers. Expects that uniform data are supplied via
				 * @ref bindProjectionBuffer(), @ref bindTransformationBuffer(),
				 * @ref bindDrawBuffer(), @ref bindTextureTransformationBuffer(),
				 * @ref bindMaterialBuffer() and @ref bindLightBuffer() instead of
				 * direct uniform setters.
				 * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
				 * @requires_gles30 Uniform buffers are not available in OpenGL ES
				 *      2.0.
				 * @requires_webgl20 Uniform buffers are not available in WebGL
				 *      1.0.
				 * @m_since_latest
				 */
				UniformBuffers = 1 << 12,

				/**
				 * Enable multidraw functionality. Implies @ref Flag::UniformBuffers
				 * and adds the value from @ref setDrawOffset() with the
				 * @glsl gl_DrawID @ce builtin, which makes draws submitted via
				 * @ref GL::AbstractShaderProgram::draw(Containers::ArrayView<const Containers::Reference<MeshView>>)
				 * pick up per-draw parameters directly, without having to rebind
				 * the uniform buffers or specify @ref setDrawOffset() before each
				 * draw. In a non-multidraw scenario, @glsl gl_DrawID @ce is
				 * @cpp 0 @ce, which means a shader with this flag enabled can be
				 * used for regular draws as well.
				 * @requires_gl46 Extension @gl_extension{ARB,uniform_buffer_object}
				 *      and @gl_extension{ARB,shader_draw_parameters}
				 * @requires_es_extension OpenGL ES 3.0 and extension
				 *      @m_class{m-doc-external} [ANGLE_multi_draw](https://chromium.googlesource.com/angle/angle/+/master/extensions/ANGLE_multi_draw.txt)
				 *      (unlisted). While the extension alone needs only OpenGL ES
				 *      2.0, the shader implementation relies on uniform buffers,
				 *      which require OpenGL ES 3.0.
				 * @requires_webgl_extension WebGL 2.0 and extension
				 *      @webgl_extension{ANGLE,multi_draw}. While the extension
				 *      alone needs only WebGL 1.0, the shader implementation
				 *      relies on uniform buffers, which require WebGL 2.0.
				 * @m_since_latest
				 */
				MultiDraw = UniformBuffers | (1 << 13),

				/**
				 * Use 2D texture arrays. Expects that the texture is supplied via
				 * @ref bindAmbientTexture(GL::Texture2DArray&) /
				 * @ref bindDiffuseTexture(GL::Texture2DArray&) /
				 * @ref bindSpecularTexture(GL::Texture2DArray&) /
				 * @ref bindNormalTexture(GL::Texture2DArray&) instead of
				 * @ref bindAmbientTexture(GL::Texture2D&) /
				 * @ref bindDiffuseTexture(GL::Texture2D&) /
				 * @ref bindSpecularTexture(GL::Texture2D&) /
				 * @ref bindNormalTexture(GL::Texture2D&) and the layer shared by
				 * all textures is set via @ref setTextureLayer() or
				 * @ref TextureTransformationUniform::layer. If
				 * @ref Flag::InstancedTextureOffset is set as well and a
				 * three-component @ref TextureOffsetLayer attribute is used
				 * instead of @ref TextureOffset, the per-instance and uniform
				 * layer numbers are added together.
				 * @requires_gl30 Extension @gl_extension{EXT,texture_array}
				 * @requires_gles30 Texture arrays are not available in OpenGL ES
				 *      2.0.
				 * @requires_webgl20 Texture arrays are not available in WebGL 1.0.
				 * @m_since_latest
				 */
				TextureArrays = 1 << 14,

				/**
				 * Enable light culling in uniform buffer workflows using the
				 * @ref PhongDrawUniform::lightOffset and
				 * @ref PhongDrawUniform::lightCount fields. If not enabled, all
				 * @ref lightCount() lights are used for every draw. Expects that
				 * @ref Flag::UniformBuffers is enabled as well.
				 * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
				 * @requires_gles30 Uniform buffers are not available in OpenGL ES
				 *      2.0.
				 * @requires_webgl20 Uniform buffers are not available in WebGL
				 *      1.0.
				 * @m_since_latest
				 */
				LightCulling = 1 << 15,
#endif

				/**
				 * Disable specular contribution in light calculation. Can result
				 * in a significant performance improvement compared to calling
				 * @ref setSpecularColor() with @cpp 0x00000000_rgbaf @ce when
				 * specular highlights are not desired.
				 * @m_since_latest
				 */
				NoSpecular = 1 << 16,

				Shadows = 1 << 17
			};

			/**
			 * @brief Flags
			 *
			 * @see @ref flags()
			 */
			typedef Containers::EnumSet<Flag> Flags;

			/**
			 * @brief Compile asynchronously
			 * @m_since_latest
			 *
			 * Compared to @ref PhongGL(Flags, UnsignedInt) can perform an
			 * asynchronous compilation and linking. See @ref shaders-async for
			 * more information.
			 * @see @ref PhongGL(CompileState&&),
			 *      @ref compile(Flags, UnsignedInt, UnsignedInt, UnsignedInt)
			 */
			static CompileState compile(Flags flags = {}, UnsignedInt lightCount = 1, UnsignedInt numShadowLevels = 3);

#ifndef MAGNUM_TARGET_GLES2
			/**
			 * @brief Compile for a multi-draw scenario asynchronously
			 * @m_since_latest
			 *
			 * Compared to @ref PhongGL(Flags, UnsignedInt, UnsignedInt, UnsignedInt)
			 * can perform an asynchronous compilation and linking. See
			 * @ref shaders-async for more information.
			 * @see @ref PhongGL(CompileState&&), @ref compile(Flags, UnsignedInt)
			 * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
			 * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
			 * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
			 */
			static CompileState compile(Flags flags, UnsignedInt lightCount, UnsignedInt numShadowLevels, UnsignedInt materialCount, UnsignedInt drawCount);
#endif

			/**
			 * @brief Constructor
			 * @param flags         Flags
			 * @param lightCount    Count of light sources
			 *
			 * While this function is meant mainly for the classic uniform
			 * scenario (without @ref Flag::UniformBuffers set), it's equivalent to
			 * @ref PhongGL(Flags, UnsignedInt, UnsignedInt, UnsignedInt) with
			 * @p materialCount and @p drawCount set to @cpp 1 @ce.
			 * @see @ref compile(Flags, UnsignedInt)
			 */
			explicit PhongGLShadows(Flags flags = {}, UnsignedInt lightCount = 1, UnsignedInt numShadowLevels = 3);

#ifndef MAGNUM_TARGET_GLES2
			/**
			 * @brief Construct for a multi-draw scenario
			 * @param flags         Flags
			 * @param lightCount    Size of a @ref PhongLightUniform buffer bound
			 *      with @ref bindLightBuffer()
			 * @param materialCount Size of a @ref PhongMaterialUniform buffer
			 *      bound with @ref bindMaterialBuffer()
			 * @param drawCount     Size of a @ref ProjectionUniform3D /
			 *      @ref TransformationUniform3D / @ref PhongDrawUniform /
			 *      @ref TextureTransformationUniform buffer bound with
			 *      @ref bindProjectionBuffer(), @ref bindTransformationBuffer(),
			 *      @ref bindDrawBuffer() and @ref bindTextureTransformationBuffer()
			 * @m_since_latest
			 *
			 * If @p flags contains @ref Flag::UniformBuffers, @p lightCount,
			 * @p materialCount and @p drawCount describe the uniform buffer sizes
			 * as these are required to have a statically defined size. The draw
			 * offset is then set via @ref setDrawOffset() and the per-draw
			 * materials and lights are specified via
			 * @ref PhongDrawUniform::materialId,
			 * @ref PhongDrawUniform::lightOffset and
			 * @ref PhongDrawUniform::lightCount.
			 *
			 * If @p flags don't contain @ref Flag::UniformBuffers,
			 * @p materialCount and @p drawCount is ignored and the constructor
			 * behaves the same as @ref PhongGL(Flags, UnsignedInt).
			 * @see @ref compile(Flags, UnsignedInt, UnsignedInt, UnsignedInt)
			 * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
			 * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
			 * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
			 */
			/** @todo this constructor will eventually need to have also joint
				count, per-vertex weight count, view count for multiview and clip
				plane count ... and putting them in arbitrary order next to each
				other is too error-prone, so it needs some other solution
				(accepting pairs of parameter type and value like in GL context
				creation, e.g., which will probably need a new enum as reusing Flag
				for this might be too confusing); what if some parameters won't be
				(unsigned) integers? like a string with shader extensions? make a
				whole Configuration class? */
			explicit PhongGLShadows(Flags flags, UnsignedInt lightCount, UnsignedInt materialCount, UnsignedInt drawCount, UnsignedInt numShadowLevels = 3);
#endif

			/**
			 * @brief Finalize an asynchronous compilation
			 * @m_since_latest
			 *
			 * Takes an asynchronous compilation state returned by @ref compile()
			 * and forms a ready-to-use shader object. See @ref shaders-async for
			 * more information.
			 */
			explicit PhongGLShadows(CompileState &&state);

			/**
			 * @brief Construct without creating the underlying OpenGL object
			 *
			 * The constructed instance is equivalent to a moved-from state. Useful
			 * in cases where you will overwrite the instance later anyway. Move
			 * another object over it to make it useful.
			 *
			 * This function can be safely used for constructing (and later
			 * destructing) objects even without any OpenGL context being active.
			 * However note that this is a low-level and a potentially dangerous
			 * API, see the documentation of @ref NoCreate for alternatives.
			 */
			explicit PhongGLShadows(NoCreateT) noexcept : GL::AbstractShaderProgram{NoCreate} {}

			/** @brief Copying is not allowed */
			PhongGLShadows(const PhongGLShadows &) = delete;

			/** @brief Move constructor */
			PhongGLShadows(PhongGLShadows &&) noexcept = default;

			/** @brief Copying is not allowed */
			PhongGLShadows &operator=(const PhongGLShadows &) = delete;

			/** @brief Move assignment */
			PhongGLShadows &operator=(PhongGLShadows &&) noexcept = default;

			/** @brief Flags */
			Flags flags() const { return _flags; }

			/**
			 * @brief Light count
			 *
			 * If @ref Flag::UniformBuffers is set, this is the statically defined
			 * size of the @ref PhongLightUniform uniform buffer.
			 * @see @ref bindLightBuffer()
			 */
			UnsignedInt lightCount() const { return _lightCount; }

#ifndef MAGNUM_TARGET_GLES2
			/**
			 * @brief Material count
			 * @m_since_latest
			 *
			 * Statically defined size of the @ref PhongMaterialUniform uniform
			 * buffer. Has use only if @ref Flag::UniformBuffers is set.
			 * @see @ref bindMaterialBuffer()
			 * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
			 * @requires_webgl20 Not defined on WebGL 1.0 builds.
			 */
			UnsignedInt materialCount() const { return _materialCount; }

			/**
			 * @brief Draw count
			 * @m_since_latest
			 *
			 * Statically defined size of each of the @ref ProjectionUniform3D,
			 * @ref TransformationUniform3D, @ref PhongDrawUniform and
			 * @ref TextureTransformationUniform uniform buffers. Has use only if
			 * @ref Flag::UniformBuffers is set.
			 * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
			 * @requires_webgl20 Not defined on WebGL 1.0 builds.
			 */
			UnsignedInt drawCount() const { return _drawCount; }
#endif

			/** @{
			 * @name Uniform setters
			 *
			 * Used only if @ref Flag::UniformBuffers is not set.
			 */

			/**
			 * @brief Set ambient color
			 * @return Reference to self (for method chaining)
			 *
			 * If @ref Flag::AmbientTexture is set, default value is
			 * @cpp 0xffffffff_rgbaf @ce and the color will be multiplied with
			 * ambient texture, otherwise default value is @cpp 0x00000000_rgbaf @ce.
			 * If @ref Flag::VertexColor is set, the color is multiplied with a
			 * color coming from the @ref Color3 / @ref Color4 attribute.
			 *
			 * Expects that @ref Flag::UniformBuffers is not set, in that case fill
			 * @ref PhongMaterialUniform::ambientColor and call
			 * @ref bindMaterialBuffer() instead.
			 * @see @ref bindAmbientTexture(), @ref Shaders-PhongGL-lights-ambient
			 */
			PhongGLShadows &setAmbientColor(const Magnum::Color4 &color);

			/**
			 * @brief Set diffuse color
			 * @return Reference to self (for method chaining)
			 *
			 * Initial value is @cpp 0xffffffff_rgbaf @ce. If
			 * @ref Flag::DiffuseTexture is set, the color will be multiplied with
			 * the texture. If @ref lightCount() is zero, this function is a no-op,
			 * as diffuse color doesn't contribute to the output in that case.
			 * If @ref Flag::VertexColor is set, the color is multiplied with a
			 * color coming from the @ref Color3 / @ref Color4 attribute.
			 *
			 * Expects that @ref Flag::UniformBuffers is not set, in that case fill
			 * @ref PhongMaterialUniform::diffuseColor and call
			 * @ref bindMaterialBuffer() instead.
			 * @see @ref bindDiffuseTexture()
			 */
			PhongGLShadows &setDiffuseColor(const Magnum::Color4 &color);

			/**
			 * @brief Set normal texture scale
			 * @return Reference to self (for method chaining)
			 * @m_since_latest
			 *
			 * Affects strength of the normal mapping. Initial value is
			 * @cpp 1.0f @ce, meaning the normal texture is not changed in any way;
			 * a value of @cpp 0.0f @ce disables the normal texture effect
			 * altogether.
			 *
			 * Expects that the shader was created with @ref Flag::NormalTexture
			 * enabled. If @ref lightCount() is zero, this function is a no-op, as
			 * normals don't contribute to the output in that case.
			 *
			 * Expects that @ref Flag::UniformBuffers is not set, in that case fill
			 * @ref PhongMaterialUniform::normalTextureScale and call
			 * @ref bindDrawBuffer() instead.
			 * @see @ref Shaders-PhongGL-normal-mapping, @ref bindNormalTexture(),
			 *      @ref Trade::MaterialAttribute::NormalTextureScale
			 */
			PhongGLShadows &setNormalTextureScale(Float scale);

			/**
			 * @brief Set specular color
			 * @return Reference to self (for method chaining)
			 *
			 * Initial value is @cpp 0xffffff00_rgbaf @ce. Expects that the shader
			 * was not created with @ref Flag::NoSpecular. If
			 * @ref Flag::SpecularTexture is set, the color will be multiplied with
			 * the texture. If you want to have a fully diffuse material, it's
			 * recommended to disable the specular contribution altogether with
			 * @ref Flag::NoSpecular. If having a dedicated shader variant is not
			 * possible, set the specular color to @cpp 0x00000000_rgbaf @ce. If
			 * @ref lightCount() is zero, this function is a no-op, as specular
			 * color doesn't contribute to the output in that case.
			 *
			 * Expects that @ref Flag::UniformBuffers is not set, in that case fill
			 * @ref PhongMaterialUniform::specularColor and call
			 * @ref bindMaterialBuffer() instead.
			 * @see @ref bindSpecularTexture()
			 */
			PhongGLShadows &setSpecularColor(const Magnum::Color4 &color);

			/**
			 * @brief Set shininess
			 * @return Reference to self (for method chaining)
			 *
			 * The larger value, the harder surface (smaller specular highlight).
			 * Initial value is @cpp 80.0f @ce. If @ref lightCount() is zero, this
			 * function is a no-op, as specular color doesn't contribute to the
			 * output in that case.
			 *
			 * Expects that @ref Flag::UniformBuffers is not set, in that case fill
			 * @ref PhongMaterialUniform::shininess and call
			 * @ref bindMaterialBuffer() instead.
			 */
			PhongGLShadows &setShininess(Float shininess);

			/**
			 * @brief Set alpha mask value
			 * @return Reference to self (for method chaining)
			 *
			 * Expects that the shader was created with @ref Flag::AlphaMask
			 * enabled. Fragments with alpha values smaller than the mask value
			 * will be discarded. Initial value is @cpp 0.5f @ce. See the flag
			 * documentation for further information.
			 *
			 * This corresponds to @m_class{m-doc-external} [glAlphaFunc()](https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glAlphaFunc.xml)
			 * in classic OpenGL.
			 *
			 * Expects that @ref Flag::UniformBuffers is not set, in that case fill
			 * @ref PhongMaterialUniform::alphaMask and call
			 * @ref bindMaterialBuffer() instead.
			 * @m_keywords{glAlphaFunc()}
			 */
			PhongGLShadows &setAlphaMask(Float mask);

#ifndef MAGNUM_TARGET_GLES2
			/**
			 * @brief Set object ID
			 * @return Reference to self (for method chaining)
			 *
			 * Expects that the shader was created with @ref Flag::ObjectId
			 * enabled. Value set here is written to the @ref ObjectIdOutput, see
			 * @ref Shaders-PhongGL-object-id for more information. Default is
			 * @cpp 0 @ce. If @ref Flag::InstancedObjectId and/or
			 * @ref Flag::ObjectIdTexture is enabled as well, this value is added
			 * to the ID coming from the @ref ObjectId attribute and/or the
			 * texture.
			 *
			 * Expects that @ref Flag::UniformBuffers is not set, in that case fill
			 * @ref PhongDrawUniform::objectId and call @ref bindDrawBuffer()
			 * instead.
			 * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
			 * @requires_gles30 Object ID output requires integer support in
			 *      shaders, which is not available in OpenGL ES 2.0.
			 * @requires_webgl20 Object ID output requires integer support in
			 *      shaders, which is not available in WebGL 1.0.
			 */
			PhongGLShadows &setObjectId(UnsignedInt id);
#endif

			/**
			 * @brief Set transformation matrix
			 * @return Reference to self (for method chaining)
			 *
			 * You need to set also @ref setNormalMatrix() with a corresponding
			 * value. Initial value is an identity matrix. If
			 * @ref Flag::InstancedTransformation is set, the per-instance
			 * transformation coming from the @ref TransformationMatrix attribute
			 * is applied first, before this one.
			 *
			 * Expects that @ref Flag::UniformBuffers is not set, in that case fill
			 * @ref TransformationUniform3D::transformationMatrix and call
			 * @ref bindTransformationBuffer() instead.
			 */
			PhongGLShadows &setTransformationMatrix(const Matrix4 &matrix);

			/**
			 * @brief Set normal matrix
			 * @return Reference to self (for method chaining)
			 *
			 * The matrix doesn't need to be normalized, as renormalization is done
			 * per-fragment anyway. You need to set also
			 * @ref setTransformationMatrix() with a corresponding value. Initial
			 * value is an identity matrix. If @ref lightCount() is zero, this
			 * function is a no-op, as normals don't contribute to the output in
			 * that case. If @ref Flag::InstancedTransformation is set, the
			 * per-instance normal matrix coming from the @ref NormalMatrix
			 * attribute is applied first, before this one.
			 *
			 * Expects that @ref Flag::UniformBuffers is not set, in that case fill
			 * @ref PhongDrawUniform::normalMatrix and call
			 * @ref bindDrawBuffer() instead.
			 * @see @ref Math::Matrix4::normalMatrix()
			 */
			PhongGLShadows &setNormalMatrix(const Matrix3x3 &matrix);

			/**
			 * @brief Set projection matrix
			 * @return Reference to self (for method chaining)
			 *
			 * Initial value is an identity matrix (i.e., an orthographic
			 * projection of the default @f$ [ -\boldsymbol{1} ; \boldsymbol{1} ] @f$
			 * cube).
			 *
			 * Expects that @ref Flag::UniformBuffers is not set, in that case fill
			 * @ref ProjectionUniform3D::projectionMatrix and call
			 * @ref bindProjectionBuffer() instead.
			 */
			PhongGLShadows &setProjectionMatrix(const Matrix4 &matrix);

			/**
			 * @brief Set texture coordinate transformation matrix
			 * @return Reference to self (for method chaining)
			 * @m_since{2020,06}
			 *
			 * Expects that the shader was created with
			 * @ref Flag::TextureTransformation enabled. Initial value is an
			 * identity matrix. If @ref Flag::InstancedTextureOffset is set, the
			 * per-instance offset coming from the @ref TextureOffset atttribute is
			 * applied first, before this matrix.
			 *
			 * Expects that @ref Flag::UniformBuffers is not set, in that case fill
			 * @ref TextureTransformationUniform::rotationScaling and
			 * @ref TextureTransformationUniform::offset and call
			 * @ref bindTextureTransformationBuffer() instead.
			 */
			PhongGLShadows &setTextureMatrix(const Matrix3 &matrix);

#ifndef MAGNUM_TARGET_GLES2
			/**
			 * @brief Set texture array layer
			 * @return Reference to self (for method chaining)
			 * @m_since_latest
			 *
			 * Expects that the shader was created with @ref Flag::TextureArrays
			 * enabled. Initial value is @cpp 0 @ce. If
			 * @ref Flag::InstancedTextureOffset is set and a three-component
			 * @ref TextureOffsetLayer attribute is used instead of
			 * @ref TextureOffset, this value is added to the layer coming from the
			 * third component.
			 *
			 * Expects that @ref Flag::UniformBuffers is not set, in that case fill
			 * @ref TextureTransformationUniform::layer and call
			 * @ref bindTextureTransformationBuffer() instead.
			 * @requires_gl30 Extension @gl_extension{EXT,texture_array}
			 * @requires_gles30 Texture arrays are not available in OpenGL ES 2.0.
			 * @requires_webgl20 Texture arrays are not available in WebGL 1.0.
			 */
			PhongGLShadows &setTextureLayer(UnsignedInt layer);
#endif

			/**
			 * @brief Set light positions
			 * @return Reference to self (for method chaining)
			 * @m_since_latest
			 *
			 * Depending on the fourth component, the value is treated as either a
			 * camera-relative position of a point light, if the fourth component
			 * is @cpp 1.0f @ce; or a direction *to* a directional light, if the
			 * fourth component is @cpp 0.0f @ce. Expects that the size of the
			 * @p positions array is the same as @ref lightCount(). Initial values
			 * are @cpp {0.0f, 0.0f, 1.0f, 0.0f} @ce --- a directional "fill" light
			 * coming from the camera.
			 *
			 * Expects that @ref Flag::UniformBuffers is not set, in that case fill
			 * @ref PhongLightUniform::position and call
			 * @ref bindLightBuffer() instead
			 * @see @ref Shaders-PhongGL-lights, @ref setLightPosition()
			 */
			PhongGLShadows &setLightPositions(Containers::ArrayView<const Vector4> positions);

			/**
			 * @overload
			 * @m_since_latest
			 */
			PhongGLShadows &setLightPositions(std::initializer_list<Vector4> positions);

#ifdef MAGNUM_BUILD_DEPRECATED
			/**
			 * @brief @copybrief setLightPositions(Containers::ArrayView<const Vector4>)
			 * @m_deprecated_since_latest Use @ref setLightPositions(Containers::ArrayView<const Vector4>)
			 *      instead. This function sets the fourth component to
			 *      @cpp 0.0f @ce to preserve the original behavior as close as
			 *      possible.
			 */
			CORRADE_DEPRECATED("use setLightPositions(Containers::ArrayView<const Vector4>) instead")
			PhongGLShadows &setLightPositions(Containers::ArrayView<const Vector3> positions);

			/**
			 * @brief @copybrief setLightPositions(std::initializer_list<Vector4>)
			 * @m_deprecated_since_latest Use @ref setLightPositions(std::initializer_list<Vector4>)
			 *      instead. This function sets the fourth component to
			 *      @cpp 0.0f @ce to preserve the original behavior as close as
			 *      possible.
			 */
			CORRADE_DEPRECATED("use setLightPositions(std::initializer_list<Vector4>) instead")
			PhongGLShadows &setLightPositions(std::initializer_list<Vector3> positions);
#endif

			/**
			 * @brief Set position for given light
			 * @return Reference to self (for method chaining)
			 * @m_since_latest
			 *
			 * Unlike @ref setLightPositions() updates just a single light
			 * position. If updating more than one light, prefer the batch function
			 * instead to reduce the count of GL API calls. Expects that @p id is
			 * less than @ref lightCount().
			 *
			 * Expects that @ref Flag::UniformBuffers is not set, in that case fill
			 * @ref PhongLightUniform::position and call @ref bindLightBuffer()
			 * instead.
			 */
			PhongGLShadows &setLightPosition(UnsignedInt id, const Vector4 &position);

#ifdef MAGNUM_BUILD_DEPRECATED
			/**
			 * @brief @copybrief setLightPosition(UnsignedInt, const Vector4&)
			 * @m_deprecated_since_latest Use @ref setLightPosition(UnsignedInt, const Vector4&)
			 *      instead. This function sets the fourth component to
			 *      @cpp 0.0f @ce to preserve the original behavior as close as
			 *      possible.
			 */
			CORRADE_DEPRECATED("use setLightPosition(UnsignedInt, const Vector4&) instead")
			PhongGLShadows &setLightPosition(UnsignedInt id, const Vector3 &position);

			/**
			 * @brief Set light position
			 * @m_deprecated_since_latest Use @ref setLightPositions(std::initializer_list<Vector4>)
			 *      with a single item instead --- it's short enough to not warrant
			 *      the existence of a dedicated overload. This function sets the
			 *      fourth component to @cpp 0.0f @ce to preserve the original
			 *      behavior as close as possible.
			 */
			CORRADE_DEPRECATED("use setLightPositions(std::initializer_list<Vector4>) instead")
			PhongGLShadows &setLightPosition(const Vector3 &position);
#endif

			/**
			 * @brief Set light colors
			 * @return Reference to self (for method chaining)
			 * @m_since_latest
			 *
			 * Initial values are @cpp 0xffffff_rgbf @ce. Expects that the size
			 * of the @p colors array is the same as @ref lightCount().
			 *
			 * Expects that @ref Flag::UniformBuffers is not set, in that case fill
			 * @ref PhongLightUniform::color and call @ref bindLightBuffer()
			 * instead.
			 * @see @ref Shaders-PhongGL-lights, @ref setLightColor()
			 */
			PhongGLShadows &setLightColors(Containers::ArrayView<const Magnum::Color3> colors);

			/**
			 * @overload
			 * @m_since_latest
			 */
			PhongGLShadows &setLightColors(std::initializer_list<Magnum::Color3> colors);

#ifdef MAGNUM_BUILD_DEPRECATED
			/**
			 * @brief @copybrief setLightColors(Containers::ArrayView<const Magnum::Color3>)
			 * @m_deprecated_since_latest Use @ref setLightColors(Containers::ArrayView<const Magnum::Color3>)
			 *      instead. The alpha channel isn't used in any way.
			 */
			CORRADE_DEPRECATED("use setLightColors(Containers::ArrayView<const Magnum::Color3>) instead")
			PhongGLShadows &setLightColors(Containers::ArrayView<const Magnum::Color4> colors);

			/**
			 * @brief @copybrief setLightColors(std::initializer_list<Magnum::Color3>)
			 * @m_deprecated_since_latest Use @ref setLightColors(std::initializer_list<Magnum::Color3>)
			 *      instead. The alpha channel isn't used in any way.
			 */
			CORRADE_DEPRECATED("use setLightColors(std::initializer_list<Magnum::Color3>) instead")
			PhongGLShadows &setLightColors(std::initializer_list<Magnum::Color4> colors);
#endif

			/**
			 * @brief Set color for given light
			 * @return Reference to self (for method chaining)
			 * @m_since_latest
			 *
			 * Unlike @ref setLightColors() updates just a single light color. If
			 * updating more than one light, prefer the batch function instead to
			 * reduce the count of GL API calls. Expects that @p id is less than
			 * @ref lightCount().
			 *
			 * Expects that @ref Flag::UniformBuffers is not set, in that case fill
			 * @ref PhongLightUniform::color and call @ref bindLightBuffer()
			 * instead.
			 */
			PhongGLShadows &setLightColor(UnsignedInt id, const Magnum::Color3 &color);

#ifdef MAGNUM_BUILD_DEPRECATED
			/**
			 * @brief @copybrief setLightColor(UnsignedInt, const Magnum::Color3&)
			 * @m_deprecated_since_latest Use @ref setLightColor(UnsignedInt, const Magnum::Color3&)
			 *      instead. The alpha channel isn't used in any way.
			 */
			CORRADE_DEPRECATED("use setLightColor(UnsignedInt, const Magnum::Color3&) instead")
			PhongGLShadows &setLightColor(UnsignedInt id, const Magnum::Color4 &color);

			/**
			 * @brief Set light color
			 * @m_deprecated_since_latest Use @ref setLightColors(std::initializer_list<Magnum::Color3>)
			 *      with a single item instead --- it's short enough to not warrant
			 *      the existence of a dedicated overload. The alpha channel isn't
			 *      used in any way.
			 */
			CORRADE_DEPRECATED("use setLightColor(std::initializer_list<Color3>) instead")
			PhongGLShadows &setLightColor(const Magnum::Color4 &color);
#endif

			/**
			 * @brief Set light specular colors
			 * @return Reference to self (for method chaining)
			 * @m_since_latest
			 *
			 * Usually you'd set this value to the same as @ref setLightColors(),
			 * but it allows for greater flexibility such as disabling specular
			 * highlights on certain lights. Initial values are
			 * @cpp 0xffffff_rgbf @ce. Expects that the size of the @p colors array
			 * is the same as @ref lightCount().
			 *
			 * Expects that @ref Flag::UniformBuffers is not set, in that case fill
			 * @ref PhongLightUniform::specularColor and call @ref bindLightBuffer()
			 * instead.
			 * @see @ref Shaders-PhongGL-lights, @ref setLightColor()
			 */
			PhongGLShadows &setLightSpecularColors(Containers::ArrayView<const Magnum::Color3> colors);

			/**
			 * @overload
			 * @m_since_latest
			 */
			PhongGLShadows &setLightSpecularColors(std::initializer_list<Magnum::Color3> colors);

			/**
			 * @brief Set specular color for given light
			 * @return Reference to self (for method chaining)
			 * @m_since_latest
			 *
			 * Unlike @ref setLightSpecularColors() updates just a single light
			 * color. If updating more than one light, prefer the batch function
			 * instead to reduce the count of GL API calls. Expects that @p id is
			 * less than @ref lightCount().
			 *
			 * Expects that @ref Flag::UniformBuffers is not set, in that case fill
			 * @ref PhongLightUniform::specularColor and call
			 * @ref bindLightBuffer() instead.
			 */
			PhongGLShadows &setLightSpecularColor(UnsignedInt id, const Magnum::Color3 &color);

			/**
			 * @brief Set light attenuation ranges
			 * @return Reference to self (for method chaining)
			 * @m_since_latest
			 *
			 * Initial values are @ref Constants::inf(). Expects that the size of
			 * the @p ranges array is the same as @ref lightCount().
			 *
			 * Expects that @ref Flag::UniformBuffers is not set, in that case fill
			 * @ref PhongLightUniform::range and call @ref bindLightBuffer()
			 * instead.
			 * @see @ref Shaders-PhongGL-lights, @ref setLightRange()
			 */
			PhongGLShadows &setLightRanges(Containers::ArrayView<const Float> ranges);

			/**
			 * @overload
			 * @m_since_latest
			 */
			PhongGLShadows &setLightRanges(std::initializer_list<Float> ranges);

			/**
			 * @brief Set attenuation range for given light
			 * @return Reference to self (for method chaining)
			 * @m_since_latest
			 *
			 * Unlike @ref setLightRanges() updates just a single light range. If
			 * updating more than one light, prefer the batch function instead to
			 * reduce the count of GL API calls. Expects that @p id is less than
			 * @ref lightCount().
			 *
			 * Expects that @ref Flag::UniformBuffers is not set, in that case fill
			 * @ref PhongLightUniform::range and call @ref bindLightBuffer()
			 * instead.
			 */
			PhongGLShadows &setLightRange(UnsignedInt id, Float range);

			/**
			 * @}
			 */

#ifndef MAGNUM_TARGET_GLES2
			/** @{
			 * @name Uniform buffer binding and related uniform setters
			 *
			 * Used if @ref Flag::UniformBuffers is set.
			 */

			/**
			 * @brief Set a draw offset
			 * @return Reference to self (for method chaining)
			 * @m_since_latest
			 *
			 * Specifies which item in the @ref TransformationUniform3D,
			 * @ref PhongDrawUniform and @ref TextureTransformationUniform buffers
			 * bound with @ref bindTransformationBuffer(), @ref bindDrawBuffer()
			 * and @ref bindTextureTransformationBuffer() should be used for
			 * current draw. Expects that @ref Flag::UniformBuffers is set and
			 * @p offset is less than @ref drawCount(). Initial value is
			 * @cpp 0 @ce, if @ref drawCount() is @cpp 1 @ce, the function is a
			 * no-op as the shader assumes draw offset to be always zero.
			 *
			 * If @ref Flag::MultiDraw is set, @glsl gl_DrawID @ce is added to this
			 * value, which makes each draw submitted via
			 * @ref GL::AbstractShaderProgram::draw(Containers::ArrayView<const Containers::Reference<MeshView>>)
			 * pick up its own per-draw parameters.
			 * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
			 * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
			 * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
			 */
			PhongGLShadows &setDrawOffset(UnsignedInt offset);

			/**
			 * @brief Set a projection uniform buffer
			 * @return Reference to self (for method chaining)
			 * @m_since_latest
			 *
			 * Expects that @ref Flag::UniformBuffers is set. The buffer is
			 * expected to contain at least one instance of
			 * @ref ProjectionUniform3D. At the very least you need to call also
			 * @ref bindTransformationBuffer(), @ref bindDrawBuffer() and
			 * @ref bindMaterialBuffer(), usually @ref bindLightBuffer() as well.
			 * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
			 * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
			 * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
			 */
			PhongGLShadows &bindProjectionBuffer(GL::Buffer &buffer);
			/**
			 * @overload
			 * @m_since_latest
			 */
			PhongGLShadows &bindProjectionBuffer(GL::Buffer &buffer, GLintptr offset, GLsizeiptr size);

			/**
			 * @brief Set a transformation uniform buffer
			 * @return Reference to self (for method chaining)
			 * @m_since_latest
			 *
			 * Expects that @ref Flag::UniformBuffers is set. The buffer is
			 * expected to contain @ref drawCount() instances of
			 * @ref TransformationUniform3D. At the very least you need to call
			 * also @ref bindProjectionBuffer(), @ref bindDrawBuffer() and
			 * @ref bindMaterialBuffer(), usually @ref bindLightBuffer() as well.
			 * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
			 * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
			 * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
			 */
			PhongGLShadows &bindTransformationBuffer(GL::Buffer &buffer);
			/**
			 * @overload
			 * @m_since_latest
			 */
			PhongGLShadows &bindTransformationBuffer(GL::Buffer &buffer, GLintptr offset, GLsizeiptr size);

			/**
			 * @brief Set a draw uniform buffer
			 * @return Reference to self (for method chaining)
			 * @m_since_latest
			 *
			 * Expects that @ref Flag::UniformBuffers is set. The buffer is
			 * expected to contain @ref drawCount() instances of
			 * @ref PhongDrawUniform. At the very least you need to call also
			 * @ref bindProjectionBuffer(), @ref bindTransformationBuffer() and
			 * @ref bindMaterialBuffer(), usually @ref bindLightBuffer() as well.
			 * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
			 * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
			 * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
			 */
			PhongGLShadows &bindDrawBuffer(GL::Buffer &buffer);
			/**
			 * @overload
			 * @m_since_latest
			 */
			PhongGLShadows &bindDrawBuffer(GL::Buffer &buffer, GLintptr offset, GLsizeiptr size);

			/**
			 * @brief Set a texture transformation uniform buffer
			 * @return Reference to self (for method chaining)
			 * @m_since_latest
			 *
			 * Expects that both @ref Flag::UniformBuffers and
			 * @ref Flag::TextureTransformation is set. The buffer is expected to
			 * contain @ref drawCount() instances of
			 * @ref TextureTransformationUniform.
			 * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
			 * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
			 * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
			 */
			PhongGLShadows &bindTextureTransformationBuffer(GL::Buffer &buffer);
			/**
			 * @overload
			 * @m_since_latest
			 */
			PhongGLShadows &bindTextureTransformationBuffer(GL::Buffer &buffer, GLintptr offset, GLsizeiptr size);

			/**
			 * @brief Set a material uniform buffer
			 * @return Reference to self (for method chaining)
			 * @m_since_latest
			 *
			 * Expects that @ref Flag::UniformBuffers is set. The buffer is
			 * expected to contain @ref materialCount() instances of
			 * @ref PhongMaterialUniform. At the very least you need to call also
			 * @ref bindProjectionBuffer(), @ref bindTransformationBuffer() and
			 * @ref bindDrawBuffer(), usually @ref bindLightBuffer() as well.
			 * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
			 * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
			 * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
			 */
			PhongGLShadows &bindMaterialBuffer(GL::Buffer &buffer);
			/**
			 * @overload
			 * @m_since_latest
			 */
			PhongGLShadows &bindMaterialBuffer(GL::Buffer &buffer, GLintptr offset, GLsizeiptr size);

			/**
			 * @brief Set a light uniform buffer
			 * @return Reference to self (for method chaining)
			 * @m_since_latest
			 *
			 * Expects that @ref Flag::UniformBuffers is set. The buffer is
			 * expected to contain @ref lightCount() instances of
			 * @ref PhongLightUniform.
			 * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
			 * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
			 * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
			 */
			PhongGLShadows &bindLightBuffer(GL::Buffer &buffer);
			/**
			 * @overload
			 * @m_since_latest
			 */
			PhongGLShadows &bindLightBuffer(GL::Buffer &buffer, GLintptr offset, GLsizeiptr size);

/**
 * @}
 */
#endif

			/** @{
			 * @name Texture binding
			 */

			/**
			 * @brief Bind an ambient texture
			 * @return Reference to self (for method chaining)
			 *
			 * Expects that the shader was created with @ref Flag::AmbientTexture
			 * enabled. If @ref Flag::TextureArrays is enabled as well, use
			 * @ref bindAmbientTexture(GL::Texture2DArray&) instead.
			 * @see @ref bindTextures(), @ref setAmbientColor(),
			 *      @ref Shaders-PhongGL-lights-ambient
			 */
			PhongGLShadows &bindAmbientTexture(GL::Texture2D &texture);

#ifndef MAGNUM_TARGET_GLES2
			/**
			 * @brief Bind an ambient array texture
			 * @return Reference to self (for method chaining)
			 * @m_since_latest
			 *
			 * Expects that the shader was created with both
			 * @ref Flag::AmbientTexture and @ref Flag::TextureArrays enabled. If
			 * @ref Flag::UniformBuffers is not enabled, the layer is set via
			 * @ref setTextureLayer(); if @ref Flag::UniformBuffers is enabled,
			 * @ref Flag::TextureTransformation has to be enabled as well and the
			 * layer is set via @ref TextureTransformationUniform::layer.
			 * @see @ref setAmbientColor(), @ref Shaders-PhongGL-lights-ambient
			 * @requires_gl30 Extension @gl_extension{EXT,texture_array}
			 * @requires_gles30 Texture arrays are not available in OpenGL ES 2.0.
			 * @requires_webgl20 Texture arrays are not available in WebGL 1.0.
			 */
			PhongGLShadows &bindAmbientTexture(GL::Texture2DArray &texture);
#endif

			/**
			 * @brief Bind a diffuse texture
			 * @return Reference to self (for method chaining)
			 *
			 * Expects that the shader was created with @ref Flag::DiffuseTexture
			 * enabled. If @ref Flag::TextureArrays is enabled as well, use
			 * @ref bindDiffuseTexture(GL::Texture2DArray&) instead. If
			 * @ref lightCount() is zero, this function is a no-op, as diffuse
			 * color doesn't contribute to the output in that case.
			 * @see @ref bindTextures(), @ref setDiffuseColor()
			 */
			PhongGLShadows &bindDiffuseTexture(GL::Texture2D &texture);

#ifndef MAGNUM_TARGET_GLES2
			/**
			 * @brief Bind a diffuse array texture
			 * @return Reference to self (for method chaining)
			 * @m_since_latest
			 *
			 * Expects that the shader was created with both
			 * @ref Flag::DiffuseTexture and @ref Flag::TextureArrays
			 * enabled. If @ref Flag::UniformBuffers is not enabled, the layer is
			 * set via @ref setTextureLayer(); if @ref Flag::UniformBuffers is
			 * enabled, @ref Flag::TextureTransformation has to be enabled as well
			 * and the layer is set via @ref TextureTransformationUniform::layer.
			 * If @ref lightCount() is zero, this function is a no-op, as diffuse
			 * color doesn't contribute to the output in that case.
			 * @see @ref setDiffuseColor()
			 * @requires_gl30 Extension @gl_extension{EXT,texture_array}
			 * @requires_gles30 Texture arrays are not available in OpenGL ES 2.0.
			 * @requires_webgl20 Texture arrays are not available in WebGL 1.0.
			 */
			PhongGLShadows &bindDiffuseTexture(GL::Texture2DArray &texture);
#endif

			/**
			 * @brief Bind a specular texture
			 * @return Reference to self (for method chaining)
			 *
			 * Expects that the shader was created with @ref Flag::SpecularTexture
			 * enabled and that @ref Flag::NoSpecular is not set. If
			 * @ref Flag::TextureArrays is enabled as well, use
			 * @ref bindSpecularTexture(GL::Texture2DArray&) instead. If
			 * @ref lightCount() is zero, this function is a no-op, as specular
			 * color doesn't contribute to the output in that case.
			 * @see @ref bindTextures(), @ref setSpecularColor()
			 */
			PhongGLShadows &bindSpecularTexture(GL::Texture2D &texture);

#ifndef MAGNUM_TARGET_GLES2
			/**
			 * @brief Bind a specular array texture
			 * @return Reference to self (for method chaining)
			 * @m_since_latest
			 *
			 * Expects that the shader was created with both
			 * @ref Flag::SpecularTexture and @ref Flag::TextureArrays enabled and
			 * that @ref Flag::NoSpecular is not set. If @ref Flag::UniformBuffers
			 * is not enabled, the layer is set via @ref setTextureLayer(); if
			 * @ref Flag::UniformBuffers is enabled,
			 * @ref Flag::TextureTransformation has to be enabled as well and the
			 * layer is set via @ref TextureTransformationUniform::layer. If
			 * @ref lightCount() is zero, this function is a no-op, as specular
			 * color doesn't contribute to the output in that case.
			 * @see @ref setSpecularColor()
			 * @requires_gl30 Extension @gl_extension{EXT,texture_array}
			 * @requires_gles30 Texture arrays are not available in OpenGL ES 2.0.
			 * @requires_webgl20 Texture arrays are not available in WebGL 1.0.
			 */
			PhongGLShadows &bindSpecularTexture(GL::Texture2DArray &texture);
#endif

			/**
			 * @brief Bind a normal texture
			 * @return Reference to self (for method chaining)
			 * @m_since{2019,10}
			 *
			 * Expects that the shader was created with @ref Flag::NormalTexture
			 * enabled and the @ref Tangent attribute was supplied. If
			 * @ref Flag::TextureArrays is enabled as well, use
			 * @ref bindNormalTexture(GL::Texture2DArray&) instead. If
			 * @ref lightCount() is zero, this function is a no-op, as normals
			 * don't contribute to the output in that case.
			 * @see @ref Shaders-PhongGL-normal-mapping,
			 *      @ref bindTextures(), @ref setNormalTextureScale()
			 */
			PhongGLShadows &bindNormalTexture(GL::Texture2D &texture);

#ifndef MAGNUM_TARGET_GLES2
			/**
			 * @brief Bind a normal array texture
			 * @return Reference to self (for method chaining)
			 * @m_since_latest
			 *
			 * Expects that the shader was created with both
			 * @ref Flag::NormalTexture and @ref Flag::TextureArrays enabled and
			 * the @ref Tangent attribute was supplied. If @ref lightCount() is
			 * zero, this function is a no-op, as normals don't contribute to the
			 * output in that case.
			 * @see @ref Shaders-PhongGL-normal-mapping,
			 *      @ref setNormalTextureScale()
			 */
			PhongGLShadows &bindNormalTexture(GL::Texture2DArray &texture);
#endif

#ifndef MAGNUM_TARGET_GLES2
			/**
			 * @brief Bind an object ID texture
			 * @return Reference to self (for method chaining)
			 * @m_since_latest
			 *
			 * Expects that the shader was created with @ref Flag::ObjectIdTexture
			 * enabled. If @ref Flag::TextureArrays is enabled as well, use
			 * @ref bindObjectIdTexture(GL::Texture2DArray&) instead. The texture
			 * needs to have an unsigned integer format.
			 * @see @ref setObjectId(), @ref Flag::TextureTransformation,
			 *      @ref setTextureMatrix()
			 * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
			 * @requires_gles30 Object ID output requires integer support in
			 *      shaders, which is not available in OpenGL ES 2.0.
			 * @requires_webgl20 Object ID output requires integer support in
			 *      shaders, which is not available in WebGL 1.0.
			 */
			PhongGLShadows &bindObjectIdTexture(GL::Texture2D &texture);

			/**
			 * @brief Bind an object ID array texture
			 * @return Reference to self (for method chaining)
			 * @m_since_latest
			 *
			 * Expects that the shader was created with both
			 * @ref Flag::ObjectIdTexture and @ref Flag::TextureArrays enabled. If
			 * @ref Flag::UniformBuffers is not enabled, the layer is set via
			 * @ref setTextureLayer(); if @ref Flag::UniformBuffers is enabled,
			 * @ref Flag::TextureTransformation has to be enabled as well and the
			 * layer is set via @ref TextureTransformationUniform::layer.
			 * @see @ref setObjectId(), @ref Flag::TextureTransformation,
			 *      @ref setTextureLayer()
			 * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4} and
			 *      @gl_extension{EXT,texture_array}
			 * @requires_gles30 Object ID output requires integer support in
			 *      shaders, which is not available in OpenGL ES 2.0. Texture
			 *      arrays are not available in OpenGL ES 2.0.
			 * @requires_webgl20 Object ID output requires integer support in
			 *      shaders, which is not available in WebGL 1.0. Texture arrays
			 *      are not available in WebGL 1.0.
			 */
			PhongGLShadows &bindObjectIdTexture(GL::Texture2DArray &texture);
#endif

			/**
			 * @brief Bind textures
			 * @return Reference to self (for method chaining)
			 *
			 * A particular texture has effect only if particular texture flag from
			 * @ref PhongGL::Flag "Flag" is set, you can use @cpp nullptr @ce for
			 * the rest. Expects that the shader was created with at least one of
			 * @ref Flag::AmbientTexture, @ref Flag::DiffuseTexture,
			 * @ref Flag::SpecularTexture or @ref Flag::NormalTexture enabled and
			 * @ref Flag::TextureArrays is not set. More efficient than setting
			 * each texture separately.
			 * @see @ref bindAmbientTexture(), @ref bindDiffuseTexture(),
			 *      @ref bindSpecularTexture(), @ref bindNormalTexture()
			 */
			PhongGLShadows &bindTextures(GL::Texture2D *ambient, GL::Texture2D *diffuse, GL::Texture2D *specular, GL::Texture2D *normal
#ifdef MAGNUM_BUILD_DEPRECATED
																												  = nullptr
#endif
			);

/**
 * @}
 */

/* Overloads to remove WTF-factor from method chaining order */
#ifndef DOXYGEN_GENERATING_OUTPUT
			PhongGLShadows &draw(GL::Mesh &mesh)
			{
				return static_cast<PhongGLShadows &>(GL::AbstractShaderProgram::draw(mesh));
			}
			PhongGLShadows &draw(GL::Mesh &&mesh)
			{
				return static_cast<PhongGLShadows &>(GL::AbstractShaderProgram::draw(mesh));
			}
			PhongGLShadows &draw(GL::MeshView &mesh)
			{
				return static_cast<PhongGLShadows &>(GL::AbstractShaderProgram::draw(mesh));
			}
			PhongGLShadows &draw(GL::MeshView &&mesh)
			{
				return static_cast<PhongGLShadows &>(GL::AbstractShaderProgram::draw(mesh));
			}
			PhongGLShadows &draw(GL::Mesh &mesh, const Containers::StridedArrayView1D<const UnsignedInt> &counts, const Containers::StridedArrayView1D<const UnsignedInt> &vertexOffsets, const Containers::StridedArrayView1D<const UnsignedInt> &indexOffsets)
			{
				return static_cast<PhongGLShadows &>(GL::AbstractShaderProgram::draw(mesh, counts, vertexOffsets, indexOffsets));
			}
#ifndef CORRADE_TARGET_32BIT
			PhongGLShadows &draw(GL::Mesh &mesh, const Containers::StridedArrayView1D<const UnsignedInt> &counts, const Containers::StridedArrayView1D<const UnsignedInt> &vertexOffsets, const Containers::StridedArrayView1D<const UnsignedLong> &indexOffsets)
			{
				return static_cast<PhongGLShadows &>(GL::AbstractShaderProgram::draw(mesh, counts, vertexOffsets, indexOffsets));
			}
			PhongGLShadows &draw(GL::Mesh &mesh, const Containers::StridedArrayView1D<const UnsignedInt> &counts, const Containers::StridedArrayView1D<const UnsignedInt> &vertexOffsets, std::nullptr_t)
			{
				return static_cast<PhongGLShadows &>(GL::AbstractShaderProgram::draw(mesh, counts, vertexOffsets, nullptr));
			}
#endif
			PhongGLShadows &draw(Containers::ArrayView<const Containers::Reference<GL::MeshView>> meshes)
			{
				return static_cast<PhongGLShadows &>(GL::AbstractShaderProgram::draw(meshes));
			}
			PhongGLShadows &draw(std::initializer_list<Containers::Reference<GL::MeshView>> meshes)
			{
				return static_cast<PhongGLShadows &>(GL::AbstractShaderProgram::draw(meshes));
			}
#endif

			/**
			 * @brief Set shadowmap matrices
			 *
			 * Matrix that transforms from world space -> shadow texture space.
			 */
			PhongGLShadows &setShadowmapMatrices(Containers::ArrayView<const Matrix4> matrices);
			/** @brief Set shadow map texture array */
			PhongGLShadows &setShadowmapTexture(GL::Texture2DArray &texture);

			PhongGLShadows &setModelMatrix(const Matrix4 matrix);

			/**
			 * @brief Set thadow bias uniform
			 *
			 * Normally it wants to be something from 0.0001 -> 0.001.
			 */
			PhongGLShadows &setShadowBias(Float bias);

		private:
			/* Creates the GL shader program object but does nothing else.
			   Internal, used by compile(). */
			explicit PhongGLShadows(NoInitT) {}

/* Prevent accidentally calling irrelevant functions */
#ifndef MAGNUM_TARGET_GLES
			using GL::AbstractShaderProgram::drawTransformFeedback;
#endif
#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
			using GL::AbstractShaderProgram::dispatchCompute;
#endif

			Flags _flags;
			UnsignedInt _lightCount{};
#ifndef MAGNUM_TARGET_GLES2
			UnsignedInt _materialCount{}, _drawCount{};
#endif
			Int _transformationMatrixUniform{0},
				_projectionMatrixUniform{1},
				_normalMatrixUniform{2},
				_textureMatrixUniform{3},
#ifndef MAGNUM_TARGET_GLES2
				_textureLayerUniform{4},
#endif
				_ambientColorUniform{5},
				_diffuseColorUniform{6},
				_specularColorUniform{7},
				_shininessUniform{8},
				_normalTextureScaleUniform{9},
				_alphaMaskUniform{10};
#ifndef MAGNUM_TARGET_GLES2
			Int _objectIdUniform{11};
#endif
			Int _lightPositionsUniform{12},
				_lightColorsUniform,		 /* 12 + lightCount, set in the constructor */
				_lightSpecularColorsUniform, /* 12 + 2*lightCount */
				_lightRangesUniform;		 /* 12 + 3*lightCount */
#ifndef MAGNUM_TARGET_GLES2
			/* Used instead of all other uniforms when Flag::UniformBuffers is set,
			   so it can alias them */
			Int _drawOffsetUniform{0};
#endif

			Int _shadowmapMatrixUniform,
				_shadowBiasUniform, _modelMatrixUniform;
			enum : Int
			{
				ShadowmapTextureLayer = 69
			};
		};

		/**
		@brief Asynchronous compilation state
		@m_since_latest

		Returned by @ref compile(). See @ref shaders-async for more information.
		*/
		class PhongGLShadows::CompileState : public PhongGLShadows
		{
			/* Everything deliberately private except for the inheritance */
			friend class PhongGLShadows;

			explicit CompileState(NoCreateT) : PhongGLShadows{NoCreate}, _vert{NoCreate}, _frag{NoCreate} {}

			explicit CompileState(PhongGLShadows &&shader, GL::Shader &&vert, GL::Shader &&frag, GL::Version version) : PhongGLShadows{std::move(shader)}, _vert{std::move(vert)}, _frag{std::move(frag)}, _version{version} {}

			Implementation::GLShaderWrapper _vert, _frag;
			GL::Version _version;
		};

		/** @debugoperatorclassenum{PhongGL,PhongGL::Flag} */
		MAGNUM_SHADERS_EXPORT Debug &operator<<(Debug &debug, PhongGLShadows::Flag value);

		/** @debugoperatorclassenum{PhongGL,PhongGL::Flags} */
		MAGNUM_SHADERS_EXPORT Debug &operator<<(Debug &debug, PhongGLShadows::Flags value);

		CORRADE_ENUMSET_OPERATORS(PhongGLShadows::Flags)

	}
}
