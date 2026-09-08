# MiniStudio 深度管线架构复盘

> 对应课程：第 33～36 课  
> 当前范围：默认 framebuffer、两个不透明矩形、单线程 OpenGL 4.1 渲染

## 1. 资源、状态与线程

当前程序使用窗口的默认 framebuffer。颜色缓冲和深度缓冲由 GLFW 创建窗口及其 OpenGL Context 时建立，存储不由 `Renderer` 或 `RenderCommand` 分配；窗口销毁、Context 失效时，它们随之结束生命周期。

深度测试开关、深度写入掩码和深度比较函数都是 Current Context 中的 OpenGL 状态。`RenderCommand` 不拥有这些状态，也不保存一份重复的 C++ 状态；它只是把 Renderer 的意图转换成 `glEnable`、`glDepthMask`、`glDepthFunc` 和 `glClear` 等调用。

当前所有窗口事件、Renderer 编排和 OpenGL 调用都在主线程执行。调用 OpenGL API 前，该线程必须绑定有效的 Current Context，否则调用没有正确的状态与资源环境可操作。

## 2. 深度概念的职责

| 概念 | 当前接口/API | 作用 |
| --- | --- | --- |
| 深度清除值 | `glClearDepth(1.0)` | 设置下一次清除深度缓冲时写入的值；它本身不执行清除 |
| 深度清除命令 | `glClear(GL_DEPTH_BUFFER_BIT)` | 把当前深度清除值写入深度缓冲；当前项目与颜色缓冲一起清除 |
| 深度测试开关 | `SetDepthTestEnabled` | 决定片元是否与深度缓冲中的已有值比较 |
| 深度写入开关 | `SetDepthWriteEnabled` | 决定通过深度测试的片元是否更新深度缓冲；关闭写入不等于关闭测试或颜色输出 |
| 深度比较函数 | `SetDepthCompare` | 决定新片元深度和已有深度满足什么关系时通过测试，例如严格小于或小于等于 |

## 3. 一帧的正确顺序

Renderer 先检查 framebuffer 宽高；尺寸为零时安全跳过本帧。Debug 构建随后清理旧 OpenGL 错误，开启深度测试，恢复深度写入，设置比较函数，再清除颜色和深度。之后激活 Shader Program、上传 projection 和 A 的 model，绑定 VAO 与纹理并绘制 A；再上传 B 的 model 并绘制 B。帧末检查本帧 OpenGL 错误，Application 再交换缓冲呈现。

深度必须在所有物体绘制之前清除一次。在 A、B 之间清除会忘掉 A 已写入的深度，使 B 无法与 A 比较。下一帧不清除则会保留旧画面的深度，让已经移动走的物体继续遮挡新帧。`glClear` 会受到深度写入掩码影响，因此即使上一帧正常结束时已经恢复写入，本帧仍在清屏边界显式调用 `SetDepthWriteEnabled(true)`，避免未来提前返回或新增状态切换破坏前置条件。

## 4. 受控错误与结果

- 在 A、B 之间清深度：B 只与清除值比较，较远的 B 也可能覆盖 A 的重叠区域。
- 下一帧开始时不清深度：旧深度残留，产生跨帧遮挡；物体转走后原位置仍可能拒绝新片元。
- 深度写入关闭时清深度：深度缓冲可能没有被更新为清除值，随后绘制继续受到旧深度影响。
- 相等深度使用 `LESS`：第二个片元的深度不严格小于已有值，测试失败，因此先画者保留。
- 相等深度使用 `LESSEQUAL`：第二个片元的深度等于已有值也能通过，因此后画者覆盖颜色，并再次写入相同深度。

最终稳定配置让 A 位于 `z=-2`、B 位于 `z=-3`，按 A → B 绘制，开启深度测试与写入，并使用 `DepthCompare::LESS`。较近的 A 会在重叠区域遮挡较远的 B，结果不依赖 B 随后绘制。

## 5. 模块边界

当前依赖方向为：

```text
Application → Renderer → RenderCommand → OpenGL
```

Application 只传递累计时间和 framebuffer 尺寸；Renderer 计算矩阵并编排一帧所需状态；无状态 RenderCommand 执行具体 OpenGL 命令。Renderer 不直接包含 GLAD，因为 GLAD 是 Windows 的函数加载实现，macOS 使用系统 `OpenGL/gl3.h`；平台差异集中在共享 OpenGL 头文件入口和构建配置中，避免 Windows 修改破坏公司 Mac。

当前只有一个 Renderer、一个 Context 和少量固定 draw call，没有证据需要缓存整套深度状态，也没有需要独立所有权管理的新 GPU 资源。因此暂不增加 `DepthStateManager`、深度资源类或通用 `RenderDevice`。只有未来出现离屏 framebuffer、自有深度纹理、多套真实渲染状态或第二图形后端时，才根据实际重复和生命周期重新评估抽象。

## 6. 验证边界

第 35 课最终代码已在家用 Windows 电脑完成 Debug 编译和持续运行，Shader 链接成功且没有 OpenGL 错误。公司 Mac 尚未对本课改动回归；Windows 结果不能替代 macOS 验证，后续在公司电脑更新仓库时应单独执行 Mac 构建与运行检查。
