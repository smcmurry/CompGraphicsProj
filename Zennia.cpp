#include "Zennia.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include <CSCI441/objects.hpp>
#include <CSCI441/OpenGLUtils.hpp>

#ifndef M_PI
#define M_PI 3.14159265
#endif

Zennia::Zennia(GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation, GLint materialColorUniformLocation, Engine::MeshData* meshes)
{
    _propAngle = 0.0f;
    _propAngleRotationSpeed = M_PI / 32.0f;

    _shaderProgramHandle = shaderProgramHandle;
    _shaderProgramUniformLocations.mvpMtx = mvpMtxUniformLocation;
    _shaderProgramUniformLocations.normalMtx = normalMtxUniformLocation;
    _shaderProgramUniformLocations.materialColor = materialColorUniformLocation;

    _rotateZenniaAngle = M_PI / 2.0f;

    _rotateWingAngle = M_PI / 2.0f;

    _color = glm::vec3(1.0f, 0.0f, 1.0f);

    _scaleProp = glm::vec3(0.2f, 0.2f, 0.2f);
    _transProp = glm::vec3(0.1f, 0.0f, 0.0f);
    meshData = Engine::_getVao(*meshes);
    _VBO_SIZE = meshes->points.size();
    for(int i = 0; i < meshes->indicies.size(); i++)
    {
        _IBO_SIZE.push_back(meshes->indicies[i].size());
    }
    x = 0;
    y = 0;
    angle = 0;
}

void Zennia::drawZennia(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx)
{
    glUseProgram(_shaderProgramHandle);

    modelMtx = glm::translate(modelMtx, glm::vec3(x, 0.2, y));
    modelMtx = glm::rotate(modelMtx, -angle, CSCI441::Y_AXIS);
    modelMtx = glm::rotate(modelMtx, static_cast<GLfloat>(-M_PI / 4.0f), CSCI441::Z_AXIS);
    modelMtx = glm::rotate(modelMtx, _propAngle, CSCI441::Y_AXIS);
    modelMtx = glm::scale(modelMtx, _scaleProp);

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
    drawObj(0, glm::vec3(0.0f, 0.0f, 0.0f));
    drawObj(1, glm::vec3(1.0f, 0.0f, 1.0f));
    drawObj(2, glm::vec3(1.0f, 1.0f, 0.0f));
    drawObj(3, glm::vec3(1.0f, 0.0f, 0.0f));
}

void Zennia::drawObj(int idx, glm::vec3 color) {
    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &color[0]);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindVertexArray(meshData.first);
    glBindBuffer(GL_ARRAY_BUFFER, meshData.second.first);
    GLint old_id;
    
    if (CSCI441_INTERNAL::_positionAttributeLocation != -1)
    {
        glEnableVertexAttribArray(CSCI441_INTERNAL::_positionAttributeLocation);
        glVertexAttribPointer(CSCI441_INTERNAL::_positionAttributeLocation, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    }
    if (CSCI441_INTERNAL::_normalAttributeLocation != -1)
    {
        glEnableVertexAttribArray(CSCI441_INTERNAL::_normalAttributeLocation);
        glVertexAttribPointer(CSCI441_INTERNAL::_normalAttributeLocation, 3, GL_FLOAT, GL_FALSE, 0, (void *)(sizeof(GLfloat) * _VBO_SIZE * 3));
    }
    if (CSCI441_INTERNAL::_texCoordAttributeLocation != -1)
    {
        glEnableVertexAttribArray(CSCI441_INTERNAL::_texCoordAttributeLocation);
        glVertexAttribPointer(CSCI441_INTERNAL::_texCoordAttributeLocation, 2, GL_FLOAT, GL_FALSE, 0, (void *)(sizeof(GLfloat) * _VBO_SIZE * 6));
    }
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &old_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData.second.second[idx]);
    glDrawElements(GL_TRIANGLES, _IBO_SIZE[idx], GL_UNSIGNED_INT, (void *)0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, old_id);
}

void Zennia::flyForward()
{
    _propAngle += _propAngleRotationSpeed;
    if (_propAngle > 2.0f * M_PI)
        _propAngle -= 2.0f * M_PI;
    move(0.01f * cos(angle), 0.01f * sin(angle));
}
const int WORLD_SIZE = 55.0;
void Zennia::flyBackward()
{
    _propAngle -= _propAngleRotationSpeed;
    if (_propAngle < 0.0f)
        _propAngle += 2.0f * M_PI;
    move(0.01f * cos(angle), 0.01f * sin(angle));
}
void Zennia::move(float dx, float dy)
{
    x += dx;
    y += dy;
    if(x > WORLD_SIZE)
        x = WORLD_SIZE;
    if(x < -WORLD_SIZE)
        x = -WORLD_SIZE;
    if(y > WORLD_SIZE)
        y = WORLD_SIZE;
    if(y < -WORLD_SIZE)
        y = -WORLD_SIZE;
}

void Zennia::_computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const
{
    // precompute the Model-View-Projection matrix on the CPU
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    // then send it to the shader on the GPU to apply to every vertex
    glProgramUniformMatrix4fv(_shaderProgramHandle, _shaderProgramUniformLocations.mvpMtx, 1, GL_FALSE, &mvpMtx[0][0]);

    glm::mat3 normalMtx = glm::mat3(glm::transpose(glm::inverse(modelMtx)));
    glProgramUniformMatrix3fv(_shaderProgramHandle, _shaderProgramUniformLocations.normalMtx, 1, GL_FALSE, &normalMtx[0][0]);
}

void Zennia::freeData() {
    glDeleteBuffers(1, &meshData.first);
    glDeleteBuffers(1, &meshData.second.first);
    glDeleteBuffers(meshData.second.second.size(), &meshData.second.second[0]);
}
