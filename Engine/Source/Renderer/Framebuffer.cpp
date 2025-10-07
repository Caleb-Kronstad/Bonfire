#include "bonfire_pch.hpp"
#include "Framebuffer.hpp"

namespace Bonfire
{
    Framebuffer::Framebuffer(unsigned int width, unsigned int height)
        : width(width), height(height)
    {
        Create();
    }
    Framebuffer::~Framebuffer()
    {
        Destroy();
    }

    void Framebuffer::Create()
    {
        glGenFramebuffers(1, &frame_buffer);
        glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

        glGenTextures(1, &color_attachment);
        glBindTexture(GL_TEXTURE_2D, color_attachment);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_attachment, 0);

        glGenTextures(1, &depth_stencil_attachment);
        glBindTexture(GL_TEXTURE_2D, depth_stencil_attachment);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth_stencil_attachment, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            Log::Error("ERROR::FRAMEBUFFER::IS NOT COMPLETE");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::Destroy()
    {
        glDeleteFramebuffers(1, &frame_buffer);
        glDeleteTextures(1, &color_attachment);
        glDeleteTextures(1, &depth_stencil_attachment);
    }

    void Framebuffer::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
        glViewport(0, 0, width, height);
    }

    void Framebuffer::Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::Resize(unsigned int new_width, unsigned int new_height)
    {
        if (new_width == 0 || new_height == 0) return;

        width = new_width;
        height = new_height;
        Destroy();
        Create();
    }
}