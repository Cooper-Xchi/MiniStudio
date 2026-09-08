# MiniStudio 纹理链路架构复盘

## 1. 当前数据流

初始化数据流：

```text
Renderer 中的局部 Shader/顶点/索引/RGBA 数据
→ ShaderProgram 编译并链接 Program
→ glBufferData 把顶点和索引复制到 VBO/EBO，VAO 记录布局
→ glTexImage2D 把 RGBA 像素复制到 Texture2D 的 GPU 存储
→ sampler uniform 记录纹理单元 0
```

每帧数据流：

```text
Clear → Use Program → Bind VAO → 在纹理单元 0 绑定 Texture2D
→ DrawElements → Present
```

`rgba_array` 是 `Renderer::Initialize()` 的局部数组，在函数返回时按 C++ 作用域规则结束生命周期。`glTexImage2D` 在返回前读取并复制像素，因此 GPU 纹理不依赖该数组继续存活。

`Renderer` 按值拥有 `Texture2D` 对象；`Texture2D` 独占 `texture_id_` 表示的 OpenGL texture handle。`Renderer` 析构时触发成员逆序析构，最终由 `Texture2D::Release()` 调用 `glDeleteTextures`。当前初始化、上传、绘制和释放均在持有 Current Context 的主线程执行。

## 2. 外部图片加入后的职责边界

请为下面四项填写“主要职责、拥有的数据、运行线程、禁止依赖”：

| 模块或数据 | 主要职责 | 拥有的数据 | 运行线程 | 禁止依赖 |
| --- | --- | --- | --- | --- |
| `Renderer` | 编排解码、失败处理、GPU 上传与绘制 | `ShaderProgram`、`VertexArray`、`Texture2D` | 当前为主线程 | 图片格式解析细节 |
| CPU 图片数据 | 保存已解码图片的尺寸与 RGBA 字节 | 宽、高和连续像素内存 | 可在创建它的线程使用或移动 | OpenGL、窗口与 GPU handle |
| 图片解码函数或模块 | 读取文件并生成 CPU 图片数据 | 不长期拥有结果；结果交给调用者 | 当前可在主线程，未来可移到后台线程 | `Texture2D`、OpenGL 与 Current Context |
| `Texture2D` | 校验并上传像素，保存采样状态，绑定和释放纹理 | OpenGL texture handle | 持有 Current Context 的主线程 | 文件路径、文件 I/O 和具体解码库 |

## 3. 最小接口草图

用不超过 30 行的 C++ 接口或伪代码表达以下过程，不写实现：

1. 文件解码产生一份自动管理生命周期的 CPU 图片数据；
2. `Texture2D` 借用这份数据完成 GPU 上传；
3. 上传完成后，CPU 图片数据可以释放，GPU 纹理继续存在；
4. 失败必须能够返回给调用者。

```cpp
#include <vector>

struct ImageData {
    int width = 0;
    int height = 0;
    std::vector<unsigned char> rgba_pixels;
};

bool LoadImageRgba(const char* path, ImageData& output);

ImageData image;
if (!LoadImageRgba("wood.png", image)) {
    return false;
}
if (!texture_.Initialize(
        image.width,
        image.height,
        image.rgba_pixels.data())) {
    return false;
}
```

`ImageData& output` 是对调用方对象的可修改借用；`rgba_pixels` 通过 `std::vector` 拥有 CPU 字节。`data()` 返回临时借用的连续内存首地址，其有效期受 `vector` 生命周期和重新分配影响。`glTexImage2D` 返回后，当前同步上传路径不再需要保留 CPU 像素。

## 4. 图片解码依赖评估

在 `stb_image`、系统图片 API、`libpng`/`libjpeg` 和自行编写解码器之间比较：

- 接入和构建成本；
- macOS、Windows 及后续 Android 的平台一致性；
- 支持的常见格式；
- 错误处理与维护成本；
- 是否适合当前受控学习素材。

本阶段选择 `stb_image`。它使用单文件方式提供 C/C++ 图片解码，能够从文件、内存或回调读取 PNG、JPEG、TGA、BMP、HDR 等常见格式，适合用最小构建成本建立跨 macOS、Windows 和后续 Android 的同步解码链路。下一课若确认引入，应固定上游版本或提交，并记录许可证来源，避免构建结果随上游变化。

代价是它使用 C 风格接口和独立内存释放函数，错误表达较简单；官方还说明其安全问题会公开讨论，修复可能需要较长时间。因此当前只把它用于仓库内受控学习素材，不把它当作不可信网络图片的安全边界。资料来源：[stb README](https://github.com/nothings/stb/blob/master/README.md)、[stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h)。

系统 ImageIO/WIC 会形成不同平台实现；`libpng`/`libjpeg` 会增加多个库及更专门的 API；自行编写解码器会把课程带入文件格式与安全解析细节。这些方案目前都不能提供与其成本相称的学习收益。

## 5. 暂不处理的范围

- 异步解码和任务队列：先让同步数据链路稳定并具备明确失败路径，再引入线程与取消。
- 纹理缓存、热重载和通用资源 Manager：当前只有一个真实纹理使用场景，尚无重复加载或更新需求。
- HDR、颜色空间和手工 Mipmap 策略：当前已为 RGBA8 图片自动生成完整 Mipmap，其他格式与策略留到真实素材需要时处理。
- 跨平台 `RenderDevice`：第一套 OpenGL 渲染器尚未完成，也没有第二种真实后端可供抽象。

## 6. 第 24 课实际链路与错误边界

第 21～23 课实现后的初始化数据流为：

```text
文件路径
→ stb_image 临时解码内存（unique_ptr + stbi_image_free）
→ ImageData::rgba_pixels（交换上下行后的 CPU RGBA 数据）
→ Texture2D 上传第 0 级并生成 Mipmap
→ glTexImage2D 返回后释放局部 ImageData
```

依赖保持单向：`Renderer → ImageLoader → stb_image`，同时 `Renderer → Texture2D → OpenGL/OpenGLDebug`。`ImageLoader` 不依赖 Renderer、窗口或 OpenGL；`Texture2D` 不依赖文件路径、文件 I/O 或解码库。`Renderer` 编排失败传播，不负责解释图片格式或管理裸纹理句柄。

`Texture2D::Initialize` 现在对自己发出的 OpenGL 初始化调用负责：输入检查通过后先清除旧错误，完成纹理创建、采样状态、0 级上传和 Mipmap 生成，再立即检查错误。如果失败，`Release()` 同时删除 GPU 资源并把 `texture_id_` 恢复为 `0`，使析构不会重复删除，也允许同一对象再次初始化。逐帧 `DrawFrame()` 的错误检查只覆盖当帧命令，不再承担初始化错误诊断。
