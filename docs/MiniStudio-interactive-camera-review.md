# MiniStudio 交互相机回归与架构复盘

> 对应课程：第 45～48 课
>
> 实测日期：2026-09-10
>
> 实测平台：公司 macOS arm64，Apple Clang 21，OpenGL 4.1 Core

## 1. 交互回归记录

本轮先在全新的 `build/lesson-48-macos-debug` 完成 Debug 配置和编译，再启动完整程序。自动化脚本通过 macOS Accessibility 与 Core Graphics 向真实 MiniStudio 窗口发送输入、改变窗口状态并截图；脚本和图片只保存在被 Git 忽略的构建目录，不属于产品代码。

| 操作 | 可复现步骤 | 预期结果 | 实际观察 | 结果与证据 |
| --- | --- | --- | --- | --- |
| 正常移动 | 激活窗口，按住 W `0.8s` 后松开，再按住 S `0.8s` 后松开；同时按住 A+D `0.7s` 后一起松开 | W/S 沿当前 forward 前进和后退；松键立即停止；A+D 抵消且不产生 NaN | W 与 S 均改变相机位置，反向输入让视点回到接近原位置；A+D 期间画面继续正常更新，松键后无持续横移或非法画面 | 通过；`lesson48-before-input.png`、`lesson48-after-w.png`、`lesson48-after-ws-roundtrip.png`、`lesson48-after-opposites.png` |
| 转头 | 左键捕获鼠标，分八步发送总计 `(+180, -70)` 像素位移，随后右键释放 | 视角按 yaw/pitch 约定变化；释放后鼠标恢复；矩阵保持有效 | 窗口保持响应，观察方向发生变化，画面没有翻转、消失或出现非法矩阵；右键后可继续执行后续窗口操作 | 通过；`lesson48-after-turn.png`；第 46 课固定角度实验已覆盖 pitch 的 `[-89°, 89°]` 限制 |
| 失焦／重新获焦 | 捕获鼠标，把 Finder 设为前台，确认 MiniStudio 标题栏变为非活动状态；失焦期间移动鼠标 `(-300, +220)`，再把 MiniStudio 设为前台 | 失焦时清除鼠标采样基准；重新获焦的第一帧只建立新基准，不把失焦期间位移算成视角增量 | 失焦和重新获焦都成功，期间程序继续运行；重新获焦首帧画面有效，没有明显视角突跳 | 通过；`lesson48-unfocused.png`、`lesson48-refocused-first-frame.png` |
| resize | 通过 Accessibility 把窗口从 `1280×992` 调整到 `920×700`，等待绘制后恢复原尺寸 | framebuffer 回调更新 viewport；projection 使用新 framebuffer 宽高比；恢复后继续显示 | 缩放后内容完整、比例正常，恢复尺寸后继续绘制，没有 OpenGL 错误 | 通过；窗口尺寸修改返回成功，`lesson48-resized.png`、`lesson48-restored-size.png` |
| 最小化／恢复 | 设置窗口最小化，保持 `1s`，再取消最小化并重新激活 | framebuffer 不可绘制时跳过本帧；恢复后重新取得有效尺寸并继续绘制 | 最小化和恢复操作都返回成功；恢复后画面完整，进程未退出，运行日志没有 OpenGL 错误 | 通过；`lesson48-restored-from-minimize.png`；本轮没有额外记录最小化期间的具体 framebuffer 数值 |
| 关闭 | 向 MiniStudio 进程发送 Esc 按下与释放 | `RequestClose()` 设置关闭标志，当前循环结束并返回 0 | 程序自然结束，退出码为 0 | 通过；程序会话退出状态 |

Windows 本轮未执行，因此 Windows 六项结果均记录为**未验证**，不能由 macOS 结果推断为通过。

## 2. 失焦与最小化

失焦表示窗口不再接收当前用户的键盘和鼠标操作。窗口仍可见，framebuffer 也可能保持非零尺寸，渲染循环可以继续。当前 `ReadMouseInput()` 在检测到 `GLFW_FOCUSED == GLFW_FALSE` 时清除 `has_cursor_baseline_` 并返回零位移；重新获焦后的第一次采样只保存当前光标位置，避免把焦点切换期间的移动解释为一次巨大转头。

最小化表示窗口不再以正常表面显示。它可能让 framebuffer 暂时变成 `0×0`，因此 `Renderer::DrawFrame()` 在宽或高小于等于零时直接返回成功，既不计算宽高比，也不发出本帧 OpenGL 绘制命令。恢复后 Application 每帧重新查询 framebuffer 尺寸，不依赖过期缓存。

两者必须分别测试，因为它们保护不同边界：失焦主要验证输入基准和按键状态，最小化主要验证零尺寸 framebuffer 和恢复绘制。一次最小化可能伴随失焦，但不能据此认为两个路径是同一种状态。

## 3. 模块职责与资源所有权

```text
main
  └── Application
        ├── GlfwWindow ───────────────→ GLFW / Window / OpenGL Context
        ├── Camera ───────────────────→ GLM
        └── Renderer
              ├── ImageLoader ───────→ stb_image / 标准库
              ├── ShaderProgram ─────→ OpenGL Program
              ├── VertexArray ───────→ OpenGL VAO / VBO / EBO
              ├── Texture2D ─────────→ OpenGL Texture
              └── RenderCommand ─────→ OpenGL Context 状态与 draw call
```

| 模块 | 单一主要职责 | 拥有的状态或资源 | 依赖方向与销毁时机 |
| --- | --- | --- | --- |
| `main` | 创建 Application、调用 `Run()` 并返回退出码 | 栈上的 `Application` | 只依赖 Application；`main` 返回前自动销毁 Application |
| `Application` | 组装模块并编排初始化和每帧执行顺序 | 按值拥有 `GlfwWindow`、`Renderer`、`Camera`；帧时间和输入意图是 `Run()` 的局部值 | 高层依赖三个模块；成员在 Application 析构时按声明顺序的逆序销毁 |
| `GlfwWindow` | 管理 GLFW、窗口与 Context，并提供不暴露句柄的输入和 framebuffer 接口 | `GLFWwindow*`、GLFW 初始化标志、鼠标前一位置与基准有效标志 | 依赖 GLFW/OpenGL；析构时先销毁窗口，再终止由它初始化的 GLFW |
| `Camera` | 保存和推进纯 CPU 相机状态，计算 view 与局部方向 | position、forward、up、yaw、pitch | 只依赖 GLM；不拥有 GLFW、窗口、Context、Shader 或 GPU 句柄 |
| `Renderer` | 初始化场景资源、计算投影和 model、上传 uniform 并发出一帧绘制 | 按值拥有 `ShaderProgram`、`VertexArray`、`Texture2D` | 依赖资源包装类与 RenderCommand；在窗口和 Context 销毁前析构 |
| `RenderCommand` | 把明确的状态和绘制意图转换成 OpenGL 调用 | 无对象实例、无持久资源 | 单向依赖 OpenGL；不会决定窗口循环、Camera 或资源所有权 |

所有初始化、事件处理、相机更新、OpenGL 调用和资源销毁目前都在主线程执行，没有跨线程共享状态或数据竞争。

## 4. 一帧的顺序与析构顺序

```text
PollEvents
  → 检查 Esc / 关闭请求
  → steady_clock 采样 elapsed 与 delta
  → 读取本帧按键和鼠标值快照
  → 根据鼠标按钮切换捕获状态
  → 用鼠标 delta 更新 Camera yaw / pitch / forward
  → 从更新后的 forward / right 组合并归一化移动方向
  → direction × camera_speed × delta_seconds
  → Camera::Move
  → 查询当前 framebuffer 尺寸
  → Camera::ViewMatrix
  → Renderer::DrawFrame
  → GlfwWindow::Present
```

输入必须在 `PollEvents()` 后读取；Camera 必须在生成 view 前完成旋转和位移；Renderer 必须在 Context 仍有效时绘制；`Present()` 最后把本帧默认 framebuffer 交换到屏幕。

`Application` 成员声明为 `window_`、`renderer_`、`camera_`。C++ 按成员声明顺序的逆序销毁：

```text
Application 析构
  1. camera_    → 只释放 CPU 状态
  2. renderer_  → texture_ → vertex_array_ → shader_program_
  3. window_    → glfwDestroyWindow → glfwTerminate
```

因此 `Texture2D`、`VertexArray` 和 `ShaderProgram` 的析构函数执行 `glDelete*` 时，`GlfwWindow` 及其 Current Context 仍然存在。窗口销毁后才终止 GLFW，GPU 资源生命周期没有越过 Context 生命周期。

## 5. 复盘结论

- 本轮 macOS 回归没有发现卡键、重新获焦首帧猛转、非法相机矩阵、resize 投影异常、最小化恢复失败或关闭失败。
- 当前不需要业务代码修复。最小化期间 framebuffer 的具体数值没有单独记录，但零尺寸保护已经存在，最小化与恢复实测没有产生 OpenGL 错误。
- Application 的帧循环仍只负责跨模块编排；GlfwWindow、Camera 和 Renderer 的职责与资源边界清楚，依赖保持从高层流程指向底层实现。
- 当前只有一个窗口、一个 Camera 和固定按键映射，没有证据支持新增 Input Manager、事件总线、多相机控制器、Singleton 或通用 `RenderDevice`。
- Windows 状态：未验证。
