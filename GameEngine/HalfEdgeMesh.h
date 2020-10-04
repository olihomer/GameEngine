#pragma once
#include "Mesh.h"
#include "Vector3D.h"
#include <map>
#include <vector>

struct Edge;

struct Vertex
{
	std::shared_ptr<Vector3D> pos;
	std::shared_ptr<Edge> edge;
};

struct Face
{
	std::shared_ptr<Edge> edge;
};

struct Edge
{
	std::shared_ptr<Vertex> vert;
	std::shared_ptr<Edge> pair;
	std::shared_ptr<Face> face;
	std::weak_ptr<Edge> next;
	bool isSplit = false;
};

class HalfEdgeMesh : public Mesh
{
public:
	HalfEdgeMesh(MeshPtr mesh);
	void Subdivide();
	~HalfEdgeMesh();
	
	std::map<std::pair<unsigned int, unsigned int>, std::shared_ptr<Edge>> m_Edges;		
	std::vector<std::shared_ptr<Face>> m_Faces;
};

