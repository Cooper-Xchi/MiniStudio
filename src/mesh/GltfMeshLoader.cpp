#include "GltfMeshLoader.h"

#include <array>
#include <bit>
#include <cstring>
#include <iostream>
#include <limits>
#include <utility>

// One implementation per executable. Image decoding remains in ImageLoader.cpp.
#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include <tiny_gltf.h>

namespace {
    static_assert(std::endian::native == std::endian::little,
                  "Lesson 63 currently supports little-endian hosts only.");
    static_assert(sizeof(float) == 4 && std::numeric_limits<float>::is_iec559);

    bool ReadVec3Accessor(
        const tinygltf::Model &model,
        int accessor_index,
        std::vector<glm::vec3> &output,
        std::string &error
    ) {
        if (model.bufferViews.empty() || model.accessors.empty() || model.buffers.empty()) {
            error = "Model must contain accessors, bufferViews, and buffers";
            return false;
        }
        if (accessor_index < 0 || static_cast<std::size_t>(accessor_index) >= model.accessors.size()) {
            error = "Accessor index out of range";
            return false;
        }
        const auto &accessor = model.accessors[accessor_index];
        if (accessor.type != TINYGLTF_TYPE_VEC3
            || accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT
            || accessor.count == 0
            || accessor.normalized == true
            || accessor.sparse.isSparse == true) {
            error = "Expected nonempty, non-normalized, non-sparse FLOAT VEC3 accessor";
            return false;
        }
        const int view_index = accessor.bufferView;
        if (view_index < 0 || static_cast<std::size_t>(view_index) >= model.bufferViews.size()) {
            error = "bufferView index out of range";
            return false;
        }
        const auto &buffer_view = model.bufferViews[view_index];
        const size_t element_size = 3 * sizeof(float);
        const size_t stride = buffer_view.byteStride == 0 ? element_size : buffer_view.byteStride;
        if (stride < element_size
            || stride % sizeof(float) != 0
            || stride > 252) {
            error = "Invalid byte stride for FLOAT VEC3 accessor";
            return false;
        }
        if (buffer_view.byteOffset % sizeof(float) != 0
            || accessor.byteOffset % sizeof(float) != 0) {
            error = "Invalid byte offset for FLOAT VEC3 accessor";
            return false;
        }
        if (accessor.byteOffset > buffer_view.byteLength) {
            error = "Invalid byte offset for FLOAT VEC3 accessor";
            return false;
        }
        const size_t remaining = buffer_view.byteLength - accessor.byteOffset;
        if (remaining < element_size) {
            error = "Expected at least one element in FLOAT VEC3 accessor";
            return false;
        }
        if (accessor.count - 1 > (remaining - element_size) / stride) {
            error = "Accessor elements exceed bufferView";
            return false;
        }
        const auto buffer_index = buffer_view.buffer;
        if (buffer_index < 0 || static_cast<std::size_t>(buffer_index) >= model.buffers.size()) {
            error = "Buffer index out of range";
            return false;
        }
        const auto &buffer = model.buffers[buffer_index];
        if (buffer_view.byteOffset > buffer.data.size()) {
            error = "bufferView byteOffset exceeds buffer size";
            return false;
        }
        if (buffer_view.byteLength > buffer.data.size() - buffer_view.byteOffset) {
            error = "bufferView exceeds buffer size";
            return false;
        }

        const size_t byte_start = buffer_view.byteOffset + accessor.byteOffset;
        std::array<float, 3> value{};
        std::vector<glm::vec3> temp;
        temp.reserve(accessor.count);
        for (std::size_t i = 0; i < accessor.count; ++i) {
            std::memcpy(value.data(), buffer.data.data() + byte_start + i * stride, element_size);
            temp.emplace_back(value[0], value[1], value[2]);
        }
        output = std::move(temp);
        error.clear();
        return true;
    }

    bool ReadIndexAccessor(
        const tinygltf::Model &model,
        int accessor_index,
        std::vector<std::uint32_t> &output,
        std::string &error
    ) {
        if (model.bufferViews.empty() || model.accessors.empty() || model.buffers.empty()) {
            error = "Model must contain accessors, bufferViews, and buffers";
            return false;
        }
        if (accessor_index < 0 || static_cast<std::size_t>(accessor_index) >= model.accessors.size()) {
            error = "Accessor index out of range";
            return false;
        }
        const auto &accessor = model.accessors[accessor_index];
        if (accessor.type != TINYGLTF_TYPE_SCALAR
            || accessor.componentType != TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT
            || accessor.count == 0
            || accessor.normalized != false
            || accessor.sparse.isSparse != false) {
            error = "Expected nonempty, non-normalized, non-sparse UNSIGNED_SHORT SCALAR accessor";
            return false;
        }
        const int view_index = accessor.bufferView;
        if (view_index < 0 || static_cast<std::size_t>(view_index) >= model.bufferViews.size()) {
            error = "bufferView index out of range";
            return false;
        }
        const auto &buffer_view = model.bufferViews[view_index];
        const size_t element_size = sizeof(std::uint16_t);
        if (buffer_view.byteStride != 0) {
            error = "Index bufferView must not define byteStride";
            return false;
        }
        const size_t stride = element_size;
        if (buffer_view.byteOffset % sizeof(std::uint16_t) != 0
            || accessor.byteOffset % sizeof(std::uint16_t) != 0) {
            error = "Invalid byte alignment for UNSIGNED_SHORT SCALAR accessor";
            return false;
        }
        if (accessor.byteOffset > buffer_view.byteLength) {
            error = "UNSIGNED_SHORT SCALAR accessor byteOffset exceeds bufferView";
            return false;
        }
        const size_t remaining = buffer_view.byteLength - accessor.byteOffset;
        if (remaining < element_size) {
            error = "Expected space for at least one UNSIGNED_SHORT SCALAR element";
            return false;
        }
        if (accessor.count - 1 > (remaining - element_size) / stride) {
            error = "Accessor elements exceed bufferView";
            return false;
        }
        const auto buffer_index = buffer_view.buffer;
        if (buffer_index < 0 || static_cast<std::size_t>(buffer_index) >= model.buffers.size()) {
            error = "Buffer index out of range";
            return false;
        }
        const auto &buffer = model.buffers[buffer_index];
        if (buffer_view.byteOffset > buffer.data.size()) {
            error = "bufferView byteOffset exceeds buffer size";
            return false;
        }
        if (buffer_view.byteLength > buffer.data.size() - buffer_view.byteOffset) {
            error = "bufferView exceeds buffer size";
            return false;
        }

        const size_t byte_start = buffer_view.byteOffset + accessor.byteOffset;
        std::uint16_t value{};
        std::vector<std::uint32_t> temp;
        temp.reserve(accessor.count);
        for (std::size_t i = 0; i < accessor.count; ++i) {
            std::memcpy(&value, buffer.data.data() + byte_start + i * stride, element_size);
            if (value == 65535) {
                error = "Index 65535 is reserved";
                return false;
            }
            // Read 2 file bytes first; appending widens the value to MeshData's 32-bit index.
            temp.emplace_back(value);
        }
        output = std::move(temp);
        error.clear();
        return true;
    }
} // namespace

bool LoadGltfPrimitive(
    const std::filesystem::path &path,
    MeshData &output,
    std::string &error
) {
    error.clear();
    if (path.extension() != ".gltf") {
        error = "Lesson 63 supports ASCII .gltf files only";
        return false;
    }

    tinygltf::TinyGLTF loader;
    tinygltf::Model model;
    std::string warning;
    const bool parsed = loader.LoadASCIIFromFile(&model, &error, &warning, path.string());
    if (!warning.empty()) {
        std::cerr << "glTF warning: " << warning << '\n';
    }
    if (!parsed) {
        if (error.empty()) error = "Failed to parse glTF";
        return false;
    }
    if (!model.extensionsRequired.empty()) {
        error = "Required glTF extensions are not supported in lesson 63";
        return false;
    }
    if (model.meshes.size() != 1 || model.meshes[0].primitives.size() != 1) {
        error = "Expected exactly one mesh with one primitive";
        return false;
    }

    const auto &primitive = model.meshes[0].primitives[0];
    if (primitive.mode != TINYGLTF_MODE_TRIANGLES || !primitive.targets.empty()) {
        error = "Expected TRIANGLES without morph targets";
        return false;
    }
    const auto position = primitive.attributes.find("POSITION");
    const auto normal = primitive.attributes.find("NORMAL");
    if (position == primitive.attributes.end() || normal == primitive.attributes.end()
        || primitive.indices < 0) {
        error = "POSITION, NORMAL, and indices are required in lesson 63";
        return false;
    }

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    MeshData temporary;
    if (!ReadVec3Accessor(model, position->second, positions, error)
        || !ReadVec3Accessor(model, normal->second, normals, error)
        || !ReadIndexAccessor(model, primitive.indices, temporary.indices, error)) {
        return false;
    }
    if (positions.empty() || positions.size() != normals.size()) {
        error = "POSITION and NORMAL must have equal, nonzero counts";
        return false;
    }
    if (temporary.indices.empty() || temporary.indices.size() % 3 != 0) {
        error = "TRIANGLES requires a nonzero multiple of three indices";
        return false;
    }
    for (const auto index: temporary.indices) {
        if (index >= positions.size()) {
            error = "Index refers to a nonexistent vertex";
            return false;
        }
    }

    temporary.vertices.reserve(positions.size());
    for (std::size_t i = 0; i < positions.size(); ++i) {
        temporary.vertices.push_back({positions[i], glm::vec3(1.0f), glm::vec2(0.0f), normals[i]});
    }
    output = std::move(temporary);
    error.clear();
    return true;
}
