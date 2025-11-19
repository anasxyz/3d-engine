#define TINYOBJLOADER_IMPLEMENTATION
#include "../external/tinyobjloader/tiny_obj_loader.h"

#include "../include/Mesh.h"
#include "../include/ObjectLoader.h"
#include "../include/TextureLoader.h"

#include <iostream>
#include <unordered_map>
#include <vector>

std::string objBaseDir = "assets/models/";
std::string mtlBaseDir = "assets/materials/";

Mesh ObjectLoader::loadOBJModel(
    const std::string &filename,
    const glm::vec4 &defaultColour = glm::vec4(1.0f)) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;
  std::unordered_map<int, GLuint> materialTextures;

  std::string objPath = objBaseDir + filename;

  bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err,
                              objPath.c_str(), mtlBaseDir.c_str(), true);
  if (!warn.empty())
    std::cout << "WARN: " << warn << "\n";
  if (!err.empty())
    std::cerr << "ERR: " << err << "\n";
  if (!ret)
    throw std::runtime_error("Failed to load OBJ: " + filename);
	if (!materials.empty()) {
		std::cout << "Loaded material: " << materials[0].name << std::endl;
	}

  for (size_t i = 0; i < materials.size(); i++) {
    const std::string &texName = materials[i].diffuse_texname;
    if (!texName.empty()) {
      std::string texPath = texName;
      GLuint texID = TextureLoader::loadTexture(texPath);
      materialTextures[i] = texID;
    }
  }

  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;

  struct Key {
    int vi, ni, ti;
    bool operator==(Key const &o) const {
      return vi == o.vi && ni == o.ni && ti == o.ti;
    }
  };
  struct KeyHash {
    std::size_t operator()(Key const &k) const {
      return ((std::hash<int>()(k.vi) ^ (std::hash<int>()(k.ni) << 1)) >> 1) ^
             (std::hash<int>()(k.ti) << 1);
    }
  };
  std::unordered_map<Key, GLuint, KeyHash> mapKeyToIndex;

  for (size_t s = 0; s < shapes.size(); s++) {
    size_t index_offset = 0;
    const auto &mesh = shapes[s].mesh;

    for (size_t f = 0; f < mesh.num_face_vertices.size(); f++) {
      int fv = mesh.num_face_vertices[f];

      int currentMatID = mesh.material_ids[f];

      for (int v = 0; v < fv; v++) {
        tinyobj::index_t idx = mesh.indices[index_offset + v];
        Key key{idx.vertex_index, idx.normal_index, idx.texcoord_index};

        auto it = mapKeyToIndex.find(key);
        if (it == mapKeyToIndex.end()) {
          Vertex vert{};
          vert.position = {attrib.vertices[3 * idx.vertex_index + 0],
                           attrib.vertices[3 * idx.vertex_index + 1],
                           attrib.vertices[3 * idx.vertex_index + 2]};

          // assign colour from material if it exists
          if (currentMatID >= 0 && currentMatID < (int)materials.size()) {
            const auto &mat = materials[currentMatID];
            vert.colour =
                glm::vec4(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2], 1.0f);
          } else {
            vert.colour = defaultColour;
          }

          if (idx.normal_index >= 0) {
            vert.normal = {attrib.normals[3 * idx.normal_index + 0],
                           attrib.normals[3 * idx.normal_index + 1],
                           attrib.normals[3 * idx.normal_index + 2]};
          } else {
            vert.normal = glm::vec3(0.0f, 0.0f, 0.0f);
          }

          if (idx.texcoord_index >= 0) {
            vert.uv = {attrib.texcoords[2 * idx.texcoord_index + 0],
                       attrib.texcoords[2 * idx.texcoord_index + 1]};
          } else {
            vert.uv = glm::vec2(0.0f, 0.0f);
          }

          GLuint newIndex = (GLuint)vertices.size();
          mapKeyToIndex[key] = newIndex;
          vertices.push_back(vert);
          indices.push_back(newIndex);
        } else {
          indices.push_back(it->second);
        }
      }
      index_offset += fv;
    }
  }

  Mesh mesh;
  mesh.setup(vertices, indices);

	std::cout << "Loaded object: " << objPath << std::endl;

  return mesh;
}
