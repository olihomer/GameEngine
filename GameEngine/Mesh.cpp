#include "Mesh.h"
#include "MeshManager.h"
#include "VertexMesh.h"
#include <vector>
#include <comdef.h>
#include "GraphicsEngine.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "TinyObjLoader.h"


Mesh::Mesh(const wchar_t* full_path) :Resource(full_path)
{
	using namespace tinyobj;
	
	OutputDebugString("Entering Mesh()\n");


	attrib_t attrib;
	std::vector<shape_t> shapes;
	std::vector<material_t> materials;
	std::string warnings;
	std::string errors;

	_bstr_t temp(full_path);
	char* path = (char*)temp;
	
	bool result = LoadObj(&attrib, &shapes, &materials, &warnings, &errors, path);

	if (shapes.size() > 1)throw std::exception("More than one shape");

	std::vector<VertexMesh> list_vertices;
	std::vector<unsigned int> list_indices;

	for (size_t s = 0; s < shapes.size(); s++)
	{
		size_t index_offset = 0;
		list_vertices.reserve(shapes[s].mesh.indices.size());
		list_indices.reserve(shapes[s].mesh.indices.size());
		
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
				list_vertices.push_back(vertex);
				list_indices.push_back((unsigned int)index_offset+v);
			}
			index_offset += num_face_verts;
		}
	}

	if (!result || !errors.empty())
	{
		throw std::exception("Could not load mesh");
	}

	void* shader_byte_code = nullptr;
	size_t shader_size = 0;

	GraphicsEngine::get()->getRenderSystem()->getVertexMeshLayoutShaderByteCodeAndSize(&shader_byte_code, &shader_size);
	m_vertex_buffer=GraphicsEngine::get()->getRenderSystem()->createVertexBuffer(&list_vertices[0], sizeof(VertexMesh), (UINT)list_vertices.size(), shader_byte_code, (UINT)shader_size);

	m_index_buffer=GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(&list_indices[0], (UINT)list_indices.size());

	OutputDebugString("Mesh created ok\n");
}

Mesh::~Mesh()
{
	OutputDebugString("CLEANUP Mesh destructor\n");
}
