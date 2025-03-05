#include <vector>

#include "mesh.h"

std::vector<Vertex> convertToVector(const float* vertices, size_t totalElements, size_t vertexSize) {
	// 计算顶点数量
	size_t vertexCount = totalElements / vertexSize;

	std::vector<Vertex> vertexVector;
	vertexVector.reserve(vertexCount);

	// 定义默认法线
	glm::vec3 defaultNormal(0.0f, 0.0f, 1.0f);

	for (size_t i = 0; i < vertexCount; ++i) {
		Vertex vertex;
		size_t offset = i * vertexSize;

		// 处理位置信息
		if (vertexSize >= 3) {
			vertex.Position = glm::vec3(vertices[offset], vertices[offset + 1], vertices[offset + 2]);
		}

		// 处理法线信息
		if (vertexSize >= 6) {
			vertex.Normal = glm::vec3(vertices[offset + 3], vertices[offset + 4], vertices[offset + 5]);
		}
		else {
			// 如果没有提供法线信息，使用默认法线
			vertex.Normal = defaultNormal;
		}

		// 处理纹理坐标信息
		if (vertexSize == 5 || vertexSize >= 8) {
			size_t texCoordOffset = (vertexSize == 5) ? offset + 3 : offset + 6;
			vertex.TexCoords = glm::vec2(vertices[texCoordOffset], vertices[texCoordOffset + 1]);
		}

		vertexVector.push_back(vertex);
	}

	return vertexVector;
}

std::vector<unsigned int> generateIndices(const std::vector<Vertex>& vertices) {
	std::vector<unsigned int> indices;
	size_t vertexCount = vertices.size();

	// 每三个顶点构成一个三角形
	for (size_t i = 0; i + 2 < vertexCount; i += 3) {
		indices.push_back(static_cast<unsigned int>(i));
		indices.push_back(static_cast<unsigned int>(i + 1));
		indices.push_back(static_cast<unsigned int>(i + 2));
	}

	return indices;
}