#pragma once
#include "resource.h"
#include "Prerequisites.h"


class Mesh :
    public Resource
{
public:
    Mesh(const wchar_t*);
    ~Mesh();
    const VertexBufferPtr& getVertexBuffer() { return m_vertex_buffer; }
    const IndexBufferPtr& getIndexBuffer() { return m_index_buffer; }
private:
    VertexBufferPtr m_vertex_buffer;
    IndexBufferPtr m_index_buffer;
};

