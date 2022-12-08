#include "Raistlin.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <CSCI441/objects.hpp>
#include <CSCI441/OpenGLUtils.hpp>

#ifndef M_PI
#define M_PI 3.14159265
#endif

Raistlin::Raistlin( GLuint shaderProgramHandle, GLint mMtxUniformLocation, GLint normalMtxUniformLocation, GLint materialColorUniformLocation ) {
    _shaderProgramHandle                            = shaderProgramHandle;
    _shaderProgramUniformLocations.mMtx             = mMtxUniformLocation;
    _shaderProgramUniformLocations.normalMtx        = normalMtxUniformLocation;
    _shaderProgramUniformLocations.materialColor    = materialColorUniformLocation;
    _colorBody = glm::vec3(0.5, 0.2, 0.2);
    _colorStaff = glm::vec3(0.3, 0.2, 0.6);
    _scaleHead= glm::vec3(1.0, 1.0, 1.0);
    _scaleBody = glm::vec3(1.0, 1.0, 1.0);
    _scaleStaff= glm::vec3(1.0, 1.0, 1.0);
    x = 0;
    y = 0;
    angle = 0;
    staff_angle = 0;
}

void Raistlin::drawRaistlin( glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) {
    glUseProgram( _shaderProgramHandle );
    modelMtx = glm::translate(modelMtx, glm::vec3(x, 0, y));
    modelMtx = glm::rotate(modelMtx, -angle, CSCI441::Y_AXIS);
    _drawBody(modelMtx, viewMtx, projMtx);
    modelMtx = glm::translate(modelMtx, glm::vec3(0.0f, 0.5f, 0.0f));
    _drawHead(modelMtx, viewMtx, projMtx);
    modelMtx = glm::rotate(modelMtx, staff_angle, CSCI441::Y_AXIS);
    modelMtx = glm::translate(modelMtx, glm::vec3(0.8f, 0.0f, 0.0f));
    _drawStaff(modelMtx, viewMtx, projMtx);
}

void Raistlin::walkForward() {

    move(-0.01f * cos(angle), -0.01f * sin(angle));
}
const int WORLD_SIZE = 55.0;
void Raistlin::move(float dx, float dy)
{
    x += dx;
    y += dy;
    staff_angle += 0.1f;
    if (x > WORLD_SIZE)
        x = WORLD_SIZE;
    if (x < -WORLD_SIZE)
        x = -WORLD_SIZE;
    if (y > WORLD_SIZE)
        y = WORLD_SIZE;
    if (y < -WORLD_SIZE)
        y = -WORLD_SIZE;
}

void Raistlin::_drawBody(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    modelMtx = glm::scale( modelMtx, _scaleBody );
    _computeAndSendMatrixUniforms(modelMtx);
    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_colorBody[0]);
    CSCI441::drawSolidCone( 0.1f, 0.5f, 20, 20 );
}

void Raistlin::_drawHead(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    _computeAndSendMatrixUniforms(modelMtx);
    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_colorBody[0]);
    CSCI441::drawSolidSphere( 0.12, 16, 16 );
}

void Raistlin::_drawStaff(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {

    modelMtx = glm::scale( modelMtx, _scaleStaff);
    _computeAndSendMatrixUniforms(modelMtx);
    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_colorStaff[0]);
    CSCI441::drawSolidCylinder( 0.02,0.02, 1.2, 16, 16 );
}


void Raistlin::_computeAndSendMatrixUniforms(glm::mat4 modelMtx) const {
    // precompute the Model-View-Projection matrix on the CPU
    // then send it to the shader on the GPU to apply to every vertex
    glProgramUniformMatrix4fv( _shaderProgramHandle, _shaderProgramUniformLocations.mMtx, 1, GL_FALSE, &modelMtx[0][0] );
    glm::mat3 normalMtx = glm::mat3( glm::transpose( glm::inverse( modelMtx )));
    glProgramUniformMatrix3fv( _shaderProgramHandle, _shaderProgramUniformLocations.normalMtx, 1, GL_FALSE, &normalMtx[0][0] );
}
