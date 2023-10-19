#pragma once

#include <filesystem>
#include <fstream>
#include <string>

inline std::string text_file_contents(const std::filesystem::path& fpath)
{
    std::string contents;
    std::ifstream stream(fpath);
    stream.seekg(0, std::ios::end);
    contents.reserve(stream.tellg());
    stream.seekg(0, std::ios::beg);
    contents.assign((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
    return contents;
}
