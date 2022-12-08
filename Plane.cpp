#include "Plane.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <CSCI441/objects.hpp>
#include <CSCI441/OpenGLUtils.hpp>

#ifndef M_PI
#define M_PI 3.14159265
#endif

Plane::Plane( GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation, GLint materialColorUniformLocation ) {
    _propAngle = 0.0f;
    _propAngleRotationSpeed = M_PI / 16.0f;

    _shaderProgramHandle                            = shaderProgramHandle;
    _shaderProgramUniformLocations.mMtx           = mvpMtxUniformLocation;
    _shaderProgramUniformLocations.normalMtx        = normalMtxUniformLocation;
    _shaderProgramUniformLocations.materialColor    = materialColorUniformLocation;

    _rotatePlaneAngle = M_PI / 2.0f;

    _orientation = {0,0};
    _position = {0, 2, 0};
    _colorBody = glm::vec3( 0.0f, 0.0f, 1.0f );
    _scaleBody = glm::vec3( 2.0f, 0.5f, 1.0f );

    _colorWing = glm::vec3( 1.0f, 0.0f, 0.0f );
    _scaleWing = glm::vec3( 1.5f, 0.5f, 1.0f );
    _rotateWingAngle = M_PI / 2.0f;

    _colorNose = glm::vec3( 0.0f, 1.0f, 0.0f );
    _rotateNoseAngle = M_PI / 2.0f;

    _colorProp = glm::vec3( 1.0f, 1.0f, 1.0f );
    _scaleProp = glm::vec3( 1.1f, 1.0f, 0.025f );
    _transProp = glm::vec3( 0.1f, 0.0f, 0.0f );

    _colorTail = glm::vec3( 1.0f, 1.0f, 0.0f );
}

void Plane::drawPlane( glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) {
    glUseProgram( _shaderProgramHandle );

    //modelMtx = glm::translate(modelMtx, getPosition());
    modelMtx = glm::rotate( modelMtx, _rotatePlaneAngle + (GLfloat)M_PI/2.0f, CSCI441::Y_AXIS );
    modelMtx = glm::rotate( modelMtx, _orientation.theta, CSCI441::Y_AXIS );
    _drawPlaneBody(modelMtx, viewMtx, projMtx);        // the body of our plane
    _drawPlaneWing(true, modelMtx, viewMtx, projMtx);  // the left wing
    _drawPlaneWing(false, modelMtx, viewMtx, projMtx); // the right wing
    _drawPlaneNose(modelMtx, viewMtx, projMtx);        // the nose
    _drawPlanePropeller(modelMtx, viewMtx, projMtx);   // the propeller
    _drawPlaneTail(modelMtx, viewMtx, projMtx);        // the tail
}

void Plane::flyForward(glm::vec3 pos) {
        _position += pos;
        _propAngle += _propAngleRotationSpeed;
        if( _propAngle > 2.0f * M_PI ) _propAngle -= 2.0f * M_PI;
}

void Plane::flyBackward(glm::vec3 pos) {
        _position += pos;
        _propAngle -= _propAngleRotationSpeed;
        if (_propAngle < 0.0f) _propAngle += 2.0f * M_PI;
}

void Plane::_drawPlaneBody(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    modelMtx = glm::scale( modelMtx, _scaleBody );

    _computeAndSendMatrixUniforms(modelMtx);

    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_colorBody[0]);

    CSCI441::drawSolidCube( 0.1 );
}

void Plane::_drawPlaneWing(bool isLeftWing, glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    modelMtx = glm::scale( modelMtx, _scaleWing );

    if( isLeftWing )
        modelMtx = glm::rotate( modelMtx, -_rotateWingAngle, CSCI441::X_AXIS );
    else
        modelMtx = glm::rotate( modelMtx, _rotateWingAngle, CSCI441::X_AXIS );

    _computeAndSendMatrixUniforms(modelMtx);

    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_colorWing[0]);

    CSCI441::drawSolidCone( 0.05, 0.2, 16, 16 );
}

void Plane::_drawPlaneNose(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    modelMtx = glm::rotate( modelMtx, _rotateNoseAngle, CSCI441::Z_AXIS );

    _computeAndSendMatrixUniforms(modelMtx);

    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_colorNose[0]);

    CSCI441::drawSolidCone( 0.025, 0.3, 16, 16 );
}

void Plane::_drawPlanePropeller(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    glm::mat4 modelMtx1 = glm::translate( modelMtx, _transProp );
    modelMtx1 = glm::rotate( modelMtx1, _propAngle, CSCI441::X_AXIS );
    modelMtx1 = glm::scale( modelMtx1, _scaleProp );

    _computeAndSendMatrixUniforms(modelMtx1);

    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_colorProp[0]);

    CSCI441::drawSolidCube( 0.1 );

    glm::mat4 modelMtx2 = glm::translate( modelMtx, _transProp );
    modelMtx2 = glm::rotate( modelMtx2, static_cast<GLfloat>(M_PI / 2.0f) + _propAngle, CSCI441::X_AXIS );
    modelMtx2 = glm::scale( modelMtx2, _scaleProp );

    _computeAndSendMatrixUniforms(modelMtx2);

    CSCI441::drawSolidCube( 0.1 );
}

void Plane::_drawPlaneTail(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    _computeAndSendMatrixUniforms(modelMtx);

    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_colorTail[0]);

    CSCI441::drawSolidCone( 0.02, 0.1, 16, 16 );
}

void Plane::_computeAndSendMatrixUniforms(glm::mat4 modelMtx) const {
    // precompute the Model-View-Projection matrix on the CPU
    // then send it to the shader on the GPU to apply to every vertex
    glProgramUniformMatrix4fv( _shaderProgramHandle, _shaderProgramUniformLocations.mMtx, 1, GL_FALSE, &modelMtx[0][0] );

    glm::mat3 normalMtx = glm::mat3( glm::transpose( glm::inverse( modelMtx )));
    glProgramUniformMatrix3fv( _shaderProgramHandle, _shaderProgramUniformLocations.normalMtx, 1, GL_FALSE, &normalMtx[0][0] );
}


glm::vec3 Plane::getPosition() const
{
    return _position;
}

void Plane::rotate(GLfloat angle)
{
    _orientation.theta += angle;
}

bool Plane::isInBounds(glm::vec3 pos)
{
    glm::vec3 temp = _position + pos;
    if (temp.x >= lowerBound.x && temp.x <= upperBound.x && temp.z >= lowerBound.y && temp.z <= upperBound.y)
    {
        return true;
    }
    return false;
}

void Plane::setBounds(glm::vec2 lower, glm::vec2 upper)
{
    lowerBound = lower;
    upperBound = upper;
}

Plane::Orientation Plane::getOrientation()
{
    return _orientation;
}

