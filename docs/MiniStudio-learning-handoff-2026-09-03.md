# MiniStudio C++ 图形渲染学习交接文档

> 更新时间：2026-09-10
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

## 5. 已检查的设备环境

设备必须明确区分：学习者的公司电脑是 macOS，家里电脑是 Windows。下表记录的是公司 Mac 环境；Windows 使用独立的 Visual Studio、CLion 和 vcpkg manifest 构建路径。两台设备的验证证据不能互相替代。

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

家用 Windows 当前使用 Visual Studio 2026 Community 的 MSVC 工具链、CLion 自带 CMake/Ninja，以及 `D:/tools/vcpkg`。Windows CMake profile 通过仓库根目录的 `vcpkg.json` 使用 manifest 模式，依赖安装在各构建目录的 `vcpkg_installed` 中；不要把旧的全局 `D:/tools/vcpkg/installed` 当成当前项目依赖路径。

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
- 稳定分支为 `main`；第 17 课已在 `codex/lesson-17-indexed-drawing` 完成、推送并合并。课程规划分支与前十七课分支均继续保留。
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
        ├── ShaderProgram.cpp
        ├── VertexArray.h
        ├── VertexArray.cpp
        ├── RenderCommand.h
        ├── RenderCommand.cpp
        ├── Renderer.h
        └── Renderer.cpp
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
- CMake 显式编译 `Application.cpp`、`GlfwWindow.cpp`、`ShaderProgram.cpp`、`VertexArray.cpp`、`RenderCommand.cpp` 和 `Renderer.cpp`，并以 `src` 作为私有头文件搜索根目录；不会把目录名误当成源文件。
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

第十一课新增独立 `VertexArray`：上传三组交错排列的位置/颜色数据到 VBO，并用 VAO 记录 `location 0/1`、步长和偏移。`Application` 最后拥有 `VertexArray`，使 VAO/VBO 在 Shader Program 和 Context 之前释放。普通与 Sanitizer 构建均无警告；正确的 18 个 `float` 得到 3 个顶点并稳定运行。验收时临时传入 17 个 `float`，初始化在创建 OpenGL 资源前安全返回失败，程序返回 `1`，恢复后重新构建成功。

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
- 已理解 `glBufferData` 按字节分配并复制 CPU 顶点数据，调用结束后 VBO 不依赖原 CPU 数组的生命周期。
- 已理解 VAO 不保存实际顶点数据，而是记录 attribute 如何从绑定的 VBO 取数；`glVertexAttribPointer` 调用时必须已有当前 VAO 和 VBO。
- 已理解交错顶点布局中 `6 * sizeof(float)` 是相邻顶点同一 attribute 的步长，`3 * sizeof(float)` 是颜色 attribute 的起始字节偏移。
- 已理解 `VertexArray` 独占 VAO/VBO，拒绝复制与移动，并在部分初始化失败时回滚已经创建的资源。
- 已理解 EBO 保存顶点索引，VAO 记录 EBO 绑定关系，`glDrawElements` 的数量参数表示索引数量，最后一个 `nullptr` 表示从当前 EBO 的第 0 字节开始读取。
- 已理解 `glBufferData` 会把局部 CPU 数组内容复制到驱动管理的 Buffer 存储；函数返回后局部顶点与索引数组按自动存储期销毁，不是由垃圾回收器回收。
- 已理解 `glTexImage2D` 会复制局部 RGBA 像素数组；sampler uniform 保存的是纹理单元编号，不是纹理对象句柄，`glActiveTexture` 选择纹理单元，`glBindTexture` 再把纹理对象绑定到该单元。
- 已理解 `stbi_load` 返回的内存必须由 `stbi_image_free` 释放；当前用带自定义 deleter 的 `std::unique_ptr` 表达临时解码内存的所有权，再把准确的 `width * height * 4` 字节复制到 `ImageData::rgba_pixels`。
- 已理解文件通常按顶行优先返回像素，而当前 OpenGL UV 把 `T=0` 放在四边形底部；测试图显示为上方蓝黄、下方红绿，证明上传链路没有自动调整行方向。
- 已理解连续 RGBA 数组中一行占 `width * 4` 字节，因此第 `row` 行从 `row * row_bytes` 开始；每次同时交换一对上下行，所以只遍历 `row_count / 2`，否则会把已经翻转的数据再次交换回去。
- 已理解 Mipmap 由第 0 级原纹理像素逐级生成，只有缩小过滤会选择 Mipmap；`GL_TEXTURE_MAG_FILTER` 只能使用 `GL_NEAREST` 或 `GL_LINEAR`，三线性过滤通过在相邻两个 Mipmap 层级内线性采样并在层级间插值来降低缩小噪点。
- 已理解 OpenGL 错误属于当前 Context 的粘滞状态，初始化错误若留到首帧会被 `ClearErrors()` 丢弃；资源模块应在自己的初始化边界完成检查、归因和失败回滚，`texture_id_ = 0` 同时避免析构重复删除并允许再次初始化。
- 已理解 GLM/GLSL 使用列向量，GLM 变换函数把新矩阵乘在已有矩阵右侧；`T × S × position` 对顶点先缩放再平移，而 `S × T` 会连平移量一起缩放。三维位置通过 `vec4(position, 1.0)` 进入齐次坐标，`glUniformMatrix4fv` 把局部矩阵值复制到当前 Program。
- 已理解 `steady_clock` 提供不受系统时间调整影响的单调时间；角度由每秒速度乘累计秒数得到，因此相同真实时间对应相同旋转角度，而每帧固定增加角度会让动画速度随帧率变化。
- 已理解 view 矩阵把世界空间转换到以相机为原点的相机空间；纯平移相机位于 `x = 0.25` 时，view 对世界应用 `-0.25`，所以同样位于世界 `x = 0.25` 的物体在相机空间中位于 `0`。

当前仍处于“刚接触并建立直觉”的阶段，不应假定已经熟练掌握智能指针、移动语义或运算符重载。

## 9. 当前代码结构与 `main.cpp` 的真实状态

```cpp
#include "app/Application.h"

int main() {
    Application app;
    return app.Run();
}
```

当前依赖为 `main → Application → GlfwWindow/Renderer`，Renderer 再单向依赖无状态 `ImageLoader` 和 `ShaderProgram/VertexArray/Texture2D/RenderCommand`；图片模块只依赖标准库与 `stb_image`，平台与渲染实现最终依赖 GLFW/OpenGL。`Application` 按值拥有窗口和 Renderer，并在主循环中用 `steady_clock` 计算累计秒数；每帧处理事件后从 GlfwWindow 查询实际 framebuffer 宽高，再把时间和两个普通整数传给 Renderer。Renderer 按值拥有 Shader Program、顶点输入资源和二维纹理，每帧根据时间上传 model，并根据 framebuffer 宽高比上传 perspective projection；静态 view 仍在初始化时上传一次。`SetMat4()` 临时借用 CPU 矩阵，OpenGL 把数值复制到 Program uniform 状态。临时 `ImageData` 在 Renderer 初始化期间拥有 CPU RGBA 字节，`Texture2D` 在上传复制后独占 OpenGL texture handle。成员声明顺序保证 Renderer 及其 GPU 资源先析构、窗口和 Context 后析构。`Application` 只编排窗口初始化、事件、时间、尺寸查询、绘制和呈现，具体渲染数据与命令留在 Renderer 内。macOS 继续使用 `OpenGL/gl3.h` 与系统 framework，Windows 通过 GLAD 加载 OpenGL 4.1 函数；平台条件集中在共享头文件与 CMake 中。第 31 课已在 macOS Sanitizer 配置下完成编译和 resize 运行回归，编译器与 Shader 日志均无警告，运行时没有 OpenGL、ASan 或 UBSan 错误。

## 10. 当前阶段与下一步

当前处于：**第 1～51 课均已完成、验收并合并到本地 main；第 52 课已由助手完成 v0.2 工程收尾与 Windows 验证，待本轮提交合并，个人季度复盘按明确要求跳过。**

逐课备课已按学习者 2026-09-09 的要求写入 [docs/lessons/README.md](lessons/README.md) 及 A～L 阶段文档。第 37～51 课已合并到本地 main，课程分支继续保留；第 52 课工程验收完成，待与第 51 课一起推送。下一项为 B01（第 53 课）CPU MeshData 与顶点布局，尚未开课、未创建分支；不直接跳到 glTF 加载。每次仍只执行一个核心任务，远期教案不代表已经实施或验收。

第 37 课启动记录（2026-09-09）：只读检查实际目录、Application、GlfwWindow、Renderer、RenderCommand、ShaderProgram 与 VertexArray，确认依赖和资源所有权保持单向。第 36 课成果 `635c891` 已在 `main` 历史中；开课前工作区干净，本地 `main`、`origin/main` 与实时查询的远端 `main` 均为 `69c98d1`。公司 macOS arm64 使用 Apple Clang 21，在独立的 `build/lesson-37-macos-debug` 目录执行 Debug 配置与编译，成功且无编译器警告；从该构建目录启动完整程序，成功创建 OpenGL 4.1 Core Context，Shader 链接成功、链接日志为空，运行期间未报告 OpenGL 错误，最终退出码为 0。退出期间有一条 macOS TSM 键盘系统诊断；本轮未通过自动化核实画面、resize 或具体 Esc 按键，不把启动运行检查写成完整交互验收，也未执行 Sanitizer 或 Windows 回归。随后从稳定 `main` 创建 `codex/lesson-37-triangle-winding`；仅更新本启动记录，核心业务代码由学习者实现。

第 37 课核心任务（35～60 分钟）：先画出当前索引 `0,1,2` 与 `2,3,0` 的方向，在 RenderCommand 中增加项目侧绕序枚举及最小正面约定接口，由实现映射到 `glFrontFace`；Renderer 在 draw 前设置约定，片元 Shader 用 `gl_FrontFacing` 区分正反面颜色。剔除保持关闭，依次对比原索引／逆时针正面、原索引／顺时针正面、仅反转一个三角形／逆时针正面，最后恢复原索引与逆时针正面。保留现有深度测试、写入、清除顺序和矩阵链路；两个矩形共用一份 EBO，因此索引变化会同时作用于两次绘制。为避免 sampler 被优化掉后 `SetInt("texture_sampler", 0)` 返回失败，调试输出保留纹理采样的非零贡献（例如原纹理占 15%、正反面标记色占 85%）。不增加剔除接口、法线、光照、资源类、依赖或持久 C++ 状态；状态由 Current Context 持有，调用仍在主线程。下一步由学习者完成接口、颜色实验与观察记录，再实际读取改动、构建运行并进行原理验收；尚未提交、推送或合并。

第 37 课首次代码检查（2026-09-09）：学习者报告三组验证符合预期。实际读取改动确认 `FrontFaceWinding` 的两个分支分别正确映射到 `GL_CW`／`GL_CCW`，Renderer 在 draw 前设置规则，片元 Shader 读取 `gl_FrontFacing` 并以正面红、背面绿与纹理各占 50% 混合；该配色与混合比例有效。macOS Debug 增量构建成功且无警告，完整程序启动成功，Shader 链接成功、日志为空，启动运行检查未报告 OpenGL 错误；本轮画面观察依据学习者报告，未独立读回 GPU 颜色。当前源码仍保留实验状态：索引为 `0,1,2` 与 `0,3,2`，正面约定为 `Clockwise`，两份 model 的旋转速度均为零。下一步由学习者恢复第二组索引为 `2,3,0`、正面约定为 `CounterClockwise`、两份旋转速度为 90／45 度每秒，并删除 RenderCommand.cpp 中未使用的 `ShaderProgram.h` 引用；保留标记色供最终检查。同时需解释仅交换同一三角形的索引顺序为何不必改变其几何位置和深度。当前不判最终验收通过，未提交、推送或合并。

第 37 课最终验收（2026-09-09）：实际再次读取改动，确认学习者已恢复索引 `0,1,2` 与 `2,3,0`、`CounterClockwise` 正面约定、Model1／Model2 每秒 90／45 度的旋转，并删除 RenderCommand.cpp 中未使用的 ShaderProgram 头文件引用。保留正面红、背面绿的标记色，与纹理各占 50%；剔除保持关闭，原有深度清除、测试、写入和 `LESS` 顺序保持不变。学习者已报告三组实验符合预期，并解释交换索引后坐标不变，所以深度不变；教学中进一步澄清改变的是同一三角形的顶点绕序，而非物体之间的 draw 提交顺序。最终源码在公司 macOS arm64 的 `build/lesson-37-macos-debug` 重新编译成功且无警告，完整程序创建 OpenGL 4.1 Core Context、Shader 链接成功、链接日志为空，运行未报告 OpenGL 错误，最终退出码为 0；有一条 macOS TSM 键盘系统诊断。画面对照依据学习者报告，本轮未独立读回 GPU 颜色，也未重新验证 resize、具体 Esc 按键、Sanitizer 或 Windows。核心实现、实验复原和原理验收均通过，助手仅更新学习记录，未代写核心业务代码。下一步等待学习者明确确认提交、推送课程分支并合并到 `main`；分支继续保留，第 38 课尚未启动。

第 37 课合并记录（2026-09-09）：学习者明确要求“提交推送合并，然后开始下一课”。已提交课程成果 `0fc5f91`，推送 `codex/lesson-37-triangle-winding`，以合并提交 `45ec882` 纳入 `main` 并推送远端；课程分支继续保留。上方验收记录中的“等待确认”是当时状态，当前 Git 操作已完成。随后同步本完成记录，再从干净、稳定的最新 `main` 开始第 38 课。

第 38 课启动记录（2026-09-09）：第 37 课完成记录提交 `530a9d3` 已同步至远端 `main` 与课程分支，实时远端查询确认两者一致。开课前工作区干净，目标课程分支不存在；只读复核 Application、Renderer、RenderCommand、目录及已有资源所有权，确认依赖继续保持单向，资源由原有 RAII 对象拥有。公司 macOS arm64 使用 Apple Clang 21，在全新 `build/lesson-38-macos-debug` 完成 Debug 配置和编译，无编译器警告；完整基线程序成功创建 OpenGL 4.1 Core Context，Shader 链接成功、日志为空，运行未报告 OpenGL 错误，最终退出码 0。有一条 macOS TSM 键盘系统诊断，本轮不计为画面、resize 或具体 Esc 按键的独立验收，也未执行 Windows 或 Sanitizer 回归。随后从稳定 `main` 创建 `codex/lesson-38-face-culling`，仅更新启动记录与进度摘要，未实现本课业务代码。

第 38 课核心任务（35～60 分钟）：在无状态 RenderCommand 中新增最小剔除开关与剔除面选择接口，例如 `SetFaceCullingEnabled(bool)`、项目枚举 `CullFace { Back, Front }` 和 `SetCullFace(CullFace)`，实现分别使用 `glEnable/glDisable(GL_CULL_FACE)` 与 `glCullFace`。Renderer 在 draw 前编排设置，保持逆时针为正面和原始一致绕序；把 Model1／Model2 的旋转轴从 Z 改为 Y，保持 90／45 度每秒，用上一课红色正面、绿色背面与纹理混合的输出观察翻面。依次对比关闭剔除、开启且剔除背面、开启且剔除正面，先预测再记录每种状态下哪些面可见；侧面对相机时投影变窄，不以固定恰好 90 度作为透视场景翻面的验收条件。完成后保存逆时针正面、开启背面剔除、Y 轴旋转的默认配置，并保留标记色供验收。保持原有深度、矩阵、资源和线程边界，不增加法线、光照、资源类、依赖或状态管理器。剔除设置属于 Current Context，由主线程调用；接口不拥有新资源，也不保存重复的 C++ 状态。验收需有三组观察、构建运行与 Shader/OpenGL 无错误证据，并能解释 `glCullFace` 指定的是丢弃哪一面，以及树叶等平面素材为什么可能需要双面显示。本课尚未提交、推送或合并。

第 38 课首次代码检查（2026-09-09）：学习者报告三组实验符合预期。实际读取改动确认剔除开关正确使用 `GL_CULL_FACE`，项目枚举 Back／Front 正确映射到 `GL_BACK`／`GL_FRONT`，两个矩形均已改为绕 Y 轴旋转，速度仍为每秒 90／45 度。新增 `SetGlobalDepth` 与 `SetGlobalCullFace` 仅组合现有无状态调用，不拥有资源或保存重复状态，当前可以保留；名称中的 Global 不代表跨窗口或跨 Context 生效，调用只修改主线程的 Current Context。深度写入在清屏之前开启，状态配置仍位于 draw 之前，职责与依赖方向保持正确。macOS Debug 编译成功且无警告，完整程序创建 OpenGL 4.1 Core Context，Shader 链接成功、链接日志为空，启动运行检查未报告 OpenGL 错误。画面对照依据学习者报告，未独立读回 GPU 颜色，未执行 Windows 或 Sanitizer 回归。当前 Renderer 仍设置 `CullFace::Front`，保留了第三组实验；下一步由学习者将其恢复为 `CullFace::Back`，保持启用剔除、逆时针正面和 Y 轴旋转，并解释树叶等平面素材为什么可能需要关闭背面剔除。最终配置与原理验收完成前，不判本课最终通过；尚未提交、推送或合并。

第 38 课最终验收（2026-09-09）：实际再次读取源码，确认最终配置为启用面剔除、`CullFace::Back`、`FrontFaceWinding::CounterClockwise`，两个矩形保持一致索引并分别以每秒 90／45 度绕 Y 轴旋转。学习者完成关闭剔除、剔除背面、剔除正面三组实验并报告结果符合预期，能够解释树叶通常以几乎无厚度的平面表示、观察角度容易看到背面，因此启用背面剔除会让背面整片消失；这类材质可按需求关闭剔除或使用真正的双面几何。`SetFaceCullingEnabled` 与 `SetCullFace` 映射正确；新增的两个组合函数不拥有资源或缓存状态，只对主线程的 Current Context 发出已有状态调用。最终源码在公司 macOS arm64 的 `build/lesson-38-macos-debug` 重新编译成功且无警告，完整程序创建 OpenGL 4.1 Core Context、Shader 链接成功、链接日志为空，运行未报告 OpenGL 错误，最终退出码为 0；有一条 macOS TSM 键盘系统诊断。画面对照依据学习者报告，本轮未独立读回 GPU 颜色，也未重新验证 resize、具体 Esc 按键、Sanitizer 或 Windows。核心实现、三组实验、最终配置和原理验收均通过，助手仅更新学习记录，未代写核心业务代码。下一步等待学习者明确确认提交、推送课程分支并合并到 `main`；第 39 课尚未启动。

第 38 课合并记录（2026-09-09）：学习者明确要求提交合并并开始下一课。已提交课程成果 `75bd3fa`，推送 `codex/lesson-38-face-culling`，以合并提交 `6daf405` 纳入 `main` 并推送远端；课程分支继续保留。上方验收记录中的“等待确认”是历史状态，当前 Git 操作已完成。随后同步本完成记录，再从干净、稳定的最新 `main` 开始第 39 课。

第 39 课启动记录（2026-09-09）：第 38 课完成记录 `f7e1397` 已同步至远端 `main` 与课程分支，实时远端查询确认两者一致。开课前工作区干净，目标分支不存在；只读检查 Renderer、VertexArray、RenderCommand 与现有目录，确认 `VertexArray` 仍可独占一套 VAO／VBO／EBO，固定 CPU 顶点与索引只在 Renderer 初始化期间存在并在上传后销毁，所有 OpenGL 调用继续在主线程的 Current Context 上执行。本课只有一个固定演示立方体，没有复用需求或第二种 Mesh 实现，因此暂不增加通用 Mesh、几何管理器或新资源所有者；使用现有 `position(3) + color(3) + uv(2)` 的 8-float 布局，以 24 个完整属性顶点和 36 个索引表达六个独立 UV 面。公司 macOS arm64 使用 Apple Clang 21，在 `build/lesson-38-macos-debug` 对合并后的稳定 `main` 重新编译成功且无警告；完整程序成功创建 OpenGL 4.1 Core Context，Shader 链接成功、日志为空，运行未报告 OpenGL 错误并以退出码 0 结束。有一条 macOS TSM 键盘系统诊断；本轮未独立验证画面、resize、具体 Esc 按键、Sanitizer 或 Windows。随后从稳定 `main` 创建 `codex/lesson-39-indexed-cube`，仅更新启动记录和进度摘要，未实现本课业务代码。

第 39 课核心任务（60～90 分钟）：把 Renderer 初始化中的固定矩形替换为一个边长 1、中心在局部原点的索引立方体。六个面各使用四条完整顶点记录，共 24 个顶点；每面用六个索引组成两个三角形，共 36 个索引。顶点继续保持 `position(3), color(3), uv(2)`，每个面按从立方体外部观察时逆时针排列四个角，并复用面内索引模式 `base+0, base+1, base+2, base+2, base+3, base+0`；六面的 base 依次为 0、4、8、12、16、20。每面分别使用完整的 `(0,0), (1,0), (1,1), (0,1)` UV，允许几何位置相同的角点因所属面和 UV 不同而成为不同顶点记录。逐面填写并先核对正面一个面的绕序，再扩展其余五面；所有索引必须小于 24。逐帧只绘制一个立方体，model 把它放在相机前方并绕 X、Y 两轴旋转；保持深度测试、深度写入、LESS、逆时针正面与背面剔除，删除第二个矩形的 model 和第二次 draw。保留现有纹理和正面标记色，预计背面被剔除后只看到正面标记色。不要修改 VertexArray 布局、创建通用 Mesh、加入法线／光照、模型导入或新依赖。验收需看到六个面随旋转正确出现且遮挡稳定，纹理方向可逐面解释，构建、Shader/OpenGL 错误和退出检查通过；并回答为什么八个共享角点不能同时表达六个面各自独立的完整 UV，以及 EBO 索引为何引用一整条属性记录。本课尚未提交、推送或合并。

第 39 课首次代码检查（2026-09-09）：学习者报告立方体运行符合预期，并正确解释八个空间角点被多个面共享，而 EBO 索引引用完整顶点记录。实际读取确认固定几何包含 24 条 `position + color + uv` 记录、36 个索引，最大索引为 23；逐面用前三个位置计算方向后，六个面分别朝 `+Z、-Z、-X、+X、+Y、-Y`，均符合从外部观察为逆时针的约定。每面使用一套完整 `0～1` UV，VertexArray 布局与 GPU 所有权未改变；逐帧已只发出一次有效 draw，深度与背面剔除配置正确。macOS Debug 增量编译成功且无警告，完整程序创建 OpenGL 4.1 Core Context、Shader 链接成功、日志为空，运行未报告 OpenGL 错误并以退出码 0 结束；画面依据学习者观察，本轮未独立读回 GPU 结果，也未执行 Windows 或 Sanitizer 回归。当前 `Model1` 只调用一次 `glm::rotate` 且轴为 `(0,1,1)`，含义是绕 YZ 平面中的单根斜轴旋转，不是分别绕 X、Y 两轴；旧 `Model2` 和第二次 draw 仍以死函数与注释块保留。下一步由学习者把单次斜轴旋转改为两次独立的 X、Y 旋转，删除 `Model2` 与注释掉的第二次 draw，并把 `Model1` 改为符合单立方体职责的名称。完成后重新构建运行，再做最终验收；当前尚未提交、推送或合并。

第 39 课第二次代码检查（2026-09-09）：学习者已删除旧 `Model2` 函数和注释掉的第二次 draw，当前只有一次有效立方体绘制；代码在 macOS Debug 下重新编译成功且无警告，完整程序创建 OpenGL 4.1 Core Context、Shader 链接成功、日志为空，运行未报告 OpenGL 错误并以退出码 0 结束。仍有两项收尾未完成：第二次旋转使用 `(0,0,1)`，形成 Y、Z 双轴而非本课要求的 X、Y 双轴；单立方体矩阵函数仍名为 `Model1`，未改成能表达用途的名称。下一步将第二个旋转轴改为 `(1,0,0)`，并把函数及调用处统一重命名为 `CubeModel` 或同等清晰名称；完成后再做最终验收。

第 39 课最终验收（2026-09-09）：学习者完成最后收尾，将矩阵函数及调用统一命名为 `CubeModel`，以两次独立 `glm::rotate` 分别绕 Y、X 轴旋转，并删除旧 `Model2` 与第二次 draw。期间曾只重命名函数定义、遗漏调用处，Apple Clang 在 `Renderer.cpp:157` 报告 `use of undeclared identifier 'Model1'`；学习者根据编译错误同步修正调用，理解了定义名称与调用必须一致。最终固定几何包含 24 条完整属性记录和 36 个索引，六面外向绕序正确、最大索引 23，每面独立使用完整 UV；`VertexArray` 仍独占一套 VAO／VBO／EBO，CPU 局部数组上传后即可销毁。逐帧只绘制一个立方体，保持深度测试、写入、LESS、逆时针正面和背面剔除。学习者报告六面旋转、纹理和遮挡符合预期，并能解释同一空间角点因不同面的 UV 需要重复为多条顶点记录，EBO 索引引用完整的 `position + color + uv` 记录。最终源码在公司 macOS arm64 的独立 `build/lesson-39-macos-debug` 从全新 CMake 配置完成编译且无警告，完整程序创建 OpenGL 4.1 Core Context、Shader 链接成功、日志为空，运行未报告 OpenGL 错误并以退出码 0 结束；有一条 macOS TSM 键盘系统诊断。画面依据学习者观察，本轮未独立读回 GPU 颜色，也未重新验证 resize、具体 Esc 按键、Sanitizer 或 Windows。核心实现、错误修正、原理回答和最终配置均通过验收，助手仅更新学习记录，未代写核心业务代码。下一步等待学习者明确确认提交、推送课程分支并合并到 `main`；第 40 课尚未启动。

第 39 课合并记录（2026-09-09）：学习者明确要求提交合并并开始下一课。已提交课程成果 `7fd143a`，推送 `codex/lesson-39-indexed-cube`，以合并提交 `c053462` 纳入 `main` 并推送远端；课程分支继续保留。上方验收记录中的“等待确认”是历史状态，当前 Git 操作已完成。随后同步本完成记录，再从干净、稳定的最新 `main` 开始第 40 课。

第 40 课启动记录（2026-09-09）：第 39 课完成记录 `eb2cd41` 已同步至远端 `main` 与课程分支，实时远端查询确认两者一致。开课前工作区干净，目标课程分支不存在；在 `build/lesson-39-macos-debug` 对合并后的稳定 `main` 重新编译成功且无警告，完整程序创建 OpenGL 4.1 Core Context、Shader 链接成功、日志为空，运行未报告 OpenGL 错误并以退出码 0 结束。有一条 macOS TSM 键盘系统诊断；本轮未独立验证画面、resize、具体 Esc 按键、Sanitizer 或 Windows。随后从稳定 `main` 创建 `codex/lesson-40-geometry-state-review`。只读检查 Application、Renderer、VertexArray、ShaderProgram、Texture2D 与 RenderCommand：Application 按值拥有窗口和 Renderer，成员逆序析构保证 Renderer 的 GPU 资源先释放、窗口与 Context 后销毁；Renderer 按值拥有 Program、VAO／VBO／EBO 包装和纹理；CPU 立方体数组与 ImageData 只在初始化期间存在，OpenGL 上传复制后不再借用；model 与 projection 是逐帧局部值，uniform 上传复制数值；深度、正面和剔除是 Current Context 状态，RenderCommand 不拥有或缓存它们。当前只有一个固定演示立方体，不需要立即新增 Mesh、状态管理器或 RenderDevice，也未修改业务代码。

第 40 课核心任务（30～50 分钟）：由学习者新增 `docs/MiniStudio-geometry-state-resource-review.md`，用一页文档记录四部分。第一，画出 `main → Application → GlfwWindow/Renderer → ShaderProgram/VertexArray/Texture2D/RenderCommand → OpenGL` 的依赖，并单独写明 GlfwWindow 创建和持有 Context 生命周期。第二，用表格列出 CPU 顶点／索引、ImageData、VAO、VBO、EBO、Program、Texture、model/projection uniform 值、深度与剔除状态各自的产生位置、所有者或状态位置、复制边界、更新频率与结束时机。第三，写出 Application 和 Renderer 成员按声明逆序析构的实际顺序，并说明为何 GPU 资源必须先于 Context 释放。第四，给出一次“立方体某一面消失”的排查顺序，至少依次覆盖暂时关闭剔除、核对正面约定、核对该面六个索引及范围、用叉积检查外向绕序、检查 model 是否存在负缩放、再区分深度问题。文档最后回答：VAO 记录 attribute 与 EBO 绑定并不等于拥有 CPU 数组；OpenGL VAO 本身也不会替 C++ 代码删除 VBO/EBO，当前是 `VertexArray` 包装类主动统一拥有并释放三个句柄。本课不重构业务代码、不新增依赖、Mesh、状态管理器或 RenderDevice。完成后由助手实际读取文档，检查每项是否与源码一致，并用现有稳定程序回归行为；尚未提交、推送或合并。

第 40 课最终验收（2026-09-09）：学习者明确表示已掌握复盘内容，并要求助手直接整理以避免重复书写。助手新增 `docs/MiniStudio-geometry-state-resource-review.md`，按照实际源码记录模块依赖、CPU 数据与 GPU 资源的复制边界、Current Context 状态、更新频率、结束时机、`Application`／`Renderer` 成员逆序析构，以及“立方体某一面消失”的排查顺序。文档明确区分：VBO 不拥有原 CPU 数组，`glBufferData` 会把数据复制到独立的 OpenGL Buffer；VAO 记录布局与绑定，不自动删除 VBO/EBO，当前 C++ `VertexArray` 包装类统一拥有并删除三个句柄。复盘确认依赖方向单向、GPU 资源先于 Context 销毁，当前无需新增通用 Mesh、状态管理器或 `RenderDevice`。本课未修改业务源码；第 39 课合并后的稳定程序已在开课时于公司 macOS arm64 独立完成配置、编译和运行检查，因此文档整理后不重复运行。文档内容与源码一致，格式检查通过后第 40 课验收完成；尚未提交、推送或合并，第 41 课尚未开始。

第 40 课合并记录（2026-09-09）：学习者明确要求提交并合并。已提交课程成果 `c4af79e`，推送 `codex/lesson-40-geometry-state-review`，并以合并提交 `efffe63` 纳入 `main`；课程分支继续保留。上方验收记录中的“尚未提交、推送或合并”是历史状态，当前 Git 操作已完成。第 41 课尚未启动。

第 41 课启动记录（2026-09-09）：第 40 课完成记录 `27ed47f` 已同步到本地与远端 `main`、`codex/lesson-40-geometry-state-review`，开课前工作区干净且目标课程分支不存在。只读检查确认静态 `camera_position` 与 view 计算、上传仍集中在 `Renderer::Initialize`；`Application` 当前只拥有窗口和 Renderer，尚无纯 CPU 场景相机。公司 macOS arm64 使用 Apple Clang 21，在全新的 `build/lesson-41-macos-debug` 完成 Debug 配置和编译，成功且无编译器警告；完整程序成功创建 OpenGL 4.1 Core Context，Shader 链接成功、日志为空，启动观察期间未报告 OpenGL 错误。尝试通过 macOS `System Events` 自动发送 Esc 时因系统辅助功能权限被拒绝，随后以 Ctrl-C 结束进程；因此本轮不记录自然退出、具体 Esc、resize、画面读回、Sanitizer 或 Windows 验证。随后从稳定 `main` 创建 `codex/lesson-41-camera-view`，启动记录以外尚未修改业务代码。

第 41 课核心任务（45～75 分钟）：新增纯 CPU `Camera` 模块，由 `Application` 按值拥有。Camera 保存 position、固定 forward `-Z` 和 up `+Y`，公开设置位置与读取 view 矩阵的最小接口；view 可用 `glm::lookAt(position, position + forward, up)` 计算。`Application` 把相机位置设为现有 `(0.25, 0, 0)`，逐帧取得 view 并作为 `const glm::mat4&` 传给 `Renderer::DrawFrame`；Renderer 每帧把该值上传到现有 `view` uniform，并删除 `Renderer::Initialize` 中的相机位置、view 计算和一次性上传。将新增 `.cpp` 加入 CMake。Camera 不依赖 GLFW、OpenGL、Renderer 或输入系统，不拥有 GPU 句柄，也暂不实现移动和鼠标旋转；`main.cpp` 保持不变。验收要求静态画面与迁移前一致，构建无警告、Shader/OpenGL 无错误，并能说明 Camera 保存 CPU 相机状态，Renderer 只保存 GPU 渲染资源并负责 uniform 上传，以及为何“改变相机”与“上传 Shader”属于不同职责。本课尚未提交、推送或合并。

第 41 课最终验收（2026-09-09）：学习者新增 `src/camera/Camera.h/.cpp` 并加入 CMake，由 `Application` 按值拥有 Camera；Camera 保存 position、固定 forward／up，以 `glm::lookAt` 计算 view，未依赖 GLFW、OpenGL、Renderer 或输入系统，也不拥有 GPU 资源。学习者将静态位置设置移到循环前，逐帧计算 view，并通过常量引用传给 Renderer；Renderer 删除初始化阶段的旧相机常量和一次性 view 上传，改为每帧上传传入的 view。首次检查指出逐帧重置位置、矩阵按值传参和头文件间接依赖，学习者自行修正前两项并表示已经掌握；助手按其此前对已掌握内容直接收尾的偏好，补齐 GLM 直接包含、参数命名与格式。旧 `translate(identity, -position)` 与新 Camera 在 position `(0.25, 0, 0)`、forward `-Z`、up `+Y` 下产生等价 view；`main.cpp` 与 GPU 资源所有权不变，所有计算和 uniform 上传仍在主线程。公司 macOS arm64 Debug 增量编译成功且无警告，完整程序创建 OpenGL 4.1 Core Context、Shader 链接成功、日志为空，启动观察期间未报告 OpenGL 错误；因自动发送 Esc 缺少系统权限，本轮以 Ctrl-C 结束，不记录自然退出、画面读回、resize、Sanitizer 或 Windows 验证。源码边界、构建结果和原理掌握通过验收；尚未提交、推送或合并，第 42 课尚未启动。

第 41 课合并记录（2026-09-09）：学习者明确要求提交、合并并开始下一课。已提交课程成果 `cc9637b`，推送 `codex/lesson-41-camera-view`，并以合并提交 `4d2005e` 纳入 `main`；课程分支继续保留。上方验收记录中的“尚未提交、推送或合并”是历史状态，当前 Git 操作已完成。随后同步完成记录，并从稳定的最新 `main` 开始第 42 课。

第 42 课启动记录（2026-09-09）：第 41 课完成记录 `67b0e1a` 已同步到本地与远端 `main`、`codex/lesson-41-camera-view`，开课前工作区干净且目标课程分支不存在。只读检查确认 `Application` 已拥有 `steady_clock` 的累计起点和 Camera，但当前只计算从启动到当前帧的累计时间，Camera 只有设置绝对位置与计算 view 的接口；Renderer 继续只使用累计时间旋转物体并上传 view。合并后的 macOS Debug 构建重新配置并编译成功且无警告，完整程序创建 OpenGL 4.1 Core Context，Shader 链接成功、日志为空，启动观察期间未报告 OpenGL 错误；本轮仍以 Ctrl-C 结束，不记录自然退出、画面读回、resize、Sanitizer 或 Windows 验证。随后从稳定 `main` 创建 `codex/lesson-42-frame-delta`，启动记录以外尚未修改业务代码。

第 42 课核心任务（35～60 分钟）：由 `Application` 同时维护累计时间与帧间时间。循环前令 `previous_time = start_time`；每帧只采样一次 `current_time`，分别计算 `elapsed_seconds = current_time - start_time` 与 `delta_seconds = current_time - previous_time`，再把 `previous_time` 更新为当前采样。累计时间继续传给 Renderer 驱动物体旋转，帧间时间只用于相机位移。Camera 新增接收位移向量的最小 `Move` 接口，执行 `position += displacement`；Application 使用明确的每秒速度，例如 `0.25` 单位／秒，按 `speed * delta_seconds` 让相机沿世界 `+X` 固定移动，再计算本帧 view。先在忽略的构建目录写一个不超过 30 行的纯 CPU 小实验，用总时长相同的两组时间步（例如 `10 × 0.1` 与 `4 × 0.25`）验证累计位移相同，再接入实际时钟。本课保存真实 delta，不做隐式上限裁剪；调试器暂停会导致下一帧位移变大，应作为当前已知行为记录。不要加入键盘、鼠标、固定时间步、全局 Time 类或输入管理器。验收要求第一帧无异常跳跃，短时间运行时相机匀速向 `+X`、画面中的立方体向左移动；两组人工时间步在允许的浮点误差内得到相同位移，并能解释每帧固定移动量为何会随帧率改变实际速度。本课尚未提交、推送或合并。

第 42 课首次代码检查（2026-09-09）：学习者在 `Application` 中以 `start_time` 初始化 `previous_time`，每帧只采样一次 `current_time`，分别计算累计秒数和帧间秒数，再更新上一帧时间点；累计时间仍交给 Renderer，Camera 新增纯 CPU `Move(displacement)` 并以 `position += displacement` 积分。学习者一度尝试把秒数赋给时间点，随后理解 `steady_clock::time_point`、两时间点相减得到的 duration 和 `.count()` 后的浮点秒数不能混用，并完成正确实现。助手按既有偏好将速度整理为具名的 `camera_speed = 0.25f`，把 `delta_times` 改为带单位的 `delta_seconds`，未改变算法。忽略构建目录中的 22 行 C++ 实验以速度 `0.25` 对比 `10 × 0.1` 与 `4 × 0.25` 两组时间步，两组均输出位移 `0.25`、退出码为 0；macOS Debug 增量编译成功且无警告，完整程序两次成功创建 OpenGL 4.1 Core Context，Shader 链接成功、日志为空，启动期间未报告 OpenGL 错误。程序均以 Ctrl-C 结束；自动化无法把该命令行 GLFW 窗口作为可控制应用读取，因此尚未独立核实画面移动和平滑性，也未验证自然退出、resize、Sanitizer 或 Windows。代码与数值实验通过，下一步等待学习者确认第一帧无跳跃、立方体平稳向左移动后做最终验收；尚未提交、推送或合并。

第 42 课最终验收（2026-09-09）：学习者确认实际运行一切正常，即第一帧没有异常跳跃，Camera 沿世界 `+X` 平稳移动，画面中的立方体持续向左。结合首次检查，当前实现明确区分累计时间与帧间秒数：累计时间只驱动物体旋转，真实 delta 只用于 `speed × delta_seconds` 的相机位移；`previous_time` 始终保存时间点并在本帧差值计算后更新。两组总时长同为一秒但帧间划分不同的人工输入均得到位移 `0.25`，验证结果不依赖帧数；当前不限制 delta，调试暂停后的单帧大位移属于已知语义。实现、数值实验、macOS 构建启动和学习者画面观察均通过，第 42 课验收完成；尚未提交、推送或合并，第 43 课尚未启动。

第 42 课合并记录（2026-09-09）：学习者明确要求提交、合并并开始下一课。已提交课程成果 `19ca5be`，推送 `codex/lesson-42-frame-delta`，并以合并提交 `84c275e` 纳入 `main`；课程分支继续保留。上方验收记录中的“尚未提交、推送或合并”是历史状态，当前 Git 操作已完成。随后同步完成记录，并从稳定的最新 `main` 开始第 43 课。

第 43 课启动记录（2026-09-09）：第 42 课完成记录 `e003e52` 已同步到本地与远端 `main`、`codex/lesson-42-frame-delta`，开课前工作区干净且目标课程分支不存在。只读检查确认 `GlfwWindow` 当前只直接查询 Esc，尚未提供可按值传递的移动按键快照；`Application` 仍无条件让 Camera 沿世界 `+X` 移动，Camera 只接收平台无关的位移向量。第 42 课合并后的 macOS Debug 构建重新编译成功且无警告，完整程序创建 OpenGL 4.1 Core Context，Shader 链接成功、日志为空，启动观察期间未报告 OpenGL 错误；本轮以 Ctrl-C 结束，不记录自然退出、画面读回、resize、Sanitizer 或 Windows 验证。随后从稳定 `main` 创建 `codex/lesson-43-input-snapshot`，启动记录以外尚未修改业务代码。

第 43 课核心任务（50～80 分钟）：在 `GlfwWindow.h` 定义只包含 `w_pressed`、`a_pressed`、`s_pressed`、`d_pressed` 四个布尔值的 `MovementKeyState`，由窗口通过 `GetMovementKeyState()` 或同等清晰接口按值返回；实现内部使用 `glfwGetKey`，不得公开 `GLFWwindow*`。`Application` 必须在本帧 `PollEvents()` 之后读取一次快照，再把 W/S 映射为世界 `-Z/+Z`、A/D 映射为世界 `-X/+X` 的移动方向，以现有 `camera_speed * delta_seconds` 计算 displacement 并调用 `Camera::Move`；删除第 42 课无条件沿 `+X` 移动的代码。Camera 不包含 GLFW 头文件、不接收按键枚举或快照，Renderer 与 `main.cpp` 保持不变。按键松开时零方向自然停止，相反方向同时按下应相互抵消；本课不归一化斜向向量，斜向速度较快作为第 47 课问题保留，也不新增事件总线、Input Manager、回调队列或新依赖。验收依次检查 W、S、A、D，W+S、A+D，松开停止及 Esc 退出，并能说明为何输入状态要在 `PollEvents()` 后读取，以及值类型快照只表达本帧状态、不应把其引用保存到下一帧。本课尚未提交、推送或合并。

第 43 课最终验收（2026-09-09）：学习者在 `GlfwWindow` 公共边界内新增四布尔值 `MovementKeyState`，由 `GetMovementKeyState()` 按值返回本帧 W/A/S/D 状态；实现使用显式 `GLFW_PRESS` 比较，未暴露 `GLFWwindow*`。`Application` 在 `PollEvents()` 后取得只读快照，将 W/S 映射为世界 `-Z/+Z`、A/D 映射为世界 `-X/+X`，用应用层 `camera_speed * delta_seconds` 形成最终位移并交给 Camera；第 42 课无条件移动已删除。首次检查发现速度被临时放入 Camera 的公开成员，学习者随后移回 Application，并删除窗口层无用的 GLM 依赖；助手仅整理新代码中的命名、冗余构造和空格。Camera 继续只接收平台无关的 displacement，Renderer 与 `main.cpp` 未改变，依赖方向符合约束。公司 macOS arm64 Debug 增量编译成功且无警告，完整程序创建 OpenGL 4.1 Core Context，Shader 链接成功、日志为空，启动期间未报告 OpenGL 错误；助手以 Ctrl-C 结束，学习者实际确认 W、S、A、D、相反方向组合、松开停止和 Esc 均符合预期。本课按计划保留未归一化的斜向速度，不新增输入管理器或事件系统。代码、构建和交互行为通过验收；尚未提交、推送或合并，第 44 课尚未启动。

第 43 课合并记录（2026-09-09）：学习者明确要求提交、合并并开始下一课。已提交课程成果 `2100f34`，推送 `codex/lesson-43-input-snapshot`，并以合并提交 `6fd1d6c` 纳入 `main`；课程分支继续保留。上方验收记录中的“尚未提交、推送或合并”是历史状态，当前 Git 操作已完成。随后同步完成记录，并从稳定的最新 `main` 开始第 44 课。

第 44 课启动记录（2026-09-09）：第 43 课完成记录 `c1e5dd8` 已同步到本地与远端 `main`、`codex/lesson-43-input-snapshot`，开课前工作区干净且目标课程分支不存在。公司 macOS arm64 使用 Apple Clang 21，在全新的 `build/lesson-44-macos-debug` 完成 Debug 配置和编译，成功且无编译器警告；完整程序创建 OpenGL 4.1 Core Context，Shader 链接成功、日志为空，启动观察期间未报告 OpenGL 错误，随后以 Ctrl-C 结束；本轮不记录自然退出、画面读回、resize、Sanitizer 或 Windows 验证。只读检查 `main`、Application、GlfwWindow、Camera 和 Renderer：`main` 只启动 Application；GlfwWindow 独占窗口与 Context 并采集 GLFW 按键，按值返回四布尔快照；Application 按值拥有窗口、Renderer 和 Camera，保存循环时间点，在事件轮询后把本帧输入、delta 和速度组合为 displacement；Camera 只保存纯 CPU position／forward／up 并计算 view；Renderer 独占 GPU 资源、接收 view 并上传 uniform。全部调用仍在主线程，依赖方向单向，未发现必须立即重构的问题。随后从稳定 `main` 创建 `codex/lesson-44-time-input-camera-review`，启动记录以外尚未修改业务代码。

第 44 课核心任务（30～50 分钟）：新增 `docs/MiniStudio-time-input-camera-review.md`，用一页文档完成本轮架构复盘。第一，画出一帧的五步数据流：`PollEvents／输入采集 → 时间与移动意图生成 → Camera 状态推进 → view 上传与绘制 → Present`，标出 `MovementKeyState`、`delta_seconds`、direction、displacement、Camera position 和 view 的箭头。第二，用表格为 GLFW 内部按键状态、值类型快照、`start_time/previous_time/current_time`、`elapsed_seconds/delta_seconds`、movement、displacement、Camera position、view 和 Program uniform 分别记录生产者、所有者、单位、生命周期、所在线程与复制边界。第三，做一次受控的临时实验：比较 `PollEvents()` 前后读取的快照，或暂时把正式快照读取移到轮询前并观察按下／松开变化可能晚一帧；实验结束必须恢复正确顺序且不保留调试输出。第四，解释 Camera 为什么可以脱离窗口与 OpenGL Context，用固定 displacement 验证 position/view；说明按值快照只属于本帧，把局部快照引用保存到下一帧会悬空或至少表达过期状态。文档最后给出结论：当前无需全局 Time、Input Manager、事件总线或后台输入线程。本课不修改最终业务行为、不新增依赖；完成后由助手读取文档，与源码逐项核对并回归现有程序。本课尚未提交、推送或合并。

第 44 课验收记录（2026-09-09）：学习者查看课程内容后确认已经掌握，并明确要求助手直接完成、提交、合并并开始下一课。助手新增 [docs/MiniStudio-time-input-camera-review.md](MiniStudio-time-input-camera-review.md)，记录五步帧数据流，并逐项说明输入、时间、位移、Camera position、view 与 Program uniform 的生产者、所有权、单位、生命周期、线程和复制边界；内容已与 Application、GlfwWindow、Camera、Renderer 和 ShaderProgram 源码核对。另在被忽略的独立构建目录编译运行 21 行纯 CPU 小实验，只编译 Camera.cpp 并使用 GLM 头文件：相机从 `x=0.25` 沿 `+X` 移动 `0.5` 后，view 的 X 平移由 `-0.25` 变为 `-0.75`，退出码为 0，证明无需窗口或 OpenGL Context。完整 macOS Debug 目标再次构建成功且无警告；本课最终不修改业务源码、不新增依赖。课程成果当前等待提交、推送和合并；未执行 Sanitizer 或 Windows 验证。

第 44 课合并记录（2026-09-09）：按学习者的明确要求，已提交课程成果 `6e52f83`，推送 `codex/lesson-44-time-input-camera-review`，并以合并提交 `35636b3` 纳入 `main`；课程分支继续保留。上方验收记录中的“等待提交、推送和合并”是历史状态，当前 Git 操作已完成。随后同步完成记录，并从稳定的最新 `main` 开始第 45 课。

第 45 课启动记录（2026-09-09）：第 44 课完成记录 `5acf5dc` 已同步到本地与远端 `main`、`codex/lesson-44-time-input-camera-review`，开课前工作区干净且目标课程分支不存在；随后从稳定 `main` 创建 `codex/lesson-45-mouse-delta-focus`。公司 macOS arm64 使用 Apple Clang 21，在全新的 `build/lesson-45-macos-debug` 完成 Debug 配置和编译，成功且无编译器警告；完整程序创建 OpenGL 4.1 Core Context，Shader 链接成功、日志为空，启动观察期间未报告 OpenGL 错误，随后以 Ctrl-C 结束；本轮不记录自然退出、画面读回、交互、Sanitizer 或 Windows 验证。开课前只读检查确认：GlfwWindow 当前独占窗口和 GLFW 输入查询，Application 在 `PollEvents()` 后编排输入，Camera 与 Renderer 不应知道鼠标或焦点；因此鼠标绝对位置、相邻采样基准和捕获状态继续由 GlfwWindow 管理，Application 只接收值类型 delta 并决定进入或退出捕获。

第 45 课核心任务（45～75 分钟）：在 `GlfwWindow` 增加值类型 `MouseDelta { double x; double y; }`，以及捕获切换、左右鼠标键状态和 `ReadMouseDelta()` 的最小公开接口。GlfwWindow 私有保存上次光标坐标与“基准是否有效”；`ReadMouseDelta()` 只在窗口已聚焦且光标已捕获时调用 `glfwGetCursorPos`，首个有效样本只建立基准并返回零，后续返回当前位置减上次位置。失焦、退出捕获或捕获状态真正发生变化时必须使基准失效，防止恢复输入后的巨大跳变；重复设置相同捕获状态不能每帧清空基准。Application 在 `PollEvents()` 后用左键进入 `GLFW_CURSOR_DISABLED`、右键恢复 `GLFW_CURSOR_NORMAL`，Esc 继续关闭窗口；每帧读取一次 delta，只打印非零的受控样本，不把它传入 Camera。原始窗口坐标的 Y 轴向下，本课保留该符号，下一课再映射为 pitch。验收时检查：捕获后的首样本为零、静止接近零、连续移动符号和大小合理、右键可释放、重新捕获及切出再切回没有巨幅 delta、W/A/S/D 与 Esc 未回归。本课不新增 Input Manager、事件总线、回调体系或相机旋转；代码由学习者实现，完成后由助手实际读取并执行 macOS 构建与运行检查。本课尚未提交、推送或合并。

第 45 课验收记录（2026-09-09）：学习者在 GlfwWindow 公共边界新增按值返回的 `MouseInputState`，包含本帧 `delta_x/delta_y` 与左右键状态；GlfwWindow 私有保存上一有效光标坐标和基准有效位。`ReadMouseInput()` 在主线程依次处理窗口焦点、按钮状态、捕获模式、首次基准和后续差值，失焦或未捕获时使基准失效；`SetCursorCaptured()` 只在模式实际变化时切换 `GLFW_CURSOR_DISABLED/NORMAL` 并清除基准。Application 在 `PollEvents()` 后读取一次快照，以左键捕获、右键释放，只打印非零 delta，未把鼠标输入传给 Camera。公司 macOS arm64 Debug 增量编译成功且无警告；完整程序创建 OpenGL 4.1 Core Context，Shader 链接成功且日志为空，运行会话记录到连续的正负方向、整数和亚像素 delta，并以退出码 0 正常结束。学习者实际确认捕获、释放、静止、连续移动、重新捕获、切出切回、W/A/S/D 和 Esc 均符合预期。本课没有修改 Camera、Renderer 或依赖关系，代码和交互行为通过验收；当前尚未提交、推送或合并，未执行 Sanitizer 或 Windows 验证。

第 45 课合并记录（2026-09-09）：按学习者的明确要求，已提交课程成果 `74ec708`，推送 `codex/lesson-45-mouse-delta-focus`，并以合并提交 `b91fb12` 纳入 `main`；课程分支继续保留。上方验收记录中的“尚未提交、推送或合并”是历史状态，当前 Git 操作已完成。随后同步完成记录，并从稳定的最新 `main` 开始第 46 课。

第 45 课合并后修正（2026-09-09）：开始第 46 课前重新构建最新 `main` 时，编译器在 `Application.cpp` 窗口初始化分支末尾发现意外文本 `tui`。该文本与课程功能无关，但会导致 `use of undeclared identifier 'tui'`，因此未在失败基线上创建新课程分支。已通过提交 `2e9efe2` 删除这三个字符；公司 macOS arm64 Debug 目标随后恢复编译成功且无警告。此记录保留失败和修复证据，避免把第 45 课运行验收时较早的成功二进制误写成合并后源码仍可构建。

第 46 课启动记录（2026-09-09）：第 45 课合并后修正记录 `52fd81a` 已同步到本地与远端 `main`、`codex/lesson-45-mouse-delta-focus`，开课前工作区干净且目标课程分支不存在；随后从稳定 `main` 创建 `codex/lesson-46-yaw-pitch-camera`。公司 macOS arm64 使用 Apple Clang 21，在全新的 `build/lesson-46-macos-debug` 完成 Debug 配置和编译，成功且无编译器警告；完整程序创建 OpenGL 4.1 Core Context，Shader 链接成功、日志为空，启动观察期间未报告 OpenGL 错误，随后以 Ctrl-C 结束；本轮不记录自然退出、交互、Sanitizer 或 Windows 验证。开课前只读检查确认：GlfwWindow 已完整拥有焦点、捕获和鼠标 delta 采样状态；Application 在轮询后读取值快照；Camera 当前只保存 position、固定 forward 和 up，并计算 view。因此本课新增的 yaw、pitch、forward 推导及俯仰限制归 Camera 所有，鼠标像素到角度的灵敏度与符号映射归 Application；Renderer 和窗口接口无需了解相机角度。

第 46 课核心任务（60～90 分钟）：为 Camera 增加按“角度增量”旋转的最小接口、只读按值返回的 forward 观察接口，以及私有 `yaw_degrees_`、`pitch_degrees_` 和更新 forward 的辅助函数。采用初始 `yaw=-90°`、`pitch=0°` 对应世界 `-Z`；每次旋转先累加 yaw/pitch，把 pitch 限制在 `[-89°, 89°]`，再用球面方向公式计算并归一化 forward。先用固定角度验证：`(-90°, 0°)` 约为 `(0, 0, -1)`，yaw 增加 `90°` 后约为 `(1, 0, 0)`，pitch 增加 `30°` 后 Y 分量约为 `0.5`。随后由 Application 用 `0.1°/pixel` 的灵敏度映射第 45 课 delta：`yaw_delta = +delta_x * sensitivity`，`pitch_delta = -delta_y * sensitivity`；鼠标 delta 已是本帧累计像素，不再乘 `delta_seconds`。更新朝向后再取得 view，移除第 45 课的鼠标打印。验收时检查左右／上下观察方向、极端上下移动不翻转或产生非法矩阵、重新捕获及焦点切换无跳变、W/A/S/D 与 Esc 未回归；本课 W/A/S/D 仍沿世界轴移动，转头后移动方向暂不跟随视线，这是第 47 课范围。本课不增加四元数、roll、局部移动、Camera 对 GLFW 的依赖或新的输入框架；代码由学习者实现，完成后由助手读取源码、执行固定角度小实验，并完成 macOS 构建与运行检查。本课尚未提交、推送或合并。

第 46 课验收记录（2026-09-10）：学习者为 Camera 增加 `Rotate(yaw_delta_degrees, pitch_delta_degrees)`、按值返回的 `Forward()`、私有 yaw/pitch 状态和 forward 更新函数；Camera 将 pitch 限制在 `[-89°, 89°]`，用角度转弧度后的球面公式生成并归一化 forward，继续只依赖 GLM 与标准数学函数。Application 删除第 45 课调试输出，以 `0.1°/pixel` 把 `delta_x` 映射为 yaw、把 `-delta_y` 映射为 pitch，没有再次乘 `delta_seconds`；W/A/S/D 仍沿世界轴。被忽略的纯 CPU 固定角度实验通过：yaw 增加 `90°` 得到 `(1, 0, 0)`，pitch 增加 `30°` 得到约 `(0, 0.5, -0.866025)`。公司 macOS arm64 Debug 增量编译成功且无警告，完整程序创建 OpenGL 4.1 Core Context，Shader 链接成功、日志为空；学习者实际确认左右、上下、连续水平旋转、极限俯仰、重新捕获和焦点切换均符合预期。辅助运行会话最终由 Ctrl-C 结束，未把它记录为本课新的 Esc 自然退出证据；Esc 路径未被本课修改，上一课已有正常退出证据。本课未执行 Sanitizer 或 Windows 验证，代码和交互行为通过验收。课程分支已存在远端框架检查点 `04b0f67`，最终修正和完成记录尚未提交、推送或合并。

第 46 课合并记录（2026-09-10）：按学习者的明确要求，已将最终修正和验收记录提交为 `79d1893`，推送 `codex/lesson-46-yaw-pitch-camera`，并以合并提交 `bd7a693` 纳入 `main`；课程分支继续保留。上方验收记录中的“尚未提交、推送或合并”是历史状态，当前 Git 操作已完成。随后同步完成记录，并从稳定的最新 `main` 开始第 47 课。

第 47 课启动记录（2026-09-10）：第 46 课完成记录 `841ee19` 已同步到本地与远端 `main`、`codex/lesson-46-yaw-pitch-camera`，开课前工作区干净且目标课程分支不存在；随后从稳定 `main` 创建 `codex/lesson-47-local-camera-movement`。公司 macOS arm64 使用 Apple Clang 21，在全新的 `build/lesson-47-macos-debug` 完成 Debug 配置和编译，成功且无编译器警告；完整程序创建 OpenGL 4.1 Core Context，Shader 链接成功、日志为空，启动观察期间未报告 OpenGL 错误，随后以 Ctrl-C 结束；本轮不记录自然退出、交互、Sanitizer 或 Windows 验证。开课前只读检查确认：GlfwWindow 继续只生产按键与鼠标值快照，Camera 按值提供归一化 forward 并接收最终世界位移，Application 仍把 W/A/S/D 直接映射到世界 X/Z。因此右方向作为相机姿态的派生量由 Camera 计算并按值提供，Application 负责把按键意图组合、归一化，再乘速度与 delta；Renderer 和窗口模块无需变化。

第 47 课核心任务（40～65 分钟）：为 Camera 增加按值返回的 `Right()`，使用 `normalize(cross(forward_, up_))` 计算局部右方向；初始朝向时应约为世界 `+X`，yaw 增加 `90°` 后应约为世界 `+Z`。Application 先用本帧鼠标 delta 调用 `Rotate()`，随后用更新后的 `Forward()` 与 `Right()` 组合移动意图：W 加 forward、S 减 forward、D 加 right、A 减 right。仅当 `dot(movement, movement) > 0` 时归一化 movement，避免相反按键抵消为零后对零向量归一化产生 NaN；最后仍用 `camera_speed * delta_seconds` 得到位移并调用 `Move()`。本阶段采用自由飞行，forward 的 Y 分量参与 W/S 移动；鼠标 delta 不乘时间，而键盘方向乘以世界单位／秒和 delta。验收时检查默认和旋转后的 forward/right 正交且长度约为 1，未归一化的斜向长度约为 `sqrt(2)`、归一化后为 1；实际运行中转头后 W/S 跟随视线，直行与斜行在相同时间内距离接近，相反按键抵消且画面不消失。课程范围止于局部轴与匀速移动，不增加碰撞、加速度、地面约束、roll 或新的控制器类；代码由学习者实现，完成后由助手读取源码、运行纯 CPU 方向实验，并完成 macOS 构建与交互检查。本课尚未提交、推送或合并。

第 47 课验收记录（2026-09-10）：学习者为 Camera 增加按值返回的 `Right()`，使用 `normalize(cross(forward_, up_))` 从当前姿态生成局部右方向；Application 在读取本帧输入后先调用 `Rotate()`，再以更新后的 forward/right 映射 W/S 与 A/D，只有 `dot(movement, movement) > 0` 时才归一化，最后用 `camera_speed * delta_seconds` 形成位移。被忽略的纯 CPU 实验验证初始 forward/right 点积为 0，原始对角合向量长度为 `1.41421`，归一化后为 1；yaw 增加 `90°` 后 forward 约为 `+X`、right 约为 `+Z`。公司 macOS arm64 Debug 增量编译成功且无警告；完整程序创建 OpenGL 4.1 Core Context，Shader 链接成功、日志为空，运行期间未报告 OpenGL 错误并以退出码 0 结束。学习者实际确认局部移动与斜向速度感觉符合预期；相反方向组合由零向量保护覆盖。本课没有修改输入接口、Renderer 或资源所有权，未执行 Sanitizer 或 Windows 验证，代码和交互行为通过验收；当前尚未提交、推送或合并。

第 47 课合并记录（2026-09-10）：按学习者的明确要求，已将课程成果和验收记录提交为 `65d1a66`，推送 `codex/lesson-47-local-camera-movement`，并以合并提交 `2cbd90e` 纳入 `main`；课程分支继续保留。上方验收记录中的“尚未提交、推送或合并”是历史状态，当前 Git 操作已完成。随后同步完成记录，并从稳定的最新 `main` 开始第 48 课。

第 48 课启动记录（2026-09-10）：第 47 课完成记录 `2a2080c` 已同步到本地与远端 `main`、`codex/lesson-47-local-camera-movement`，开课前工作区干净且目标课程分支不存在；随后从稳定 `main` 创建 `codex/lesson-48-interactive-camera-review`。公司 macOS arm64 使用 Apple Clang 21，在全新的 `build/lesson-48-macos-debug` 完成 Debug 配置和编译，成功且无编译器警告；完整程序创建 OpenGL 4.1 Core Context，Shader 链接成功、日志为空，启动观察期间未报告 OpenGL 错误，随后以 Ctrl-C 结束，因此本轮只记录启动基线，不记录自然退出或完整交互结果，也未执行 Sanitizer 或 Windows 验证。开课前只读检查确认：main 只启动 Application；Application 按值拥有 GlfwWindow、Renderer 和 Camera 并编排帧循环；GlfwWindow 管理 GLFW、窗口、Context、输入快照与鼠标基准；Camera 只保存 CPU 相机状态和数学操作；Renderer 独占 Shader Program、VAO/VBO/EBO 与纹理；所有调用仍在主线程，成员逆序析构保证 GPU 资源先于窗口和 Context 释放。

第 48 课核心任务（35～60 分钟）：建立交互相机回归表，分别记录正常移动、转头、失焦／重新获焦、resize、最小化／恢复、关闭六类操作的步骤、预期结果、公司 macOS 实测结果与证据；未执行的平台或操作明确写“未验证”，不得推断为通过。重点检查按键松开后无持续移动，重新捕获或获焦后的首帧无猛转，极端俯仰不翻转，窗口 framebuffer 为零时跳过绘制且恢复后继续显示，Esc 或窗口关闭能正常退出。随后画出或列出 main、Application、GlfwWindow、Camera、Renderer、RenderCommand 和 OpenGL 的依赖与所有权，说明成员析构顺序为何使 GPU 资源在 Context 销毁前释放，并回答失焦与最小化为何必须分开记录。复盘结果写入 `docs/MiniStudio-interactive-camera-review.md`；本课不增加多相机、输入绑定系统、事件总线或新的渲染抽象，只有回归发现真实缺陷时才做最小修复。完成后由助手读取记录和相关源码、重新构建，并按已执行的交互项验收。本课尚未提交、推送或合并。

第 48 课验收记录（2026-09-10）：已完成 `docs/MiniStudio-interactive-camera-review.md`，记录六类操作的步骤、预期、公司 macOS arm64 实测结果与证据，并复核 main、Application、GlfwWindow、Camera、Renderer、RenderCommand 和 OpenGL 的职责、单向依赖、线程及资源所有权。全新的 `build/lesson-48-macos-debug` 使用 Apple Clang 21 完成 Debug 配置和编译，无编译器警告；程序创建 OpenGL 4.1 Core Context，Shader 链接成功且日志为空。受控 UI 回归向真实窗口发送 W/S、A+D、鼠标捕获与位移，切换 Finder 造成失焦后重新获焦，resize 至 `920×700` 再恢复，最小化 `1s` 再恢复，最后发送 Esc；关键步骤均截图，运行日志没有 OpenGL 错误，程序以退出码 0 结束。结果未发现卡键、相反输入非法向量、首帧猛转、resize 投影异常、最小化恢复失败或关闭失败；最小化期间的具体 framebuffer 数值未单独记录，Windows 未验证。复盘确认 Application 的成员逆序析构为 Camera、Renderer、GlfwWindow，使 Renderer 的 GPU 资源在窗口和 Context 销毁前释放；Camera 仍只拥有 CPU 状态，没有理由增加新抽象或修改业务代码。本课通过验收，当前尚未提交、推送或合并。

第 48 课合并记录（2026-09-10）：按学习者的明确要求，已将复盘文档和验收记录提交为 `b0c19e2`，推送 `codex/lesson-48-interactive-camera-review`，并以合并提交 `ca226d8` 纳入 `main`；课程分支继续保留。上方验收记录中的“尚未提交、推送或合并”是历史状态，当前 Git 操作已完成。随后同步完成记录，并从稳定的最新 `main` 开始第 49 课。

第 49 课启动记录（2026-09-10）：第 48 课完成记录 `1f6a40b` 已同步到本地与远端 `main`、`codex/lesson-48-interactive-camera-review`，开课前工作区干净且目标课程分支不存在；随后从稳定 `main` 创建 `codex/lesson-49-alpha-cutout`。公司 macOS arm64 使用 Apple Clang 21，在全新的 `build/lesson-49-macos-debug` 完成 Debug 配置和编译，成功且无编译器警告；完整程序创建 OpenGL 4.1 Core Context，Shader 链接成功、日志为空，启动观察期间未报告 OpenGL 错误，随后以 Ctrl-C 结束；本轮不记录自然退出、交互、Sanitizer 或 Windows 验证。开课前只读检查确认：Texture2D 已能接收任意 RGBA 字节并复制到 GPU，不需要新增图片依赖；Alpha 裁剪属于 Renderer 的固定演示场景与片元 Shader，带孔图片可由 Renderer.cpp 内的无状态辅助函数生成；前景平面所需 VAO 和纹理继续由 Renderer 按值拥有，Application、GlfwWindow、Camera、RenderCommand 和底层资源接口无需扩张。

第 49 课核心任务（40～65 分钟）：在 Renderer 中把现有立方体资源命名明确为 cube，并增加由 Renderer 独占的 cutout plane VertexArray 与 Texture2D；在 Renderer.cpp 的匿名命名空间或同等局部边界内生成一张小型 RGBA 图片，实心区域 alpha 为 255、中心孔 alpha 为 0，再通过现有 Texture2D 上传，禁止新增文件解码依赖。增加一个位于旋转立方体前方、面向初始相机的四顶点六索引平面；片元 Shader 先采样纹理，当 `texel.a < 0.5` 时执行 `discard`，否则输出 texel。每帧分别上传立方体和平面的 model，绑定各自 VAO 与纹理并绘制，保持深度测试和深度写入开启；最终明确恢复背面剔除、剔除 Back、正面逆时针的默认状态，平面若选择双面显示只能在其绘制边界临时关闭剔除并在之后恢复。先做一次故障注入：去掉 `discard`、只让孔像素 alpha 为 0，观察在未启用混合时孔仍以 RGB 写颜色并参与深度；再恢复裁剪。验收要求实心区域正常遮挡后方立方体、中心孔能看到后方立方体，移动和转头后深度关系仍正确，运行无 Shader 或 OpenGL 错误；能解释 alpha 为 0 只是一个输出分量，而 `discard` 会终止该片元并阻止颜色与深度写入。本课不做半透明、混合、Alpha-to-Coverage、材质系统或透明排序；代码由学习者实现，完成后由助手读取源码、构建并运行验收。本课尚未提交、推送或合并。

第 49 课验收记录（2026-09-10）：学习者将 Renderer 的原立方体资源明确命名为 cube，并增加由 Renderer 按值独占的 cutout plane VertexArray 与 Texture2D；`CreateCenterHoleImage()` 收在 Renderer.cpp 的匿名命名空间，生成一次 `16×16` RGBA 图片后由现有 Texture2D 复制到 GPU，中心 `8×8` 区域保留红色 RGB 但 alpha 为 0，外围为不透明绿色。片元 Shader 在 alpha 小于 `0.5` 时执行 `discard`；每帧先在背面剔除、深度测试和深度写入开启时绘制旋转立方体，再只为前景平面关闭剔除，绘制完成后恢复 Back 与 CounterClockwise。公司 macOS arm64 的 Debug 增量编译成功且无警告；完整程序创建 OpenGL 4.1 Core Context，Shader 链接成功、日志为空，运行期间未报告 OpenGL 错误。实测截图确认绿色实心区遮挡后方场景，中心孔同时显示旋转立方体和清屏背景；学习者确认最终交互与双面显示符合预期，Esc 使程序以退出码 0 结束。只输出 alpha 0 而不裁剪的故障注入未由助手单独留存截图；最终源码和现象能够区分 alpha 通道值与真正终止颜色／深度写入的 `discard`。Application、窗口、Camera、RenderCommand、Texture2D 接口和依赖方向均未变化；未执行 Sanitizer 或 Windows 验证。本课通过验收，当前尚未提交、推送或合并。

第 49 课合并记录（2026-09-10）：按学习者的明确要求，已将 Alpha 裁剪实现和验收记录提交为 `bfbb200`，推送 `codex/lesson-49-alpha-cutout`，并以合并提交 `d51ad68` 纳入 `main`；课程分支继续保留。上方验收记录中的“尚未提交、推送或合并”是历史状态，当前 Git 操作已完成。第 50 课尚未开始。

第 50 课启动记录（2026-09-10）：开始时发现另一条已完成任务链已把远端 `main` 推进到第 49 课，原本地第 37 课实验分支没有未提交内容，因此没有重复提交或覆盖；本地 `main` 以 `--ff-only` 更新到 `origin/main=0be6f9f`，工作区干净且目标课程分支不存在。家用 Windows 首次使用旧 CLion 增量构建目录时，程序在立方体 VAO 初始化阶段返回失败；只读检查与临时诊断确认 `VertexArray` 句柄呈 MSVC Debug 的 `0xCCCCCCCC` 未初始化标记，而源码仍有正确成员默认值，原因是跨 44 个提交增量更新后旧调用方对象文件与新版 `Renderer` 类布局不一致。临时诊断代码已全部移除。随后使用全新的 `build/lesson-50-windows-debug` 目录重新配置，vcpkg manifest 安装路径、GLFW、GLAD 与 GLM 均正确，12 个目标完整编译并链接成功；程序持续运行 10 秒，创建 OpenGL 4.1 Core Context、Shader 链接成功且日志为空，未报告 OpenGL 错误，最后由助手以 Ctrl-C 结束，因此只记录 Windows 启动基线，不记录自然退出或完整交互验收。公司 Mac 的第 49 课验证仍独立有效，本轮未重新验证 macOS。随后从稳定 `main` 创建 `codex/lesson-50-alpha-blending`。

第 50 课核心任务（45～75 分钟）：明确使用非预乘 alpha，先手算 `source=(1,0,0)`、`destination=(0,0,1)`、`alpha=0.5` 的 RGB 结果；在无状态 RenderCommand 中增加混合开关和最小混合因子接口，把源因子 `SourceAlpha`、目标因子 `OneMinusSourceAlpha` 映射到 OpenGL。将第 49 课前景平面的生成纹理改为固定透明度，移除会把 alpha 0 实验提前丢弃的 `discard`；不透明立方体先在混合关闭、深度测试与写入开启时绘制，随后透明平面保持深度测试开启、关闭深度写入、开启混合并按 `SRC_ALPHA / ONE_MINUS_SRC_ALPHA` 绘制，结束后恢复深度写入、关闭混合并恢复默认剔除状态。依次用 alpha 0、约 0.5（128/255）和 1（255/255）观察结果；本课只处理一层透明，不做排序、预乘 alpha、Alpha-to-Coverage 或 Material 系统。验收需确认三组趋势、透明平面仍受前方不透明物体的深度遮挡、跨帧清深度正常，Windows Debug 构建运行无 Shader/OpenGL 错误，并解释源／目标颜色、关闭深度写入与关闭深度测试的区别。核心业务代码由学习者实现，完成后由助手读取与验证。

第 50 课验收记录（2026-09-10）：学习者在 RenderCommand 中增加 `SetBlendingEnabled()`、`SetBlendFunction()` 和最小 `BlendFactor` 枚举，将非预乘 alpha 所需的 `SourceAlpha` 与 `OneMinusSourceAlpha` 映射到 OpenGL；Renderer 把第 49 课的裁剪平面改为固定 alpha 约 `0.5` 的半透明平面，移除 `discard`，并统一使用 translucent 命名。每帧先以默认深度与剔除状态绘制不透明立方体，再为半透明平面保留深度测试、关闭深度写入、开启标准 alpha 混合并临时关闭剔除，绘制后恢复深度写入、混合和剔除状态。学习者分别测试 alpha 0、128 和预期结果，确认画面符合趋势；能手算红色覆盖蓝色、alpha 0.5 时 RGB 为 `(0.5, 0.0, 0.5)`，并正确解释关闭深度写入不会取消深度比较，而关闭深度测试会让后画片元不经比较直接参与后续颜色处理。最终源码通过 `git diff --check`；家用 Windows 在加载 Visual Studio 2026 x64 开发环境后重新编译 `Renderer.cpp` 并成功链接，完整程序创建 OpenGL 4.1 Context、Shader 链接成功且日志为空，持续运行 10 秒未报告 OpenGL 错误，随后由助手以 Ctrl-C 结束。此次只形成 Windows 验证证据，未重新验证 macOS、Sanitizer 或自然退出。本课通过验收，当前尚未提交、推送或合并。

第 50 课合并记录（2026-09-10）：按学习者明确要求，课程实现与验收记录已提交为 `88962f6` 并推送 `codex/lesson-50-alpha-blending`，随后以合并提交 `03fa7ca` 纳入 `main`；课程分支继续保留。首次推送 `main` 时本机直连 GitHub 443 连续超时；只读诊断确认 Windows 用户代理为 `127.0.0.1:7890`、Git 未配置代理且本地代理端口可用，通过一次性 `http.proxy` 参数成功读取远端后完成普通推送，未修改 Git 持久配置，也未使用强制推送。上方“尚未提交、推送或合并”是历史状态。

第 51 课启动记录（2026-09-10）：第 50 课合并记录提交 `b7b323d` 已通过 Windows 用户代理的一次性 Git 参数推送，本地 `main`、`origin/main` 和实时查询的远端 `main` 一致；工作区干净且目标分支不存在。合并后的 Windows Debug 构建重新编译 RenderCommand 与 Renderer 并成功链接，随后从稳定 `main` 创建 `codex/lesson-51-transparent-sorting`。只读检查确认 Application 继续只把 Camera 生成的 view 矩阵交给 Renderer；Camera 不需要暴露位置或拥有渲染状态；RenderCommand 已具备深度、混合和剔除所需命令；Renderer 继续按值拥有 ShaderProgram、共享透明平面的 VertexArray 与 Texture2D。本课的两个透明绘制项是 Renderer 每帧临时构造的 CPU 值，不拥有 GPU 资源，排序后复用同一 VAO 与纹理；为让两个平面颜色可区分，只需给现有 ShaderProgram 增加一个最小 `vec4` uniform 上传接口，不新增 Material、场景系统或资源管理器。本轮启动证据仅来自家用 Windows，未重新验证 macOS 或 Sanitizer。

第 51 课核心任务（60～90 分钟）：先在黑色背景上手算 alpha 均为 `0.5` 的红、蓝两层分别按红→蓝和蓝→红绘制后的 RGB，确认标准 alpha 混合具有顺序相关性。复用当前透明平面的 VAO 与纹理，增加 `ShaderProgram::SetVec4(const char*, const glm::vec4&)` 供片元 Shader 接收每个绘制项的 tint；不透明立方体使用白色 tint。Renderer.cpp 的局部边界定义只含 model、tint 和相机空间 z 的 `TransparentDrawItem`，每帧构造两个世界中不相交但屏幕有重叠的红、蓝半透明平面，用 `view * model * vec4(0,0,0,1)` 得到中心的相机空间 z，并用 `std::sort` 按远到近排序；当前相机前方 z 为负，因此更小的 z 先画。保持不透明阶段深度测试与写入开启；透明阶段保持测试、关闭写入、开启 `SRC_ALPHA / ONE_MINUS_SRC_ALPHA`，按排序结果逐项上传 tint 与 model 后绘制，最后统一恢复混合、深度写入和剔除。先故意反转比较器观察重叠颜色，再恢复正确顺序并移动、转动相机验证排序变化。本课只解决两个可按中心整体排序的平面，不处理相交面、循环遮挡、三角形级排序、预乘 alpha、OIT 或通用场景／Material。验收要求两种顺序的结果可预测且画面差异可复现，相机移动后排序依据仍正确，状态恢复不影响下一帧，并能解释为何世界 z 排序会随相机方向失效以及物体中心排序的局限。

第 51 课中途检查（2026-09-10）：学习者完成 `ShaderProgram::SetVec4()`，沿用现有 Program 与 location 检查，通过 `glUniform4fv(location, 1, glm::value_ptr(value))` 上传单个向量；片元 Shader 输出 `tint * texel`。Renderer 每帧在立方体 draw 前上传白色 tint，在透明平面 draw 前上传红色 tint，均检查返回值，且 tint alpha 保持 1，使最终透明度仍为 `128/255`。助手实际读取三个源码文件的改动；复用家用 Windows 的 `build/lesson-50-windows-debug`，MSVC x64 重新编译 Renderer、ShaderProgram 并成功链接，未输出编译警告；完整程序持续运行 10 秒，OpenGL 4.1 Context 与 Shader 链接正常，未报告 OpenGL 错误，随后由助手 Ctrl-C 结束。本轮未独立采集画面或验证自然退出，未执行 macOS 回归。颜色手算中的遗漏已讲清：第二层要与第一层叠加黑色背景后的颜色继续混合。下一步由学习者实现两个临时 CPU 绘制项、相机空间深度计算与远到近排序，并验证交换顺序和移动相机的结果；第 51 课整体仍在进行中。

第 51 课排序实现检查（2026-09-10）：学习者已把红、蓝两个绘制项保存到 `std::array<TransparentDrawItem, 2>`，分别以 `view * model * vec4(0,0,0,1)` 计算并保存相机空间 z，通过比较器 `a.view_z < b.view_z` 排序整个绘制项；先用独立 CubeModel、白色 tint 和立方体资源绘制不透明物体，再遍历排序后的数组，上传各项 model 与 tint 并复用透明平面的 VAO 和纹理绘制。两次透明 draw 前均设置标准 alpha 混合、关闭深度写入与剔除，保留深度测试，循环后恢复状态；公共状态与资源绑定目前在循环内重复调用，可后续移到循环前整理。助手实际读取改动并在家用 Windows MSVC x64 下重新编译和链接成功，完整程序运行 10 秒，OpenGL 4.1 Context 与 Shader 链接正常，未报告 OpenGL 错误，随后 Ctrl-C 结束；未独立核实画面、自然退出或 macOS。当前代码检查与启动检查通过；仍待学习者对比正反排序的重叠颜色、移动相机并说明相机空间排序及中心排序的局限后完成整课验收。

第 51 课最终验收记录（2026-09-10）：在上方最终排序实现检查后，学习者针对初始视角下正反比较器的重叠颜色，以及恢复远到近排序后移动到平面另一侧观察的三项实验，明确确认“符合预期”；画面证据来自学习者反馈，助手未独立截图或读回 GPU 像素。最终代码保留 `<` 比较器、逐帧相机空间深度计算和按排序数组绘制的流程，与此前 Windows Debug 成功编译及运行 10 秒的源码一致，因此文档收尾未重复运行相同检查。学习者独立解释了前后关系应相对于相机而非世界 z 判断；对穿插问题起初理解为平行平面的投影重叠，助手以三维相交的两张平面为例解释交线两侧所需绘制顺序相反，学习者随后确认理解物体中心排序不能保证穿插透明面正确。本课限定于受控的可整体排序场景，不据此宣称解决任意透明场景。ShaderProgram 保持 GPU Program 独占所有权，Renderer 继续拥有 GPU 资源；排序仅移动本帧 CPU 绘制项，所有渲染调用仍在持有 Current Context 的主线程。公共状态和资源绑定的循环内重复调用属于可选整理，不阻碍本课验收。家用 Windows 构建与启动证据、学习者的视觉反馈及原理纠正共同完成验收；未执行 macOS、Sanitizer 或自然退出回归。第 51 课通过，当前尚未提交、推送或合并，等待学习者确认本课 Git 操作后进入第 52 课。

第 51 课本地合并记录（2026-09-10）：学习者明确要求“52我也会了，不用复盘你直接写，完事一并提交合并”，授权第 51、52 课一起提交、推送并合并。已将第 51 课成果提交为 `9ecba97`，通过合并提交 `19fc863` 纳入本地 main；课程分支 `codex/lesson-51-transparent-sorting` 保留，计划与第 52 课一起推送。上方最终验收记录中的等待确认是历史状态，本轮不再要求重复确认。

第 52 课启动与边界记录（2026-09-10）：开始前已读取 README、课程路线、交接与 A/B 阶段教案，核对目录、职责、所有权及依赖方向；远端 main 为第 50 课记录提交 `b7b323d`，本地 main 在已验收且可构建的第 51 课基线上完成上述合并，工作区干净，随后创建 `codex/lesson-52-v02-validation`。学习者明确要求助手直接实现并跳过复盘，因此本课只执行 v0.2 工程收尾、回归和演示，不追问个人季度总结，不虚构实际学习工时或新的能力结论，也不一次实现未来模块。

第 52 课工程验收记录（2026-09-10）：CMake 与 vcpkg 版本更新为 `0.2.0`。Renderer 将透明绘制集中到私有边界，模型和排序列表生成留在实现文件匿名命名空间；公共状态与资源绑定移到循环前，成功及 uniform 失败退出都恢复深度写入、混合和剔除，帧首显式关闭混合，移除未使用的单平面模型函数。未新增资源所有者、场景系统、Material、RenderDevice 或依赖；Application、Camera、GlfwWindow 和原有平台加载路径未改变。新增默认关闭的 `MINISTUDIO_BUILD_TESTS` 与 CTest 集成目标，复用实际 Renderer 和资源模块。家用 Windows MSVC 19.51 x64 在全新 Debug、Release 及默认关闭测试的目录完成配置编译，无编译器警告；Debug/Release 回归均 1/1 通过，测试程序正常退出 0。真实 GPU 读回覆盖正反视角、三种窗口尺寸合计 7728 个采样点，验证受控透明颜色、立方体深度、透明不写深度、动画、重复帧和状态恢复；另验证 alpha 零与 discard 的差异、资源移动和 Context 存活期间释放、图片缺失失败路径、Camera 数值、零尺寸跳过、resize 与关闭标志。暂将排序 `<` 反转为 `>` 后测试在重叠区检出颜色错误，恢复正确比较器后重新构建并通过。已导出 90 帧真实 Renderer 的 GPU 数据，生成约 6 秒 GIF，并检查正反视角预览；这不是鼠标交互录屏，不把值接口和关闭标志检查冒充完整 Application 交互回归。实际命令、所有权与状态图、输出和限制见 [v0.2 验证与交接](MiniStudio-v0.2-validation.md)。本轮未运行 macOS 或 Sanitizer；Windows 使用既有依赖安装目录且关闭 manifest 安装，没有安装或更新依赖。

第 52 课当前交接（2026-09-10）：工程收尾和当前主机验证完成，个人季度复盘按要求跳过；待按已有授权提交本课、合并 main，并与第 51 课一起推送。课程分支继续保留。下一项为 B01（第 53 课）CPU MeshData 与顶点布局，尚未开课、未创建分支。本课只验证受控的可整体排序场景；物体中心排序仍不解决任意相交、循环遮挡或某些斜视局部遮挡。

macOS 使用 Homebrew GLFW 3.4 和系统 `OpenGL::GL`；Windows 使用 vcpkg manifest 提供 GLFW 与 GLAD，GLAD 只在 Windows 条件分支初始化。当前代码已经拆分应用、窗口、Shader Program、顶点输入资源、Texture2D 和无状态渲染命令，并通过 `glDrawElements` 呈现 24 顶点、36 索引的纹理立方体；model 随时间绕 X、Y 两轴旋转，projection 使用实际 framebuffer 宽高比。

第 33 课启动记录（2026-09-08）：只读核对目录、职责、资源所有权与依赖方向；拉取远端引用后确认 `main` 与 `origin/main` 同为 `48596d3`，第 32 课已合并，工作区干净。基线从独立的 `build/lesson-33-debug` 目录完成 Debug 配置和编译，无编译器警告；本轮尚未重新进行运行验收。随后从该稳定 `main` 创建 `codex/lesson-33-depth-testing`。

本课核心任务控制在 45～75 分钟：复用现有 Shader Program、VAO/EBO 和 Texture2D，用两份独立 model 矩阵绘制不同深度、部分重叠的不透明矩形；在 RenderCommand 内封装深度测试开关与颜色/深度清除，由 Renderer 编排。保留已有 view、动态 aspect 与时间驱动旋转，所有 OpenGL 调用仍由持有 Current Context 的主线程发出。本课使用窗口默认 framebuffer 的深度缓冲，由窗口系统管理其存储生命周期；不新增 GPU 资源所有者或通用状态管理器。验收对比关闭/开启深度测试以及两种 draw 顺序，确认开启后较近矩形始终遮住较远矩形；本课同时讲解了每次 draw 前上传对应 model 的必要性。学习者已完成核心代码与最终理解验收。

第 33 课检查记录（2026-09-08）：学习者分别构造位于 `z = -2` 与 `z = -3` 的 model，以每秒 90 度和 45 度绕 Z 轴旋转；两次 draw 前分别上传 model。已修正把 `glClearDepth` 当成清除命令，以及把清除值设为 `0.0` 导致 `GL_LESS` 拒绝所有片元的两处错误。最终业务代码保留 `SetDepthTestEnabled(true)`、清除值 `1.0`、每帧一次颜色/深度联合清除与 A → B 绘制顺序。macOS Debug 构建通过且无警告；学习者确认开启深度测试后的遮挡符合预期。使用忽略的构建目录内的临时测试副本，对开启/关闭和 A → B/B → A 四种状态，在 0、0.5、1 秒及重复 0 秒执行真实 GPU 颜色/深度读回：开启时两种顺序的结果完全一致，并与关闭测试、近处 A 最后绘制的颜色参考一致；关闭时交换顺序会改变重叠区域颜色，深度保持 `1.0`；开启时重叠位置深度约为 `0.950951`。重复时间点的整帧结果一致，验证逐帧清除未残留旧深度。Shader 链接成功，测试期间无 OpenGL 错误，测试未修改业务源码。本轮未重新执行完整窗口的 resize/Esc 人工回归，课程尚未提交、推送或合并。

第 33 课最终验收记录（2026-09-08）：学习者明确解释了两种错误清除时机的后果——同一帧在 A、B 之间清深度会忘掉 A，让较远但后画的 B 覆盖重叠区域；下一帧不清深度，则物体转走后原位置仍残留上一帧的深度，继续错误地影响遮挡。最后补充实际 GlfwWindow/Renderer 集成检查：窗口逻辑尺寸为 640×480、960×480、480×640 时，framebuffer 分别为 1280×960、1920×960、960×1280，resize callback 更新的 viewport 与查询尺寸一致，各尺寸绘制成功，零尺寸帧安全跳过，RequestClose/ShouldClose 路径通过。完整 Debug 程序另行实际启动并以退出码 0 正常结束；本轮未通过自动化工具核实具体 Esc 按键事件，不把普通关闭记录成 Esc 测试。整个过程未报告 Shader 或 OpenGL 错误，代码逻辑与上一轮四组 GPU 对照时一致；验收整理仅去除源码文件尾多余空行并更新文档，没有代写核心练习。第 33 课验收通过。

第 33 课合并记录（2026-09-08）：学习者在验收后明确要求推送并开始下一课，已按本次授权提交课程成果（`3f3b847`）、推送 `codex/lesson-33-depth-testing`，并以合并提交 `114866b` 纳入 `main`。课程分支继续保留，后续进度文档与课程成果一起同步。

第 34 课启动记录（2026-09-08）：确认远端 `main` 与 `codex/lesson-33-depth-testing` 均已同步到 `ab77725`，工作区干净，目标课程分支不存在。重新只读核对 Application、Renderer、RenderCommand 与资源模块，确认状态切换由 Renderer 编排，底层命令不反向依赖应用；Shader、VAO/EBO、Texture2D 的所有权继续由原有 RAII 对象管理。稳定 `main` 从全新 `build/lesson-34-debug` 目录完成 Debug 配置、编译且无警告，随后创建 `codex/lesson-34-depth-write-mask`。当前只更新课程启动记录，未实现本课业务代码。

第 34 课核心任务（30～50 分钟）：在 RenderCommand 中增加 `SetDepthWriteEnabled(bool enabled)`，用 `glDepthMask` 控制深度写入；Renderer 保持深度测试开启、`GL_LESS`、现有 A → B 顺序及两份 model，依次对比 A/B 都允许写入、仅禁止 A 写入、仅禁止 B 写入三种配置。学习者先预测并记录重叠区域结果，再实际运行核对，理解禁止写入时仍会执行深度比较，通过测试的片元也仍可更新颜色。本课只修改 RenderCommand 的头文件/实现与 Renderer 的编排，不增加 GPU 资源类、持久 C++ 状态、依赖或混合功能；OpenGL 状态由 Current Context 持有，所有调用继续在主线程执行。

本课清除边界：`glClear` 受深度写入掩码影响，因此每帧必须先恢复深度写入再清颜色/深度，然后应用各矩形的实验配置；一帧结束时恢复允许写入。最终保留两个矩形都允许深度写入的正常配置。验收需有三组运行对照、无警告/无 OpenGL 错误的构建运行结果，并能解释关闭写入与关闭测试的不同，以及恢复写入为什么必须发生在清屏之前。练习已完成并合并，最终验证与配置见下文。

第 34 课代码检查记录（2026-09-08）：学习者已实现 `SetDepthWriteEnabled(bool)`，并修正清屏前与帧末的状态恢复。当前 Renderer 先允许写入再清屏，A 绘制前关闭写入，B 绘制前开启写入，帧末恢复允许写入；仍保留第②组实验配置。Debug 编译通过且无警告。使用构建目录内的临时副本，在 A/B 都写、仅 A 不写、仅 B 不写三种配置下，按 0、0.5、1、0 秒读取 GPU 颜色和深度：重叠处分别显示 A、B、A；第②组 A 的非重叠区域仍有颜色而深度保持清除值，第③组 B 的可见区域仍有颜色而不更新深度。每次 DrawFrame 前故意关闭写入，仍能正确清屏；帧末查询写入掩码为开启，重复时间点的结果完全一致。Shader 链接成功，无 OpenGL 错误，临时对照未修改业务源码。下一步由学习者把 A 的写入开关恢复为 `true`，保留两个矩形都写深度的正常配置，并解释禁止 B 写入为何仍能让它被 A 遮挡；本课尚未提交、推送或合并。

第 34 课最终验收记录（2026-09-08）：学习者明确解释了第③组中 B 尽管禁止写入深度，仍须与 A 已记录的深度比较，因更远而无法通过 `GL_LESS`，所以被 A 遮挡。核心接口及状态恢复由学习者实现；验收收尾仅由助手把遗留的 A 禁止写入实验开关恢复为 `true`，最终 A/B 都允许写入，保留帧首、逐物体和帧末的显式配置。最终源码与此前通过的正常配置测试副本完全一致；重新编译无警告，使用实际构建的 Renderer 执行多帧 GPU 读回与写入掩码检查，结果逐字节匹配此前正常配置，无 Shader 或 OpenGL 错误。本课未修改窗口、资源所有权或线程模型，不重复第 33 课已完成的窗口尺寸与退出回归。第 34 课验收通过。

第 34 课合并与当日收尾记录（2026-09-08）：学习者在验收后明确要求提交合并并结束当天学习。已提交课程成果（`11f7446`），以合并提交 `3fd8cd8` 纳入 `main`，同步课程分支、稳定分支与完成记录；保留 `codex/lesson-34-depth-write-mask`。提交前确认最终代码与已通过 GPU 对照的正常配置完全一致，`git diff --check` 通过。本次不启动第 35 课，不创建下一课程分支。

第 35 课启动记录（2026-09-08）：从远端更新后确认 `main` 与 `origin/main` 同为 `1cd0705`，工作区干净，第 34 课已完成合并。重新读完 README、课程路线和交接文档，并只读检查 Application、Renderer、RenderCommand 与 GPU 资源所有权：深度状态继续由 Renderer 编排，无状态 RenderCommand 负责具体 OpenGL 调用，Application 不需要修改。本机从独立 `build/lesson-35-debug` 目录完成 Windows Debug 配置、编译与启动，程序持续运行，Shader 链接成功且没有 OpenGL 错误；随后从最新 `main` 创建 `codex/lesson-35-depth-compare`。

第 35 课核心任务（35～60 分钟）：把深度测试的启用与比较函数分开，在 RenderCommand 中用项目自己的 `enum class` 表达 `Less` 和 `LessEqual`，再让两个矩形处于相同相机空间深度，交换绘制顺序对比“严格小于”和“小于等于”在相等深度时由先画还是后画者保留。深度写入保持开启，每帧仍只在所有 draw 前清一次颜色和深度；不新增资源类、依赖、混合或模板化状态系统。完成四组受控观察后恢复两个矩形的不同深度与 `Less` 正常配置，再进行构建、OpenGL 错误与原理验收。

第 35 课最终验收记录（2026-09-08）：学习者把深度测试启用与比较函数拆开，新增项目侧 `DepthCompare` 枚举，由无状态 RenderCommand 将 `LESS`、`LESSEQUAL`、`EQUAL`、`NOTEQUAL`、`GREATER` 与 `GREATEREQUAL` 映射到对应 OpenGL 深度函数；所有 `switch` 分支均显式结束，Renderer 不依赖 GLAD 或 OpenGL 枚举。实验期间让 A、B 处于相等深度并交换绘制顺序，观察确认 `LESS` 下先画者保留、`LESSEQUAL` 下后画者覆盖；学习者能够解释相等的新深度无法通过严格小于，但能通过小于等于。最终恢复 A 位于 `z=-2`、B 位于 `z=-3`、A → B 顺序及 `LESS`，并在清屏前显式开启深度写入；每帧只清一次颜色和深度。Windows Debug 编译成功且无警告，完整程序持续运行，Shader 链接成功且未报告 OpenGL 错误。公司 Mac 尚未对本课改动回归，Windows 结果不替代 macOS 验证。第 35 课已完成、提交并合并，课程分支继续保留。下一步第 36 课进行本轮深度管线架构复盘。

第 36 课启动记录（2026-09-08）：第 35 课提交 `bc88b73` 已通过合并提交 `5a12503` 进入 `main`，课程分支与稳定分支均已推送。Windows 家用电脑重新编译并持续运行合并后的 Debug 程序，Shader 链接成功且无 OpenGL 错误；随后从最新本地 `main` 创建 `codex/lesson-36-depth-pipeline-review`。本课只复盘深度清除、测试、写入、比较函数、调用顺序、主线程状态与模块依赖，不新增业务功能或依赖。

第 36 课最终验收记录（2026-09-08）：新增 `docs/MiniStudio-depth-pipeline-review.md`，梳理默认 framebuffer 深度缓冲的所有权、Current Context 中的深度状态、主线程约束，以及深度清除值、清除命令、测试开关、写入掩码和比较函数各自的职责；记录一帧内状态设置、清屏、绘制与交换缓冲的正确顺序，并总结物体之间清深度、跨帧不清深度、关闭写入时清深度和相等深度比较四个受控实验。复盘确认当前依赖保持 `Application → Renderer → RenderCommand → OpenGL`，暂不引入状态管理器、通用 `RenderDevice` 或新依赖。本课仅修改文档；第 35 课合并后的 Windows Debug 构建和运行证据继续有效，公司 Mac 仍需独立回归。第 36 课已完成并验收，等待提交、推送课程分支并合并到 `main`。

第 36 课合并状态核对（2026-09-09）：实际 Git 历史确认课程成果 `635c891` 已通过 `b05e087` 合并，随后 `7f149c3` 更新完成记录；当前本地 `main` 与 `origin/main` 均为 `7f149c3`。因此上方 2026-09-08 验收时的“等待合并”是历史状态，当前已合并。此次按学习者要求补齐整条路线的逐课教案，并同步当前进度摘要；未新增业务代码或运行验证，Windows 证据继续按原记录保留，公司 macOS 仍需独立回归。

仓库远端和 AI 约束准备已经完成，初始基线和第 1 周的四课均已合并到 `main`。

第四课已在 `codex/lesson-04-sanitizer-debugging` 完成并合并：Sanitizer 配置有效，受控的悬空指针访问已被复现、读懂并修复，普通构建和 Sanitizer 构建均运行正常。第 1 周已经完成生命周期、所有权移动、RAII 边界和 Sanitizer 故障定位四项核心练习。

第 5 课已完成 CMake 包查找与链接、GLFW 初始化、窗口创建、最小事件处理和有序清理，并已提交、推送和合并。学习者能够解释关闭标志、窗口销毁责任，以及 `find_package` 与 `target_link_libraries` 的区别。

第 6 课已完成 Context hints、OpenGL 4.1 Core Profile 创建、主线程 Context 绑定和实际属性查询，并已提交、推送和合并。学习者能够解释 hints 的生效时机、current Context 的线程含义，以及版本不可用时窗口创建失败而不会静默降级。

第 7 课已完成事件轮询、`Esc` 按键状态查询、关闭标志设置和统一清理流程，并已提交、推送和合并。学习者能够解释 `glfwPollEvents` 的位置，以及设置关闭标志并不等于立即销毁窗口。

第 8 课已完成 window size、framebuffer size、viewport 和 resize callback，并已提交、推送和合并。学习者能够解释逻辑尺寸与物理像素尺寸的区别、为什么 viewport 使用 framebuffer 尺寸，以及回调的触发条件和当前执行线程。

第 9 课已完成最小项目骨架：`main` 负责启动，`Application` 按值拥有 `GlfwWindow` 并编排流程，`GlfwWindow` 封装具体平台调用和资源释放。

第 10 课已完成独立 `ShaderProgram` 模块和受控 Shader 语法错误验收，并已提交、推送及合并回 `main`。

第 11 课已完成独立 `VertexArray` 模块、交错顶点布局和非法数量输入验收，并已提交、推送及合并回 `main`。

第 12 课已在 `codex/lesson-12-colored-triangle` 完成：新增无状态 `RenderCommand` 模块和 `GlfwWindow::Present()`，建立 `Clear → Use Program → Bind VAO → DrawTriangles → Present` 的一帧顺序。普通与 Sanitizer 构建无警告，RGB 插值三角形、背景清屏、窗口缩放和 Esc 退出均已实际验证；学习者能够解释 Program、VAO/VBO、draw call 与双缓冲呈现的关系。课程代码和里程碑记录已提交、推送并合并回 `main`。

第 13 课已在 `codex/lesson-13-renderer-boundary` 完成：新增拥有 `ShaderProgram` 和 `VertexArray` 的最小 Renderer，把 Shader 源码、顶点数据和 `Clear → Use → Bind → Draw` 流程移出 Application；Application 只拥有 Window 与 Renderer，并继续负责编排事件、绘制和双缓冲呈现。窗口初始化新增可选副屏独占全屏模式，仅在开关启用时查询显示器；副屏或视频模式不可用时会打印提示并安全退回普通窗口。普通与 Sanitizer 构建无警告，普通窗口、副屏全屏、重构后的彩色三角形和析构路径均已实际验证；学习者能够解释资源所有权、逆序析构以及 Present 保持在窗口侧的依赖原因。课程代码和里程碑记录已提交、推送并合并回 `main`。

第 14 课已在 `codex/lesson-14-raii-move-only` 完成：`ShaderProgram` 与 `VertexArray` 保持独占 OpenGL 资源所有权，禁止复制并实现 `noexcept` move 构造和 move 赋值；`Renderer` 显式遵循相同的 move-only 规则，`GlfwWindow` 因当前 Context 与 GLFW 全局生命周期约束继续保持不可移动。资源接管前会释放目标对象原有资源，接管后会清空源对象句柄，`Release()` 保持私有并将状态复位。普通与 Sanitizer 构建无警告；临时验证程序通过类型特征检查，并让真实 OpenGL Program、VAO 和 VBO 经历 move 构造、self-move 与 move 赋值，三次均成功绘制三角形且 ASan/UBSan 无报错。学习者能够解释 move 是资源句柄所有权转移而不是复制或重建 GPU 资源。课程代码和里程碑记录已提交、推送并合并回 `main`。

第 15 课已在 `codex/lesson-15-opengl-error-debug` 完成：新增不拥有资源的 `OpenGLDebug` 命名空间模块，用 `ClearErrors()` 排空当前 Context 的旧错误，用 `CheckErrors(label)` 循环读取并报告本次渲染产生的错误；`Renderer::DrawFrame()` 仅在未定义 `NDEBUG` 时执行检查，Release 构建没有每帧查询开销。课程通过一次只发生一帧的负数 draw count 故障注入稳定复现并识别 `0x501`（`GL_INVALID_VALUE`），随后移除注入；Debug、Release 与 Sanitizer 三套构建均无警告，正常渲染无 OpenGL 错误日志，一帧正常退出路径通过 ASan/UBSan。学习者能够解释检查顺序及 Release 移除检查的性能原因。课程代码和里程碑记录已提交、推送并合并回 `main`。

第 16 课已在 `codex/lesson-16-v0-1-release` 完成：CMake 项目版本明确为 0.1.0；README 补齐 Homebrew 依赖、Debug/Release/Sanitizer 可复现命令、v0.1 运行标准和最新目录；新增 `docs/MiniStudio-v0.1-lifecycle.md`，记录所有权、Context、主线程、初始化、逐帧流程、逆序析构、move-only 与错误检查边界。三种配置均从全新 Unix Makefiles 构建目录成功配置和编译且无警告；Sanitizer 版本实际显示彩色三角形，resize callback 持续收到 framebuffer 尺寸，Esc 后以退出码 0 正常结束，未出现 OpenGL、ASan 或 UBSan 错误。学习者能够独立解释 v0.1 的主要生命周期关系。课程代码和里程碑记录已提交、推送并合并回 `main`，第一个月正式结束。

第 16 课后完成了一次 Windows 兼容维护：新增平台条件 OpenGL 头文件入口和 Windows GLAD 4.1 加载，在 vcpkg manifest 中把 GLFW/GLAD 限定为 Windows 依赖；macOS 继续使用原有系统头文件与 framework。Windows Debug、Release 配置和编译通过，实际创建 OpenGL 4.1 Core Context 并成功链接 Shader。

第 17 课已在 `codex/lesson-17-indexed-drawing` 完成：`VertexArray` 新增并独占 EBO，把索引数据复制到 `GL_ELEMENT_ARRAY_BUFFER`，在释放、move 构造和 move 赋值中保持单一所有权；`RenderCommand` 改用 `glDrawElements`，Renderer 用 4 个顶点和 6 个索引绘制两个三角形组成的矩形。Windows Debug 与 Release 编译通过，Debug 实际持续运行且没有 OpenGL 错误。学习者能够解释索引数量、EBO 字节偏移，以及局部 CPU 数组在 `glBufferData` 完成后可以按作用域销毁的原因。

第 18 课已在 `codex/lesson-18-texture2d` 完成、验收并合并：新增 move-only `Texture2D`，由 Renderer 按值拥有；顶点布局扩展为 position、color、UV，片元 Shader 通过 `sampler2D` 从 0 号纹理单元采样，程序生成的 2×2 RGBA 像素经 `glTexImage2D` 上传。Windows Debug/Release 均完成干净重建并持续运行，学习者确认四色纹理矩形显示正确，且能够解释纹理对象、纹理单元、sampler 和局部像素数组生命周期的关系。验收时还定位到一次增量构建旧对象文件导致的类布局不一致；干净重建后问题消失。下一课计划对比 wrapping 与 filtering，继续使用程序生成纹理，暂不引入图片解码依赖。

第 19 课已在 `codex/lesson-19-texture-sampling` 完成、验收并合并：学习者把 UV 扩展到 `[0, 2]`，观察并解释了 `GL_REPEAT`、`GL_CLAMP_TO_EDGE` 与 `GL_MIRRORED_REPEAT` 的坐标映射，再把 UV 恢复为 `[0, 1]`，对比 `GL_NEAREST` 与 `GL_LINEAR` 的放大效果。最终状态使用 `GL_CLAMP_TO_EDGE` 和线性放大过滤，并保留最近点缩小过滤。学习者能够解释 sampler uniform 与纹理绑定中的 `0` 都指向纹理单元 0，也能区分放大过滤和缩小过滤的触发条件。课程同时移除了未使用的 Shader varying，并让 `VertexArray` 的 move 构造初始化列表与成员声明顺序一致。macOS 从全新 Debug 构建目录配置、编译成功且没有编译器警告；Shader 链接日志为空，程序成功创建 OpenGL 4.1 Core Context 并启动，未报告 OpenGL 错误。课程提交与分支均已推送，合并结果已进入稳定 `main`。下一步在第 20 课进行纹理链路架构小复盘，再说明外部图片解码依赖的用途、替代方案、平台限制与维护成本。

第 20 课已在 `codex/lesson-20-texture-pipeline-review` 完成、验收并合并：新增纹理链路架构记录，区分一次性初始化与逐帧绘制，记录 `Renderer`、CPU `ImageData`、无状态图片解码函数和 GPU `Texture2D` 的主要职责、所有权、线程与禁止依赖。学习者能够解释 `ImageData&` 是对调用方对象的可修改借用，`vector::data()` 返回受容器生命周期约束的像素指针；也理解 CPU 解码不需要 OpenGL Context，未来可以放到后台线程，而 `glTexImage2D` 上传必须留在拥有 Current Context 的线程。依赖评估选择 `stb_image`，用于仓库内受控学习素材；暂不引入异步任务、纹理缓存、热重载、HDR、完整颜色空间/MipMap 策略或跨平台 `RenderDevice`。本课没有修改业务代码或安装依赖，开始前已从最新 `main` 完成 macOS Debug 干净构建且无警告。课程提交与分支均已推送，合并结果已进入稳定 `main`。下一步在第 21 课固定并接入 `stb_image`，实现同步文件解码、RAII CPU 像素所有权、主线程 GPU 上传和缺失文件失败路径。

第 21 课已在 `codex/lesson-21-stb-image-loading` 完成、验收并合并：仓库固定 `stb_image` 到提交 `2c980bb59875b0d32144a71867fbdebb2f77cd20`，并记录来源、许可证、SHA-256 与升级流程；新增 `ImageData` 和无状态 `LoadImageRgba`，用带 `stbi_image_free` deleter 的 `std::unique_ptr` 管理解码内存，强制输出 RGBA 后复制到 `std::vector<unsigned char>`。Renderer 在拥有 Current Context 的主线程解码测试 PNG 并上传到现有 `Texture2D`，构建时把资源复制到程序目录。独立测试确认 64×64 图片得到 16384 字节，缺失文件返回失败且输出保持空状态；macOS Sanitizer 构建无警告，程序成功创建 OpenGL 4.1 Core Context、链接 Shader 并显示纹理，没有 OpenGL、ASan 或 UBSan 错误。非对称测试图的文件顶行是红绿、底行是蓝黄，实际显示为上方蓝黄、下方红绿，验证了文件行顺序与当前 UV 原点的上下差异。学习者能够解释解码临时内存、`ImageData`、GPU 纹理各自的所有者，以及 `byte_count` 作为 `[begin, begin + count)` 复制范围终点的含义。下一步在第 22 课明确图片行方向与 UV 约定，并选择统一的翻转位置。

第 22 课已在 `codex/lesson-22-texture-orientation` 完成、验收并合并：`LoadImageRgba` 在复制解码结果后显式交换上下像素行，把 CPU RGBA 数据统一为底行优先，不修改 Renderer 的 UV，也不使用 `stb_image` 的全局翻转状态。实现使用 `row * row_bytes` 定位行首，只遍历一半高度并交换完整行。独立 ASan/UBSan 测试确认 64×64、16384 字节、四角顺序和缺失文件失败路径均正确；macOS Sanitizer 构建无警告，程序成功显示上方红绿、下方蓝黄，未报告 OpenGL 或 Sanitizer 错误。学习者能够解释行偏移和只遍历一半高度的原因。下一步在第 23 课为外部纹理生成 Mipmap，并对比缩小采样效果。

第 23 课已在 `codex/lesson-23-texture-mipmaps` 完成、验收并合并：新增确定性的 2048×2048 单像素棋盘测试图，先用只含 0 级的 `GL_NEAREST` 缩小采样观察噪点，再在 `glTexImage2D` 上传第 0 级后调用 `glGenerateMipmap`，并把缩小过滤设为 `GL_LINEAR_MIPMAP_LINEAR`；放大过滤保持 `GL_LINEAR`。学习者曾把 Mipmap 枚举用于放大过滤，随后理解并修正了该状态错误。独立的隐藏 Context 测试确认 `Texture2D::Initialize` 成功且 `glGetError()` 为 `GL_NO_ERROR`；macOS Sanitizer 构建无警告，完整程序成功显示稳定灰色，没有 Shader、OpenGL、ASan 或 UBSan 错误。学习者能够解释缩小时使用多级纹理的原因，以及后续层级必须以已经上传的第 0 级像素为输入。下一步在第 24 课进行架构与初始化错误边界复盘。

第 24 课已在 `codex/lesson-24-texture-error-boundary` 完成、验收并合并：`Texture2D::Initialize` 在自身 OpenGL 调用前清除旧错误，上传和生成 Mipmap 后立即检查并标记错误；失败时复用 `Release()` 删除纹理并把句柄清零，再向 Renderer 传播 `false`。受控测试第一次用非法放大过滤稳定得到 `Texture2D::Initialize : 0x500` 和失败返回，同一对象在回滚后第二次合法初始化成功，且没有残留 OpenGL 错误。最终 macOS Sanitizer 构建无警告，完整程序持续运行且没有 Shader、OpenGL、ASan 或 UBSan 错误。学习者能够解释初始化错误不能留给首帧检查，以及空句柄对析构和重试的意义。下一步在第 25 课评估并接入矩阵工具，建立最小模型矩阵 uniform 数据链路。

第 25 课已在 `codex/lesson-25-model-matrix` 完成、验收并合并：经依赖评估和学习者确认后，CMake 接入 Homebrew GLM，Windows vcpkg manifest 同步加入 GLM；`ShaderProgram` 新增 `SetMat4`，通过 `glm::value_ptr` 把矩阵上传到当前 Program。顶点 Shader 新增 `model` uniform，Renderer 构造 `T × S` 模型矩阵，将四色矩形缩小一半并把中心移动到 `x = 0.25`。macOS Sanitizer 构建无警告，程序持续运行且没有 Shader、OpenGL、ASan 或 UBSan 错误，实际画面与预期一致。学习者能够解释列向量、齐次坐标、GLM 右乘组合、矩阵从右向左作用，以及交换平移和缩放顺序后平移量为何从 `0.25` 变成 `0.125`。下一步在第 26 课把累计帧时间传给 Renderer，并逐帧更新旋转矩阵。

第 26 课已在 `codex/lesson-26-frame-time-rotation` 完成、验收并合并：Application 使用 `steady_clock` 记录起点并在每帧把时间差转换为浮点秒数，Renderer 的 `DrawFrame(float)` 每帧构造 `T × R × S` 模型矩阵，以每秒 90 度绕 Z 轴旋转，并在 uniform 或 Debug OpenGL 检查失败时把 `false` 传播给顶层。macOS Sanitizer 构建无警告，矩形保持一半尺寸、中心固定在 `x = 0.25` 并约每 4 秒旋转一圈，持续运行没有 Shader、OpenGL、ASan 或 UBSan 错误。学习者能够解释相同累计时间产生相同角度，以及每帧固定增加角度为何会随帧率改变速度。下一步在第 27 课增加 view 矩阵并验证相机的逆变换。

第 27 课已在 `codex/lesson-27-view-matrix` 完成、验收并合并：顶点 Shader 新增独立 `view` uniform，并按 `view × model × position` 把局部坐标依次转换到世界和相机空间。Renderer 初始化时把世界位置 `(0.25, 0, 0)` 的静态相机转换为 `(-0.25, 0, 0)` 的 view 平移并上传一次；每帧 model 仍保持 `T × R × S` 动画，因此物体中心从世界 `x = 0.25` 转换到相机空间 `x = 0`，并在屏幕中心持续旋转。macOS Sanitizer 构建无警告，持续运行没有 Shader、OpenGL、ASan 或 UBSan 错误。学习者要求暂时保留新增但未使用的 `ShaderProgram::TransformObjectToWorld()`；该接口把场景变换参数放进了 Shader 资源模块，将在第 28 课架构复盘时重新评估。学习者能够解释 view 使用相机世界变换的逆。

第 28 课已在 `codex/lesson-28-transform-architecture-review` 完成、验收并合并：新增变换链路架构记录，明确 Application 产生每帧累计时间、Renderer 计算 model/view 并编排上传、ShaderProgram 独占 Program handle 并负责 uniform 上传。`model` 每帧随时间更新，静态 `view` 初始化时上传一次；相机开始移动后，view 也应按数据变化重新计算和上传。学习者删除了未使用且与 Renderer 重复的 `ShaderProgram::TransformObjectToWorld()`，并能够解释 CPU 局部矩阵通过 `SetMat4()` 被临时借用、随后由 OpenGL 复制到 Program uniform 状态。当前没有多个物体、持久可编辑变换或可移动相机，因此不新增 `Transform`、`Camera` 或矩阵管理器。macOS Sanitizer 构建无警告，程序仍显示位于屏幕中心、持续旋转的半尺寸纹理矩形，没有 Shader、OpenGL、ASan 或 UBSan 错误。下一课进入 projection 矩阵。

第 29 课已在 `codex/lesson-29-projection-matrix` 完成、验收并合并：顶点 Shader 新增 `projection` uniform，并按 `projection × view × model × position` 输出裁剪空间坐标。Renderer 初始化时用 `glm::ortho(-0.5, 0.5, -0.5, 0.5)` 创建静态正交投影并上传一次，使原本处于相机空间 `[-0.25, 0.25]` 的矩形映射到裁剪空间 `[-0.5, 0.5]`，画面宽高各变为原来的两倍。学习者能够解释 `view` 控制观察位置、`projection` 控制可见体积映射、`gl_Position` 保存裁剪空间坐标，以及正交范围恢复为 `[-1, 1]` 后画面为何恢复原大小。macOS Sanitizer 构建无警告，程序成功显示居中旋转的放大纹理矩形，没有 Shader、OpenGL、ASan 或 UBSan 错误。

第 30 课已在 `codex/lesson-30-perspective-projection` 完成、验收并合并：Renderer 用 `glm::perspective` 创建垂直视野角 45 度、初始宽高比 `1280 / 960`、near `0.1`、far `100` 的透视 projection，并把物体从 `z = 0` 移到相机前方的 `z = -2`。受控实验把物体临时移到 `z = -4`，学习者先预测并实际确认屏幕宽高约减半，随后代码恢复为 `z = -2`。学习者能够解释视野角增大后物体变小，near/far 是沿相机观察方向的正距离，在当前约定中对应相机空间 `z = -0.1` 与 `z = -100`；也能判断处在裁剪体积之外的 `z = 0` 和 `z = -101` 不可见。macOS Sanitizer 构建无警告，最终版本成功显示居中旋转的透视纹理矩形，没有 Shader、OpenGL、ASan 或 UBSan 错误。一次运行观察到实际 framebuffer 宽高比与硬编码值不同，下一课将用真实 framebuffer 尺寸更新 projection。

第 31 课已在 `codex/lesson-31-framebuffer-aspect` 完成、验收并合并：GlfwWindow 新增只读的 `GetFramebufferSize(int&, int&)`，通过引用输出当前物理像素尺寸，不暴露 GLFW handle 或保存重复状态。Application 在 `PollEvents()` 后查询尺寸，把当前循环内的宽高值传给扩展后的 `Renderer::DrawFrame()`；Renderer 对零尺寸帧返回成功并跳过绘制，随后用浮点宽高比逐帧构造和上传 perspective projection。实际把窗口明显拉宽和拉高后，viewport 与 projection 始终使用一致的 framebuffer 比例，旋转纹理没有额外拉伸或压扁。验收还删除了 resize callback 中会产生大量输出的旧尺寸日志，保留 viewport 更新。学习者能够解释 OpenGL 渲染目标使用 framebuffer 物理像素，以及查询放在事件处理之后可避免 projection 使用旧尺寸而 viewport 已更新到新尺寸。macOS Sanitizer 构建无警告，resize 期间没有 Shader、OpenGL、ASan 或 UBSan 错误。

第 32 课已在 `codex/lesson-32-projection-architecture-review` 完成、验收并合并：更新变换链路架构记录，补充 framebuffer 宽高和 projection 的产生位置、更新频率、局部生命周期及完整 `projection × view × model × position` 顺序。复盘确认当前每帧执行尺寸查询、aspect 与 perspective 计算、uniform location 查询和 projection 上传；单物体、单 draw call 场景没有性能证据支持增加缓存。学习者起初选择让 resize callback 直接更新 Renderer，随后能够解释这种方案会让 GlfwWindow 保存 Renderer 指针或引用，并隐藏 Renderer 生命周期、Current Context 和激活 Program 等前置条件。当前继续由同时拥有 Window 与 Renderer 的 Application 显式传递数据，未来只有在 Profiling 证明有需要时才考虑缓存宽高、尺寸 dirty 状态或 uniform location。

课程已按目标岗位职责扩展为 24 个月核心路线和第 25～36 个月专家能力进阶，新增 Android/OpenGL ES、Vulkan、移动端 Profiling、图片/动画/视频/3D 素材引擎、AI Tool Calling、Metal 验证和规模化架构演进。当前仅更新规划，不代表这些未来模块已经开始。

## 11. 课程路线与逐课教案入口

完整课程阶段、验收规则、求职时间线和暂缓内容见[课程路线](MiniStudio-curriculum-24-36-months.md)。每节课讲什么、做什么和怎样验收见[逐课教案](lessons/README.md)；第 37～52 课已有详细步骤，B～L 使用阶段教案编号，实际开课时再登记连续课号。当前仍只执行下一节课程，不批量实施远期任务。

| 周 | 核心目标 | 状态 |
| --- | --- | --- |
| 第 1 周 | CMake/C++20、对象生命周期、RAII、`unique_ptr`、移动语义、LLDB、Sanitizer | 已完成；四课均已验收并合并到 `main` |
| 第 2 周 | 链接 GLFW/OpenGL，创建 4.1 Core Context，事件循环和 Retina viewport | 已完成；四课均已验收并合并到 `main` |
| 第 3 周 | 项目骨架、职责解耦、Shader、VAO/VBO、彩色三角形与错误日志 | 已完成；第 9～12 课均已验收并合并到 `main` |
| 第 4 周 | 最小 RAII 封装、Debug/Release、故障定位、README 与生命周期说明 | 已完成；第 16 课及 v0.1.0 收尾已完成并合并 |
| 第 5 周 | EBO 索引绘制与纹理起步 | 第 17～20 课已完成、验收并合并 |
| 第 6 周 | 外部图片数据链路 | 第 21～24 课已完成、验收并合并 |
| 第 7 周 | 模型矩阵与坐标变换 | 第 25～28 课已完成、验收并合并 |
| 第 8 周 | 投影与裁剪空间 | 第 29～32 课已完成、验收并合并 |
| 第 9 周 | 深度测试 | 第 33～36 课均已完成、验收并合并 |
| 第 10～13 周 | 绕序与剔除、立方体、交互相机、透明基础及 v0.2 | 第 37～51 课已验收并合并到本地 main；第 52 课工程验收完成、待合并，个人复盘按要求跳过 |

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
- [docs/lessons/README.md](lessons/README.md) 是逐课备课入口，分阶段保存讲解内容、核心练习、边界与验收；近期第 37～52 课另有操作步骤、易错点和追问。它保存计划，不替代实际验收记录。
- 本文档是当前学习状态的主要记录，后续应在阶段、代码状态或下一步发生变化时同步更新。
- `graphics-rendering-learning-brief.md` 是最初的长期学习需求和协作原则来源；其中 12 个月要求已被学习者当前提出的 24～36 个月路线取代，“第一次阅读后完成的任务”和“启动语”也只属于历史启动说明。
- 如果文档之间出现冲突，依次以学习者当前明确请求、实际仓库状态、本文档的较新记录为准。
