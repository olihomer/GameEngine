#pragma once
#include "resource.h"
#include "Prerequisites.h"
#include "Graphics2D.h"
#include "VertexMesh.h"
#include <vector>


class Mesh :
    public Resource
{
public:
    Mesh();
    Mesh(const wchar_t*);
    void PrepareBuffers();
    Mesh(BGRA* bitmap,int width, int height, float horiz_scale, float vert_scale, int sample_scale);
    ~Mesh();
    const VertexBufferPtr& getVertexBuffer() { return m_vertex_buffer; }
    const IndexBufferPtr& getIndexBuffer() { return m_index_buffer; }
private:
    VertexBufferPtr m_vertex_buffer;
    IndexBufferPtr m_index_buffer;
    std::vector<VertexMesh> m_list_vertices;
    std::vector<unsigned int> m_list_indices;
    friend class HalfEdgeMesh;
};

