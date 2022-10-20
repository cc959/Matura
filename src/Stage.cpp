
#include "Stage.h"

Stage::Stage(string path)
{
	PluginManager::Manager<Trade::AbstractImporter> manager;

	Trade::AnySceneImporter importer(manager);

	if (importer.openFile(path))
		Import(importer);
}

Stage::Stage(Trade::AnySceneImporter &importer)
{
	Import(importer);
}

Stage::Stage()
{
}

void Stage::Import(Trade::AnySceneImporter &importer)
{

	_manipulator.setParent(&_scene);

	_textures = ImportTextures(importer);
	_materials = ImportMaterials(importer);
	_meshes = ImportMeshes(importer);

	Optional<Trade::SceneData> scene;
	if (!(scene = importer.scene(importer.defaultScene())) || !scene->is3D() || !scene->hasField(Trade::SceneField::Parent) || !scene->hasField(Trade::SceneField::Mesh))
	{
		Fatal{} << "Cannot load scene" << importer.defaultScene() << importer.sceneName(importer.defaultScene());
	}

    reset();

	Containers::Array<Containers::Pair<UnsignedInt, Int>> parents = scene->parentsAsArray(); // object index, parent

	for (const Containers::Pair<UnsignedInt, Int> &parent : parents) // instantiate objects
        addObject(new Object3D{}, parent.first(), importer.objectName(parent.first()));

	for (const Containers::Pair<UnsignedInt, Int> &parent : parents) // set parents of objects
        getObject(parent.first())->setParent(parent.second() == -1 ? &_manipulator : getObject(parent.second()));

	for (const Containers::Pair<UnsignedInt, Matrix4> &transformation : scene->transformations3DAsArray()) // object index, transformations
	{
		if (Object3D *object = getObject(transformation.first()))
			object->setTransformation(transformation.second());
	}

	for (int i = 0; i < importer.lightCount(); i++)
	{
		Optional<Trade::LightData> light = importer.light(i);

		if (!light)
			continue;
		if (light->type() == Trade::LightData::Type::Ambient)
			_ambientLight += light->color() * light->intensity();
		if (light->type() == Trade::LightData::Type::Directional)
		{
			_lights.push_back(Vector4{getObject(importer.lightName(i))->translation(), 0});
			_lightColors.push_back(light->color() * light->intensity());
		}
		if (light->type() == Trade::LightData::Type::Point)
		{
			_lights.push_back(Vector4{getObject(importer.lightName(i))->translation(), 1});
			_lightColors.push_back(light->color() * light->intensity());
		}
		Debug{} << light->color() << " * " << light->intensity();
	}

	for (const Containers::Pair<UnsignedInt, Containers::Pair<UnsignedInt, Int>> &
			 meshMaterial : scene->meshesMaterialsAsArray())
	{
		Object3D *object = getObject(meshMaterial.first());		   // object to assign material to
		auto &[mesh, radius] = _meshes[meshMaterial.second().first()]; // mesh to assign material to
		if (!object || !mesh)
			continue;

		Int materialId = meshMaterial.second().second();

		new ShadowCasterDrawable(*object, *mesh, radius, _shadowCasters);

		/* Material not available / not loaded, use a default material */
		if (materialId == -1 || !_materials[materialId])
		{
			Debug{} << "Object has no material";
			// new ColoredDrawable{*object, _coloredShader, *mesh, 0xffffff_rgbf,
			// 					_drawables};
		}
		else
		{
			/* Color-only material */
			new PhongDrawable{*object, *this, *mesh, (*_materials[materialId]), _shadowReceivers};
		}
	}

	_cameras = vector<SceneGraph::Camera3D *>(importer.cameraCount());

	if (importer.cameraCount())
		for (auto cameraObject : scene->camerasAsArray())
		{
			Object3D *cameraManipulator = new Object3D(getObject(cameraObject.first()));

            addObject(cameraManipulator, importer.cameraName(cameraObject.second()) + "_Blender");

			_cameras[cameraObject.second()] = new SceneGraph::Camera3D(*cameraManipulator);
			cameraManipulator->rotateYLocal(-90.0_degf);
		}

	for (int i = 0; i < importer.cameraCount(); i++)
	{
		if (!importer.camera(importer.cameraName(i)))
		{
			Warning{} << "Error loading camera" << importer.cameraName(i);
		}

		Trade::CameraData cameraData = *importer.camera(importer.cameraName(i));

		if (cameraData.type() == Trade::CameraType::Perspective3D)
		{
			(*_cameras[i])
				.setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
				.setProjectionMatrix(Matrix4::perspectiveProjection(cameraData.fov(), cameraData.aspectRatio(), cameraData.near(), cameraData.far()))
				.setViewport(GL::defaultFramebuffer.viewport().size());

			Debug{} << cameraData.fov();
		}
		else
		{
			(*_cameras[i])
				.setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
				.setProjectionMatrix(Matrix4::orthographicProjection(cameraData.size(), cameraData.near(), cameraData.far()))
				.setViewport(GL::defaultFramebuffer.viewport().size());
		}
	}

	if (_cameras.empty())
	{
        addCamera(30.0_degf, 0.1, 1000, "Camera");
	}

	_player = ImportAnimations(importer, _animationData, *this);
}

int Stage::getCameraID(SceneGraph::Camera3D *camera)
{
	auto it = find(_cameras.begin(), _cameras.end(), camera);
	if (it == _cameras.end())
		return -1;
	else
		return it - _cameras.begin();
}

Animation::Player<Float> Stage::ImportAnimations(Trade::AnySceneImporter &importer, vector<Optional<Trade::AnimationData>> &animationData, Objects &objects)
{
	Animation::Player<Float> player;
	// import animations
	animationData = vector<Optional<Trade::AnimationData>>(importer.animationCount());
	for (int j = 0; j < importer.animationCount(); j++)
	{
		Debug{} << importer.animationName(j);
		animationData[j] = importer.animation(j);

		auto &data = animationData[j];

		if (!data)
		{
			Warning{} << "Unable to import animation " << importer.animationName(j);
			continue;
		}

		for (UnsignedInt i = 0; i != data->trackCount(); ++i)
		{
			Object3D &animatedObject = *objects.getObject(data->trackTarget(i));
			Debug{} << data->trackTarget(i) << ":" << data->trackTargetType(i);
			if (data->trackTargetType(i) == Trade::AnimationTrackTargetType::Translation3D)
			{
				const auto callback = [](Float, const Vector3 &translation, Object3D &object)
				{
					object.setTranslation(translation);
				};
				CORRADE_INTERNAL_ASSERT(data->trackType(i) == Trade::AnimationTrackType::Vector3);
				player.addWithCallback(data->track<Vector3>(i), callback, animatedObject);
				// Debug{} << data->track<Vector3>(i).values();
			}

			if (data->trackTargetType(i) == Trade::AnimationTrackTargetType::Rotation3D)
			{
				const auto callback = [](Float, const Quaternion &rotation, Object3D &object)
				{
					object.setRotation(rotation);
				};
				assert(data->trackType(i) == Trade::AnimationTrackType::Quaternion);
				player.addWithCallback(data->track<Quaternion>(i), callback, animatedObject);

				// Debug{} << data->track<Quaternion>(i).values();
			}

			if (data->trackTargetType(i) == Trade::AnimationTrackTargetType::Scaling3D)
			{
				const auto callback = [](Float, const Vector3 &scaling, Object3D &object)
				{
					object.setScaling(scaling);
				};
				CORRADE_INTERNAL_ASSERT(data->trackType(i) ==
										Trade::AnimationTrackType::Vector3);
				player.addWithCallback(data->track<Vector3>(i), callback, animatedObject);
			}
		}
	}
	return player;
}

vector<Optional<GL::Texture2D>> Stage::ImportTextures(Trade::AnySceneImporter &importer)
{
	vector<Optional<GL::Texture2D>> textures{importer.textureCount()};
	for (UnsignedInt i = 0; i != importer.textureCount(); ++i)
	{
		Optional<Trade::TextureData> textureData =
			importer.texture(i);
		if (!textureData || textureData->type() != Trade::TextureType::Texture2D)
		{
			Warning{} << "Cannot load texture" << i
					  << importer.textureName(i);
			continue;
		}

		Optional<Trade::ImageData2D> imageData =
			importer.image2D(textureData->image());

		if (!imageData /*|| !imageData->isCompressed()*/)
		{
			Warning{} << "Cannot load image" << textureData->image()
					  << importer.image2DName(textureData->image());
			continue;
		}
		else
		{
			Debug{} << "Loaded image" << importer.image2DName(textureData->image());
		}

		(*(textures[i] = GL::Texture2D{}))
			.setMagnificationFilter(textureData->magnificationFilter())
			.setMinificationFilter(textureData->minificationFilter(),
								   textureData->mipmapFilter())
			.setWrapping(GL::SamplerWrapping::Repeat)
			.setStorage(Math::log2(imageData->size().max()) + 1,
						GL::textureFormat(imageData->format()), imageData->size())
			.setSubImage(0, {}, *imageData)
			.generateMipmap();

		Debug{} << imageData->format();
	}

	return textures;
}

vector<Optional<Trade::PhongMaterialData>> Stage::ImportMaterials(Trade::AnySceneImporter &importer)
{
	vector<Optional<Trade::PhongMaterialData>> materials{importer.materialCount()};

	for (UnsignedInt i = 0; i != importer.materialCount(); ++i)
	{
		Optional<Trade::MaterialData> materialData;
		if (!(materialData = importer.material(i)))
		{
			Warning{} << "Cannot load material" << i
					  << importer.materialName(i);
			continue;
		}

		materials[i] = std::move(*materialData).as<Trade::PhongMaterialData>();
	}
	return materials;
}

vector<std::pair<Optional<GL::Mesh>, float>> Stage::ImportMeshes(Trade::AnySceneImporter &importer)
{
	vector<std::pair<Optional<GL::Mesh>, float>> meshes(importer.meshCount());
	for (UnsignedInt i = 0; i != importer.meshCount(); ++i)
	{
		Optional<Trade::MeshData> meshData;
		if (!(meshData = importer.mesh(i)))
		{
			Warning{} << "Cannot load mesh" << i << importer.meshName(i);
			continue;
		}

		float radiusSquared = 0;

		for (auto vertex : meshData->positions3DAsArray())
			radiusSquared = max(radiusSquared, vertex.dot());

		meshes[i].second = sqrt(radiusSquared);

		for (int i = 0; i < meshData->attributeCount(); i++)
			Debug{} << meshData->attributeName(i);

		MeshTools::CompileFlags flags;
		if (!meshData->hasAttribute(Trade::MeshAttribute::Normal))
			flags |= MeshTools::CompileFlag::GenerateFlatNormals;
		meshes[i].first = MeshTools::compile(*meshData, flags);
	}
	return meshes;
}
