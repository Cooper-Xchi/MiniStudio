#pragma once

namespace RenderCommand {
    enum class DepthCompare {
        LESSEQUAL,
        LESS,
        EQUAL,
        NOTEQUAL,
        GREATER,
        GREATEREQUAL,
    };

    enum class FrontFaceWinding {
        CounterClockwise,
        Clockwise
    };

    enum class CullFace {
        Back,
        Front
    };

    void SetFaceCullingEnabled(bool enabled);
    void SetCullFace(CullFace face);

    void SetFrontFaceWinding(FrontFaceWinding winding);

    void Clear(float red, float green, float blue, float alpha);
    void DrawIndexedTriangles(int vertex_count);

    void SetDepthTestEnabled(bool enabled);

    void SetDepthWriteEnabled(bool enabled);

    void SetDepthCompare(DepthCompare compare);

    void SetGlobalDepth(bool enableDepth,bool writeDepth,DepthCompare mode);

    void SetGlobalCullFace(bool enable,CullFace face,FrontFaceWinding winding);

}
