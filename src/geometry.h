#pragma once
#include "openglbase.h"
#include "mesh.h"

class Geometry
{
public:
	int getVertexCount() const { return vertices_.size(); }
	std::vector<Vertex> getVertices() const { return vertices_; }
	std::vector<uint32_t> getIndices() const { return indices_; }

	void Draw();
	
protected:
	std::vector<Vertex> vertices_;
	std::vector<uint32_t> indices_;
};