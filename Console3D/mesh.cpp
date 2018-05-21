#include "core.h"

console3d::core::Mesh::Mesh(std::vector<Vertex> vertexes, std::vector<VertexTriangle> triangles)
	: vertexes(vertexes), triangles(triangles)
{

}

console3d::core::Mesh::Mesh() {}