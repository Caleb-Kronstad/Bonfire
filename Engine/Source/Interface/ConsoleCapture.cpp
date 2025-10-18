#include "bonfire_pch.hpp"
#include "ConsoleCapture.hpp"

namespace Bonfire
{
    ConsoleBuffer::ConsoleBuffer(std::vector<std::string>& lines, std::mutex& mutex)
        : lines(lines), mutex(mutex), current_line(""), original_buffer(nullptr)
    {
    }

    void ConsoleBuffer::SetOriginalBuffer(std::streambuf* buf)
    {
        original_buffer = buf;
    }

    std::streambuf::int_type ConsoleBuffer::overflow(int_type c)
    {
        if (c != EOF)
        {
            if (original_buffer)
            {
                original_buffer->sputc(c);
            }
            if (c == '\n')
            {
                std::lock_guard<std::mutex> lock(mutex);
                lines.push_back(current_line);
                current_line.clear();
                
                if (lines.size() > 1000)
                {
                    lines.erase(lines.begin());
                }
            }
            else
            {
                current_line += static_cast<char>(c);
            }
        }
        return c;
    }

    ConsoleCapture::ConsoleCapture()
        : buffer(lines, mutex), old_cout_buffer(nullptr), old_cerr_buffer(nullptr)
    {
    }

    ConsoleCapture::~ConsoleCapture()
    {
        StopCapture();
    }

    void ConsoleCapture::StartCapture()
    {
        if (!old_cout_buffer)
        {
            old_cout_buffer = std::cout.rdbuf(&buffer);
            buffer.SetOriginalBuffer(old_cout_buffer);
            std::cout.rdbuf(&buffer);
            old_cerr_buffer = std::cerr.rdbuf(&buffer);
            std::cerr.rdbuf(&buffer);
        }
    }

    void ConsoleCapture::StopCapture()
    {
        if (old_cout_buffer)
        {
            std::cout.rdbuf(old_cout_buffer);
            old_cout_buffer = nullptr;
        }
        if (old_cerr_buffer)
        {
            std::cerr.rdbuf(old_cerr_buffer);
            old_cerr_buffer = nullptr;
        }
    }

    void ConsoleCapture::Clear()
    {
        std::lock_guard<std::mutex> lock(mutex);
        lines.clear();
    }

    std::vector<std::string> ConsoleCapture::GetLines()
    {
        std::lock_guard<std::mutex> lock(mutex);
        return lines;
    }

    size_t ConsoleCapture::GetLineCount()
    {
        std::lock_guard<std::mutex> lock(mutex);
        return lines.size();
    }
}
