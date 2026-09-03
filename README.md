# MiniStudio

MiniStudio 是一个持续演进的现代 C++ 与实时渲染学习项目。它的目标不是快速拼出一个大型引擎，而是通过一系列始终可编译、可运行的小版本，逐步建立图形开发所需的工程、调试和架构能力。

## 学习目标

- 掌握现代 C++ 的对象生命周期、RAII、所有权、移动语义和常用工程工具。
- 理解 CPU 到 GPU 的数据流，以及 Buffer、Shader、Texture、Framebuffer 等核心对象。
- 逐步实现模型加载、材质与光照、后处理、资源管理、异步加载、动画和编辑界面。
- 通过真实调试与性能数据验证结果，而不是把“看完教程”当作完成标准。

当前的图形 API 路线是 **OpenGL 4.1 Core Profile + GLFW + macOS OpenGL.framework**。OpenGL 只作为第一套学习后端；完成基础渲染器后，再根据目标平台选择 Metal 或 Vulkan。

## 当前进度

项目已经完成第 1 周：**C++ 生命周期、RAII、`std::unique_ptr` 与基本工具链**。

已经完成：

- 初始化本地 Git 仓库和最小 CMake 工程。
- 配置 C++20，以及 Clang/GCC 的常用编译警告。
- 成功配置、编译并运行生命周期实验。
- 初步学习构造、析构、`std::move` 和 `std::unique_ptr`。
- 完成第一课的 CLion 生命周期调试，观察栈对象、堆对象、`this`、`get()` 和逆序析构。
- 完成第二课的 `unique_ptr` 移动实验，验证所有权转移、移动后空状态、对象地址稳定和单次析构。
- 完成第三课的 RAII 与函数所有权边界实验，理解引用借用、按值转移以及所有者离开作用域时的自动释放。
- 完成第四课的 Sanitizer 故障定位实验，使用 AddressSanitizer 定位并修复一次受控的 heap-use-after-free。

尚未开始 GLFW 或 OpenGL。第 1 周的四课均已完成并合并回 `main`；下一步是在新的课程分支开始第 2 周。

仓库使用 `main` 保存已验收的稳定基线，并通过 `origin` 同步到 GitHub。独立的仓库用 SSH 密钥已配置为可写 Deploy key。四个已合并的课程分支均继续保留；后续课程遵守相同的独立分支规则。项目级 AI 协作边界和课程分支规则记录在 [`AGENTS.md`](AGENTS.md)。

## 构建与运行

当前已在 macOS arm64、Apple Clang 和 CMake 环境下验证。终端使用 Unix Makefiles：

```bash
cmake -S . -B build \
  -G "Unix Makefiles" \
  -DCMAKE_BUILD_TYPE=Debug

cmake --build build --parallel
./build/ministudio
```

CLion 可以使用自身提供的 Ninja，并将构建产物放在独立的 `cmake-build-*` 目录中。

需要 AddressSanitizer 和 UndefinedBehaviorSanitizer 时，使用独立构建目录：

```bash
cmake -S . -B cmake-build-sanitize \
  -G "Unix Makefiles" \
  -DCMAKE_BUILD_TYPE=Debug \
  -DMINISTUDIO_ENABLE_SANITIZERS=ON

cmake --build cmake-build-sanitize --parallel
./cmake-build-sanitize/ministudio
```

## 版本路线

| 阶段 | 重点 |
| --- | --- |
| v0.1～v0.2 | CMake、窗口、三角形、纹理、相机、变换和 Shader 错误处理 |
| v0.3～v0.4 | 图片/glTF、基础材质与光照、Framebuffer、阴影、PBR 和后处理 |
| v0.5～v0.6 | 场景层级、资源缓存、Handle、热重载、后台解析和主线程 GPU 上传 |
| v0.7～v0.8 | 动画、粒子、ImGui，以及一次有数据对比的性能优化 |
| v0.9～v1.0 | RenderDevice、Metal/Vulkan 迁移，以及模拟 AI 素材任务接入 |

## 仓库结构

```text
MiniStudio/
├── AGENTS.md
├── CMakeLists.txt
├── README.md
├── MiniStudio-learning-handoff-2026-09-03.md
└── src/
    └── main.cpp
```

详细的学习背景、路线、当前状态和下一步验收问题见 [`MiniStudio-learning-handoff-2026-09-03.md`](MiniStudio-learning-handoff-2026-09-03.md)。该交接文档是当前学习进度的主要记录；README 只提供稳定的项目入口和概览。AI 助手在本仓库中的操作和教学边界见 [`AGENTS.md`](AGENTS.md)。
