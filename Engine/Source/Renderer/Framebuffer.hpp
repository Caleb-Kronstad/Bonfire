#pragma once

namespace Bonfire
{
    class Framebuffer
    {
    public:
        Framebuffer(unsigned int width, unsigned int height);
        ~Framebuffer();

        void Bind();
        void Unbind();
        void Resize(unsigned int width, unsigned int height);

        unsigned int GetColorAttachment() const { return color_attachment; }
        unsigned int GetWidth() const { return width; }
        unsigned int GetHeight() const { return height; }

    private:
        void Create();
        void Destroy();

        unsigned int frame_buffer = 0;
        unsigned int color_attachment = 0;
        unsigned int depth_stencil_attachment = 0;
        unsigned int width;
        unsigned int height;
    };
}
