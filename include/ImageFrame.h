#pragma once

#include "GlobalIncludes.h"
#include "Joystick.h"
#include "Frame.h"

class ImageFrame : public Frame
{
public:
	ImageFrame(string path, const Timeline &timeline, ImGuiIntegration::Context &guiContext);
	ImageFrame(Containers::ArrayView<const char> raw, const Timeline &timeline, ImGuiIntegration::Context &guiContext);

	ImageFrame(Containers::Optional<Trade::ImageData2D> &image, const Timeline &timeline, ImGuiIntegration::Context &guiContext);

private:
	ImageFrame(const Timeline &timeline, ImGuiIntegration::Context &guiContext);

	void FromImage(Containers::Optional<Trade::ImageData2D> &image);

	void draw3D() override;
	void setupGUI() override;

	static GL::Mesh *_quad;
	static Shaders::FlatGL2D *_shader;
	GL::Texture2D _texture;
};