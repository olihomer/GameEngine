#include <memory>
#include "Vector3D.h"
#include "Vector4D.h"
#include <windows.h>

#pragma once

// LEFT HANDED COORDINATE SYSTEM: POSITIVE X = RIGHT, POSITIVE Y = UP, POSITIVE Z = AWAY
// ROTATIONS = ANTICLOCKWISE AS LOOKING ALONG POSITIVE AXIS


class Matrix4x4{
public:
	float m_mat[4][4] = {};

	void setIdentity()
	{
		::memset(m_mat, 0, sizeof(float) * 16);
		m_mat[0][0] = 1;
		m_mat[1][1] = 1;
		m_mat[2][2] = 1;
		m_mat[3][3] = 1;
	}

	void setTranslation(const Vector3D& translation)
	{
		m_mat[3][0] = translation.m_x;
		m_mat[3][1] = translation.m_y;
		m_mat[3][2] = translation.m_z;
	}

	void setScale(const Vector3D& scale)
	{
		m_mat[0][0] = scale.m_x;
		m_mat[1][1] = scale.m_y;
		m_mat[2][2] = scale.m_z;
	}

	void setRotX(float psi)
	{
		m_mat[1][1] = cosf(psi);
		m_mat[1][2] = sinf(psi);
		m_mat[2][1] = -sinf(psi);
		m_mat[2][2] = cosf(psi);
	}

	void setRotY(float theta)
	{
		m_mat[0][0] = cosf(theta);
		m_mat[0][2] = -sinf(theta);
		m_mat[2][0] = sinf(theta);
		m_mat[2][2] = cosf(theta);
	}

	void setRotZ(float phi)
	{
		m_mat[0][0] = cosf(phi);
		m_mat[0][1] = sinf(phi);
		m_mat[1][0] = -sinf(phi);
		m_mat[1][1] = cosf(phi);
	}

	void setMatrix(Matrix4x4 matrix)
	{
		::memcpy(&m_mat, &matrix, sizeof(float) * 16);
	}


	void operator *=(const Matrix4x4& matrix)
	{
		Matrix4x4 out;
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
			{
				out.m_mat[i][j] =
					m_mat[i][0] * matrix.m_mat[0][j] + m_mat[i][1] * matrix.m_mat[1][j] +
					m_mat[i][2] * matrix.m_mat[2][j] + m_mat[i][3] * matrix.m_mat[3][j];
			}		
		::memcpy(m_mat, out.m_mat, sizeof(float) * 16);
	}

	void setOrthoLH(float width, float height, float near_plane, float far_plane)
	{
		setIdentity();
		m_mat[0][0] = 2.0f / width;
		m_mat[1][1] = 2.0f / height;;
		m_mat[2][2] = 1.0f / (far_plane - near_plane);
		m_mat[3][2] = -(near_plane / (far_plane - near_plane));
	}
	

	void setPerspLH(int fov, float aspect, float near_plane,float far_plane)
	{
		float theta = (float)fov / 2.0f * 0.0174532f;
		
		setIdentity();
		m_mat[0][0] = 1 / tanf(theta);
		m_mat[1][1] = aspect / tanf(theta);
		m_mat[2][2] = far_plane / (far_plane - near_plane);
		m_mat[3][2] = -(near_plane * far_plane) / (far_plane - near_plane);
		m_mat[2][3] = 1.0f;
		m_mat[3][3] = 0;
	}

	Matrix4x4()
	{
		setIdentity();
	}
	
	float getDeterminant()
	{
		Vector4D minor, v1, v2, v3;
		float det;

		v1 = Vector4D(this->m_mat[0][0], this->m_mat[1][0], this->m_mat[2][0], this->m_mat[3][0]);
		v2 = Vector4D(this->m_mat[0][1], this->m_mat[1][1], this->m_mat[2][1], this->m_mat[3][1]);
		v3 = Vector4D(this->m_mat[0][2], this->m_mat[1][2], this->m_mat[2][2], this->m_mat[3][2]);

		minor.cross(v1, v2, v3);
		det = -(this->m_mat[0][3] * minor.m_x + this->m_mat[1][3] * minor.m_y + this->m_mat[2][3] * minor.m_z +
			this->m_mat[3][3] * minor.m_w);
		return det;
	}

	void invert()
	{
		int a, i, j;
		Matrix4x4 out;
		Vector4D v, vec[3];
		float det = 0.0f;

		det = this->getDeterminant();
		if (!det) return;
		for (i = 0; i < 4; i++)
		{
			for (j = 0; j < 4; j++)
			{
				if (j != i)
				{
					a = j;
					if (j > i) a = a - 1;
					vec[a].m_x = (this->m_mat[j][0]);
					vec[a].m_y = (this->m_mat[j][1]);
					vec[a].m_z = (this->m_mat[j][2]);
					vec[a].m_w = (this->m_mat[j][3]);
				}
			}
			v.cross(vec[0], vec[1], vec[2]);

			out.m_mat[0][i] = (powf(-1.0f, (float)i) * v.m_x / det);
			out.m_mat[1][i] = (powf(-1.0f, (float)i) * v.m_y / det);
			out.m_mat[2][i] = (powf(-1.0f, (float)i) * v.m_z / det);
			out.m_mat[3][i] = (powf(-1.0f, (float)i) * v.m_w / det);
		}

		this->setMatrix(out);
	}

	void debugPrint(const char* txt)
	{
		char debug[300];

		sprintf_s(debug, "%s\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n", txt,m_mat[0][0], m_mat[0][1], m_mat[0][2], m_mat[0][3], m_mat[1][0], m_mat[1][1], m_mat[1][2], m_mat[1][3], m_mat[2][0], m_mat[2][1], m_mat[2][2], m_mat[2][3], m_mat[3][0], m_mat[3][1], m_mat[3][2], m_mat[3][3]);
		OutputDebugString(debug);
	}

};