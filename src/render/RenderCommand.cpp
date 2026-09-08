#include "opengl/OpenGLHeaders.h"

namespace RenderCommand {
    void Clear(float red, float green, float blue, float alpha){
        glClearColor(red, green, blue, alpha);
        glClearDepth(1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    }

    void DrawIndexedTriangles(int vertex_count) {
        glDrawElements(GL_TRIANGLES, vertex_count, GL_UNSIGNED_INT, nullptr);
    }


    void SetDepthTestEnabled(bool enabled) {
        if (!enabled) {
            glDisable(GL_DEPTH_TEST);
        }else {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
        }
    }

    void SetDepthWriteEnabled(bool enabled) {
        if (!enabled) {
            glDepthMask(GL_FALSE);
        }else {
            glDepthMask(GL_TRUE);
        }
    }
}
