#include "HalfEdgeMesh.h"
#include "VertexMesh.h"
#include "Mesh.h"
#include <iostream>
#include <unordered_set>



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

		edge1->vert->vertex_data = &(m_list_vertices[index_v2]); // vertex position at end of edge
		edge1->vert->vertexIndex = index_v2;
		edge1->vert->edge = edge2; // vertex points to edge coming out of it
		edge2->vert->vertex_data = &(m_list_vertices[index_v3]);
		edge2->vert->vertexIndex = index_v3;
		edge2->vert->edge = edge3;
		edge3->vert->vertex_data = &(m_list_vertices[index_v1]);
		edge3->vert->vertexIndex = index_v1;
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
	
	const bool DEBUG = true;

	int i = 0;
	Vector3D newPosition, newNormal;
	Vector2D newTexcoords;
	VertexMesh v1, v2, v3, v4, newVertex;
	VertexMesh newVertices[6]; // six new vertices: existing three plus three new
	unsigned int newVertexIndices[6]; // indices for each new vertex
	
	Face f1, f2;

	int num_vertices = (int)m_list_vertices.size();
	int num_indices = (int)m_list_indices.size();

	std::cout << "Start of subdivide\n" << "Indices: " << num_indices << " Vertices: " << num_vertices << std::endl;

	std::cout << m_Faces.size() << " faces" << std::endl;

	unsigned int newVertexIndex = num_vertices;

	for (auto f : m_Faces) //iterate over faces
	{
		if(DEBUG)std::cout << std::endl << "Face number: " << i << " " << std::endl;

		std::shared_ptr<Edge> face_edges[3];
		face_edges[0] = f->edge;				//get three edges
		face_edges[1] = face_edges[0]->next.lock();
		face_edges[2] = face_edges[1]->next.lock();

		
		newVertices[0] = *(face_edges[0]->vert->vertex_data); // get three vertices
		newVertices[1] = *(face_edges[0]->vert->vertex_data);
		newVertices[2] = *(face_edges[0]->vert->vertex_data);

		int currentFaceVertexCount = 0;

		for (int e = 0; e < 3; e++) //iterate over three edges
		{
			if(DEBUG)std::cout << "Edge number: " << e << " " << std::endl;
			std::shared_ptr<Edge> edge = face_edges[e]; // pointer to current edge

			v1 = *(edge->vert->vertex_data); // get first vertex attached to edge
			newVertexIndices[currentFaceVertexCount] = edge->vert->vertexIndex; // get its existing index
			if (DEBUG)std::cout << "Vertex 1: " << v1.vx_coords << " Index = " << newVertexIndices[currentFaceVertexCount] << " current face vertex count = " << currentFaceVertexCount << std::endl;
			currentFaceVertexCount++;

			if (!edge->isSplit)
			{
				if(DEBUG)std::cout << "Edge not yet split" << std::endl;

				if (edge->pair != nullptr)
				{
					//interior edge, so has two faces
					if(DEBUG)std::cout << "Found pair, must be an interior edge" << std::endl;

					v2 = *(edge->pair->vert->vertex_data);
					if(DEBUG)std::cout << "Vertex 2: " << v2.vx_coords << std::endl;
					

					f1 = *(edge->face); // get the two faces
					f2 = *(edge->pair->face);

					v3 = *(edge->next.lock()->vert->vertex_data); // get the two opposite vertices at far end of faces
					v4 = *(edge->pair->next.lock()->vert->vertex_data);

					if(DEBUG)std::cout << "Vertex 3: " << v3.vx_coords << std::endl << "Vertex 4: " << v4.vx_coords << std::endl;

					newPosition = (v1.vx_coords + v2.vx_coords) * (3.0f / 8.0f) + (v3.vx_coords + v4.vx_coords) * (1.0f / 8.0f); //calculate position, normal and texcoords of new vertex
					newNormal = (v1.nx_coords + v2.nx_coords) * (3.0f / 8.0f) + (v3.nx_coords + v4.nx_coords) * (1.0f / 8.0f);
					newNormal.normalize();
					newTexcoords = (v1.tx_coords + v2.tx_coords) * (3.0f / 8.0f) + (v3.tx_coords + v4.tx_coords) * (1.0f / 8.0f);
					
					float newTex= (v1.tex + v2.tex) * (3.0f / 8.0f) + (v3.tex + v4.tex) * (1.0f / 8.0f);
					newVertex = VertexMesh(newPosition, newTexcoords, newNormal,Vector4D(),(unsigned int)newTex);
					
					newVertices[currentFaceVertexCount] = newVertex; // add new vertex to current face vertex list
					newVertexIndices[currentFaceVertexCount] = newVertexIndex; // assign incremental index to new vertex
					m_list_vertices.push_back(newVertex); // add new vertex to list of vertices

					if (DEBUG)std::cout << "New vertex at: " << newPosition << " with VertexIndex " << newVertexIndex << " current face vertex count = " << currentFaceVertexCount << std::endl;
					
					currentFaceVertexCount++;

					edge->pair->isSplit = true; //Mark pair as having been split
					edge->isSplit = true; //Mark edge as split
					edge->pair->splitVertexIndex = newVertexIndex; // record index of vertex to be used by pair edge
					newVertexIndex++;
				}
				else
				{
					//boundary edge, one face only
					if (DEBUG)std::cout << "Empty pair, must be a boundary edge" << std::endl;

					f1 = *(edge->face);

					v2 = *(edge->next.lock()->next.lock()->vert->vertex_data);
					
					edge->vert->isBoundary = true; //mark first vertex as Boundary
					edge->next.lock()->next.lock()->vert->isBoundary = true; //mark second vertex as Boundary

					if (DEBUG)std::cout << "Marking Vertex Index: " << edge->vert->vertexIndex << " and " << edge->next.lock()->next.lock()->vert->vertexIndex << " as boundary vertices" << std::endl;

					if (DEBUG)std::cout << "Vertex 2: " << v2.vx_coords << std::endl;
					
					newPosition = (v1.vx_coords + v2.vx_coords) * 0.5f;		//calculate position, normal and texcoords of new vertex
					newNormal = (v1.nx_coords + v2.nx_coords) * 0.5f;		
					newNormal.normalize();
					newTexcoords = (v1.tx_coords + v2.tx_coords) * 0.5f;
					
					float newTex = (v1.tex + v2.tex) * 0.5f; 
					newVertex = VertexMesh(newPosition, newTexcoords, newNormal, Vector4D(), (unsigned int)newTex);

					newVertices[currentFaceVertexCount] = newVertex; // add new vertex to current face vertex list
					newVertexIndices[currentFaceVertexCount] = newVertexIndex; // assign incremental index to new vertex
					m_list_vertices.push_back(newVertex); // add new vertex to list of vertices
					
					if (DEBUG)std::cout << "New vertex at: " << newPosition << " with VertexIndex " << newVertexIndex << " current face vertex count = " << currentFaceVertexCount << std::endl;

					currentFaceVertexCount++;
					newVertexIndex++;

					/* IF WE WANTED TO ADD TO HALF-EDGE STRUCTURE
					
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
					*/

					edge->isSplit = true; //Mark edge as split
				}
			
			}
			else
			{
				if (DEBUG)std::cout << "Edge has already been split" << std::endl;
				newVertexIndices[currentFaceVertexCount] = edge->splitVertexIndex; // recall stored new vertex index
				if (DEBUG)std::cout << "Reusing vertex at: " << m_list_vertices[newVertexIndices[currentFaceVertexCount]].vx_coords << " with VertexIndex " << newVertexIndices[currentFaceVertexCount] << " current face vertex count = " << currentFaceVertexCount << std::endl;

				currentFaceVertexCount++;

			}
		}
		// Face complete, reconstruct triangles
		m_list_refined_indices.push_back(newVertexIndices[0]);
		m_list_refined_indices.push_back(newVertexIndices[3]);
		m_list_refined_indices.push_back(newVertexIndices[1]);

		m_list_refined_indices.push_back(newVertexIndices[1]);
		m_list_refined_indices.push_back(newVertexIndices[3]);
		m_list_refined_indices.push_back(newVertexIndices[5]);

		m_list_refined_indices.push_back(newVertexIndices[2]);
		m_list_refined_indices.push_back(newVertexIndices[5]);
		m_list_refined_indices.push_back(newVertexIndices[3]);

		m_list_refined_indices.push_back(newVertexIndices[4]);
		m_list_refined_indices.push_back(newVertexIndices[1]);
		m_list_refined_indices.push_back(newVertexIndices[5]);

		i++;
	}

	num_vertices = (int)m_list_vertices.size();
	num_indices = (int)m_list_refined_indices.size();

	std::cout << std::endl << "End of subdivide stage one \n" << "Refined Indices: " << num_indices << " Vertices: " << num_vertices << std::endl;
	
	//Now recompute original vertex positions

	for (auto e : m_Edges) // iterate over edges
	{
		std::unordered_set<std::shared_ptr<Vertex>> neighbours;
		
		std::shared_ptr<Edge> tempEdge = (e.second);
		std::shared_ptr<Vertex> tempVert = (tempEdge->vert);

		if (tempVert->isBoundary)
		{
			if (DEBUG)std::cout << "Skipping boundary vertex: " << tempVert->vertexIndex << std::endl;
			continue; //don't recalculate if a boundary vertex
		}

		unsigned int currentvertex_index = tempVert->vertexIndex;
		Vector3D currentvertex_pos = tempVert->vertex_data->vx_coords;
		Vector3D currentvertex_normal = tempVert->vertex_data->nx_coords;

		if(DEBUG)std::cout << "Started with Vertex Index: " << currentvertex_index << " at " << currentvertex_pos << std::endl;
		
		//find neighbours
		
		tempEdge = (tempEdge->next.lock());
		tempVert = (tempEdge->vert);

		neighbours.insert(tempVert);
		if (DEBUG)std::cout << "Neighbour with Vertex Index: " << tempVert->vertexIndex << " at " << tempVert->vertex_data->vx_coords << std::endl;
		
		while (tempEdge->pair != nullptr)
		{
			tempEdge = (tempEdge->pair->next.lock());
			tempVert = (tempEdge->vert);
			if (neighbours.find(tempVert) != neighbours.end())
			{
				if(DEBUG)std::cout << "Duplicate with Vertex Index: " << tempVert->vertexIndex << " at " << tempVert->vertex_data->vx_coords << std::endl;
				break;
			}
			neighbours.insert(tempVert);
			if (DEBUG)std::cout << "** Neighbour with Vertex Index: " << tempVert->vertexIndex << " at " << tempVert->vertex_data->vx_coords << std::endl;
		}

		if (DEBUG)std::cout << "No more neighbours" << std::endl;
		
		//neigbours is now a set of Vertex pointers to all neigbouring points

		Vector3D average_pos;
		Vector3D average_normal;

		for (auto n : neighbours)
		{
			if (DEBUG)std::cout << "Neighbour:" << n->vertex_data->vx_coords << std::endl;
			average_pos = average_pos + n->vertex_data->vx_coords;
			average_normal = average_normal + n->vertex_data->nx_coords;
		}
		average_pos = average_pos * (1.0f/(float)neighbours.size());
		average_normal = average_normal * (1.0f / (float)neighbours.size());

		if(DEBUG)std::cout << "Average of " << neighbours.size() << " = " << average_pos << std::endl;

		float beta;

		if (neighbours.size() > 3)
		{
			beta = 3.0f / (8.0f * (float)neighbours.size());
		}
		else
		{
			if (neighbours.size() == 3)beta = 3.0f / 16.0f;
			if (neighbours.size() == 2)beta = 1.0f / 8.0f;
			if (neighbours.size() == 1)beta = 0.0f;
		}

		Vector3D newpos = currentvertex_pos * (1 - ((float)neighbours.size() * beta)) + average_pos * ((float)neighbours.size() * beta);
		Vector3D newnormal = currentvertex_normal * (1 - ((float)neighbours.size() * beta)) + average_normal * ((float)neighbours.size() * beta);
		newnormal.normalize();

		if(DEBUG)std::cout << "Beta = " << beta << " New pos: " << newpos << std::endl;

		m_list_vertices[currentvertex_index].vx_coords = newpos;
		m_list_vertices[currentvertex_index].nx_coords = newnormal;


	}
	

	m_list_indices = m_list_refined_indices;
	
	OutputDebugString("Subdivide() - preparing buffers\n");

	PrepareBuffers();

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
		e.second->vert.reset();
		e.second.reset();
	}
	m_Edges.clear();
}

