#pragma once
#include "resource.h"
#include "Prerequisites.h"
#include "Graphics2D.h"


class Mesh :
    public Resource
{
public:
    Mesh(const wchar_t*);
    Mesh(BGRA* bitmap,int width, int height, float horiz_scale, float vert_scale, int sample_scale);
    ~Mesh();
    const VertexBufferPtr& getVertexBuffer() { return m_vertex_buffer; }
    const IndexBufferPtr& getIndexBuffer() { return m_index_buffer; }
private:
    VertexBufferPtr m_vertex_buffer;
    IndexBufferPtr m_index_buffer;
};

