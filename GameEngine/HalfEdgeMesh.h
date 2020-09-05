#pragma once
#include "Mesh.h"
#include "Vector3D.h"

struct Edge;

struct Vertex
{
	Vector3D pos;
	Edge* edge;
};

struct Face
{
	Edge* edge;
};

struct Edge
{
	Vertex* vert;
	Edge* pair;
	Face* face;
	Edge* next;
};


class HalfEdgeMesh : public Mesh
{

};

