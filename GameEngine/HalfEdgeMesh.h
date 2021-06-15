#pragma once
#include "Mesh.h"
#include "Vector3D.h"
#include <map>
#include <vector>

struct Edge;

struct Vertex
{
	//VertexMesh* vertex_data; //coords etc.
	std::shared_ptr<Edge> edge; //edge coming out of vertex
	unsigned int vertexIndex; // index on vertex list
	bool isBoundary = false;
};

struct Face
{
	std::shared_ptr<Edge> edge; // one of the edges in the face
};

struct Edge
{
	std::shared_ptr<Vertex> vert; //vertex at end of edge
	std::shared_ptr<Edge> pair; // edge in opposite direction
	std::shared_ptr<Face> face; // face that edge belongs to
	std::weak_ptr<Edge> next; // next edge in face
	bool isSplit = false;
	unsigned int splitVertexIndex = 0;
};

class HalfEdgeMesh : public Mesh
{
public:
	HalfEdgeMesh(MeshPtr mesh);
	void Subdivide();
	~HalfEdgeMesh();
	
	std::map<std::pair<unsigned int, unsigned int>, std::shared_ptr<Edge>> m_Edges;		
	std::vector<std::shared_ptr<Face>> m_Faces;
	std::vector<unsigned int> m_list_refined_indices; // Refined vertex index list to populate in sub-division algorithm
};
