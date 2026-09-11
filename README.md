# MiniStudio

现代 C++ 与实时渲染学习项目。当前使用 OpenGL 4.1、GLFW、GLM；先完成第一套桌面渲染器，再按课程进入移动端和其他后端。

- 版本：v0.2.0。第 1～56 课已完成；下一课是第 57 课（B04），网格与光照数据边界复盘。
- 学什么、下一课做什么：只看 [逐课任务清单](docs/lessons.md)。
- AI 协作规则：[AGENTS.md](AGENTS.md)。详细旧记录保留在 Git 历史，不再维护多份交接、复盘或路线文档。

## 运行效果与操作

当前显示由一盏方向光照亮的旋转立方体和红、蓝两层半透明平面，背景为黑色。W/A/S/D 移动，按住左 Shift 两倍加速，左键捕获鼠标并转头，右键释放，Esc 退出；窗口缩放会更新 viewport 和投影比例。

[v0.2 短演示](docs/media/ministudio-v02-demo.gif)来自真实 GPU 帧读回，使用预设相机轨迹，不是鼠标交互录屏。

## 构建

要求 CMake 3.25+、C++20 和可用的 OpenGL 4.1 图形环境。公司电脑为 macOS，家里电脑为 Windows，两边的工具链、依赖安装和构建目录必须分开。

### macOS

使用 Homebrew 的 GLFW、GLM 和系统 OpenGL.framework，不安装 Windows GLAD。

```bash
brew install cmake glfw glm
cmake -S . -B build/macos-debug -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug
cmake --build build/macos-debug --parallel
./build/macos-debug/ministudio
```

### Windows

在已加载 Visual Studio x64 开发环境的终端执行；CMake 和 Ninja 需在 PATH 中，也可使用 CLion 配置的工具。`VCPKG_ROOT` 指向本机 vcpkg，仓库 manifest 提供 GLFW、GLM 和仅 Windows 使用的 GLAD。

```powershell
cmake -S . -B build/windows-debug -G Ninja `
  -DCMAKE_BUILD_TYPE=Debug `
  -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
cmake --build build/windows-debug --parallel
./build/windows-debug/ministudio.exe
```

若提示缺少 GLM，先核对当前构建目录使用的 manifest 依赖与工具链，不要把旧的全局 vcpkg include 路径硬编码到项目或 macOS 配置中。

### Release、测试与 Sanitizer

- Release：另用构建目录，配置时设 `-DCMAKE_BUILD_TYPE=Release`。
- 集成测试默认关闭；配置时增加 `-DMINISTUDIO_BUILD_TESTS=ON`，构建后执行 `ctest --test-dir <构建目录> --output-on-failure`。测试创建真实 OpenGL Context，需要图形桌面和驱动；不等同于纯 headless CI。
- Clang/GNU 下可在独立 Debug 目录增加 `-DMINISTUDIO_ENABLE_SANITIZERS=ON`，启用 ASan/UBSan；当前项目不在 MSVC 下启用这组选项。
- 资源会复制到可执行文件目录；从仓库根目录或可执行文件目录运行。Debug 保留逐帧 OpenGL 错误检查，Release 移除该检查。

## 当前验证状态

2026-09-10 的 v0.2：家用 Windows MSVC x64 全新 Debug、Release 和默认关闭测试的构建均通过，未输出编译器警告；Debug/Release CTest 均 1/1 通过。覆盖相机数值、资源移动/释放、透明排序颜色与深度、状态恢复、resize 和关闭标志；故意反转排序后测试能检出错误，恢复后通过。Alpha 裁剪已从当前默认场景替换为半透明混合，回归程序仍用独立的受控 Shader 验证 alpha 为零与 `discard` 的颜色和深度差异。

2026-09-11 在公司 macOS arm64 使用 AppleClang 21 完成全新 Debug 配置与编译，未输出编译器警告；CTest 1/1 通过。完整程序创建 OpenGL 4.1 Core Context、Shader 链接成功，实际画面正常，Esc 退出码为 0。本轮未完整复测 W/A/S/D 与鼠标交互，也未运行 Sanitizer。第 52 课的测试和整理由助手完成，个人季度复盘按要求跳过。

2026-09-11 第 53 课将立方体和透明平面的 CPU 数据整理为带类型顶点的 `MeshData`，由无状态 primitive 工厂生成并交给 `VertexArray` 上传；macOS Debug 编译无警告，CPU 数据契约、非法索引、资源移动／释放及完整 OpenGL 画面回归 CTest 1/1 通过。Windows 尚未验证本课改动。

2026-09-11 第 54 课为内置网格补充逐面法线及对应顶点属性，以物体空间法线颜色显示立方体方向，同时保留透明平面的原有混合路径；macOS Debug 编译无警告，法线数据契约和完整 OpenGL 回归 CTest 1/1 通过，实际画面符合预期。Windows 尚未验证本课改动。

2026-09-11 第 55 课将场景 GLSL 迁入 `assets/shaders`，由无状态文本加载器按路径同步读取，失败时保留调用方旧值；GLSL 文件可由 IDE 插件独立编辑。macOS Debug 构建和完整 OpenGL 回归 CTest 1/1 通过，包含复制后 Shader 的读取、编译、链接及缺失文件路径；Windows 尚未验证本课改动。

2026-09-11 第 56 课在世界空间实现固定方向光的 Lambert 漫反射，立方体使用白色材质，透明平面保留原有纹理与混合路径，并将背景调整为黑色以增强明暗对比；增量构建会重新复制外部 Shader。macOS Debug 编译无警告，uniform 上传、漫反射截断、世界法线变换及完整 OpenGL 回归 CTest 1/1 通过；Windows 尚未验证本课改动。

## 代码位置

`src/app` 编排主循环，`src/platform` 管窗口和输入，`src/camera` 保存纯 CPU 相机状态，`src/mesh` 保存 CPU 网格数据并生成内置 primitive，`src/io` 负责无状态文本读取，`src/render` 管绘制及 GPU RAII 资源，`src/image` 负责无状态解码；GLSL 位于 `assets/shaders`，`tests/V02Regression.cpp` 是可选集成回归。

Application 按值拥有窗口、Renderer 和 Camera；Renderer 的 GPU 资源必须在窗口和 Context 之前销毁。GLFW/OpenGL 调用及资源释放均在持有 Current Context 的主线程。
