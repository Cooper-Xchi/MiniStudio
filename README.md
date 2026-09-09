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

当前里程碑版本为 **v0.1.0**。

## 当前进度

项目第 1～42 课均已通过验收并合并，第 43 课「输入快照驱动相机平移」已完成并验收、待提交合并。当前依赖形成 `Application → GlfwWindow/Camera/Renderer → RenderCommand → OpenGL`；窗口提供值类型按键快照，Application 映射移动意图，Camera 保持纯 CPU 职责。

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
- 完成第十二课的彩色三角形绘制：每帧依次清屏、绑定 Program 和 VAO、发出 draw call，再交换双缓冲呈现；实际验证了 RGB 插值、窗口缩放和 Esc 退出。
- 完成第十三课的最小 `Renderer` 边界，将 Shader 源码、顶点数据、资源所有权和绘制流程移出 `Application`，同时保持窗口呈现职责独立；窗口初始化可选在副屏独占全屏，并在副屏不可用时安全退回普通窗口。
- 完成第十四课的 RAII 与移动语义审查：`ShaderProgram`、`VertexArray` 和 `Renderer` 禁止复制并支持 `noexcept` 移动，资源句柄在所有权转移后清空源对象；实际验证了 move 构造、self-move、move 赋值以及单次释放。
- 完成第十五课的 OpenGL 错误诊断：新增无状态 `OpenGLDebug` 模块，Debug 构建在每帧 OpenGL 调用前排空旧错误并在绘制后报告本帧错误；通过一次受控的负数顶点数量注入定位到 `GL_INVALID_VALUE`，Release 构建则通过 `NDEBUG` 移除每帧检查。
- 完成第十六课的 v0.1.0 收尾：补齐生命周期说明、依赖与三种构建配置，并从全新构建目录完成 Debug、Release、Sanitizer 回归；实际验证彩色三角形、窗口缩放、Esc 正常退出和无 Sanitizer/OpenGL 错误。
- 完成 Windows OpenGL 兼容维护：Windows 通过 GLAD 加载 OpenGL 4.1 函数，macOS 继续使用系统 `OpenGL.framework`，两条平台路径由 CMake 条件隔离。
- 完成第十七课的 EBO 索引绘制：`VertexArray` 独占 VAO、VBO 和 EBO，用 4 个顶点与 6 个索引绘制矩形，并保持失败回滚、释放和移动所有权语义。
- 完成第十八课的最小 `Texture2D`：Renderer 按值拥有纹理资源，顶点布局加入 UV，片元 Shader 通过 sampler 和 0 号纹理单元采样程序生成的 2×2 RGBA 四色纹理。
- 完成第十九课的纹理采样实验：使用超出 `[0, 1]` 的 UV 对比重复与边缘钳制，并在放大场景中对比最近点和线性过滤；同时清理 Shader varying 与 C++ 成员初始化顺序警告。
- 完成第二十课的纹理链路架构复盘：区分 CPU 图片数据、无状态解码函数、GPU `Texture2D` 和 Renderer 的职责、所有权、线程与依赖方向，并选择 `stb_image` 作为下一阶段的受控图片解码依赖。
- 完成第二十一课的同步图片加载链路：固定 `stb_image` 版本，实现无状态 RGBA 解码与缺失文件失败路径，由 Renderer 在主线程上传外部 PNG，并观察图片行顺序与纹理 UV 原点造成的上下翻转。
- 完成第二十二课的图片方向约定：在解码结果中按完整行交换上下像素，避免 `stb_image` 全局翻转状态，并用非对称测试图验证屏幕方向恢复正确。
- 完成第二十三课的纹理 Mipmap：用 2048×2048 单像素棋盘对比基础级最近点缩小与三线性 Mipmap 过滤，理解 0 级上传、层级生成、缩小过滤和放大过滤的不同职责。
- 完成第二十四课的纹理链路架构与错误边界复盘：初始化阶段就检查并归因 OpenGL 错误，失败后通过 `Release()` 释放纹理并清零句柄，避免错误被首帧清除。
- 完成第二十五课的最小模型矩阵链路：接入 GLM，新增 `ShaderProgram::SetMat4`，用 `T × S` 把四色矩形缩小一半并将中心向右移动到 `x = 0.25`。
- 完成第二十六课的累计帧时间与旋转：Application 把 `steady_clock` 累计秒数传给 Renderer，Renderer 每帧上传 `T × R × S` 模型矩阵，并将绘制失败传播到顶层退出码。
- 完成第二十七课的静态 view 矩阵：用 `view × model × position` 建立局部、世界和相机空间顺序，验证相机位于 `x = 0.25` 时世界需要反向平移 `-0.25`。
- 完成第二十八课的变换链路架构复盘：记录时间、model、view 和 Shader Program 的职责、更新频率、所有权与主线程约束，并删除重复且职责错位的 `TransformObjectToWorld()`。
- 完成第二十九课的正交投影矩阵：顶点 Shader 使用 `projection × view × model × position` 输出裁剪空间坐标，并用更小的正交可见范围验证投影产生的放大效果。
- 完成第三十课的透视投影与深度：使用 45 度视野角和 near/far 裁剪距离，把物体放到 `z = -2`，并用 `z = -4` 的受控实验验证屏幕尺寸约减半。
- 完成第三十一课的动态 framebuffer 宽高比：窗口模块查询物理像素尺寸，Application 传递普通整数，Renderer 更新 projection，并安全跳过 framebuffer 为零的帧。
- 完成第三十二课的 projection 与 resize 架构复盘：记录逐帧查询、矩阵计算和 uniform 上传的成本，保持 Window、Application 与 Renderer 的单向依赖，暂不增加无性能证据的缓存状态。
- 完成第三十三课的深度测试：区分清除值与清除命令，每帧同时清颜色和深度，并用开关测试、交换绘制顺序及 GPU 读回验证两个矩形的遮挡；理解同一帧须保留物体间深度，而下一帧须清除旧深度。
- 完成第三十四课的深度写入控制：封装 `glDepthMask`，用三组配置验证禁止写入仍参与深度测试且可以输出颜色，并通过清屏前恢复写入避免上一帧状态影响深度清除；最终恢复两个矩形都写深度。
- 完成第三十五课的深度比较函数实验：深度测试启用与比较规则分别设置，项目侧枚举隔离 OpenGL 常量，并用相等深度和交换绘制顺序对比严格小于与小于等于。
- 完成第三十六课的深度管线架构复盘：记录默认 framebuffer 深度缓冲的所有权、Current Context 状态、逐帧调用顺序和受控错误，确认现有模块仍保持最小单向依赖。
- 完成第三十七课的三角形绕序与正面判定：以项目侧枚举封装 `glFrontFace`，保持剔除关闭，用 `gl_FrontFacing` 对比两种正面约定和单个三角形反转；最终恢复一致的逆时针绕序及原有旋转，保留纹理与标记色各占 50% 的输出。macOS Debug 构建与运行检查通过，Windows 尚未验证本课。
- 完成第三十八课的背面剔除实验：分别封装剔除开关与被剔除面选择，让两个矩形绕 Y 轴翻面，并对比关闭剔除、剔除背面和剔除正面；最终保存逆时针正面与背面剔除配置。macOS Debug 构建与运行检查通过，Windows 尚未验证本课。
- 完成第三十九课的索引立方体：使用 24 条完整属性记录和 36 个索引表达六个独立 UV 面，只绘制一个绕 X、Y 轴旋转的纹理立方体，并保持深度测试和背面剔除。macOS Debug 构建与运行检查通过，Windows 尚未验证本课。
- 完成第四十课的几何、状态与资源架构复盘：记录 CPU 数据复制边界、GPU 句柄所有权、Current Context 状态、成员析构顺序及面消失排查流程，确认当前无需新增通用 Mesh、状态管理器或 `RenderDevice`。
- 完成第四十一课的最小 Camera：由 Application 按值拥有纯 CPU 相机状态，以固定 forward/up 计算静态 view；Renderer 只借用矩阵并逐帧上传 uniform，静态画面保持等价。
- 完成第四十二课的帧间时间与移动积分：Application 区分累计时间与帧间秒数，Camera 按速度乘 delta 的位移沿世界轴移动；相同总时长的不同时间步得到相同位移。
- 完成第四十三课的输入快照驱动相机平移：GlfwWindow 按值返回 W/A/S/D 状态，Application 在事件轮询后把按键映射为移动方向并结合 delta 形成位移，Camera 不依赖平台输入。

前 42 课均已完成并合并回 `main`。第 43 课成果位于 `codex/lesson-43-input-snapshot`，已完成并验收但尚未提交、推送或合并；第 44 课尚未启动。

仓库使用 `main` 保存已验收的稳定基线，并通过 `origin` 同步到 GitHub。独立的仓库用 SSH 密钥已配置为可写 Deploy key。已合并的课程分支均继续保留；后续课程遵守相同的独立分支规则。项目级 AI 协作边界和课程分支规则记录在 [`AGENTS.md`](AGENTS.md)。

## 构建与运行

当前已在 macOS arm64、Apple Clang、CMake 3.25 以上、Homebrew GLFW 3.4 和 GLM 1.0.3 环境下验证。首次构建前需要安装 CMake、GLFW 和 GLM：

```bash
brew install cmake glfw glm
```

Debug 构建保留 OpenGL 错误检查：

```bash
cmake -S . -B build/debug \
  -G "Unix Makefiles" \
  -DCMAKE_BUILD_TYPE=Debug

cmake --build build/debug --parallel
./build/debug/ministudio
```

Release 构建启用优化，并通过 `NDEBUG` 移除每帧 OpenGL 错误检查：

```bash
cmake -S . -B build/release \
  -G "Unix Makefiles" \
  -DCMAKE_BUILD_TYPE=Release

cmake --build build/release --parallel
./build/release/ministudio
```

需要 AddressSanitizer 和 UndefinedBehaviorSanitizer 时，使用独立构建目录：

```bash
cmake -S . -B build/sanitize \
  -G "Unix Makefiles" \
  -DCMAKE_BUILD_TYPE=Debug \
  -DMINISTUDIO_ENABLE_SANITIZERS=ON

cmake --build build/sanitize --parallel
./build/sanitize/ministudio
```

CLion 可以使用自身提供的 Ninja，并将构建产物放在独立的 `cmake-build-*` 目录中。

### Windows 构建

Windows 使用 vcpkg 提供 GLFW 和 GLAD；GLAD 只在 Windows 上负责加载 OpenGL 4.1 函数，macOS 仍使用系统的 `OpenGL.framework`。配置时将 `VCPKG_ROOT` 替换为本机 vcpkg 路径：

```powershell
cmake -S . -B build/windows `
  -G Ninja `
  -DCMAKE_BUILD_TYPE=Debug `
  -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"

cmake --build build/windows --parallel
./build/windows/ministudio.exe
```

## v0.1 运行验收

程序启动后应显示一个带 RGB 插值颜色的三角形和蓝灰色背景。拖动窗口边缘时，viewport 应跟随实际 framebuffer 尺寸更新；按下 Esc 后程序应通过统一退出路径关闭。

Debug 和 Sanitizer 版本在正常路径下不应输出 OpenGL 错误、AddressSanitizer 错误或 UndefinedBehaviorSanitizer 错误。Release 版本应保持相同画面，但不执行每帧 `glGetError()` 检查。

对象所有权、Context、逐帧顺序和逆序析构说明见 [`docs/MiniStudio-v0.1-lifecycle.md`](docs/MiniStudio-v0.1-lifecycle.md)。

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

完整的阶段目标、验收标准和求职时间线见[课程路线](docs/MiniStudio-curriculum-24-36-months.md)。每节课的讲解内容、核心练习、边界和验收标准见[逐课教案索引](docs/lessons/README.md)，第 37～52 课另有详细步骤、易错点和追问，其中第 37～42 课已合并、第 43 课已验收、第 44～52 课待执行。24 个月是核心路线，25～36 个月是进阶路线；课程不能替代岗位要求的商业项目年限。

## 仓库结构

```text
MiniStudio/
├── AGENTS.md
├── CMakeLists.txt
├── README.md
├── vcpkg.json
├── assets/
│   └── textures/
│       └── lesson21-quadrants.png
├── docs/
│   ├── lessons/                       # 逐课索引与 A～L 阶段教案
│   ├── MiniStudio-curriculum-24-36-months.md
│   ├── MiniStudio-learning-handoff-2026-09-03.md
│   ├── MiniStudio-depth-pipeline-review.md
│   ├── MiniStudio-geometry-state-resource-review.md
│   ├── MiniStudio-texture-pipeline-review.md
│   ├── MiniStudio-transform-pipeline-review.md
│   └── MiniStudio-v0.1-lifecycle.md
├── third_party/
│   └── stb/
│       ├── README.md
│       └── stb_image.h
└── src/
    ├── main.cpp
    ├── app/
    │   ├── Application.h
    │   └── Application.cpp
    ├── image/
    │   ├── ImageData.h
    │   ├── ImageLoader.h
    │   └── ImageLoader.cpp
    ├── platform/
    │   ├── GlfwWindow.h
    │   └── GlfwWindow.cpp
    ├── opengl/
    │   └── OpenGLHeaders.h
    └── render/
        ├── ShaderProgram.h
        ├── ShaderProgram.cpp
        ├── VertexArray.h
        ├── VertexArray.cpp
        ├── RenderCommand.h
        ├── RenderCommand.cpp
        ├── OpenGLDebug.h
        ├── OpenGLDebug.cpp
        ├── Texture2D.h
        ├── Texture2D.cpp
        ├── Renderer.h
        └── Renderer.cpp
```

完整课程路线见 [`docs/MiniStudio-curriculum-24-36-months.md`](docs/MiniStudio-curriculum-24-36-months.md)，当前状态和下一步见 [`docs/MiniStudio-learning-handoff-2026-09-03.md`](docs/MiniStudio-learning-handoff-2026-09-03.md)，v0.1 生命周期说明见 [`docs/MiniStudio-v0.1-lifecycle.md`](docs/MiniStudio-v0.1-lifecycle.md)。README 只提供稳定的项目入口和概览；AI 助手在本仓库中的操作和教学边界见 [`AGENTS.md`](AGENTS.md)。
