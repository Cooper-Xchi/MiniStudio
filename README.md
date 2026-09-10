# MiniStudio

现代 C++ 与实时渲染学习项目。当前使用 OpenGL 4.1、GLFW、GLM；先完成第一套桌面渲染器，再按课程进入移动端和其他后端。

- 版本：v0.2.0。第 1～52 课已完成并合并；下一课是第 53 课（B01），尚未开始。
- 学什么、下一课做什么：只看 [逐课任务清单](docs/lessons.md)。
- AI 协作规则：[AGENTS.md](AGENTS.md)。详细旧记录保留在 Git 历史，不再维护多份交接、复盘或路线文档。

## 运行效果与操作

当前显示旋转的纹理立方体和红、蓝两层半透明平面，背景为蓝灰色。W/A/S/D 移动，左键捕获鼠标并转头，右键释放，Esc 退出；窗口缩放会更新 viewport 和投影比例。

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

2026-09-10 的 v0.2：家用 Windows MSVC x64 全新 Debug、Release 和默认关闭测试的构建均通过，未输出编译器警告；Debug/Release CTest 均 1/1 通过。覆盖相机数值、资源移动/释放、Alpha 裁剪、透明排序颜色与深度、状态恢复、resize 和关闭标志；故意反转排序后测试能检出错误，恢复后通过。

公司 macOS 保留原构建路径，但本次 v0.2 未实测；本轮也未运行 Sanitizer。历史 macOS 成功记录不能代替当前版本回归。第 52 课的测试和整理由助手完成，个人季度复盘按要求跳过。

## 代码位置

`src/app` 编排主循环，`src/platform` 管窗口和输入，`src/camera` 保存纯 CPU 相机状态，`src/render` 管绘制及 GPU RAII 资源，`src/image` 负责无状态解码；`tests/V02Regression.cpp` 是可选集成回归。

Application 按值拥有窗口、Renderer 和 Camera；Renderer 的 GPU 资源必须在窗口和 Context 之前销毁。GLFW/OpenGL 调用及资源释放均在持有 Current Context 的主线程。
