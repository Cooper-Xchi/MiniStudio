# MiniStudio

MiniStudio 是一个持续演进的现代 C++ 与实时渲染学习项目。它的目标不是快速拼出一个大型引擎，而是通过一系列始终可编译、可运行的小版本，逐步建立图形开发所需的工程、调试和架构能力。

## 学习目标

- 掌握现代 C++ 的对象生命周期、RAII、所有权、移动语义和常用工程工具。
- 理解 CPU 到 GPU 的数据流，以及 Buffer、Shader、Texture、Framebuffer 等核心对象。
- 逐步实现模型加载、材质与光照、后处理、资源管理、异步加载、动画和编辑界面。
- 建立 Android/OpenGL ES、Vulkan 和移动端性能经验，再用 Metal 验证跨后端设计。
- 打通图片、动画、视频、3D 素材与 AI Agent/Tool Calling 的客户端创作链路。
- 通过真实调试与性能数据验证结果，而不是把“看完教程”当作完成标准。

当前第一套图形 API 是 **OpenGL 4.1 Core Profile + GLFW + macOS OpenGL.framework**。完成桌面渲染器后，目标岗位路线依次进入 Android/OpenGL ES 和 Vulkan；Metal 放在专家能力进阶阶段验证。第一套渲染器完成前不并行学习多套 API。

## 当前进度

项目已经完成前 2 周和第 3 周第 9～11 课。当前已经建立最小应用、窗口、`ShaderProgram` 与顶点资源模块边界，下一步完成第一条 GPU 绘制链路。

已经完成：

- 初始化本地 Git 仓库和最小 CMake 工程。
- 配置 C++20，以及 Clang/GCC 的常用编译警告。
- 成功配置、编译并运行生命周期实验。
- 初步学习构造、析构、`std::move` 和 `std::unique_ptr`。
- 完成第一课的 CLion 生命周期调试，观察栈对象、堆对象、`this`、`get()` 和逆序析构。
- 完成第二课的 `unique_ptr` 移动实验，验证所有权转移、移动后空状态、对象地址稳定和单次析构。
- 完成第三课的 RAII 与函数所有权边界实验，理解引用借用、按值转移以及所有者离开作用域时的自动释放。
- 完成第四课的 Sanitizer 故障定位实验，使用 AddressSanitizer 定位并修复一次受控的 heap-use-after-free。
- 完成第五课的 GLFW 窗口实验，理解 CMake 包查找与链接、GLFW 初始化、窗口句柄、关闭标志和手动清理顺序。
- 完成第六课的 OpenGL Context 实验，理解创建前 hints、主线程 Context 绑定、实际属性查询和版本不可用时的失败行为。
- 完成第七课的事件循环与键盘输入实验，理解事件轮询、按键状态、关闭标志和统一清理流程。
- 完成第八课的窗口尺寸与 framebuffer 尺寸实验，使用实际 framebuffer 像素设置 viewport，并在尺寸变化时通过回调同步更新。
- 完成第九课的项目骨架整理，将启动、应用流程和 GLFW 窗口资源拆分为单向依赖的 `main`、`Application` 与 `GlfwWindow`。
- 完成第十课的 `ShaderProgram` 模块，能够编译顶点/片元 Shader、链接 Program、输出完整错误日志，并按 Context 生命周期释放 OpenGL 资源。
- 完成第十一课的 `VertexArray` 模块，将交错排列的位置/颜色数据上传到 VBO，以 VAO 记录 attribute 布局，并按 Context 生命周期释放顶点资源。

前十课均已完成并合并回 `main`。第十一课已在 `codex/lesson-11-vertex-resources` 通过技术与口头验收，当前等待提交、推送并合并；尚未执行三角形 draw call。

仓库使用 `main` 保存已验收的稳定基线，并通过 `origin` 同步到 GitHub。独立的仓库用 SSH 密钥已配置为可写 Deploy key。已合并的课程分支均继续保留；后续课程遵守相同的独立分支规则。项目级 AI 协作边界和课程分支规则记录在 [`AGENTS.md`](AGENTS.md)。

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
| v0.9～v1.0 | Vulkan、RenderDevice，以及 AI 素材任务和 Tool Calling 接入 |

## 24～36 个月课程路线

| 时间 | 重点 | 目标 |
| --- | --- | --- |
| 第 1～6 月 | C++、OpenGL、模型、材质、光照与后处理 | 建成第一套可解释、可调试的桌面渲染器 |
| 第 7～12 月 | 场景、资源、热更新、异步加载、动画与 ImGui | 形成第一版公开作品集 |
| 第 13～18 月 | 性能分析、稳定性、Android 与 OpenGL ES | 获得真机兼容和优化证据 |
| 第 19～24 月 | Vulkan、跨后端抽象、素材引擎与 AI Tool Calling | 具备投递同方向岗位的完整项目证据 |
| 第 25～30 月 | 2D 动画、视频素材和 Metal | 扩展多媒体与第三平台能力 |
| 第 31～36 月 | 大场景、性能架构、AI 工作流和真实协作 | 建立高级/专家方向的能力证据 |

完整的阶段目标、验收标准、前四周课程和求职时间线见 [`docs/MiniStudio-curriculum-24-36-months.md`](docs/MiniStudio-curriculum-24-36-months.md)。24 个月是核心路线，25～36 个月是进阶路线；课程不能替代岗位要求的商业项目年限。

## 仓库结构

```text
MiniStudio/
├── AGENTS.md
├── CMakeLists.txt
├── README.md
├── docs/
│   ├── MiniStudio-curriculum-24-36-months.md
│   └── MiniStudio-learning-handoff-2026-09-03.md
└── src/
    ├── main.cpp
    ├── app/
    │   ├── Application.h
    │   └── Application.cpp
    ├── platform/
    │   ├── GlfwWindow.h
    │   └── GlfwWindow.cpp
    └── render/
        ├── ShaderProgram.h
        ├── ShaderProgram.cpp
        ├── VertexArray.h
        └── VertexArray.cpp
```

完整课程路线见 [`docs/MiniStudio-curriculum-24-36-months.md`](docs/MiniStudio-curriculum-24-36-months.md)，当前状态和下一步见 [`docs/MiniStudio-learning-handoff-2026-09-03.md`](docs/MiniStudio-learning-handoff-2026-09-03.md)。README 只提供稳定的项目入口和概览；AI 助手在本仓库中的操作和教学边界见 [`AGENTS.md`](AGENTS.md)。
