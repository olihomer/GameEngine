#pragma once
#include "Vector2D.h"
#include "Vector3D.h"
#include "Vector4D.h"

class VertexMesh
{
public:
	VertexMesh() {};
	VertexMesh(Vector3D vx, Vector2D tx, Vector3D nx, Vector4D col_rgba,unsigned int tex) : vx_coords(vx), tx_coords(tx), nx_coords(nx), col_rgba(col_rgba),tex((float)tex) {};
	VertexMesh(Vector3D vx, Vector2D tx, Vector3D nx,Vector4D col_rgba) : vx_coords(vx), tx_coords(tx),nx_coords(nx),col_rgba(col_rgba) {};
	VertexMesh(Vector3D vx, Vector2D tx, Vector3D nx) : vx_coords(vx), tx_coords(tx), nx_coords(nx), col_rgba() {};

	Vector3D vx_coords = {};
	Vector2D tx_coords = {};
	Vector3D nx_coords = {};
	Vector4D col_rgba = {};
	float tex=0.0f;
};