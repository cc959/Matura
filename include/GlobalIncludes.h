#pragma once

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/DebugStl.h>
#include <Magnum/ImageView.h>
#include <Magnum/Mesh.h>
#include <Magnum/PixelFormat.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/Math/Color.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/TranslationRotationScalingTransformation3D.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/SceneGraph/Object.h>
#include <Magnum/Shaders/PhongGL.h>
#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/ImageData.h>
#include <Magnum/Trade/MeshData.h>
#include <Magnum/Trade/PhongMaterialData.h>
#include <Magnum/Trade/SceneData.h>
#include <Magnum/Trade/LightData.h>
#include <Magnum/Trade/TextureData.h>
#include <Magnum/Trade/AnimationData.h>
#include <Magnum/Trade/CameraData.h>
#include <MagnumPlugins/AnyImageImporter/AnyImageImporter.h>
#include <Magnum/Timeline.h>
#include <Magnum/Image.h>
#include <Magnum/Animation/Animation.h>
#include <Magnum/Animation/Player.h>
#include <Magnum/GL/PixelFormat.h>
#include <Magnum/Math/Quaternion.h>
#include <MagnumPlugins/AnySceneImporter/AnySceneImporter.h>
#include <MagnumPlugins/GltfImporter/GltfImporter.h>
#include <Magnum/ImGuiIntegration/Context.hpp>
#include <MagnumPlugins/AnySceneImporter/AnySceneImporter.h>
#include <Magnum/Resource.h>
#include <Magnum/GL/Framebuffer.h>
#include <Magnum/GL/TextureArray.h>
#include <Magnum/SceneGraph/AbstractFeature.h>
#include <Magnum/Shaders/VertexColorGL.h>
#include <Magnum/Shaders/VectorGL.h>
#include <Magnum/Shaders/DistanceFieldVectorGL.h>
#include <Magnum/Text/Renderer.h>
#include <Magnum/Text/GlyphCache.h>
#include <Magnum/Text/AbstractFont.h>
#include <MagnumPlugins/StbTrueTypeFont/StbTrueTypeFont.h>

#include <Corrade/Containers/Iterable.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/Containers/StringStl.h>
#include <Corrade/Utility/Resource.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Matrix4.h>

#include <Magnum/ImageView.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/PixelFormat.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/SceneGraph/FeatureGroup.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/TextureTools/DistanceField.h>

#include <Magnum/Primitives/Plane.h>

#include <bits/stdc++.h>

using namespace std;

using namespace Magnum;

using namespace Containers;
using namespace Math::Literals;

typedef SceneGraph::Object<SceneGraph::TranslationRotationScalingTransformation3D> Object3D;
typedef SceneGraph::Scene<SceneGraph::TranslationRotationScalingTransformation3D> Scene3D;

typedef Platform::Sdl2Application SDLApp;