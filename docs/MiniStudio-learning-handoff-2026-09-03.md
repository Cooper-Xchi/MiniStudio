# MiniStudio C++ 图形渲染学习交接文档

> 更新时间：2026-09-04
> 用途：作为新 Codex 任务的背景与进度附件。本文是学习上下文，不代表要求一次完成全部路线。新任务应从“当前状态”和“下一步”继续，不要重新初始化项目。

## 1. 学习者背景

- 当前方向：技术美术（Technical Artist），偏实时渲染。
- 已有经验：Unity、HDRP、Shader、材质、纹理、后处理、Blender、Substance Designer / Painter。
- 图形学基础：理解常见实时渲染概念，但知识还不够系统。
- C++ 水平：入门阶段，能阅读部分代码；正在学习对象生命周期、RAII、智能指针、移动语义和工程工具。
- 尚未系统使用 OpenGL、OpenGL ES、Vulkan、Metal 或 DirectX 编写渲染器。
- 学习方式：通过小项目理解原理，不直接复制无法解释的大型成品。
- 默认投入时间：每周 8～10 小时，尚待确认是否长期可持续。

## 2. 长期目标

1. 使用现代 C++ 设计客户端渲染模块。
2. 理解并封装渲染管线、场景系统、资源系统和图形 API。
3. 实现 2D/3D 渲染、材质、纹理、动画、粒子和后处理。
4. 掌握图片、模型、动画等素材的解析、加载、缓存、编辑和实时预览。
5. 掌握多线程资源加载、内存管理、CPU/GPU 同步和性能分析。
6. 理解 Windows、macOS、iOS、Android 的图形兼容问题。
7. 最终理解 AI 素材任务与客户端渲染系统之间的数据协议、异步任务和格式转换。

课程已根据目标岗位重新规划为 24～36 个月：前 24 个月先达到同方向初级岗位或引擎向 TA 的竞争水平，第 25～36 个月继续积累移动端、多媒体、跨后端、性能架构与 AI 创作工具证据。课程不能替代目标岗位要求的多年商业项目经验。

## 3. 学习优先级

1. 现代 C++ 与工程工具
2. 图形学基础和坐标空间
3. OpenGL 小型渲染器
4. 场景、资源、动画及渲染架构
5. 多线程、异步加载和性能分析
6. Android/OpenGL ES 与 Vulkan，之后用 Metal 验证跨后端设计
7. 跨平台抽象
8. AI 素材接入、Agent、Tool Calling 和 RAG

在第一套渲染器完成前，不同时学习多套图形 API，不提前设计跨平台 `RenderDevice`。

## 4. 主项目与版本目标

项目名：**MiniStudio**。版本号表示可验收的项目能力，不与月份一一对应；完整时间线见 `MiniStudio-curriculum-24-36-months.md`。

| 版本 | 目标 |
| --- | --- |
| v0.1 | 创建窗口、绘制三角形、完成基础 CMake 工程 |
| v0.2 | 纹理、相机、坐标变换、Shader 编译和错误输出 |
| v0.3 | 加载图片与 glTF 模型，实现基础材质和光照 |
| v0.4 | Framebuffer、阴影、PBR、Tone Mapping 和简单后处理 |
| v0.5 | 场景层级、资源缓存、Handle、自动释放和 Shader 热重载 |
| v0.6 | 后台解析素材、任务队列、取消机制和主线程 GPU 上传 |
| v0.7 | 简单动画/粒子系统和 ImGui 编辑界面 |
| v0.8 | 定位一次真实性能问题，记录优化前后数据 |
| v0.9 | 使用 Vulkan 迁移核心场景，并在两个真实后端之后抽象 RenderDevice |
| v1.0 | 模拟 AI 素材任务的请求、轮询、取消、重试、下载、缓存与场景导入 |

最终项目需要保留源码、构建说明、架构说明、关键对象生命周期说明、性能报告和演示素材。每个版本必须保持可编译、可运行。

## 5. 已检查的本机环境

| 项目 | 当前状态 |
| --- | --- |
| 操作系统 | macOS 26.5.2，arm64 |
| GPU | Apple M5 Pro，20 核 GPU，支持 Metal 4 |
| 编译器 | Apple Clang 21.0.0 |
| 调试器 | LLDB 2100 |
| CMake | 4.3.1 |
| Git | 2.53.0 |
| 命令行构建工具 | GNU Make 3.81；终端 PATH 中没有 Ninja |
| IDE | Xcode 26.6、CLion 2026.1.2、VS Code 1.133.0 |
| 已有库 | GLFW 3.4、GLM 1.0.3、SDL2 2.32.10 |
| 系统框架 | OpenGL.framework、Metal.framework 均存在 |

完整 Xcode 已安装，但当前 `xcode-select` 指向 Command Line Tools。默认 `xcrun metal` 因此不可用；通过完整 Xcode 的 `DEVELOPER_DIR` 可以找到 Metal 编译器。当前阶段不需要修改系统设置。

CLion 使用它自带的 Ninja，因此 CLion 内可以使用 Ninja；终端练习继续使用 `Unix Makefiles`，两个构建目录分别保持独立。

## 6. API 路线决定

第一套 API 暂定为：

> **OpenGL 4.1 Core Profile + GLFW + macOS OpenGL.framework**

理由：首次成像链路短，便于在学习 C++、CMake 和调试器的同时理解 Buffer、Shader、Framebuffer 与 draw call；本机已有 GLFW 和 OpenGL.framework，无需立刻安装依赖。

边界：OpenGL 在 macOS 已被弃用，因此它只作为第一套学习后端，不作为未来生产技术。当前目标岗位偏客户端跨平台渲染与 AI 创作工具：完成桌面渲染器后先进入 Android/OpenGL ES，再以 Vulkan 作为主要现代显式 API；Metal 放入第 25～36 个月，用于验证 Apple 平台和跨后端设计。目标岗位发生变化时可在季度复盘中调整。

## 7. 当前仓库状态

仓库根目录：

```text
MiniStudio/
```

Git 状态：

- 已执行 `git init`。
- 稳定分支为 `main`；第 9 课已完成、推送并合并。第 10 课已在 `codex/lesson-10-shader-program` 通过技术与口头验收，当前改动尚未提交、推送或合并。
- 已创建包含最小 CMake 工程、学习文档和 AI 约束的初始基线提交。
- 已配置 Git 远端 `origin`：`git@github.com:Cooper-Xchi/MiniStudio.git`。
- 已按 GitHub 官方指纹核验并信任 `github.com` 的 Ed25519 主机密钥。
- 已为本仓库单独生成 SSH 密钥；私钥保存在仓库外，不被 Git 跟踪。
- 本仓库的本地 `core.sshCommand` 已固定使用上述独立密钥，不影响其他仓库使用的密钥。
- 新公钥已添加为 GitHub 仓库的可写 Deploy key；SSH 认证以及远端读写访问已经验证。
- 本地 `main` 已推送并跟踪 `origin/main`。
- `.idea/` 已被正确忽略，目前没有被 Git 跟踪。

当前项目文件：

```text
MiniStudio/
├── .gitignore
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
        └── ShaderProgram.cpp
```

`.gitignore` 当前包含：

```gitignore
build/
cmake-build-*/
.idea/
.DS_Store
```

`README.md` 保存稳定项目概览，`docs/MiniStudio-curriculum-24-36-months.md` 保存完整课程和阶段验收，详细学习状态仍以本文档为准。

## 8. 已完成内容

### 工程部分

- 创建了本地 Git 仓库和最小目录结构。
- 创建了项目级 `AGENTS.md`，约束 AI 的教学方式、修改边界、Git 操作和验收要求。
- 编写了 `CMakeLists.txt`。
- 编写了项目入口 `README.md`，记录目标、构建方式和当前学习检查点。
- 项目使用 C++20，并关闭编译器私有语言扩展。
- Apple Clang/GCC 路径开启 `-Wall -Wextra -Wpedantic`。
- 增加 `MINISTUDIO_ENABLE_SANITIZERS` CMake 选项；开启时为 Clang/GCC 编译和链接 AddressSanitizer、UndefinedBehaviorSanitizer，并保留帧指针。
- 使用 `find_package(glfw3 3.4 REQUIRED)` 查找已安装的 GLFW，并将其导出的 `glfw` 目标以 `PRIVATE` 方式链接到 `ministudio`。
- 使用 `find_package(OpenGL REQUIRED)` 查找系统 OpenGL，并将 `OpenGL::GL` 目标链接到 `ministudio`；macOS 构建定义 `GL_SILENCE_DEPRECATION`，避免系统弃用提示掩盖项目自身警告。
- CMake 显式编译 `Application.cpp`、`GlfwWindow.cpp` 和 `ShaderProgram.cpp`，并以 `src` 作为私有头文件搜索根目录；不会把目录名误当成源文件。
- 已使用终端完成一次实际配置、编译和运行，构建成功且没有警告。

已验证的命令：

```bash
cmake -S . -B build \
  -G "Unix Makefiles" \
  -DCMAKE_BUILD_TYPE=Debug

cmake --build build --parallel
./build/ministudio

cmake -S . -B cmake-build-sanitize \
  -G "Unix Makefiles" \
  -DCMAKE_BUILD_TYPE=Debug \
  -DMINISTUDIO_ENABLE_SANITIZERS=ON

cmake --build cmake-build-sanitize --parallel
./cmake-build-sanitize/ministudio
```

第一课验收时的运行输出：

```text
construct: stack
construct: heap
main is ending
destroy: heap
destroy: stack
```

第二课验收时的运行输出：

```text
construct: heap
0
<heap-address>
<same-heap-address>
main is ending
destroy: heap
```

第三课验收时的运行输出：

```text
construct: heap
observe: heap
consume: heap
destroy: heap
0
main is ending
```

第四课先在 `heap_probe.reset()` 后解引用 `borrowed_probe`，AddressSanitizer 报告 `heap-use-after-free`：对象在 `main.cpp:37` 分配、在 `main.cpp:45` 释放、在 `main.cpp:46` 被非法读取。把借用限定在对象生命周期内后，普通构建和 Sanitizer 构建的验收输出均为：

```text
construct: heap
probe: heap
destroy: heap
```

第五课已实际创建 `800×600`、标题为 `MiniStudio` 的 GLFW 窗口。窗口保持显示，点击关闭后循环退出，程序按 `glfwDestroyWindow`、`glfwTerminate` 的顺序清理并正常结束。普通构建和 Sanitizer 构建均编译通过；运行时输出为：

```text
GLFW initialized!
GLFW Window created!
```

第六课在创建窗口前请求 OpenGL 4.1、Core Profile 和 forward-compatible Context，创建成功后将 Context 绑定到主线程，并查询实际属性。普通构建和 Sanitizer 构建均编译通过；普通构建实际运行得到：

```text
GLFW initialized!
GLFW context version 4.1
GLFW core profile equal profile? 1
GLFW Window created!
```

第七课在事件循环中先处理系统事件，再查询 `Esc` 的按键状态；按下后仅设置窗口关闭标志，让循环自然退出并复用统一清理路径。实际运行按 `Esc` 后窗口关闭，Shell 退出码为 `0`。macOS 同时输出过一条与键盘处理有关的 TSM 系统诊断信息，但不影响程序结果。

第八课分别查询窗口逻辑尺寸和 framebuffer 物理像素尺寸，用 framebuffer 尺寸设置初始 `glViewport`，并注册 framebuffer size callback 在尺寸变化时同步更新 viewport。当前显示环境实测初始尺寸均为 `800×600`；拖动窗口时回调连续输出新的 framebuffer 尺寸，按 `Esc` 后退出码为 `0`。普通构建和 Sanitizer 构建均无警告。

第九课将单文件程序拆分为 `main`、`Application` 和 `GlfwWindow`：`main` 只创建并运行应用；`Application` 按值拥有窗口并编排初始化、事件和退出流程；`GlfwWindow` 独占 `GLFWwindow*`，封装 GLFW/OpenGL 调用并在析构时统一销毁窗口和终止 GLFW。窗口缩放、`Esc` 退出和退出码 `0` 已实际验证，普通与 Sanitizer 构建均无警告。

第十课新增独立 `ShaderProgram`：分别编译顶点与片元 Shader、读取编译日志、链接 Program，并使用返回值逐层传播失败。`Application` 先拥有 `GlfwWindow`、后拥有 `ShaderProgram`，因此析构时先删除 Program，再销毁窗口与 OpenGL Context。普通与 Sanitizer 构建均无警告；正确源码链接成功。验收时临时删除片元 Shader 分号，程序输出完整 `0:6` 语法错误、跳过链接并返回 `1`，恢复源码后重新构建成功。

### 已讲解的概念

- CLion、CMake、Ninja、Clang、LLDB 各自的职责。
- 本地 Git 仓库与 Git 远端的区别。
- `.gitignore` 的作用。
- 构造函数、析构函数和成员初始化列表。
- `explicit` 如何阻止意外的隐式类型转换。
- `std::move` 表达“允许移动资源”，它本身不执行搬运。
- `std::unique_ptr` 的唯一所有权与自动析构。
- `std::make_unique<T>()` 的基本过程。
- 简化版智能指针内部原理：保存裸指针、析构时 `delete`、禁止复制、允许移动。
- `operator` 是运算符重载关键字，例如 `operator=` 和 `operator->`。
- 已通过 CLion 断点观察两个对象构造和析构时的 `this` 与 `name_`。
- 已确认 `heap_probe` 是局部的独占智能指针对象，拥有堆上的 `LifeTimeProbe`；`heap_probe.get()` 返回不转移所有权的裸指针。
- 已理解同一作用域的局部变量按声明顺序的逆序析构，以及 `unique_ptr` 禁止复制、允许移动所有权的原因。
- 已验证移动 `unique_ptr` 只转移所有权和内部地址值，不重新构造或搬移堆对象。
- 已验证移动后的源 `unique_ptr` 为空，新所有者保留原地址，最终只删除对象一次。
- 已区分裸指针保存的对象地址与裸指针变量自身的地址，并理解裸指针不拥有对象、对象销毁后会悬空。
- 已区分 `const T&` 的非拥有借用、`T*` 的可空非拥有借用，以及 `unique_ptr<T>` 按值传参表达的所有权转移。
- 已理解 RAII 是把资源释放责任绑定到所有者对象的生命周期；`unique_ptr` 函数参数在提前 `return` 时仍会析构并自动删除其拥有的对象。
- 已理解 `unique_ptr::reset()` 删除被管理对象并让所有者进入空状态，但之前取得的裸指针仍保存旧地址并成为悬空指针。
- 已能从 AddressSanitizer 报告中对应非法访问、释放和分配位置，并理解 Sanitizer 依靠运行时插桩发现普通编译通常无法证明的动态生命周期错误。
- 已理解独立 Sanitizer 构建目录能够隔离编译选项、CMake 缓存和构建产物，也避免把诊断开销默认带入普通构建。
- 已区分 GLFW 头文件、GLFW 动态库和 CMake 导入目标；`find_package` 加载包配置，`target_link_libraries` 建立链接依赖并应用目标携带的使用要求。
- 已理解 `glfwInit`/`glfwTerminate` 管理 GLFW 的全局生命周期，`GLFWwindow*` 是不透明窗口句柄，调用方负责用 `glfwDestroyWindow` 销毁。
- 已理解关闭按钮只设置窗口关闭标志，事件循环读取该标志后退出，窗口随后才由代码显式销毁。
- 已理解 Context hints 必须在 `glfwCreateWindow` 前设置，因为它们约束随后创建的窗口及其 OpenGL Context，而不能修改已经创建的 Context。
- 已理解 `glfwMakeContextCurrent` 把窗口关联的 Context 绑定到调用线程；当前程序中的调用线程是主线程。
- 已理解版本 hints 表达最低兼容要求：无法满足 OpenGL 4.1 时窗口创建失败并返回空指针，而不是静默降级；查询 Context 属性用于确认实际版本和 Profile。
- 已理解 `glfwPollEvents` 处理当前事件队列并更新输入状态，轮询式输入应在事件处理后查询本轮最新按键状态。
- 已理解 `glfwGetKey` 返回按键状态；当前练习明确比较 `GLFW_PRESS`，不把任意非零值笼统当作按下。
- 已理解 `glfwSetWindowShouldClose` 只修改关闭标志，不立即销毁窗口；`Esc` 和关闭按钮最终都从循环退出并经过同一套销毁与终止代码。
- 已区分 window size 的逻辑屏幕坐标与 framebuffer size 的物理像素；两者在内容缩放为 `1×` 时通常相同，在高 DPI 环境中可能不同。
- 已理解 `glViewport` 规定标准化设备坐标映射到 framebuffer 的像素区域，因此必须使用实际 framebuffer 尺寸，而不是假定它等于窗口逻辑尺寸。
- 已理解 framebuffer 尺寸变化会触发注册的 callback；当前程序在主线程调用 `glfwPollEvents`，因此回调也在主线程的事件处理中执行。
- 已理解构造 `Application` 时会先构造其按值成员 `window_`，随后才进入 `Application` 构造函数体；销毁时先执行 `Application` 析构函数体，再销毁 `window_`。
- 已区分对象成员与指针成员：`GlfwWindow window_` 表示直接拥有对象，`GlfwWindow* window_` 只保存地址且不会构造所指对象。
- 已理解所有权与解耦是两个问题：按值成员表达 `Application` 拥有窗口，不公开 `GLFWwindow*` 则避免应用层直接依赖 GLFW 实现。
- 已理解初始化成功但窗口创建失败时，`Application::Run` 返回后成员仍会自动析构，并依据初始化状态调用 `glfwTerminate`，从而保持统一清理路径。
- 已理解 OpenGL Context 不是线程，而是绑定到调用线程的状态与资源环境；OpenGL API 操作调用线程的 Current Context。
- 已理解 Shader 的源码提交、编译与状态查询，以及 Program 的附加、链接、使用和释放顺序；单阶段编译错误与跨阶段链接错误需要使用不同日志 API。
- 已理解临时 Shader 在链接结束后可以删除，长期资源由 `ShaderProgram` 独占；编译或链接任一步失败时，当前函数必须释放尚未移交的资源并传播失败。
- 已理解成员按声明顺序构造、逆序析构；`window_` 先声明、`shader_program_` 后声明，使 Program 在 OpenGL Context 之前销毁。

当前仍处于“刚接触并建立直觉”的阶段，不应假定已经熟练掌握智能指针、移动语义或运算符重载。

## 9. 当前代码结构与 `main.cpp` 的真实状态

```cpp
#include "app/Application.h"

int main() {
    Application app;
    return app.Run();
}
```

当前依赖为 `main → Application → GlfwWindow/ShaderProgram → GLFW/OpenGL`。`Application` 按值拥有窗口和 Shader Program，通过公开的意图型接口使用它们，不取得原始 OpenGL 资源 ID；具体 GLFW/OpenGL 头文件和调用分别留在实现文件中。代码已经通过普通与 Sanitizer 构建，没有编译警告；窗口流程、Shader 编译、Program 链接、完整错误日志和自动清理均已实际验证。

## 10. 当前阶段与下一步

当前处于：**第 3 周第 10 课已完成验收，等待提交、推送并合并。**

本机 Homebrew GLFW 3.4 已接入，头文件为 `/opt/homebrew/opt/glfw/include/GLFW/glfw3.h`，CMake 包配置导出的目标名为 `glfw`。系统 OpenGL 通过 `OpenGL::GL` 链接。当前代码已经拆分应用、窗口和 Shader Program，能编译并链接 GLSL、输出错误日志，并支持关闭按钮和 `Esc` 退出；尚未进入顶点 Buffer 或三角形。

仓库远端和 AI 约束准备已经完成，初始基线和第 1 周的四课均已合并到 `main`。

第四课已在 `codex/lesson-04-sanitizer-debugging` 完成并合并：Sanitizer 配置有效，受控的悬空指针访问已被复现、读懂并修复，普通构建和 Sanitizer 构建均运行正常。第 1 周已经完成生命周期、所有权移动、RAII 边界和 Sanitizer 故障定位四项核心练习。

第 5 课已完成 CMake 包查找与链接、GLFW 初始化、窗口创建、最小事件处理和有序清理，并已提交、推送和合并。学习者能够解释关闭标志、窗口销毁责任，以及 `find_package` 与 `target_link_libraries` 的区别。

第 6 课已完成 Context hints、OpenGL 4.1 Core Profile 创建、主线程 Context 绑定和实际属性查询，并已提交、推送和合并。学习者能够解释 hints 的生效时机、current Context 的线程含义，以及版本不可用时窗口创建失败而不会静默降级。

第 7 课已完成事件轮询、`Esc` 按键状态查询、关闭标志设置和统一清理流程，并已提交、推送和合并。学习者能够解释 `glfwPollEvents` 的位置，以及设置关闭标志并不等于立即销毁窗口。

第 8 课已完成 window size、framebuffer size、viewport 和 resize callback，并已提交、推送和合并。学习者能够解释逻辑尺寸与物理像素尺寸的区别、为什么 viewport 使用 framebuffer 尺寸，以及回调的触发条件和当前执行线程。

第 9 课已完成最小项目骨架：`main` 负责启动，`Application` 按值拥有 `GlfwWindow` 并编排流程，`GlfwWindow` 封装具体平台调用和资源释放。

第 10 课已完成独立 `ShaderProgram` 模块和受控 Shader 语法错误验收。待学习者确认后提交、推送并合并回 `main`；完成合并后再从最新 `main` 创建第 11 课分支，学习顶点数据、VBO、VAO、attribute 与资源所有权。

课程已按目标岗位职责扩展为 24 个月核心路线和第 25～36 个月专家能力进阶，新增 Android/OpenGL ES、Vulkan、移动端 Profiling、图片/动画/视频/3D 素材引擎、AI Tool Calling、Metal 验证和规模化架构演进。当前仅更新规划，不代表这些未来模块已经开始。

## 11. 课程路线入口与前四周计划

完整课程阶段、验收规则、求职时间线和暂缓内容见 `MiniStudio-curriculum-24-36-months.md`。当前仍只执行下一节课程，不批量展开远期任务。

| 周 | 核心目标 | 状态 |
| --- | --- | --- |
| 第 1 周 | CMake/C++20、对象生命周期、RAII、`unique_ptr`、移动语义、LLDB、Sanitizer | 已完成；四课均已验收并合并到 `main` |
| 第 2 周 | 链接 GLFW/OpenGL，创建 4.1 Core Context，事件循环和 Retina viewport | 已完成；四课均已验收并合并到 `main` |
| 第 3 周 | 项目骨架、职责解耦、Shader、VAO/VBO、彩色三角形与错误日志 | 进行中；第 9～10 课已完成，第 10 课等待提交、推送并合并 |
| 第 4 周 | 最小 RAII 封装、Debug/Release、故障定位、README 与生命周期说明 | 未开始 |

## 12. 协作要求

- 每次只安排一个能在 20～90 分钟内完成的核心任务。
- 每个新的核心学习任务开始前，从稳定的最新 `main` 创建独立课程分支；AI 创建的分支使用 `codex/lesson-<两位序号>-<英文主题>` 命名。
- 同一课程的提问、纠错、检查和验收继续使用原课程分支，不重复建分支；提交、推送和合并仍需学习者明确确认。
- 先解释目标、概念和验收标准，再让学习者动手。
- 核心练习不直接给完整答案；优先提供接口、伪代码、TODO 或局部示例。
- 完成练习后，应实际读取代码并编译、运行或检查结果。
- 遇到错误先引导定位原因，不立即整体重写。
- 新增依赖前说明用途、替代方案、平台限制和维护成本。
- 区分“面试需要知道”“项目必须实现”和“以后再学”，控制范围。
- 对初学者解释陌生语法时，说明对象由谁拥有、何时销毁、在哪个线程运行以及潜在风险。
- 在第一套 OpenGL 渲染器完成前，不并行学习 Metal、Vulkan、DirectX。
- 每次新课先做架构只读检查；`main.cpp` 只保留组装和顶层流程，有状态或拥有资源的职责逐步拆成内聚对象，依赖保持单向。
- 面向对象用于职责、状态和生命周期封装；无状态逻辑可使用普通函数，禁止深继承、Singleton、万能 Manager 和无真实需求的接口层。

## 13. 新任务建议启动语

将本文作为附件，然后发送：

> 请阅读这份 MiniStudio 学习交接文档。先只读检查当前 MiniStudio 仓库的状态，核对交接信息是否仍然准确，然后从第 10 节的“当前阶段与下一步”继续。每次只给我一个 20～90 分钟的核心任务；我完成后请实际检查并验收，不要提前跳到 OpenGL，也不要直接给出核心练习的完整答案。

## 14. 文档关系与使用方式

- `AGENTS.md` 是项目级 AI 执行约束，规定教学方式、修改权限、课程分支、Git 安全边界和验证要求。
- `README.md` 是项目入口，保存相对稳定的项目目标、构建命令和路线概览。
- `docs/MiniStudio-curriculum-24-36-months.md` 是当前有效的完整课程路线，保存 24 个月核心路线、第 25～36 个月进阶路线、阶段验收和求职时间线。
- 本文档是当前学习状态的主要记录，后续应在阶段、代码状态或下一步发生变化时同步更新。
- `graphics-rendering-learning-brief.md` 是最初的长期学习需求和协作原则来源；其中 12 个月要求已被学习者当前提出的 24～36 个月路线取代，“第一次阅读后完成的任务”和“启动语”也只属于历史启动说明。
- 如果文档之间出现冲突，依次以学习者当前明确请求、实际仓库状态、本文档的较新记录为准。
