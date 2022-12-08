#include "Engine.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>
#include <CSCI441/OpenGLEngine.hpp>
#include <CSCI441/objects.hpp>
#include <CSCI441/OpenGLUtils.hpp>

//*************************************************************************************
//
// Helper Functions
namespace Engine
{
    // Reads in geometry data from a file.
    MeshData *readOBJ(std::string filename)
    {

        std::string myText;

        // Read from the text file
        std::ifstream MyReadFile(filename);

        // Use a while loop together with the getline() function to read the file line by line
        std::vector<glm::vec3> verticies;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> tex_coords;
        std::vector<glm::vec3> points;
        std::vector<glm::vec3> point_normals;
        std::vector<glm::vec2> point_coords;
        std::vector<std::vector<uint32_t>> indices;

        std::cout << MyReadFile.is_open() << std::endl;
        int i = 0;
        while (getline(MyReadFile, myText))
        {
            std::stringstream spliter(myText);
            std::string segment;
            getline(spliter, segment, ' ');
            if (segment == "o")
            {
                std::vector<uint32_t> newData4;
                indices.push_back(newData4);
            }
            else if (segment == "v")
            {
                getline(spliter, segment, ' ');
                std::stringstream s(segment);
                float x;
                s >> x;
                getline(spliter, segment, ' ');
                std::stringstream s2(segment);
                float y;
                s2 >> y;
                getline(spliter, segment, ' ');
                std::stringstream s3(segment);
                float z;
                s3 >> z;
                verticies.push_back(glm::vec3(x, y, z));
                points.push_back(glm::vec3(x, y, z));
                point_normals.push_back(glm::vec3(0, 0, 0));
                point_coords.push_back(glm::vec2(0, 0));
            }
            else if (segment == "vn")
            {
                getline(spliter, segment, ' ');
                std::stringstream s(segment);
                float x;
                s >> x;
                getline(spliter, segment, ' ');
                std::stringstream s2(segment);
                float y;
                s2 >> y;
                getline(spliter, segment, ' ');
                std::stringstream s3(segment);
                float z;
                s3 >> z;
                normals.push_back(glm::vec3(x, y, z));
            }
            else if (segment == "vt")
            {
                getline(spliter, segment, ' ');
                std::stringstream s(segment);
                float x;
                s >> x;
                getline(spliter, segment, ' ');
                std::stringstream s2(segment);
                float y;
                s2 >> y;
                tex_coords.push_back(glm::vec2(x, y));
            }
            else if (segment == "f")
            {
                int id;
                int t_id;
                int n_id;
                std::string vertexData;
                getline(spliter, segment, ' ');
                std::stringstream s(segment);
                {
                    getline(s, vertexData, '/');
                    std::stringstream s2(vertexData);
                    s2 >> id;
                }
                {
                    getline(s, vertexData, '/');
                    if (vertexData == "")
                    {
                        t_id = -1;
                    }
                    else
                    {
                        std::stringstream s2(vertexData);
                        s2 >> t_id;
                    }
                }
                {
                    getline(s, vertexData, '/');
                    std::stringstream s2(vertexData);
                    s2 >> n_id;
                }
                indices[indices.size() - 1].push_back(id - 1);
                points[id - 1] = verticies[id - 1];
                point_normals[id - 1] = normals[n_id - 1];
                if (t_id != -1)
                {
                    point_coords[id - 1] = tex_coords[t_id - 1];
                }
                else
                {
                    point_coords[id - 1] = glm::vec2(0, 0);
                }
                getline(spliter, segment, ' ');
                std::stringstream s3(segment);
                {
                    getline(s3, vertexData, '/');
                    std::stringstream s2(vertexData);
                    s2 >> id;
                }
                {
                    getline(s3, vertexData, '/');
                    if (vertexData == "")
                    {
                        t_id = -1;
                    }
                    else
                    {
                        std::stringstream s2(vertexData);
                        s2 >> t_id;
                    }
                    std::stringstream s2(vertexData);
                    s2 >> t_id;
                }
                {
                    getline(s3, vertexData, '/');
                    std::stringstream s2(vertexData);
                    s2 >> n_id;
                }
                indices[indices.size() - 1].push_back(id - 1);
                points[id - 1] = verticies[id - 1];
                point_normals[id - 1] = normals[n_id - 1];
                if (t_id != -1)
                {
                    point_coords[id - 1] = tex_coords[t_id - 1];
                }
                else
                {
                    point_coords[id - 1] = glm::vec2(0, 0);
                }
                getline(spliter, segment, ' ');
                std::stringstream s5(segment);
                {
                    getline(s5, vertexData, '/');
                    std::stringstream s2(vertexData);
                    s2 >> id;
                }
                {
                    getline(s5, vertexData, '/');
                    if (vertexData == "")
                    {
                        t_id = -1;
                    }
                    else
                    {
                        std::stringstream s2(vertexData);
                        s2 >> t_id;
                    }
                }
                {
                    getline(s5, vertexData, '/');
                    std::stringstream s2(vertexData);
                    s2 >> n_id;
                }
                indices[indices.size() - 1].push_back(id - 1);
                points[id - 1] = verticies[id - 1];
                point_normals[id - 1] = normals[n_id - 1];
                if (t_id != -1)
                {
                    point_coords[id - 1] = tex_coords[t_id - 1];
                }
                else
                {
                    point_coords[id - 1] = glm::vec2(0, 0);
                }
            }
        }

        // Close the file
        MyReadFile.close();

        auto data = new MeshData{
                points,
                point_normals,
                point_coords,
                indices};
        return data;
    }

    // Generate vao, vbo, and ibos for the mesh from readObj
    BufferData _getVao(MeshData meshes)
    {
        GLuint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        GLuint vbo;
        glGenBuffers(1, &vbo);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * meshes.points.size() * 8, nullptr, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * meshes.points.size() * 3, &meshes.points[0]);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * meshes.points.size() * 3, sizeof(GLfloat) * meshes.points.size() * 3, &meshes.normals[0]);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * meshes.points.size() * 6, sizeof(GLfloat) * meshes.points.size() * 2, &meshes.texCoords[0]);

        std::vector<GLuint> ibos;
        for (int i = 0; i < meshes.indicies.size(); i++)
        {
            ibos.push_back(0);
            glGenBuffers(1, &ibos[i]);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibos[i]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * meshes.indicies[i].size(), 0, GL_STATIC_DRAW);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(uint32_t) * meshes.indicies[i].size(), &meshes.indicies[i][0]);
        }

        std::vector<GLuint> ibo_sizes;

        for(int i = 0; i < meshes.indicies.size(); i++)
        {
            ibo_sizes.push_back(meshes.indicies[i].size());
        }

        return BufferData
                {
                        vao,
                        vbo,
                        ibos,
                        (GLuint) meshes.points.size(),
                        ibo_sizes,
                };
    }
}

//Render a mesh at the given index from a buffer data from  the _getVao function
void Engine::drawObj(BufferData meshData, int idx)
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindVertexArray(meshData.vao);
    glBindBuffer(GL_ARRAY_BUFFER, meshData.vbo);
    GLint old_id;

    if (CSCI441_INTERNAL::_positionAttributeLocation != -1)
    {
        glEnableVertexAttribArray(CSCI441_INTERNAL::_positionAttributeLocation);
        glVertexAttribPointer(CSCI441_INTERNAL::_positionAttributeLocation, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    }
    if (CSCI441_INTERNAL::_normalAttributeLocation != -1)
    {
        glEnableVertexAttribArray(CSCI441_INTERNAL::_normalAttributeLocation);
        glVertexAttribPointer(CSCI441_INTERNAL::_normalAttributeLocation, 3, GL_FLOAT, GL_FALSE, 0, (void *)(sizeof(GLfloat) * meshData.vbo_size * 3));
    }
    if (CSCI441_INTERNAL::_texCoordAttributeLocation != -1)
    {
        glEnableVertexAttribArray(CSCI441_INTERNAL::_texCoordAttributeLocation);
        glVertexAttribPointer(CSCI441_INTERNAL::_texCoordAttributeLocation, 2, GL_FLOAT, GL_FALSE, 0, (void *)(sizeof(GLfloat) * meshData.vbo_size * 6));
    }
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &old_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData.ibos[idx]);
    glDrawElements(GL_TRIANGLES, meshData.ibo_size[idx], GL_UNSIGNED_INT, (void *)0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, old_id);
}