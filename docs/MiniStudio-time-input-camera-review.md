# MiniStudio 时间、输入和相机依赖复盘

> 对应课程：第 41～44 课
>
> 当前范围：单线程 GLFW/OpenGL 主循环、W/A/S/D 状态输入、纯 CPU Camera

## 1. 一帧的数据流

```text
1. PollEvents / 输入采集
   OS 事件 → GLFW 内部按键状态 → MovementKeyState 值快照
                                      │
2. 时间与移动意图生成                 │
   steady_clock → elapsed_seconds     │
                → delta_seconds       │
   MovementKeyState → movement direction
   direction × camera_speed × delta_seconds → displacement
                                      │
3. Camera 状态推进                    ▼
   position += displacement → 新的 Camera position
                                      │
4. view 上传与绘制                    ▼
   position / forward / up → view → Renderer → Program uniform → draw
                                      │
5. Present                            ▼
   默认 framebuffer → 交换前后缓冲 → 屏幕
```

`Application` 依次编排这五步。输入必须在本帧 `PollEvents()` 后读取；Camera 必须先更新 position，再计算本帧 view；Renderer 在 draw 前把 view 复制到 Program uniform；`Present()` 最后显示本帧结果。

## 2. 数据的所有权、单位与生命周期

| 数据 | 生产者 | 所有者或保存位置 | 单位／含义 | 生命周期 | 线程 | 复制边界 |
| --- | --- | --- | --- | --- | --- | --- |
| GLFW 内部按键状态 | 操作系统事件经 `glfwPollEvents` 更新 | GLFW 与对应窗口内部状态 | W/A/S/D 的按下或释放 | 窗口存在期间持续更新 | 主线程 | `glfwGetKey` 读取当前缓存状态 |
| `MovementKeyState` | `GlfwWindow::GetMovementKeyState` | `Application::Run` 当前循环的局部值 | 四个布尔按键状态 | 当前帧 | 主线程 | 函数按值返回，Application 获得独立快照 |
| `start_time` | `steady_clock::now` | `Application::Run` 局部值 | 单调时钟时间点 | 整个运行循环 | 主线程 | 初始化一次，不跨模块传递 |
| `previous_time` | 首次取 `start_time`，之后取上一帧 `current_time` | `Application::Run` 局部值 | 上一帧采样时间点 | 整个运行循环，每帧覆盖 | 主线程 | `time_point` 值赋值 |
| `current_time` | 每帧一次 `steady_clock::now` | 当前循环局部值 | 当前帧采样时间点 | 当前帧 | 主线程 | 同一次采样同时参与两个 duration 计算 |
| `elapsed_seconds` | `current_time - start_time` | 当前循环局部值 | 程序累计秒数 | 当前帧 | 主线程 | 以 `float` 传给 Renderer，驱动物体旋转 |
| `delta_seconds` | `current_time - previous_time` | 当前循环局部值 | 上一帧到当前帧的秒数 | 当前帧 | 主线程 | 只参与本帧移动积分 |
| movement direction | Application 根据快照生成 | 当前循环局部 `glm::vec3` | 无量纲世界方向 | 当前帧 | 主线程 | 不跨模块保存 |
| displacement | direction × 速度 × delta | 表达式临时值，随后传给 Camera | 世界单位 | 一次 `Camera::Move` 调用 | 主线程 | 以常量引用借用，Camera 立即累加数值 |
| Camera position | `SetPosition` 初始化、`Move` 累加 | Application 按值拥有的 `Camera` 成员 | 世界坐标 | Application 生命周期 | 主线程 | Camera 内部持久状态，不暴露可变引用 |
| view | `Camera::ViewMatrix` | 当前循环局部 `glm::mat4` | 世界空间到相机空间的变换 | 当前帧 | 主线程 | 以常量引用借给 Renderer |
| Program 的 view uniform | `ShaderProgram::SetMat4` 调用 `glUniformMatrix4fv` | Current Context 中的 OpenGL Program 状态 | 顶点 Shader 使用的 4×4 矩阵 | 被下次上传覆盖或 Program 删除 | 主线程 | OpenGL 在调用边界复制 16 个矩阵分量 |

`time_point` 表示时钟上的一个点，两个时间点相减得到 duration，duration 转换并调用 `.count()` 后才得到以秒表示的 `float`。`previous_time` 因此只能接收另一个时间点，不能接收 `delta_seconds`。

## 3. 输入更新顺序实验

GLFW 保存最近一次已经处理的按键状态。正确顺序是先调用 `PollEvents()`，再通过 `GetMovementKeyState()` 复制本帧快照。

受控实验可以在一个临时版本中分别读取轮询前后的快照，并只在二者不同时打印结果：

```text
keys_before = GetMovementKeyState()
PollEvents()
keys_after  = GetMovementKeyState()
```

在按下或松开的边界帧，`keys_before` 可能仍是上一次事件处理后的缓存值，`keys_after` 才包含本轮事件。若正式移动使用轮询前的值，开始和停止都可能晚一帧，表现为轻微输入延迟。当前源码已恢复为 `PollEvents()` 后读取一次快照，没有保留临时日志。

`MovementKeyState` 按值返回后由当前循环局部变量拥有，四个布尔值不会继续引用 GLFW 内存。把局部快照的地址或引用保存到下一帧会越过对象生命周期；即使另外复制并长期保存，它表达的也只是过去某一帧的状态，不能代替下一次输入采集。

## 4. Camera 的独立验证边界

Camera 只依赖 GLM，保存 position、forward 和 up，并提供 `SetPosition`、`Move` 与 `ViewMatrix`。它不包含 GLFW/OpenGL 头文件，不读取按键，不调用 Shader Program，也不拥有 GPU 句柄。因此测试 Camera 时只需：

1. 设置一个已知 position；
2. 传入固定 displacement；
3. 检查移动前后的 view 矩阵。

这个实验不需要窗口、OpenGL Context 或 Renderer。以固定朝向 `-Z` 和 up `+Y` 为例，相机从 `x = 0.25` 沿 `+X` 移动 `0.5` 后到达 `x = 0.75`，view 的 X 平移分量相应从 `-0.25` 变为 `-0.75`。

## 5. 当前架构结论

GlfwWindow 负责处理平台事件并生成不暴露 GLFW 句柄的值快照；Application 负责帧时间、按键到移动方向的映射、速度和逐帧调用顺序；Camera 负责纯 CPU 相机状态及 view 计算；Renderer 负责 GPU 资源、uniform 上传与绘制；`main` 只负责创建和运行 Application。

当前只有一个窗口、一个主循环、四个连续按键状态和一个 Camera，职责与依赖方向清楚。现阶段不增加全局 Time、Input Manager、事件总线或后台输入线程；当出现多个独立消费者、可配置映射、输入设备组合或真实异步需求时，再根据具体重复和耦合决定拆分。
