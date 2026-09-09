# MiniStudio 几何、状态与资源架构复盘

> 对应课程：第 37～40 课
>
> 当前范围：单线程 OpenGL 4.1、一个索引纹理立方体、默认 framebuffer

## 1. 模块依赖与职责

```text
main
  └── Application
        ├── GlfwWindow ───────────────→ GLFW / OpenGL Context
        └── Renderer
              ├── ImageLoader ───────→ stb_image / 标准库
              ├── ShaderProgram ─────→ OpenGL
              ├── VertexArray ───────→ OpenGL
              ├── Texture2D ─────────→ OpenGL
              └── RenderCommand ─────→ OpenGL
```

`main` 只创建并运行 `Application`。`Application` 按值拥有窗口和 Renderer，编排事件、时间、framebuffer 尺寸、绘制和呈现。`GlfwWindow` 创建窗口与 OpenGL Context，并把 Context 绑定到主线程。`Renderer` 组织固定场景、矩阵、资源绑定和 draw call。资源包装类独占 OpenGL 句柄，`RenderCommand` 只把渲染意图转换成 OpenGL 状态与绘制调用。

依赖保持从高层流程指向底层实现。资源模块和 RenderCommand 不反向依赖 Application，也不需要知道窗口循环如何组织。

## 2. 数据、资源与状态生命周期

| 对象或状态 | 产生位置 | 所有者或保存位置 | CPU/GPU 边界 | 更新频率 | 结束时机 |
| --- | --- | --- | --- | --- | --- |
| 立方体顶点与索引 | `Renderer::Initialize` 的局部数组 | 初始化函数的栈作用域 | `glBufferData` 复制到 VBO/EBO | 初始化一次 | `Initialize` 返回时局部数组销毁 |
| `ImageData` | `LoadImageRgba` 填充 | `Renderer::Initialize` 的局部对象 | `glTexImage2D` 复制 RGBA 像素到纹理 | 初始化一次 | 纹理上传完成后随局部对象销毁 |
| VAO/VBO/EBO | `VertexArray::Initialize` | C++ `VertexArray` 对象保存三个句柄 | OpenGL 驱动管理实际对象存储 | 初始化一次，逐帧绑定 | `VertexArray::Release` 分别删除三个句柄 |
| Shader Program | `ShaderProgram::Initialize` | C++ `ShaderProgram` 对象 | Shader 源码交给 OpenGL 编译、链接；临时 Shader 链接后删除 | Program 初始化一次，逐帧使用 | `ShaderProgram::Release` 删除 Program |
| Texture | `Texture2D::Initialize` | C++ `Texture2D` 对象 | 像素上传后由 OpenGL 纹理存储保存 | 初始化一次，逐帧绑定 | `Texture2D::Release` 删除纹理 |
| view 矩阵 | `Renderer::Initialize` 的局部值 | 上传后保存在 Program uniform 状态 | `glUniformMatrix4fv` 复制矩阵值 | 当前静态相机只上传一次 | Program 删除或 uniform 被覆盖 |
| model 矩阵 | `CubeModel` 的局部返回值 | 当前帧的 CPU 值；上传后在 Program uniform 中 | `glUniformMatrix4fv` 复制矩阵值 | 每帧 | CPU 临时值调用结束后销毁；GPU 值下次上传时覆盖 |
| projection 矩阵 | `Renderer::DrawFrame` 的局部值 | 当前帧的 CPU 值；上传后在 Program uniform 中 | `glUniformMatrix4fv` 复制矩阵值 | 每帧按 framebuffer 宽高比计算 | CPU 局部值帧内销毁；GPU 值下次上传时覆盖 |
| 深度状态 | Renderer 编排，RenderCommand 发出调用 | 主线程的 Current Context | `glEnable`、`glDepthMask`、`glDepthFunc` 修改 Context 状态 | 当前每帧显式设置 | Context 销毁时结束，或被后续调用覆盖 |
| 正面与剔除状态 | Renderer 编排，RenderCommand 发出调用 | 主线程的 Current Context | `glFrontFace`、`glEnable(GL_CULL_FACE)`、`glCullFace` 修改 Context 状态 | 当前每帧显式设置 | Context 销毁时结束，或被后续调用覆盖 |

`glBufferData`、`glTexImage2D` 和 `glUniformMatrix4fv` 都在调用边界复制当前数据。调用结束后，OpenGL 不再借用原 CPU 数组、图片字节或局部矩阵。

VAO 记录 attribute 格式、对应的 VBO 绑定和 EBO 绑定，但不拥有原 CPU 顶点数组。OpenGL 的 VAO 也不会自动删除 VBO/EBO；当前项目由 `VertexArray` 包装类统一承担 VAO、VBO、EBO 三个句柄的释放责任。

## 3. 所有权与析构顺序

`Application` 的成员声明顺序是 `window_`、`renderer_`，C++ 按声明顺序的逆序销毁成员：

```text
Application 销毁
  1. renderer_
  2. window_
```

`Renderer` 的成员声明顺序是 `shader_program_`、`vertex_array_`、`texture_`，因此实际销毁顺序是：

```text
Renderer 销毁
  1. texture_       → 删除 Texture
  2. vertex_array_  → 删除 VAO、VBO、EBO
  3. shader_program_→ 删除 Program
```

随后 `GlfwWindow` 才销毁窗口并终止 GLFW。这样 GPU 资源释放时主线程仍有有效的 Current Context；如果窗口先销毁，后续 `glDelete*` 就失去所需的 Context 环境。

## 4. “立方体某一面消失”的排查顺序

1. 固定一个容易复现的时间或观察角度，确认消失的是整面、单个三角形，还是被其他面遮挡。
2. 暂时关闭面剔除。如果该面重新出现，优先检查绕序、正面约定或会翻转绕序的变换。
3. 确认正面约定仍为 `CounterClockwise`，剔除目标仍为 `Back`。
4. 检查该面的六个 EBO 索引，确认两个三角形引用预期的四条顶点记录，且所有索引均小于 24。
5. 对第一个三角形计算 `(p1 - p0) × (p2 - p0)`，确认结果指向立方体外侧；再核对第二个三角形方向一致。
6. 检查 model 是否包含负缩放。奇数个轴上的负缩放会镜像几何并翻转投影后的绕序；当前正值缩放和旋转不会翻转绕序。
7. 恢复背面剔除。如果关闭剔除时该面仍不正常，再检查深度：帧首是否开启深度写入并清除深度、比较函数是否为 `LESS`、是否在物体之间误清深度。
8. 必要时关闭剔除并保留 `gl_FrontFacing` 标记色，直接判断该面的两个三角形被识别为正面还是背面。

这个顺序先区分剔除与深度，再缩小到状态、索引、绕序或变换，避免通过随意修改 near/far 或关闭深度测试掩盖真正原因。

## 5. 当前架构结论

CPU 固定几何只服务当前单立方体演示，Renderer 仍能清楚表达初始化与逐帧绘制。GPU 句柄分别由 `ShaderProgram`、`VertexArray` 和 `Texture2D` 管理，Context 状态由 Renderer 显式编排、RenderCommand 无状态执行。当前没有第二种几何使用方式、多套真实状态组合或第二图形后端，因此暂不增加通用 Mesh、状态管理器或 `RenderDevice`；出现实际复用、膨胀或后端差异时再根据证据拆分。
