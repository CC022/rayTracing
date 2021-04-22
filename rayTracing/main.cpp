#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "glm.hpp"
#include "matrix_transform.hpp"
#include "quaternion.hpp"
#include "renderer.hpp"

using std::cout; using std::endl;

class wrappedBuffer {
public:
    size_t size;
    void* ptr = nullptr;
    wrappedBuffer(size_t size) : size(size) {ptr = ::operator new(size);}
    ~wrappedBuffer() {::operator delete(ptr);}
};

glm::mat4 getLocalTransformMatrixFromNode(tinygltf::Node &node) {
    glm::mat4 matrix(1.0f);
    if (node.matrix.size() == 16) {
        for (int i=0; i<4; i++) {
            for (int j=0; j<4; j++) {
                matrix[i][j] = static_cast<float>(node.matrix[4*i + j]);
            }
        }
    } else {
        if (node.scale.size() == 3) {
            glm::scale(matrix, glm::vec3(node.scale[0], node.scale[1], node.scale[2]));
        }
        if (node.rotation.size() == 4) {
            matrix = matrix * glm::mat4_cast(glm::quat(node.rotation[0],node.rotation[1],node.rotation[2],node.rotation[3]));
        }
        if (node.translation.size() == 3) {
            glm::translate(matrix, glm::vec3(node.translation[0], node.translation[1], node.translation[2]));
        }
    }
    return matrix;
}

struct vertexOuput {
    glm::vec4 pos;
    glm::vec3 normal;
    glm::vec2 texCoord0;
};

struct primitiveGroup{
    size_t primitiveCount = 0;
    size_t verticesCount = 0;
    std::vector<unsigned short> indices;
    std::vector<glm::vec3> position;
    std::vector<glm::vec3> normal;
    std::vector<glm::vec2> texCoord;
    std::vector<vertexOuput> verticesOutput;
};

struct primitive {
    int primitiveMode = TINYGLTF_MODE_TRIANGLES;
    vertexOuput vertices[3];
};

template<typename T>
void copyFromBuffer(std::vector<T>&dst, uint8_t *srcBuffer, size_t byteStride, size_t byteOffset) {
    byteStride = byteStride == 0 ? sizeof(T) : byteStride;
    for (size_t i=0; i<dst.size(); i++) {
        dst[i] = *(reinterpret_cast<T*>(srcBuffer + byteOffset + i*byteStride));
    }
}

inline void vertexShader(primitiveGroup &primitiveGroup);

inline void primitiveAssembly(size_t indexId, size_t idxOffset, std::vector<primitive> &primitives, primitiveGroup &primitiveGroup);

class gltfLoader {
    tinygltf::Model model;
    tinygltf::TinyGLTF tinyGltfCtx;
    std::string err;
    std::string warn;
    std::vector<wrappedBuffer> bufferviewMemoryList;
    std::vector<std::vector<primitiveGroup>> primitiveGroupListPerMesh;
    std::vector<glm::mat4> nodeGlobalTransformList;

    void traverseNode(size_t nodeIdx, tinygltf::Model &model, glm::mat4 globalTransformMatrix ,std::vector<glm::mat4> &nodeGlobalTransformList) {
        tinygltf::Node &node = model.nodes.at(nodeIdx);
        // process node
        nodeGlobalTransformList.at(nodeIdx) = globalTransformMatrix * getLocalTransformMatrixFromNode(node);
        for (int childIdx : node.children) {
            traverseNode(childIdx, model, nodeGlobalTransformList.at(nodeIdx) ,nodeGlobalTransformList);
        }
    }
    
public:
    std::vector<primitive> primitives; // vector holding all primitives in the model
    bool loadGLTFFile(std::string file) {
        return tinyGltfCtx.LoadASCIIFromFile(&model, &err, &warn, file);
    }
    
    void loadBufferViewsIntoMemory() {
        bufferviewMemoryList.reserve(model.bufferViews.size());
        // copy all bufferviews to memory
        for (auto bufferview : model.bufferViews) {
            bufferviewMemoryList.emplace_back(bufferview.byteLength);
            void *bufferPtr = bufferviewMemoryList.back().ptr;
            memcpy(bufferPtr, model.buffers.at(bufferview.buffer).data.data() + bufferview.byteOffset, bufferview.byteLength);
        }
    }
    
    void preprocessNodes() {
        nodeGlobalTransformList.resize(model.nodes.size());
        tinygltf::Scene &defaultScene = (model.defaultScene >= 0) ? model.scenes.at(model.defaultScene) : model.scenes.at(0);
        traverseNode(defaultScene.nodes.front(), model, glm::mat4(1.0f) ,nodeGlobalTransformList);
    }
    
    void loadDataIntoVectors() {
        // MARK: mesh2PrimitivesMap
        primitiveGroupListPerMesh.resize(model.meshes.size());
        size_t totalPrimitivesCount = 0;
        size_t totalVerticesCount = 0;
        
        for (size_t nodeIdx=0; nodeIdx<model.nodes.size(); nodeIdx++) {
            if (model.nodes.at(nodeIdx).mesh >= 0) {
                size_t meshIdx = model.nodes.at(nodeIdx).mesh;
                tinygltf::Mesh &mesh = model.meshes.at(meshIdx);
                std::vector<primitiveGroup> thisprimitiveGroupList;
                for (auto &primitive : mesh.primitives) {
                    if (primitive.mode != TINYGLTF_MODE_TRIANGLES) {cout << "unsupported primitive mode " << primitive.mode << endl; continue;}
                    primitiveGroup thisprimitiveGroup;
                    auto &indicesAccessor = model.accessors.at(primitive.indices);
                    thisprimitiveGroup.primitiveCount = indicesAccessor.count / 3; // 3 vertices = 1 triangle
                    thisprimitiveGroup.indices.resize(indicesAccessor.count);
                    totalPrimitivesCount = totalPrimitivesCount + thisprimitiveGroup.primitiveCount;
                    copyFromBuffer(thisprimitiveGroup.indices,
                                   static_cast<uint8_t*>(bufferviewMemoryList.at(indicesAccessor.bufferView).ptr),
                                   model.bufferViews.at(indicesAccessor.bufferView).byteStride,
                                   indicesAccessor.byteOffset);
                    
                    for (auto &attribute : primitive.attributes) {
                        auto &accessor = model.accessors.at(attribute.second);
                        thisprimitiveGroup.verticesCount = accessor.count;
                        totalVerticesCount = totalVerticesCount + thisprimitiveGroup.verticesCount;
                        auto &bufferview = model.bufferViews.at(accessor.bufferView);
                        if (attribute.first.compare("NORMAL") == 0) {
                            thisprimitiveGroup.normal.resize(accessor.count);
                            copyFromBuffer(thisprimitiveGroup.normal,
                                           static_cast<uint8_t*>(bufferviewMemoryList.at(accessor.bufferView).ptr),
                                           bufferview.byteStride,
                                           accessor.byteOffset);
                        }
                        else if (attribute.first.compare("POSITION") == 0) {
                            thisprimitiveGroup.position.resize(accessor.count);
                            copyFromBuffer(thisprimitiveGroup.position,
                                           static_cast<uint8_t*>(bufferviewMemoryList.at(accessor.bufferView).ptr),
                                           bufferview.byteStride,
                                           accessor.byteOffset);
                        }
                        else if (attribute.first.compare("TEXCOORD_0") == 0) {
                            thisprimitiveGroup.texCoord.resize(accessor.count);
                            copyFromBuffer(thisprimitiveGroup.texCoord,
                                           static_cast<uint8_t*>(bufferviewMemoryList.at(accessor.bufferView).ptr),
                                           bufferview.byteStride,
                                           accessor.byteOffset);
                        }
                    }
                    thisprimitiveGroup.verticesOutput.resize(thisprimitiveGroup.verticesCount);
                    thisprimitiveGroupList.push_back(thisprimitiveGroup);
                }
                // apply node transform to meshees in the node
                for (auto &primitiveGroup : thisprimitiveGroupList) {
                    for (auto &pos : primitiveGroup.position) {
                        pos = glm::vec3(nodeGlobalTransformList.at(nodeIdx) * glm::vec4(pos, 1.0f));
                    }
                }
                primitiveGroupListPerMesh.at(meshIdx) = thisprimitiveGroupList;
            }
        }
        primitives.resize(totalPrimitivesCount);
    }
    
    void assemblePrimitives() {
        size_t processedPrimitiveCount = 0;
        for (auto &primitiveGroupVector : primitiveGroupListPerMesh) {
            for (auto primitiveGroup : primitiveGroupVector) {
                for (size_t vertexId=0; vertexId<primitiveGroup.verticesCount; vertexId++) {
                    vertexOuput &vertexOut = primitiveGroup.verticesOutput[vertexId];
                    vertexOut.pos = glm::vec4(primitiveGroup.position[vertexId], 1.0f);
                    vertexOut.normal = primitiveGroup.normal[vertexId];
                    vertexOut.texCoord0 = primitiveGroup.texCoord[vertexId];
                }
                for (size_t indexId=0; indexId < primitiveGroup.indices.size(); indexId++) {
                    primitives[indexId / 3 + processedPrimitiveCount].vertices[indexId % 3] = primitiveGroup.verticesOutput[primitiveGroup.indices[indexId]];
                }
                processedPrimitiveCount += primitiveGroup.primitiveCount;
            }
        }
    }
    
};


int main(int argc, const char * argv[]) {
    gltfLoader loader;
    loader.loadGLTFFile("duck.gltf");
    loader.loadBufferViewsIntoMemory();
    loader.preprocessNodes();
    loader.loadDataIntoVectors();
    loader.assemblePrimitives();
    
    renderer renderer0(800, 600, 128 ,vec3(10,7,5), vec3(0, 0.5, 0), 15, 0.01);
    hittable **list = new hittable*[loader.primitives.size()];
    for (size_t i=0; i<loader.primitives.size(); i++) {
        glm::vec4 &pos0 = loader.primitives[i].vertices[0].pos;
        glm::vec4 &pos1 = loader.primitives[i].vertices[1].pos;
        glm::vec4 &pos2 = loader.primitives[i].vertices[2].pos;
        vec3 v0 = vec3(pos0.x, pos0.y, pos0.z);
        vec3 v1 = vec3(pos1.x, pos1.y, pos1.z);
        vec3 v2 = vec3(pos2.x, pos2.y, pos2.z);
        float U0 = loader.primitives[i].vertices[0].texCoord0[0];
        float U1 = loader.primitives[i].vertices[1].texCoord0[0];
        float U2 = loader.primitives[i].vertices[2].texCoord0[0];
        float V0 = loader.primitives[i].vertices[0].texCoord0[1];
        float V1 = loader.primitives[i].vertices[1].texCoord0[1];
        float V2 = loader.primitives[i].vertices[2].texCoord0[1];
        list[i] = new triangle(v0, v1, v2, U0, U1, U2, V0, V1, V2, new lambertian(new imageTexture("DuckCM.ppm")));
    }
    hittableList scene = hittableList(list, loader.primitives.size());
    renderer0.loadWorld(scene);
    renderer0.render();
    renderer0.writeToPPMFile("render.ppm");
    return 0;
}
