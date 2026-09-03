# MiniStudio C++ 图形渲染学习交接文档

> 更新时间：2026-09-03
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

现实阶段目标是先达到初级图形开发或引擎向 TA 的可面试水平，再逐渐靠近高级 C++ 图形岗位。

## 3. 学习优先级

1. 现代 C++ 与工程工具
2. 图形学基础和坐标空间
3. OpenGL 小型渲染器
4. 场景、资源、动画及渲染架构
5. 多线程、异步加载和性能分析
6. Metal 或 Vulkan 中的一种现代显式图形 API
7. 跨平台抽象
8. AI 素材接入、Agent、Tool Calling 和 RAG

在第一套渲染器完成前，不同时学习多套图形 API，不提前设计跨平台 `RenderDevice`。

## 4. 主项目与版本目标

项目名：**MiniStudio**

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
| v0.9 | 抽象 RenderDevice，将核心场景迁移至 Metal 或 Vulkan |
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

边界：OpenGL 在 macOS 已被弃用，因此它只作为 v0.1～v0.8 的学习后端，不作为未来生产技术。v0.9 如果目标偏 macOS/iOS，优先 Metal；如果目标偏 Windows/Android 跨平台，再选择 Vulkan/MoltenVK。

## 7. 当前仓库状态

仓库根目录：

```text
MiniStudio/
```

Git 状态：

- 已执行 `git init`。
- 当前稳定分支为 `main`；第一课分支 `codex/lesson-01-lifetime-debug` 已提交、推送并合并，分支暂时保留。
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
├── MiniStudio-learning-handoff-2026-09-03.md
├── README.md
└── src/
    └── main.cpp
```

`.gitignore` 当前包含：

```gitignore
build/
cmake-build-*/
.idea/
.DS_Store
```

`README.md` 已补充项目目标、当前进度、构建方式、版本路线和仓库结构，作为稳定的项目入口。详细学习状态仍以本文档为准。

## 8. 已完成内容

### 工程部分

- 创建了本地 Git 仓库和最小目录结构。
- 创建了项目级 `AGENTS.md`，约束 AI 的教学方式、修改边界、Git 操作和验收要求。
- 编写了 `CMakeLists.txt`。
- 编写了项目入口 `README.md`，记录目标、构建方式和当前学习检查点。
- 项目使用 C++20，并关闭编译器私有语言扩展。
- Apple Clang/GCC 路径开启 `-Wall -Wextra -Wpedantic`。
- 已使用终端完成一次实际配置、编译和运行，构建成功且没有警告。

已验证的命令：

```bash
cmake -S . -B build \
  -G "Unix Makefiles" \
  -DCMAKE_BUILD_TYPE=Debug

cmake --build build --parallel
./build/ministudio
```

第一课验收时的运行输出：

```text
construct: stack
construct: heap
main is ending
destroy: heap
destroy: stack
```

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

当前仍处于“刚接触并建立直觉”的阶段，不应假定已经熟练掌握智能指针、移动语义或运算符重载。

## 9. 当前 `main.cpp` 的真实状态

```cpp
#include <iostream>
#include <memory>
#include <string>
#include <utility>

struct LifeTimeProbe {
    explicit LifeTimeProbe(std::string name)
        :name_(std::move(name)){
        std::cout << "construct: " << name_<<'\n';
    }

    ~LifeTimeProbe() {
        std::cout << "destroy: " << name_<<'\n';
    }

    std::string name_;
};

int main() {
    LifeTimeProbe stack_probe("stack");
    auto heap_probe = std::make_unique<LifeTimeProbe>("heap");
    std::cout << "main is ending" << std::endl;
    return 0;
}
```

代码已经通过实际配置、编译和运行检查，没有编译警告；对象命名与输出格式已修正。

## 10. 当前阶段与下一步

当前处于：**第 1 周——C++ 生命周期、RAII、智能指针与基本工具链。**

还没有开始 GLFW 或 OpenGL；不要跳到窗口和三角形。

仓库远端和 AI 约束准备已经完成，初始基线已推送到 `main`。第一课已在 `codex/lesson-01-lifetime-debug` 完成并合并：代码格式检查通过，实际编译运行无警告，学习者能够解释两个对象的构造/析构、局部变量逆序析构、`unique_ptr` 所有权和 `get()` 的非拥有语义。

第一课的 Git 收尾已经完成，课程分支保留。下一门课程尚未开始；开始时必须从最新 `main` 创建新的课程分支。当前仍应继续第 1 周的 C++ 与工具链内容，不进入 GLFW 或 OpenGL。

## 11. 前四周计划

| 周 | 核心目标 | 状态 |
| --- | --- | --- |
| 第 1 周 | CMake/C++20、对象生命周期、RAII、`unique_ptr`、移动语义、LLDB、Sanitizer | 进行中；最小构建和生命周期调试已完成，Sanitizer 尚未完成 |
| 第 2 周 | 链接 GLFW/OpenGL，创建 4.1 Core Context，事件循环和 Retina viewport | 未开始 |
| 第 3 周 | Shader 编译、VAO/VBO、彩色三角形、错误日志 | 未开始 |
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

## 13. 新任务建议启动语

将本文作为附件，然后发送：

> 请阅读这份 MiniStudio 学习交接文档。先只读检查当前 MiniStudio 仓库的状态，核对交接信息是否仍然准确，然后从第 10 节的“当前阶段与下一步”继续。每次只给我一个 20～90 分钟的核心任务；我完成后请实际检查并验收，不要提前跳到 OpenGL，也不要直接给出核心练习的完整答案。

## 14. 文档关系与使用方式

- `AGENTS.md` 是项目级 AI 执行约束，规定教学方式、修改权限、课程分支、Git 安全边界和验证要求。
- `README.md` 是项目入口，保存相对稳定的项目目标、构建命令和路线概览。
- 本文档是当前学习状态的主要记录，后续应在阶段、代码状态或下一步发生变化时同步更新。
- `graphics-rendering-learning-brief.md` 是最初的长期学习需求和协作原则来源。其中“第一次阅读后完成的任务”和“启动语”属于历史启动说明，不应在每次新任务中重复执行。
- 如果文档之间出现冲突，依次以学习者当前明确请求、实际仓库状态、本文档的较新记录为准。
