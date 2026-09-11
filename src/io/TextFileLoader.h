//
// Created by 陈泓池 on 2026/9/11.
//
#pragma once
#include <filesystem>
#include <string>

[[nodiscard]] bool LoadTextFile(
    const std::filesystem::path& path,
    std::string& output
);
