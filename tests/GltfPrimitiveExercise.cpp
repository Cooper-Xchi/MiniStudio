#include <array>
#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>

#include "mesh/GltfMeshLoader.h"

namespace {

MeshData MakeSentinel() {
    MeshData mesh;
    mesh.vertices.push_back({glm::vec3(9.0f), glm::vec3(0.25f),
                             glm::vec2(0.5f), glm::vec3(1.0f, 0.0f, 0.0f)});
    mesh.indices = {42};
    return mesh;
}

bool IsSentinel(const MeshData& mesh) {
    return mesh.vertices.size() == 1 && mesh.indices == std::vector<std::uint32_t>{42}
        && mesh.vertices[0].position == glm::vec3(9.0f)
        && mesh.vertices[0].color == glm::vec3(0.25f)
        && mesh.vertices[0].uv == glm::vec2(0.5f)
        && mesh.vertices[0].normal == glm::vec3(1.0f, 0.0f, 0.0f);
}

bool CheckTriangle(const std::filesystem::path& path) {
    MeshData mesh = MakeSentinel();
    std::string error = "stale error";
    if (!LoadGltfPrimitive(path, mesh, error)) {
        std::cerr << "FAIL " << path.filename() << ": " << error << '\n';
        return false;
    }
    const std::array expected_positions{
        glm::vec3(-0.5f, -0.5f, 0.0f),
        glm::vec3(0.5f, -0.5f, 0.0f),
        glm::vec3(0.0f, 0.5f, 0.0f)
    };
    bool correct = error.empty() && mesh.vertices.size() == 3
        && mesh.indices == std::vector<std::uint32_t>{0, 1, 2};
    if (mesh.vertices.size() == 3) {
        for (std::size_t i = 0; i < 3; ++i) {
            const auto& vertex = mesh.vertices[i];
            correct = correct && vertex.position == expected_positions[i]
                && vertex.normal == glm::vec3(0.0f, 0.0f, 1.0f)
                && vertex.color == glm::vec3(1.0f) && vertex.uv == glm::vec2(0.0f);
        }
    }
    std::cout << (correct ? "PASS " : "FAIL ") << path.filename() << " values\n";
    return correct;
}

bool CheckRejected(const std::filesystem::path& path, std::string_view expected_error = {}) {
    if (path.filename() != "missing.gltf" && !std::filesystem::is_regular_file(path)) {
        std::cerr << "FAIL missing rejection fixture: " << path << '\n';
        return false;
    }
    MeshData mesh = MakeSentinel();
    std::string error;
    const bool loaded = LoadGltfPrimitive(path, mesh, error);
    // An unfinished reader rejecting everything is not evidence of validation.
    const bool correct = !loaded && !error.empty() && !error.starts_with("TODO")
        && (expected_error.empty() || error.find(expected_error) != std::string::npos)
        && IsSentinel(mesh);
    std::cout << (correct ? "PASS " : "FAIL ") << path.filename()
              << " rejection + rollback: " << error << '\n';
    return correct;
}

} // namespace

int main(int argc, char** argv) {
    const std::filesystem::path directory = argc > 1 ? argv[1] : "assets/models/lesson63";
    constexpr std::array valid_fixtures{
        "packed.gltf", "interleaved.gltf",
        "index-view-offset-2.gltf", "index-accessor-offset-2.gltf"
    };
    for (const char* name : valid_fixtures) {
        if (!std::filesystem::is_regular_file(directory / name)) {
            std::cerr << "Fixture not found: " << directory / name
                      << "\nRun from the repository or executable directory, or pass a fixture directory.\n";
            return 1;
        }
    }
    bool passed = true;
    for (const char* name : valid_fixtures) {
        passed = CheckTriangle(directory / name) && passed;
    }
    for (const char* name : {"short-view.gltf", "bad-type.gltf", "sparse.gltf",
                              "bad-accessor.gltf", "missing.gltf"}) {
        passed = CheckRejected(directory / name) && passed;
    }
    // Ensure these fail for the intended index rule, not an unrelated early rejection.
    passed = CheckRejected(directory / "bad-index.gltf", "Index refers to a nonexistent vertex") && passed;
    passed = CheckRejected(directory / "reserved-index.gltf", "Index 65535 is reserved") && passed;
    std::cout << (passed ? "Lesson 63 CPU checks passed.\n"
                        : "Lesson 63 is not complete; fix the failed checks.\n");
    return passed ? 0 : 1;
}
