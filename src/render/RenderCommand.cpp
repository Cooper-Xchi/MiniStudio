#include "opengl/OpenGLHeaders.h"

namespace RenderCommand {
    void Clear(float red, float green, float blue, float alpha){
        glClearColor(red, green, blue, alpha);
        glClear(GL_COLOR_BUFFER_BIT);

    }

    void DrawIndexedTriangles(int vertex_count) {
        glDrawElements(GL_TRIANGLES, vertex_count, GL_UNSIGNED_INT, nullptr);
    }
}
