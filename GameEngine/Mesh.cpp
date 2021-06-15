#include "Mesh.h"
#include "MeshManager.h"
#include "VertexMesh.h"
#include <vector>
#include <comdef.h>
#include "GraphicsEngine.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "TinyObjLoader.h"


Mesh::Mesh()
{
}

Mesh::Mesh(const wchar_t* full_path) :Resource(full_path)
{
	using namespace tinyobj;
	
	OutputDebugString("Entering Mesh(const wchar_t)\n");


	attrib_t attrib;
	std::vector<shape_t> shapes;
	std::vector<material_t> materials;
	std::string warnings;
	std::string errors;

	_bstr_t temp(full_path);
	char* path = (char*)temp;
	
	bool result = LoadObj(&attrib, &shapes, &materials, &warnings, &errors, path);

	if (shapes.size() > 1)throw std::exception("More than one shape");

	for (size_t s = 0; s < shapes.size(); s++)
	{
		size_t index_offset = 0;
		//m_list_vertices.reserve(shapes[s].mesh.indices.size());
		m_list_indices.reserve(shapes[s].mesh.indices.size());
		
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
		{
			unsigned char num_face_verts = shapes[s].mesh.num_face_vertices[f];

			for (unsigned char v = 0; v < num_face_verts; v++)
			{
				index_t index = shapes[s].mesh.indices[index_offset + v];

				real_t vx = attrib.vertices[index.vertex_index * 3 + 0];
				real_t vy = attrib.vertices[index.vertex_index * 3 + 1];
				real_t vz = attrib.vertices[index.vertex_index * 3 + 2];

				real_t tx = attrib.texcoords[index.texcoord_index * 2 + 0];
				real_t ty = attrib.texcoords[index.texcoord_index * 2 + 1];

				real_t nx = attrib.normals[index.normal_index * 3 + 0];
				real_t ny = attrib.normals[index.normal_index * 3 + 1];
				real_t nz = attrib.normals[index.normal_index * 3 + 2];

				VertexMesh vertex(Vector3D(vx, vy, vz), Vector2D(tx, ty), Vector3D(nx,ny,nz));
				m_list_vertices.push_back(vertex);
				m_list_indices.push_back((unsigned int)index_offset+v);
			}
			index_offset += num_face_verts;
		}
	}

	if (!result || !errors.empty())
	{
		throw std::exception("Could not load mesh");
	}

	PrepareBuffers();

	OutputDebugString("Mesh from file created ok\n");
}

void Mesh::PrepareBuffers()
{
	void* shader_byte_code = nullptr;
	size_t shader_size = 0;

	GraphicsEngine::get()->getRenderSystem()->getVertexMeshLayoutShaderByteCodeAndSize(&shader_byte_code, &shader_size);
	m_vertex_buffer = GraphicsEngine::get()->getRenderSystem()->createVertexBuffer(&m_list_vertices[0], sizeof(VertexMesh), (UINT)m_list_vertices.size(), shader_byte_code, (UINT)shader_size);

	m_index_buffer = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(&m_list_indices[0], (UINT)m_list_indices.size());
}


Mesh::Mesh(BGRA* bitmap, int width, int height, float horiz_scale, float vert_scale, int sample_scale) :Resource(nullptr)
{
	OutputDebugString("Entering Mesh(BGRA*)\n");

	Vector3D v1, v2, v3, v4, v5, v6;
	Vector3D n1, n2, n3, n4,n5,n6,norm;
	float miny = 999;
	float maxy = 0;

	//m_list_vertices.reserve(width*height); //one vertex per pixel in bitmap
	m_list_indices.reserve((width-1)*(height-1)*6); //six indices (two triangles) per square

	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			//Create vertex buffer from bitmap points
			BGRA col = bitmap[j * width + i];
			float vx = i * horiz_scale;
			float vy = (float)(col.a * vert_scale);
			float vz = -j * horiz_scale;
			
			if (vy < miny)miny = vy;
			if (vy > maxy)maxy = vy;

			v1 = v2 = v3 = v4 = v5 = v6 = Vector3D(0, 0, 0);

			// get six vectors (pointing towards current vertex) to compute face normals
			if (j > 0)v1 = Vector3D(0, ((float)bitmap[j * width + i].a * vert_scale) - ((float)bitmap[(j - 1) * width + i].a * vert_scale),-1);
			if(i < width - 1)v2 = Vector3D(-1, ((float)bitmap[j * width + i].a * vert_scale) - ((float)bitmap[j * width + (i +1)].a * vert_scale),0);
			if (j < height - 1)v3 = Vector3D(0, ((float)bitmap[j * width + i].a * vert_scale) - ((float)bitmap[(j+1)*width + i ].a * vert_scale),1);
			if (i > 0)v4 = Vector3D(1, ((float)bitmap[j * width + i].a * vert_scale) - ((float)bitmap[j * width + (i - 1)].a * vert_scale),0);
			if (j > 0 && i < width - 1)v5 = Vector3D(-1, ((float)bitmap[j * width + i].a * vert_scale) - ((float)bitmap[(j - 1) * width + (i - 1)].a * vert_scale), -1);
			if (i > 0 && j < height - 1)v6 = Vector3D(1, ((float)bitmap[j * width + i].a * vert_scale) - ((float)bitmap[(j + 1) * width + (i - 1)].a * vert_scale), 1);

			//calculate six face normals

			n1 = Vector3D::cross(v4, v1);
			n2 = Vector3D::cross(v1, v2);
			n3 = Vector3D::cross(v2, v3);
			n4 = Vector3D::cross(v3, v4);
			n5 = Vector3D::cross(v1, v5);
			n6 = Vector3D::cross(v3, v6);

			norm = n1 + n2 + n3 + n4 + n5 + n6;
			
			if((i==width-1 && j==height-1) || norm.magnitude() == 0)norm=Vector3D(0, 1, 0); //edge cases
			
			norm.normalize();
			
			int tex = 2;
			// test if surrounded by water
			if (i < width - 1 && j < height - 1 && i>0 && j>0 && bitmap[j * width + i].r + bitmap[(j + 1) * width + i].r + bitmap[(j + 1) * width + (i + 1)].r + bitmap[j * width + (i + 1)].r + bitmap[(j - 1) * width + i].r + bitmap[j * width + (i - 1)].r + bitmap[(j - 1) * width + (i - 1)].r + bitmap[(j - 1) * width + (i + 1)].r + bitmap[(j + 1) * width + (i - 1)].r == 0)tex = 0;

			VertexMesh vertex(Vector3D(vx, vy, vz), Vector2D((float)i, (float)j), norm, Vector4D(), (unsigned int)tex);
			m_list_vertices.push_back(vertex);

			//Create index buffer = pair of triangles for each square

			if (j < height - 1 && i < width - 1)
			{
				m_list_indices.push_back(j * width + i);
				m_list_indices.push_back(j * width + i + 1);
				m_list_indices.push_back((j + 1) * width + i);

				m_list_indices.push_back(j * width + i + 1);
				m_list_indices.push_back((j + 1) * width + i + 1);
				m_list_indices.push_back((j + 1) * width + i);
			}
		}
	}

	PrepareBuffers();

	OutputDebugString("Mesh from bitmap created ok\n");
	std::cout << "Min: " << miny << " Max:" << maxy << std::endl;
}

Mesh::~Mesh()
{
	OutputDebugString("CLEANUP Mesh destructor\n");
}
