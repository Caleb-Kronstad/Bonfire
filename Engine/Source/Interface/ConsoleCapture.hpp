#pragma once

#include "bonfire_pch.hpp"

namespace Bonfire
{
    class ConsoleBuffer : public std::streambuf
    {
    public:
        ConsoleBuffer(std::vector<std::string>& lines, std::mutex& mutex);

    protected:
        virtual int_type overflow(int_type c) override;

    private:
        std::vector<std::string>& lines;
        std::mutex& mutex;
        std::string current_line;
    };

    class ConsoleCapture
    {
    public:
        ConsoleCapture();
        ~ConsoleCapture();

        void StartCapture();
        void StopCapture();
        void Clear();
        std::vector<std::string> GetLines();
        size_t GetLineCount();

    private:
        std::vector<std::string> lines;
        std::mutex mutex;
        ConsoleBuffer buffer;
        std::streambuf* old_cout_buffer;
        std::streambuf* old_cerr_buffer;
    };
}