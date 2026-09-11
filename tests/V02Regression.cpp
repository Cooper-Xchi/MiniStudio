#include "camera/Camera.h"
#include "image/ImageLoader.h"
#include "io/TextFileLoader.h"
#include "mesh/MeshData.h"
#include "mesh/PrimitiveMeshes.h"
#include "opengl/OpenGLHeaders.h"
#include "platform/GlfwWindow.h"
#include "render/OpenGLDebug.h"
#include "render/RenderCommand.h"
#include "render/Renderer.h"
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {
void Require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

bool Near(float a, float b, float tolerance = 0.0001f) {
    return std::abs(a - b) <= tolerance;
}

void CheckTextFileLoading() {
    std::string source;
    Require(LoadTextFile("assets/shaders/scene.vert", source),
            "External vertex Shader did not load");
    Require(source.find("#version 410 core") != std::string::npos &&
            source.find("gl_Position") != std::string::npos,
            "External vertex Shader contents are incomplete");

    Require(LoadTextFile("assets/shaders/scene.frag", source),
            "External fragment Shader did not load");
    Require(source.find("#version 410 core") != std::string::npos &&
            source.find("fragment_color") != std::string::npos,
            "External fragment Shader contents are incomplete");

    source = "preserve caller output";
    std::cout << "Expected missing-text-file diagnostic follows:\n";
    Require(!LoadTextFile("assets/shaders/__missing__.frag", source) &&
            source == "preserve caller output",
            "Text loading failure must preserve the caller's output");
    std::cout << "PASS external Shader text loading and failure rollback\n";
}

void CheckPrimitiveMeshes() {
    const MeshData cube = CreateTexturedCubeMesh();
    Require(cube.vertices.size() == 24 && cube.indices.size() == 36,
            "Textured cube must contain 24 vertices and 36 indices");

    const std::array<glm::vec2, 4> expected_uvs{{{0, 0}, {1, 0}, {1, 1}, {0, 1}}};
    const std::array<glm::vec3, 4> expected_colors{{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}, {1, 1, 1}}};
    const std::array<glm::vec3, 6> expected_normals{{
        {0, 0, 1}, {0, 0, -1}, {-1, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, -1, 0}
    }};
    for (std::size_t vertex = 0; vertex < cube.vertices.size(); ++vertex) {
        const auto& value = cube.vertices[vertex];
        Require(glm::length(value.uv - expected_uvs[vertex % 4]) < 0.0001f,
                "Cube face UV layout changed during MeshData extraction");
        Require(glm::length(value.color - expected_colors[vertex % 4]) < 0.0001f,
                "Cube face colors changed during MeshData extraction");
        Require(Near(std::abs(value.position.x), 0.5f) &&
                Near(std::abs(value.position.y), 0.5f) &&
                Near(std::abs(value.position.z), 0.5f),
                "Cube position data changed during MeshData extraction");
        Require(glm::length(value.normal - expected_normals[vertex / 4]) < 0.0001f,
                "Cube face normal is incorrect");
    }
    for (std::size_t face = 0; face < 6; ++face) {
        const auto vertex = static_cast<std::uint32_t>(face * 4);
        const auto index = face * 6;
        Require(cube.indices[index] == vertex && cube.indices[index + 1] == vertex + 1 &&
                cube.indices[index + 2] == vertex + 2 && cube.indices[index + 3] == vertex + 2 &&
                cube.indices[index + 4] == vertex + 3 && cube.indices[index + 5] == vertex,
                "Cube triangle indices changed during MeshData extraction");
    }

    const MeshData plane = CreateTexturedPlaneMesh();
    Require(plane.vertices.size() == 4 && plane.indices == std::vector<std::uint32_t>({0, 1, 2, 2, 3, 0}),
            "Textured plane topology changed during MeshData extraction");
    for (std::size_t vertex = 0; vertex < plane.vertices.size(); ++vertex) {
        const auto& value = plane.vertices[vertex];
        Require(Near(std::abs(value.position.x), 0.8f) &&
                Near(std::abs(value.position.y), 0.8f) && Near(value.position.z, 0.0f),
                "Textured plane position data changed during MeshData extraction");
        Require(glm::length(value.uv - expected_uvs[vertex]) < 0.0001f,
                "Textured plane UV layout changed during MeshData extraction");
        Require(glm::length(value.normal - glm::vec3(0, 0, 1)) < 0.0001f,
                "Textured plane normal is incorrect");
    }
    std::cout << "PASS CPU primitive MeshData contracts\n";
}

struct Frame {
    int width;
    int height;
    std::vector<unsigned char> rgba;
    std::vector<float> depth;
};

Frame ReadFrame(int width, int height) {
    const auto pixels = static_cast<std::size_t>(width) * height;
    Frame frame{width, height, std::vector<unsigned char>(pixels * 4), std::vector<float>(pixels)};
    glReadBuffer(GL_BACK);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, frame.rgba.data());
    glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, frame.depth.data());
    Require(OpenGLDebug::CheckErrors("regression readback"), "GPU readback failed");
    return frame;
}

void SavePpm(const Frame& frame, const std::filesystem::path& path) {
    std::ofstream out(path, std::ios::binary);
    out << "P6\n" << frame.width << ' ' << frame.height << "\n255\n";
    for (int y = frame.height - 1; y >= 0; --y) {
        for (int x = 0; x < frame.width; ++x) {
            const auto index = (static_cast<std::size_t>(y) * frame.width + x) * 4;
            out.write(reinterpret_cast<const char*>(frame.rgba.data() + index), 3);
        }
    }
    Require(static_cast<bool>(out), "Could not write capture");
}

void CheckCamera() {
    Camera camera;
    const glm::vec3 position(0.25f, 0.0f, 1.5f);
    camera.SetPosition(position);
    Require(glm::length(glm::vec3(camera.ViewMatrix() * glm::vec4(position, 1))) < 0.0001f,
            "View must map the camera position to the origin");
    camera.Move(glm::vec3(0.5f, 0.0f, 0.0f));
    Require(Near(camera.ViewMatrix()[3].x, -0.75f), "Camera movement/view mismatch");
    camera.Rotate(90.0f, 0.0f);
    Require(glm::length(camera.Forward() - glm::vec3(1, 0, 0)) < 0.0001f, "Yaw rotation failed");
    camera.Rotate(0.0f, 200.0f);
    Require(Near(camera.Forward().y, std::sin(glm::radians(89.0f))), "Pitch clamp failed");
    Require(Near(glm::length(camera.Forward()), 1) && Near(glm::length(camera.Right()), 1) &&
            Near(glm::dot(camera.Forward(), camera.Right()), 0), "Camera basis is not orthonormal");
    std::cout << "PASS camera movement, yaw, pitch clamp and basis\n";
}

void CheckResourcesAndCutout(int width, int height) {
    GLint program_id = 0, vao_id = 0, vbo_id = 0, ebo_id = 0, texture_id = 0;
    {
        const char* vertex = R"(#version 410 core
layout(location=0) in vec3 position;
layout(location=2) in vec2 uv;
out vec2 uv_coord;
void main() { uv_coord=uv; gl_Position=vec4(position,1); }
)";
        const char* fragment = R"(#version 410 core
in vec2 uv_coord;
out vec4 fragment_color;
uniform sampler2D tex;
uniform int cutout;
uniform vec4 tint;
void main() {
    vec4 value=texture(tex,uv_coord)*tint;
    if (cutout != 0 && value.a < 0.5) discard;
    fragment_color=value;
}
)";
        MeshData quad_mesh;
        quad_mesh.vertices = {
            MeshData::Vertex{{-1, -1, 0}, {1, 1, 1}, {0, 0}},
            MeshData::Vertex{{ 1, -1, 0}, {1, 1, 1}, {1, 0}},
            MeshData::Vertex{{ 1,  1, 0}, {1, 1, 1}, {1, 1}},
            MeshData::Vertex{{-1,  1, 0}, {1, 1, 1}, {0, 1}},
        };
        quad_mesh.indices = {0, 1, 2, 2, 3, 0};

        ShaderProgram original_shader;
        Require(original_shader.Initialize(vertex, fragment), "Cutout shader initialization failed");
        ShaderProgram shader(std::move(original_shader));
        Require(!original_shader.SetVec4("tint", glm::vec4(1)), "Moved-from shader should be empty");
        shader.Use();
        Require(shader.SetInt("tex", 0) && shader.SetVec4("tint", glm::vec4(1)), "Uniform upload failed");
        Require(!shader.SetVec4("missing_uniform", glm::vec4(1)), "Missing uniform must fail");
        glGetIntegerv(GL_CURRENT_PROGRAM, &program_id);
        std::array<float, 4> uploaded{};
        glGetUniformfv(static_cast<GLuint>(program_id), glGetUniformLocation(static_cast<GLuint>(program_id), "tint"), uploaded.data());
        Require(std::all_of(uploaded.begin(), uploaded.end(), [](float v) { return Near(v, 1); }), "vec4 was not copied to the Program");

        VertexArray empty_quad;
        Require(!empty_quad.Initialize(MeshData{}), "Empty MeshData must fail");

        MeshData invalid_mesh = quad_mesh;
        invalid_mesh.indices.push_back(static_cast<std::uint32_t>(invalid_mesh.vertices.size()));
        VertexArray invalid_quad;
        Require(!invalid_quad.Initialize(invalid_mesh), "Out-of-range mesh index must fail");

        VertexArray original_quad;
        Require(original_quad.Initialize(quad_mesh), "Quad initialization failed");
        VertexArray quad(std::move(original_quad));
        Require(original_quad.VertexCount() == 0 && original_quad.IndexCount() == 0 &&
                quad.VertexCount() == 4 && quad.IndexCount() == 6,
                "VAO move lost mesh ownership/counts");
        quad.Bind();
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &vao_id);
        glGetVertexAttribiv(0, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &vbo_id);
        glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &ebo_id);
        Texture2D original_texture;
        Require(!original_texture.Initialize(0, 1, nullptr), "Invalid texture input must fail");
        {
            const unsigned char pixels[] = {255,0,0,0, 0,255,0,255};
            Require(original_texture.Initialize(2, 1, pixels), "Texture initialization failed");
        }
        Texture2D texture(std::move(original_texture));
        texture.Bind(0);
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &texture_id);
        RenderCommand::SetGlobalDepth(true, true, RenderCommand::DepthCompare::LESS);
        RenderCommand::SetBlendingEnabled(false);
        RenderCommand::SetFaceCullingEnabled(false);
        for (int cutout : {0, 1}) {
            RenderCommand::Clear(0, 0, 0, 1);
            Require(shader.SetInt("cutout", cutout), "Cutout toggle failed");
            RenderCommand::DrawIndexedTriangles(quad.IndexCount());
            const auto frame = ReadFrame(width, height);
            const auto left = static_cast<std::size_t>(height / 2) * width + width / 4;
            const auto right = static_cast<std::size_t>(height / 2) * width + width * 3 / 4;
            Require(frame.rgba[left * 4] == (cutout ? 0 : 255), "Alpha-zero/discard color contract failed");
            Require(Near(frame.depth[left], cutout ? 1.0f : 0.5f), "Discard must suppress depth writes");
            Require(frame.rgba[right * 4 + 1] == 255 && Near(frame.depth[right], 0.5f), "Solid cutout region failed");
        }
    }
    glUseProgram(0);
    Require(!glIsProgram(static_cast<GLuint>(program_id)) && !glIsVertexArray(static_cast<GLuint>(vao_id)) &&
            !glIsBuffer(static_cast<GLuint>(vbo_id)) && !glIsBuffer(static_cast<GLuint>(ebo_id)) &&
            !glIsTexture(static_cast<GLuint>(texture_id)), "RAII did not release GPU resources");
    Require(OpenGLDebug::CheckErrors("resource/cutout regression"), "Resource/cutout GL error");
    ImageData image;
    Require(LoadImageRgba("assets/textures/lesson21-quadrants.png", image), "Bundled PNG did not load");
    const auto original = image.rgba_pixels;
    std::cout << "Expected missing-file diagnostic follows:\n";
    Require(!LoadImageRgba("assets/textures/__v02_missing__.png", image) && image.rgba_pixels == original,
            "Image loading failure must preserve the caller's output");
    std::cout << "PASS vec4 upload, MeshData validation, resource moves/destruction, PNG failure path and alpha cutout\n";
}

// Independent pixel-ray reference, not the renderer's object-center sorting algorithm.
float CubeIntersection(const glm::vec3& eye, const glm::vec3& direction) {
    const glm::vec3 low(-0.15f, -0.25f, -2.25f), high(0.35f, 0.25f, -1.75f);
    float entry = 0.0f, leave = std::numeric_limits<float>::infinity();
    for (int axis = 0; axis < 3; ++axis) {
        if (std::abs(direction[axis]) < 0.000001f) {
            if (eye[axis] < low[axis] || eye[axis] > high[axis]) return -1;
        } else {
            const float a = (low[axis] - eye[axis]) / direction[axis];
            const float b = (high[axis] - eye[axis]) / direction[axis];
            entry = std::max(entry, std::min(a, b));
            leave = std::min(leave, std::max(a, b));
        }
    }
    return leave >= entry ? entry : -1;
}

void CheckPixels(const Frame& frame, const glm::vec3& eye, const glm::mat4& view) {
    const glm::mat4 projection = glm::perspective(glm::radians(45.0f),
        static_cast<float>(frame.width) / frame.height, 0.1f, 100.0f);
    const glm::mat4 inverse = glm::inverse(projection * view);
    const std::array<glm::vec3, 2> centers{{{0.2f, 0.2f, -1.0f}, {-0.2f, -0.2f, -1.5f}}};
    const std::array<glm::vec3, 2> colors{{{1,0,0}, {0,0,1}}};
    std::array<int, 4> region_counts{};
    int cube_samples = 0;
    for (int y = 7; y < frame.height; y += 13) {
        for (int x = 7; x < frame.width; x += 13) {
            const float nx = 2.0f * (x + 0.5f) / frame.width - 1.0f;
            const float ny = 2.0f * (y + 0.5f) / frame.height - 1.0f;
            const glm::vec4 near_point = inverse * glm::vec4(nx, ny, -1, 1);
            const glm::vec3 ray = glm::normalize(glm::vec3(near_point) / near_point.w - eye);
            const auto index = static_cast<std::size_t>(y) * frame.width + x;
            const float cube_t = CubeIntersection(eye, ray);
            if (cube_t > 0) {
                const glm::vec4 clip = projection * view * glm::vec4(eye + cube_t * ray, 1);
                Require(Near(frame.depth[index], clip.z / clip.w * 0.5f + 0.5f), "Opaque cube depth/transparent depth mask failed");
                ++cube_samples;
                continue;
            }
            std::array<float, 2> distances{-1, -1};
            bool edge = false;
            int mask = 0;
            for (int i = 0; i < 2; ++i) {
                const float t = (centers[i].z - eye.z) / ray.z;
                const glm::vec3 delta = glm::abs(eye + t * ray - centers[i]);
                edge = edge || (std::abs(delta.x - 0.8f) < 0.01f || std::abs(delta.y - 0.8f) < 0.01f);
                if (t > 0 && delta.x < 0.8f && delta.y < 0.8f) {
                    distances[i] = t;
                    mask |= 1 << i;
                }
            }
            if (edge) continue;
            glm::vec3 expected(0.36f, 0.5f, 0.6f);
            const float alpha = 128.0f / 255.0f;
            const int first = distances[0] > distances[1] ? 0 : 1;
            for (int i : {first, 1 - first}) {
                if (distances[i] > 0) expected = colors[i] * alpha + expected * (1 - alpha);
            }
            for (int channel = 0; channel < 3; ++channel) {
                if (std::abs(static_cast<int>(frame.rgba[index * 4 + channel]) - std::lround(expected[channel] * 255)) > 2) {
                    std::ostringstream message;
                    message << "Blend color mismatch at " << x << ',' << y << " channel=" << channel
                            << " actual=" << static_cast<int>(frame.rgba[index * 4 + channel])
                            << " expected=" << expected[channel] * 255;
                    throw std::runtime_error(message.str());
                }
            }
            Require(Near(frame.depth[index], 1.0f), "Transparent surface wrote depth");
            ++region_counts[mask];
        }
    }
    Require(cube_samples > 0 && std::all_of(region_counts.begin(), region_counts.end(), [](int n) { return n > 0; }),
            "Reference did not cover cube/background/red/blue/overlap");
    std::cout << "PASS pixel rays " << frame.width << 'x' << frame.height << " regions=";
    for (int n : region_counts) std::cout << n << ' ';
    std::cout << "cube=" << cube_samples << '\n';
}

Frame Render(Renderer& renderer, int width, int height, const glm::mat4& view, float time = 0) {
    Require(renderer.DrawFrame(time, width, height, view), "Renderer returned failure");
    GLboolean writes = GL_FALSE;
    glGetBooleanv(GL_DEPTH_WRITEMASK, &writes);
    Require(writes == GL_TRUE && glIsEnabled(GL_DEPTH_TEST) && glIsEnabled(GL_CULL_FACE) &&
            !glIsEnabled(GL_BLEND), "Renderer leaked transparent state");
    return ReadFrame(width, height);
}

void CaptureDemo(Renderer& renderer, int width, int height, const std::filesystem::path& directory) {
    std::filesystem::create_directories(directory);
    for (int frame = 0; frame < 90; ++frame) {
        const float angle = glm::radians(frame * 4.0f);
        const glm::vec3 target(0.0f, 0.0f, -1.5f);
        const glm::vec3 eye = target + glm::vec3(3 * std::sin(angle), 0.4f, 3 * std::cos(angle));
        std::ostringstream name;
        name << "frame-" << std::setw(3) << std::setfill('0') << frame << ".ppm";
        SavePpm(Render(renderer, width, height, glm::lookAt(eye, target, glm::vec3(0,1,0)), frame / 15.0f), directory / name.str());
    }
    std::cout << "Captured 90 GPU frames (6 seconds at 15 fps) to " << directory.string() << '\n';
}

void RunIntegration(const std::filesystem::path& captures) {
    GlfwWindow window;
    Require(window.Initialize(640, 480, "MiniStudio v0.2 regression", false), "Window/context initialization failed");
    GLFWwindow* native = glfwGetCurrentContext();
    Require(native != nullptr, "Missing current context");
    glfwHideWindow(native);
    glfwSwapInterval(0);
    window.PollEvents();
    glDisable(GL_DITHER);
    int width = 0, height = 0;
    window.GetFramebufferSize(width, height);
    Require(width > 0 && height > 0, "Empty framebuffer");
    CheckResourcesAndCutout(width, height);
    GLint renderer_program = 0;
    {
        Renderer renderer;
        Require(renderer.Initialize(), "Renderer initialization failed");
        const glm::vec3 front_eye(0.25f, 0.0f, 1.5f), back_eye(0.25f, 0.0f, -4.0f);
        const auto front = glm::lookAt(front_eye, front_eye + glm::vec3(0,0,-1), glm::vec3(0,1,0));
        const auto back = glm::lookAt(back_eye, back_eye + glm::vec3(0,0,1), glm::vec3(0,1,0));
        const auto baseline = Render(renderer, width, height, front);
        CheckPixels(baseline, front_eye, front);
        CheckPixels(Render(renderer, width, height, back), back_eye, back);
        Require(Render(renderer, width, height, front, 0.125f).rgba != baseline.rgba, "Cube rotation did not change the frame");
        Require(Render(renderer, width, height, front).rgba == baseline.rgba, "Stale color/depth survived into the next frame");
        Require(renderer.DrawFrame(0, 0, height, front) && renderer.DrawFrame(0, width, 0, front), "Zero-size frame was not skipped");
        Require(ReadFrame(width, height).rgba == baseline.rgba, "Zero-size frame modified color");
        RenderCommand::SetDepthWriteEnabled(false);
        RenderCommand::SetBlendingEnabled(true);
        RenderCommand::SetDepthCompare(RenderCommand::DepthCompare::GREATER);
        RenderCommand::SetGlobalCullFace(true, RenderCommand::CullFace::Front, RenderCommand::FrontFaceWinding::Clockwise);
        Require(Render(renderer, width, height, front).rgba == baseline.rgba, "Frame start did not restore the opaque baseline");
        for (const auto& size : {std::array<int,2>{960,480}, std::array<int,2>{480,640}}) {
            glfwSetWindowSize(native, size[0], size[1]);
            window.PollEvents();
            window.GetFramebufferSize(width, height);
            GLint viewport[4]{};
            glGetIntegerv(GL_VIEWPORT, viewport);
            Require(viewport[2] == width && viewport[3] == height, "Resize callback did not update viewport");
            CheckPixels(Render(renderer, width, height, front), front_eye, front);
            window.Present();
        }
        if (!captures.empty()) {
            glfwSetWindowSize(native, 640, 480);
            window.PollEvents();
            window.GetFramebufferSize(width, height);
            CaptureDemo(renderer, width, height, captures);
        }
        const auto mouse = window.ReadMouseInput();
        Require(mouse.delta_x == 0 && mouse.delta_y == 0, "Uncaptured cursor should not rotate camera");
        Require(!window.ShouldClose(), "Window closed unexpectedly");
        window.RequestClose();
        Require(window.ShouldClose(), "RequestClose did not set the close flag");
        glGetIntegerv(GL_CURRENT_PROGRAM, &renderer_program);
        std::cout << "PASS animation, repeated frames, state restoration, zero-size, resize and close flag\n";
    }
    glUseProgram(0);
    Require(!glIsProgram(static_cast<GLuint>(renderer_program)), "Renderer Program survived destruction");
    Require(OpenGLDebug::CheckErrors("v0.2 teardown"), "GPU teardown error");
}
}

int main(int argc, char** argv) {
    try {
        if (argc != 1 && (argc != 3 || std::string(argv[1]) != "--capture-dir")) {
            std::cerr << "Usage: ministudio_regression [--capture-dir directory]\n";
            return 2;
        }
        CheckTextFileLoading();
        CheckPrimitiveMeshes();
        CheckCamera();
        RunIntegration(argc == 3 ? std::filesystem::path(argv[2]) : std::filesystem::path{});
        std::cout << "PASS MiniStudio v0.2 regression; resources released before Context\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "FAIL " << error.what() << '\n';
        return 1;
    }
}
