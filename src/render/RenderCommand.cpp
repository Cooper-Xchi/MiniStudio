#include "RenderCommand.h"

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
        }
    }

    void SetDepthWriteEnabled(bool enabled) {
        if (!enabled) {
            glDepthMask(GL_FALSE);
        }else {
            glDepthMask(GL_TRUE);
        }
    }

    void SetDepthCompare(DepthCompare compare) {
        switch (compare) {
            case DepthCompare::LESS:
                glDepthFunc(GL_LESS);
                break;
            case DepthCompare::GREATER:
                glDepthFunc(GL_GREATER);
                break;
            case DepthCompare::LESSEQUAL:
                glDepthFunc(GL_LEQUAL);
                break;
            case DepthCompare::GREATEREQUAL:
                glDepthFunc(GL_GEQUAL);
                break;
            case DepthCompare::EQUAL:
                glDepthFunc(GL_EQUAL);
                break;
            case DepthCompare::NOTEQUAL:
                glDepthFunc(GL_NOTEQUAL);
                break;
        }
    }
}
