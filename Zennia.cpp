#include "Zennia.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include <CSCI441/objects.hpp>
#include <CSCI441/OpenGLUtils.hpp>

#ifndef M_PI
#define M_PI 3.14159265
#endif

Zennia::Zennia(GLuint shaderProgramHandle, GLint mMtxUniformLocation, GLint normalMtxUniformLocation, GLint materialColorUniformLocation, Engine::MeshData *meshes)
{
    //Initialize variables
    _propAngle = 0.0f;
    _propAngleRotationSpeed = M_PI / 32.0f;

    _shaderProgramHandle = shaderProgramHandle;
    _shaderProgramUniformLocations.mMtx = mMtxUniformLocation;
    _shaderProgramUniformLocations.normalMtx = normalMtxUniformLocation;
    _shaderProgramUniformLocations.materialColor = materialColorUniformLocation;

    _rotateZenniaAngle = M_PI / 2.0f;

    _rotateWingAngle = M_PI / 2.0f;

    _color = glm::vec3(1.0f, 0.0f, 1.0f);

    _scaleProp = glm::vec3(0.2f, 0.2f, 0.2f);
    _transProp = glm::vec3(0.1f, 0.0f, 0.0f);
    meshData = Engine::_getVao(*meshes);
    _VBO_SIZE = meshes->points.size();
    for (int i = 0; i < meshes->indicies.size(); i++)
    {
        _IBO_SIZE.push_back(meshes->indicies[i].size());
    }
    x = 0;
    y = 0;
    angle = 0;
}

void Zennia::drawZennia(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx)
{
    modelMtx = glm::translate(modelMtx, glm::vec3(x, 0.2, y));
    modelMtx = glm::rotate(modelMtx, -angle, CSCI441::Y_AXIS);
    modelMtx = glm::rotate(modelMtx, static_cast<GLfloat>(-M_PI / 4.0f), CSCI441::Z_AXIS); // put the flower at a diagonal angle
    modelMtx = glm::rotate(modelMtx, _propAngle, CSCI441::Y_AXIS); //animate the rotation of the flower.
    modelMtx = glm::scale(modelMtx, _scaleProp);

    _computeAndSendMatrixUniforms(modelMtx);
    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &glm::vec3(0.1f, 0.1f, 0.1f)[0]);
    Engine::drawObj(meshData, 0); // draw the stem
    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &glm::vec3(1.0f, 0.1f, 1.0f)[0]);
    Engine::drawObj(meshData, 1); // draw the petals
    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &glm::vec3(1.0f, 1.0f, 0.1f)[0]);
    Engine::drawObj(meshData, 2); // draw the center flowery things
    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &glm::vec3(1.0f, 0.1f, 0.1f)[0]);
    Engine::drawObj(meshData, 3); // draw the cylindery things near the center
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
    move(-0.01f * cos(angle), -0.01f * sin(angle));
}
void Zennia::move(float dx, float dy)
{
    x += dx;
    y += dy;
    //collision detection with edge
    if (x > WORLD_SIZE)
        x = WORLD_SIZE;
    if (x < -WORLD_SIZE)
        x = -WORLD_SIZE;
    if (y > WORLD_SIZE)
        y = WORLD_SIZE;
    if (y < -WORLD_SIZE)
        y = -WORLD_SIZE;
}

void Zennia::_computeAndSendMatrixUniforms(glm::mat4 modelMtx) const {
    // then send it to the shader on the GPU to apply to every vertex
    glProgramUniformMatrix4fv(_shaderProgramHandle, _shaderProgramUniformLocations.mMtx, 1, GL_FALSE, &modelMtx[0][0]);

    glm::mat3 normalMtx = glm::mat3(glm::transpose(glm::inverse(modelMtx)));
    glProgramUniformMatrix3fv(_shaderProgramHandle, _shaderProgramUniformLocations.normalMtx, 1, GL_FALSE, &normalMtx[0][0]);
}

void Zennia::freeData()
{
    glDeleteBuffers(1, &meshData.vao);
    glDeleteBuffers(1, &meshData.vbo);
    glDeleteBuffers(meshData.ibos.size(), &meshData.ibos[0]);
}
