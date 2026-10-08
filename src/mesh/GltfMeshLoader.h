#pragma once

#include <filesystem>
#include <string>

#include "MeshData.h"

// Lesson 63: load one mesh with one TRIANGLES primitive from an ASCII .gltf.
// POSITION/NORMAL: non-sparse, non-normalized FLOAT VEC3; indices: UNSIGNED_SHORT.
// Mesh-local geometry only. Color defaults to white; UV defaults to zero.
// Synchronous CPU work on the calling thread; no OpenGL context is needed.
// On success output owns all values. On failure output is unchanged and error is set.
[[nodiscard]] bool LoadGltfPrimitive(
    const std::filesystem::path& path,
    MeshData& output,
    std::string& error
);
