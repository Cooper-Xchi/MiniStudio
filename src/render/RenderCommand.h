#pragma once

namespace RenderCommand {

    void Clear(float red, float green, float blue, float alpha);
    void DrawIndexedTriangles(int vertex_count);

    void SetDepthTestEnabled(bool enabled);

    void SetDepthWriteEnabled(bool enabled);

}
