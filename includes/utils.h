#include <vector>

#include "mesh.h"

std::vector<Vertex> convertToVector(const float* vertices, size_t totalElements, size_t vertexSize) {
	// ���㶥������
	size_t vertexCount = totalElements / vertexSize;

	std::vector<Vertex> vertexVector;
	vertexVector.reserve(vertexCount);

	// ����Ĭ�Ϸ���
	glm::vec3 defaultNormal(0.0f, 0.0f, 1.0f);

	for (size_t i = 0; i < vertexCount; ++i) {
		Vertex vertex;
		size_t offset = i * vertexSize;

		// ����λ����Ϣ
		if (vertexSize >= 3) {
			vertex.Position = glm::vec3(vertices[offset], vertices[offset + 1], vertices[offset + 2]);
		}

		// ��������Ϣ
		if (vertexSize >= 6) {
			vertex.Normal = glm::vec3(vertices[offset + 3], vertices[offset + 4], vertices[offset + 5]);
		}
		else {
			// ���û���ṩ������Ϣ��ʹ��Ĭ�Ϸ���
			vertex.Normal = defaultNormal;
		}

		// ��������������Ϣ
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

	// ÿ�������㹹��һ��������
	for (size_t i = 0; i + 2 < vertexCount; i += 3) {
		indices.push_back(static_cast<unsigned int>(i));
		indices.push_back(static_cast<unsigned int>(i + 1));
		indices.push_back(static_cast<unsigned int>(i + 2));
	}

	return indices;
}