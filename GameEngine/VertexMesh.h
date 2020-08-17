#pragma once
#include "Vector2D.h"
#include "Vector3D.h"

class VertexMesh
{
public:
	VertexMesh() {};
	VertexMesh(Vector3D vx, Vector2D tx, Vector3D nx) : vx_coords(vx), tx_coords(tx),nx_coords(nx) {};

	Vector3D vx_coords = {};
	Vector2D tx_coords = {};
	Vector3D nx_coords = {};
};