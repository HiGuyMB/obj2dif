#include <iostream>
#include <DIFBuilder/DIFBuilder.hpp>
#include <tiny_obj_loader.h>

int main(int argc, const char **argv) {
	//Read everything we can
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;
	tinyobj::LoadObj(&attrib, &shapes, &materials, &err, argv[1]);

	//Default material
	materials.push_back(tinyobj::material_t());

	for (const tinyobj::shape_t shape : shapes) {
		DIF::DIFBuilder builder;

		int vertStart = 0;
		for (int i = 0; i < shape.mesh.num_face_vertices.size(); i ++) {
			tinyobj::index_t idx[3] = {
					shape.mesh.indices[vertStart + 2],
					shape.mesh.indices[vertStart + 1],
					shape.mesh.indices[vertStart + 0]
			};

			DIF::DIFBuilder::Triangle triangle;

			for (int j = 0; j < 3; j ++) {
				triangle.points[j].vertex = glm::vec3(
						attrib.vertices[(idx[j].vertex_index * 3) + 0],
						attrib.vertices[(idx[j].vertex_index * 3) + 1],
						attrib.vertices[(idx[j].vertex_index * 3) + 2]
				);
				triangle.points[j].uv = glm::vec2(
						 attrib.texcoords[(idx[j].texcoord_index * 2) + 0],
						-attrib.texcoords[(idx[j].texcoord_index * 2) + 1]
				);
				triangle.points[j].normal = glm::vec3(
						attrib.normals[(idx[j].normal_index * 3) + 0],
						attrib.normals[(idx[j].normal_index * 3) + 1],
						attrib.normals[(idx[j].normal_index * 3) + 2]
				);
			}

			int material = shape.mesh.material_ids[i];
			builder.addTriangle(triangle, (material == -1 ? shape.name : materials[material].diffuse_texname));

			vertStart += 3;
		}

		DIF::DIF dif;
		builder.build(dif);

		std::ofstream outStr(std::string(argv[2]) + shape.name + ".dif");
		dif.write(outStr, DIF::Version());
	}


	return 0;
}