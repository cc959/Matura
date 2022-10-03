#include "ImageFrame.h"

GL::Mesh *ImageFrame::_mesh = nullptr;
Shaders::FlatGL2D *ImageFrame::_shader = nullptr;

ImageFrame::ImageFrame(Containers::Optional<Trade::ImageData2D>& image, const Timeline &timeline, ImGuiIntegration::Context &guiContext) : ImageFrame{timeline, guiContext}
{
	FromImage(image);
}

ImageFrame::ImageFrame(string path, const Timeline &timeline, ImGuiIntegration::Context &guiContext) : ImageFrame{timeline, guiContext}
{
	PluginManager::Manager<Trade::AbstractImporter> manager;
	Trade::AnyImageImporter importer(manager);

	if (!importer.openFile(path))
	{
		Warning{} << "Unable to open image" << path;
	}

	Containers::Optional<Trade::ImageData2D> image = importer.image2D(0);

	FromImage(image);
}

ImageFrame::ImageFrame(Containers::ArrayView<const char> raw, const Timeline &timeline, ImGuiIntegration::Context &guiContext) : ImageFrame{timeline, guiContext}
{
	PluginManager::Manager<Trade::AbstractImporter> manager;
	Trade::AnyImageImporter importer(manager);

	if (!importer.openData(raw))
	{
		Warning{} << "Unable to open image";
	}

	Containers::Optional<Trade::ImageData2D> image = importer.image2D(0);

	FromImage(image);
}

void ImageFrame::FromImage(Containers::Optional<Trade::ImageData2D>& image)
{
	CORRADE_INTERNAL_ASSERT(image);

	_texture.setWrapping(GL::SamplerWrapping::ClampToEdge)
		.setMagnificationFilter(GL::SamplerFilter::Linear)
		.setMinificationFilter(GL::SamplerFilter::Linear)
		.setStorage(1, GL::textureFormat(image->format()), image->size())
		.setSubImage(0, {}, *image);
}

ImageFrame::ImageFrame(const Timeline &timeline, ImGuiIntegration::Context &guiContext) : Frame{timeline, guiContext}
{
	if (!_shader)
		_shader = new Shaders::FlatGL2D(Shaders::FlatGL2D::Flag::Textured);

	if (!_mesh)
	{

		struct QuadVertex
		{
			Vector2 position;
			Vector2 textureCoordinates;
		};

		_mesh = new GL::Mesh();

		(*_mesh) = MeshTools::compile(Primitives::planeSolid(Primitives::PlaneFlag::TextureCoordinates));
	}
}

void ImageFrame::draw3D()
{
	_shader->setColor(Color4(1, 1, 1, 1)).bindTexture(_texture).draw(*_mesh);
}

void ImageFrame::setupGUI()
{
}