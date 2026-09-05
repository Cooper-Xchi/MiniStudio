#pragma once

#if defined(__APPLE__)
#include <OpenGL/gl3.h>
#elif defined(_WIN32)
#include <glad/glad.h>
#else
#error "MiniStudio currently supports OpenGL only on macOS and Windows."
#endif
