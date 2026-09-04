# MiniStudio v0.1 生命周期说明

本文记录 MiniStudio v0.1 的对象所有权、OpenGL Context、初始化、逐帧执行和析构顺序。当前版本采用单线程模型，所有 GLFW 与 OpenGL 调用都在主线程执行。

## 所有权结构

```text
Application
├── GlfwWindow
│   └── GLFWwindow 与随窗口创建的 OpenGL Context
└── Renderer
    ├── ShaderProgram
    │   └── OpenGL Program ID
    └── VertexArray
        ├── OpenGL VAO ID
        └── OpenGL VBO ID
```

`Application` 按值拥有 `GlfwWindow` 和 `Renderer`，`Renderer` 按值拥有 `ShaderProgram` 和 `VertexArray`。`ShaderProgram` 与 `VertexArray` 分别负责释放自己拥有的 OpenGL 资源。

`RenderCommand` 和 `OpenGLDebug` 只包含无状态工具函数，不拥有需要析构的对象或 GPU 资源，因此使用 namespace，而不创建没有状态的类。

## 启动顺序

程序从 `main()` 开始，在栈上创建 `Application`，然后进入 `Application::Run()`：

1. `GlfwWindow::Initialize()` 初始化 GLFW、创建 Window 和 OpenGL Context，并把 Context 绑定为主线程的 current Context；
2. `Renderer::Initialize()` 编译 Shader、链接 Program，再创建 VAO 和 VBO、上传顶点数据并记录 attribute 布局；
3. 初始化全部成功后进入事件与渲染循环。

这个顺序不能颠倒。Shader、Program、VAO 和 VBO 都是当前 OpenGL Context 中的资源，创建它们之前必须先让有效 Context 在当前线程上 current。

如果任一步返回失败，`Application::Run()` 会返回非零退出码。随后栈上的 `Application` 自动析构，其成员再通过各自的析构函数清理已经成功创建的资源，不需要在每个返回分支重复编写完整清理流程。

## 每帧执行顺序

主循环中的顺序是：

```text
PollEvents
    ↓
检查 Esc，必要时设置关闭标志
    ↓
Renderer::DrawFrame
    ├── Debug：排空旧 OpenGL 错误
    ├── Clear
    ├── Use Program
    ├── Bind VAO
    ├── DrawTriangles
    └── Debug：读取并报告本帧错误
    ↓
Present
```

绘制命令先写入后缓冲，`Present()` 再交换前后缓冲，把完成的一帧显示到屏幕。下一帧继续写入新的后缓冲。事件、绘制、呈现和 OpenGL 错误查询目前都在主线程执行。

## 退出与析构顺序

`Application` 中先声明 `window_`，后声明 `renderer_`。C++ 按成员声明的相反顺序析构，因此退出时顺序是：

```text
VertexArray：删除 VBO 和 VAO
    ↓
ShaderProgram：删除 Program
    ↓
Renderer 析构完成
    ↓
GlfwWindow：销毁 Window 和关联的 Context
    ↓
glfwTerminate
```

Renderer 的 OpenGL 资源会在 Window 和 Context 仍然有效时释放。Window 不负责删除 Program、VAO 或 VBO；这些资源始终由各自的 RAII 对象负责。资源成员清理后会把 ID 和相关 CPU 状态复位，避免对象仍表现得像资源有效。

## move-only 资源

`ShaderProgram`、`VertexArray` 和 `Renderer` 禁止复制。直接复制 OpenGL ID 会产生两个自认为拥有同一资源的对象，最终可能重复删除同一个资源。

移动操作转移的是资源句柄的所有权，不会复制或重建 GPU 资源。move 赋值先释放目标对象原有资源，再接管源对象的 ID；源对象的 ID 随后清零，使它保持可析构但不再拥有该资源的状态。

`GlfwWindow` 当前保持不可复制、不可移动。窗口同时涉及 GLFW 全局初始化状态、OpenGL Context 的线程绑定和回调关系，在没有真实移动需求前不扩大其状态转换范围。

## Debug 错误检查

`OpenGLDebug::ClearErrors()` 在一组 OpenGL 调用前循环读取错误，直到得到 `GL_NO_ERROR`；`OpenGLDebug::CheckErrors(label)` 在调用后读取并报告新产生的错误。这样可以把错误归因到被包围的渲染阶段，而不是误报此前残留的错误。

错误状态属于当前 OpenGL Context，因此检查必须发生在 Window 创建、Context current 之后，并与被检查的 OpenGL 调用位于同一线程。

这些检查放在 `#ifndef NDEBUG` 中。Debug 构建保留诊断能力；Release 构建定义 `NDEBUG`，不执行每帧 `glGetError()`，避免不必要的运行时开销。

## 当前边界

v0.1 只使用主线程和一个 OpenGL Context。它没有后台渲染线程、跨 Context 资源共享或通用 `RenderDevice`。如果以后把 OpenGL 调用移动到其他线程，必须显式转移 Context 的 current 关系并重新审查同步与资源销毁线程。

## v0.1.0 回归证据

2026-09-04 使用 Apple Clang、CMake 3.25 以上、Homebrew GLFW 3.4 和 macOS OpenGL.framework 完成最终回归：

- 从三个全新 Unix Makefiles 构建目录分别配置并编译 Debug、Release 和 Sanitizer，三种配置均无编译警告；
- CMake 生成信息确认项目版本为 0.1.0，Release 使用优化和 `NDEBUG`；
- Sanitizer 版本实际显示 RGB 插值三角形和蓝灰色背景；
- 拖动窗口时 framebuffer resize callback 持续收到新尺寸，画面保持正常；
- 按 Esc 后程序通过统一路径以退出码 0 结束；
- 正常运行与退出过程中没有 OpenGL、AddressSanitizer 或 UndefinedBehaviorSanitizer 错误。
