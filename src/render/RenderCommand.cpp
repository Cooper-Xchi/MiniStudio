#include "RenderCommand.h"
#include "opengl/OpenGLHeaders.h"


namespace {
    GLenum BlendFactorMap(RenderCommand::BlendFactor factor) {
        switch (factor) {
            case RenderCommand::BlendFactor::SourceAlpha:
                return GL_SRC_ALPHA;
            case RenderCommand::BlendFactor::OneMinusSourceAlpha:
                return GL_ONE_MINUS_SRC_ALPHA;
        }
        return GL_ONE;
    }
}

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

    void SetFrontFaceWinding(FrontFaceWinding winding) {
        switch (winding) {
            case FrontFaceWinding::Clockwise:
                glFrontFace(GL_CW);
                break;
            case FrontFaceWinding::CounterClockwise:
                glFrontFace(GL_CCW);
                break;
        }
    }

    void SetFaceCullingEnabled(bool enabled) {
        if (!enabled) {
            glDisable(GL_CULL_FACE);
        }else {
            glEnable(GL_CULL_FACE);
        }
    }

    void SetCullFace(CullFace face) {
        switch (face) {
            case CullFace::Back:
                glCullFace(GL_BACK);
                break;
            case CullFace::Front:
                glCullFace(GL_FRONT);
                break;
        }
    }

    void SetGlobalDepth(bool enableDepth,bool writeDepth,DepthCompare mode) {
        RenderCommand::SetDepthTestEnabled(enableDepth);
        RenderCommand::SetDepthWriteEnabled(writeDepth);
        RenderCommand::SetDepthCompare(mode);
    }

    void SetGlobalCullFace(bool enable,CullFace face,FrontFaceWinding winding) {
        RenderCommand::SetFaceCullingEnabled(enable);
        RenderCommand::SetCullFace(face);
        RenderCommand::SetFrontFaceWinding(winding);
    }

    void SetBlendingEnabled(bool enabled) {
        if (!enabled) {
            glDisable(GL_BLEND);
        }else {
            glEnable(GL_BLEND);
        }
    }

    void SetBlendFunction(
        BlendFactor source,
        BlendFactor destination
    ) {
        glBlendFunc(BlendFactorMap(source),BlendFactorMap(destination));
    }
}
