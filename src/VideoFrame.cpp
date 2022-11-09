#include "VideoFrame.h"

GL::Mesh *VideoFrame::_quad = nullptr;
Shaders::FlatGL2D *VideoFrame::_shader = nullptr;


VideoFrame::VideoFrame(string path, const Timeline &timeline, ImGuiIntegration::Context &guiContext) : Frame{timeline,
                                                                                                             guiContext} {
    if (!_shader)
        _shader = new Shaders::FlatGL2D(Shaders::FlatGL2D::Flag::Textured);

    if (!_quad) {

        struct QuadVertex {
            Vector2 position;
            Vector2 textureCoordinates;
        };

        _quad = new GL::Mesh();

        (*_quad) = MeshTools::compile(Primitives::planeSolid(Primitives::PlaneFlag::TextureCoordinates));
    }


    _cap.open(path);
    _fps = _cap.get(CAP_PROP_FPS);

    loadToRam(_bufferFrames);
}

void VideoFrame::loadToRam(int count) {
    if (count > 1) {
        for (int i = 0; i < count; i++)
            loadToRam();
        return;
    }

    Mat frame;
    _captureMutex.lock();
    {
        while (!_cap.read(frame))
            _cap.set(CAP_PROP_POS_FRAMES, 0);
    }
    _captureMutex.unlock();


    cvtColor(frame, frame, COLOR_BGR2RGB);
    flip(frame, frame, 0);

    _ramBufferMutex.unlock();
    _ramFrames[_loadedFrameAt] = std::move(frame);
    _ramBufferMutex.unlock();

    cerr << "Loaded frame " << _loadedFrameAt << "to ram!\n";

    _loadedFrameAt++;

}

void VideoFrame::loadToVRam() {
    _ramBufferMutex.lock();

    for (auto [index, frame]: _ramFrames) {
        Vector2i size(frame.size().width, frame.size().height);

        auto data = Containers::Array<char>((char *) (frame.data),
                                            frame.elemSize() * frame.size().width * frame.size().height);
        ImageView2D image = ImageView2D(PixelFormat::RGB8Unorm, size, data);

        _bufferMutex.lock();
        _frames[index]
                .setWrapping(GL::SamplerWrapping::ClampToEdge)
                .setMagnificationFilter(GL::SamplerFilter::Linear)
                .setMinificationFilter(GL::SamplerFilter::Linear)
                .setStorage(1, GL::textureFormat(image.format()), image.size())
                .setSubImage(0, {}, image);
        _bufferMutex.unlock();

        data.release();
    }
    _ramFrames.clear();
    _ramBufferMutex.unlock();
}

void VideoFrame::draw3D() {
    loadToVRam();

    int frame = (_timeline.previousFrameTime() - _startTime) / (1 / _fps);

    _bufferMutex.lock();
    if (_frames.begin()->first < frame) {
        _frames.erase(_frames.begin());
    }

    if (_frames.size() < _bufferFrames) {
        thread bufferThread(&VideoFrame::loadToRam, this, _bufferFrames - _frames.size());
        bufferThread.detach();
    }

    _shader->setColor(Color4(1, 1, 1, 1)).bindTexture(_frames[frame]).draw(*_quad);
    _bufferMutex.unlock();
}

void VideoFrame::setupGUI() {
}

void VideoFrame::Leave() {
    _frames.clear();
    _ramFrames.clear();

    _cap.set(CAP_PROP_POS_FRAMES, 0);
    _loadedFrameAt = 0;
    thread bufferThread(&VideoFrame::loadToRam, this, int(VideoFrame::_bufferFrames));
    bufferThread.detach();
}

void VideoFrame::Enter() {
    _startTime = _timeline.previousFrameTime();
}

