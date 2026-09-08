# MiniStudio 变换链路架构复盘

## 1. 当前职责与更新频率

| 数据或对象 | 产生或管理位置 | 更新时机 | 生命周期或所有者 |
| --- | --- | --- | --- |
| `elapsed_seconds` | `Application::Run()` | 主循环每帧计算 | 当前循环迭代中的局部值，按值传给 Renderer |
| framebuffer 宽高 | GlfwWindow 查询，Application 传递 | 每帧处理事件后查询 | Application 当前循环迭代中的两个局部整数 |
| `model` | `Renderer::DrawFrame()` | 每帧随累计时间更新 | `DrawFrame()` 的局部矩阵 |
| `view` | `Renderer::Initialize()` | 当前静态相机只计算一次；相机移动后按变化更新 | 初始化期间的局部矩阵 |
| `projection` | `Renderer::DrawFrame()` | 当前按每帧 framebuffer 宽高比更新 | `DrawFrame()` 的局部矩阵 |
| `ShaderProgram` / `program_id_` | Renderer / ShaderProgram | 初始化时创建，绘制时使用 | Renderer 按值拥有 ShaderProgram，ShaderProgram 独占 Program handle |

`model` 描述物体从模型空间到世界空间的变换，`view` 描述世界空间到相机空间的变换，`projection` 把相机空间映射到裁剪空间。顶点输入 `position` 是模型空间坐标；当前顶点 Shader 按 `projection × view × model × position` 计算。

## 2. 数据和依赖方向

```text
main → Application（主循环与时间）
     → Renderer（变换计算与绘制编排）
     → ShaderProgram（Program 生命周期与 uniform 上传）
     → OpenGL Program uniform
```

`SetMat4()` 通过 `const glm::mat4&` 临时借用 CPU 局部矩阵。`glUniformMatrix4fv()` 在调用期间读取并复制 16 个浮点数到当前 Program 的 uniform 状态，因此局部矩阵在函数结束时可以销毁。uniform 值会保持到再次上传、Program 重新链接或 Program 删除。

当前窗口、计时、矩阵计算、uniform 上传、绘制和 GPU 资源释放都发生在持有 Current OpenGL Context 的主线程。矩阵本身不拥有 GPU 资源；ShaderProgram 负责通过 RAII 释放自己的 Program handle。

## 3. 第 28 课设计决定

删除未使用的 `ShaderProgram::TransformObjectToWorld()`。该函数硬编码物体的位置、旋转和缩放，不读取 Program 状态，也不执行 Shader 操作，并与 `Renderer::DrawFrame()` 中根据时间计算的真实 model 矩阵重复。

当前继续由 Renderer 直接构造少量矩阵。项目还没有多个物体、可编辑的持久变换或可移动相机，因此暂不新增 `Transform`、`Camera`、矩阵管理器或通用渲染接口；出现对应真实状态和复用需求时再提取职责。

`ShaderProgram::SetMat4()` 保持最小上传接口。调用方必须先激活对应 Program；每帧查询 uniform location 的成本在当前单物体场景中可以接受，后续只有性能数据或更多绘制对象证明有需要时才缓存 location。

## 4. 第 32 课 projection 与 resize 复盘

当前每帧在 `PollEvents()` 后执行以下数据流：

```text
GlfwWindow 查询 framebuffer 物理像素宽高
→ Application 按值传递时间、宽和高
→ Renderer 检查零尺寸并计算 aspect 与 perspective projection
→ ShaderProgram 查询 uniform location 并复制矩阵到 Program uniform
```

这条路径每帧包含一次 GLFW 尺寸查询、一次 perspective 矩阵计算、一次 uniform location 查询和一次 uniform 上传。当前场景只有一个物体和一次 draw call，没有 Profiling 数据表明这些操作构成瓶颈，因此保留这条简单、显式并且已经通过 resize 验证的路径。

不让 framebuffer resize callback 直接调用 Renderer。这样做会让平台窗口模块保存 Renderer 指针或引用，并把 Renderer 生命周期、Current Context 和当前激活 Program 等前置条件隐藏在 callback 中。Application 同时拥有 Window 和 Renderer，继续由它在主循环中编排两者能够保持单向依赖和清晰顺序。

如果后续性能数据证明这里有明显成本，可让 Renderer 缓存上一次宽高，或让窗口只记录尺寸变化状态，再由 Application 显式触发 projection 更新；也可以在 ShaderProgram 内缓存已验证的 uniform location。没有数据前不增加这些持久状态。
