#include "TextFileLoader.h"

#include <array>
#include <fstream>
#include <iostream>
#include <utility>

bool LoadTextFile(
    const std::filesystem::path& path,
    std::string& output
) {
    std::ifstream file(path, std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Failed to open text file: " << path << '\n';
        return false;
    }

    std::string temporary;
    std::array<char, 4096> buffer{};

    while (file.read(buffer.data(), buffer.size()) || file.gcount() > 0) {
        temporary.append(buffer.data(), static_cast<std::size_t>(file.gcount()));
    }

    // 到达 EOF 是正常结束；其他流错误表示读取失败。
    if (!file.eof()) {
        std::cerr << "Failed to read text file: " << path << '\n';
        return false;
    }

    output = std::move(temporary);
    return true;
}
