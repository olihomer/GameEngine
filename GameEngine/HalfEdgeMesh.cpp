#include "HalfEdgeMesh.h"
#include "VertexMesh.h"
#include "Mesh.h"
#include <iostream>

HalfEdgeMesh::HalfEdgeMesh(MeshPtr mesh)
{
	OutputDebugString("Entering HalfEdgeMesh Constructor\n");
	
	m_list_indices = mesh->m_list_indices;
	m_list_vertices = mesh->m_list_vertices;

	int num_vertices = (int)m_list_vertices.size();
	int num_indices = (int)m_list_indices.size();
	
	std::cout << "Indices: " << num_indices << " Vertices: " << num_vertices << std::endl;

	for (int i = 0; i < num_indices; i+=3) // One triangle face at a time = three edges
	{
		unsigned int index_v1 = m_list_indices[i];
		unsigned int index_v2 = m_list_indices[i+1];
		unsigned int index_v3 = m_list_indices[i+2];

		std::pair index_e1(index_v1, index_v2);
		std::pair index_e2(index_v2, index_v3);
		std::pair index_e3(index_v3, index_v1);

		std::shared_ptr<Edge> edge1 = std::make_shared<Edge>(); // create three edges
		std::shared_ptr<Edge> edge2 = std::make_shared<Edge>();
		std::shared_ptr<Edge> edge3 = std::make_shared<Edge>();
		
		edge1->face = std::make_shared<Face>(); // all three edges point to same face
		edge2->face = edge1->face;
		edge3->face = edge2->face;
		
		edge1->next = edge2; // each edge points to next in face
		edge2->next = edge3;
		edge3->next = edge1;

		edge1->vert = std::make_shared<Vertex>(); // create three vertices
		edge2->vert = std::make_shared<Vertex>();
		edge3->vert = std::make_shared<Vertex>();

		edge1->vert->pos = std::make_shared<Vector3D>(m_list_vertices[index_v2].vx_coords); // vertex position at end of edge
		edge1->vert->edge = edge2; // vertex points to edge coming out of it
		edge2->vert->pos = std::make_shared<Vector3D>(m_list_vertices[index_v3].vx_coords);
		edge2->vert->edge = edge3;
		edge3->vert->pos = std::make_shared<Vector3D>(m_list_vertices[index_v1].vx_coords);
		edge3->vert->edge = edge1;

		edge1->face->edge = edge1; // face can point to any one of the edges
		
		if (m_Edges.find(std::make_pair(index_v2,index_v1)) != m_Edges.end()) //try to find opposite for edge1
		{
			edge1->pair = m_Edges[std::make_pair(index_v2, index_v1)];
			m_Edges[std::make_pair(index_v2, index_v1)]->pair = edge1;
		}
	
		if (m_Edges.find(std::make_pair(index_v3, index_v2)) != m_Edges.end()) //try to find opposite for edge3
		{
			edge2->pair = m_Edges[std::make_pair(index_v3, index_v2)];
			m_Edges[std::make_pair(index_v3, index_v2)]->pair = edge2;
		}

		if (m_Edges.find(std::make_pair(index_v1, index_v3)) != m_Edges.end()) //try to find opposite for edge3
		{
			edge3->pair = m_Edges[std::make_pair(index_v1, index_v3)];
			m_Edges[std::make_pair(index_v1, index_v3)]->pair = edge3;
		}

		m_Edges.insert(std::make_pair(index_e1, edge1)); // insert three edges into map
		m_Edges.insert(std::make_pair(index_e2, edge2));
		m_Edges.insert(std::make_pair(index_e3, edge3));
		m_Faces.push_back(edge1->face);
	}

	PrepareBuffers();

	OutputDebugString("HalfEdgeMesh Constructed ok\n");
}

void HalfEdgeMesh::Subdivide()
{
	OutputDebugString("Entering Subdivide()\n");
	
	std::vector<unsigned int> newIndices;
	std::vector<VertexMesh> newVertices;

	int i = 0;
	Vector3D newVertex;
	Vector3D v1, v2,v3,v4;
	Face f1, f2;

	int new_vertex_index = (int)m_list_vertices.size();

	std::cout << m_Faces.size() << " faces" << std::endl;

	for (auto f : m_Faces) //iterate over faces
	{
		std::cout << std::endl << "Face number: " << i << " " << std::endl;

		std::shared_ptr<Edge> face_edges[3];
		face_edges[0] = f->edge;				//get three edges
		face_edges[1] = face_edges[0]->next.lock();
		face_edges[2] = face_edges[1]->next.lock();

		for (int e = 0; e < 3; e++) //iterate over three edges
		{
			std::cout << "Edge number: " << e << " " << std::endl;
			std::shared_ptr<Edge> edge = face_edges[e]; // pointer to current edge

			if (!edge->isSplit)
			{
				std::cout << "Edge not yet split" << std::endl;

				Vector3D v1 = *(edge->vert->pos); // get first vertex attached to edge
				std::cout << "Vertex 1: " << v1 << std::endl;

				if (edge->pair != nullptr)
				{
					//interior edge, so has two faces
					std::cout << "Found pair, must be an interior edge" << std::endl;

					Vector3D v2 = *(edge->pair->vert->pos);
					std::cout << "Vertex 2: " << v2 << std::endl;

					f1 = *(edge->face); // get the two faces
					f2 = *(edge->pair->face);

					v3 = *(edge->next.lock()->vert->pos); // get the two opposite vertices at far end of faces
					v4 = *(edge->pair->next.lock()->vert->pos);

					std::cout << "Vertex 3: " << v3 << std::endl << "Vertex 4: " << v4 << std::endl;

					newVertex = (v1 + v2) * (3.0f / 8.0f) + (v3 + v4) * (1.0f / 8.0f);
					std::cout << "New vertex at: " << newVertex << std::endl;
					
					edge->pair->isSplit = true; //Mark pair as having been split
					edge->isSplit = true; //Mark edge as split
				}
				else
				{
					//boundary edge, one face only
					std::cout << "Empty pair, must be a boundary edge" << std::endl;

					f1 = *(edge->face);

					v2 = *(edge->next.lock()->next.lock()->vert->pos);
					std::cout << "Vertex 2: " << v2 << std::endl;

					newVertex = (v1 + v2) * 0.5f;		// Vector3D of new vertex
					std::cout << "New vertex at: " << newVertex << std::endl;

					//std::pair new_index1(edge_index.first, new_vertex_index);	//create two new edges to/from new vertex
					//std::pair new_index2(new_vertex_index, edge_index.second);
					std::shared_ptr<Edge> new_edge1 = std::make_shared<Edge>();
					std::shared_ptr<Edge> new_edge2 = std::make_shared<Edge>();


					new_edge1->face = edge->face; //new edges point to same face as existing edge
					new_edge2->face = edge->face;

					new_edge1->next = new_edge2; // each edge points to next in face
					new_edge2->next = edge->next;
					//edge.next.lock()->next.lock()->next = new_edge1; // previous edge of existing edge points to first new edge

					new_edge1->vert = std::make_shared<Vertex>(); // create three vertices
					new_edge2->vert = std::make_shared<Vertex>();

					new_edge1->vert->pos = std::make_shared<Vector3D>(newVertex); // vertex position at end of edge
					new_edge1->vert->edge = new_edge2; // vertex points to edge coming out of it

					new_edge2->vert = edge->vert;
					edge->isSplit = true; //Mark edge as split
				}
			
			}
			else
			{
				std::cout << "Edge has already been split" << std::endl;
			}
		}
	i++;
	}

	OutputDebugString("Subdivide() completed ok\n");
}

HalfEdgeMesh::~HalfEdgeMesh()
{
	OutputDebugString("CLEANUP HalfEdgeMesh destructor\n");
	for (auto e : m_Edges)
	{
		e.second->face->edge.reset();
		e.second->face.reset();
		e.second->pair.reset();
		e.second->next.reset();
		e.second->vert->edge.reset();
		e.second->vert->pos.reset();
		e.second->vert.reset();
		e.second.reset();
	}
	m_Edges.clear();
}

