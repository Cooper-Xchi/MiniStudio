#include "opengl/OpenGLHeaders.h"

namespace RenderCommand {
    void Clear(float red, float green, float blue, float alpha){
        glClearColor(red, green, blue, alpha);
        glClear(GL_COLOR_BUFFER_BIT);

    }

    void DrawTriangles(int vertex_count) {
        glDrawArrays(GL_TRIANGLES, 0, vertex_count);
    }
}
