#pragma once
#include "Vector2D.h"
#include "Vector3D.h"

class VertexMesh
{
public:
	VertexMesh() {};
	VertexMesh(Vector3D vx, Vector2D tx) : vx_coords(vx), tx_coords(tx) {};

	Vector3D vx_coords = {};
	Vector2D tx_coords = {};
};