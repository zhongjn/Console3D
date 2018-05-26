#include "my3d.h"

console3d::my3d::Mesh::Mesh(std::vector<Vertex> vertexes, std::vector<Triangle> triangles)
	: vertexes(vertexes), triangles(triangles)
{

}

console3d::my3d::Mesh::Mesh() {}