#ifndef ENGINE_H
#define ENGINE_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>
#include <CSCI441/OpenGLEngine.hpp>

//*************************************************************************************
//
// Helper Functions
namespace Engine {
    struct MeshData
    {
        std::vector<glm::vec3> points;
        std::vector<glm::vec3> normals; 
        std::vector<glm::vec2> texCoords;
        std::vector<std::vector<uint32_t>> indicies;
    };
    MeshData* readOBJ(std::string filename);

    std::pair<GLuint, std::pair<GLuint, std::vector<GLuint>>> _getVao(MeshData meshes);
}
#endif