#include "Jammss.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <CSCI441/objects.hpp>
#include <CSCI441/OpenGLUtils.hpp>

#ifndef M_PI
#define M_PI 3.14159265
#endif

Jammss::Jammss( GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation, GLint materialColorUniformLocation ) {

    _legAngle1 = -M_PI / 32.0f;
    _legAngle2 = M_PI / 32.0f;
    _legAngle3 = 0.0f;
    _legAngleRotationSpeed = M_PI / 32.0f;

    _leg1Toggle = true;
    _leg2Toggle = false;
    _leg3Toggle = true;

    _shaderProgramHandle                            = shaderProgramHandle;
    _shaderProgramUniformLocations.mvpMtx           = mvpMtxUniformLocation;
    _shaderProgramUniformLocations.normalMtx        = normalMtxUniformLocation;
    _shaderProgramUniformLocations.materialColor    = materialColorUniformLocation;

    _rotatePlaneAngle = M_PI / 2.0f;

    _rotateBodyAngle = -M_PI / 4.0f;

    _colorBody = glm::vec3( 0.82f, 0.80f, 0.76f );
    _scaleBody = glm::vec3( 2.0f, 0.5f, 1.0f );

    _scaleArm = glm::vec3( 0.25f, 0.5f, 0.25f );
    _scaleLeg = glm::vec3( 0.25f, 0.25f, 0.25f );
    _scaleHead = glm::vec3(1.0f, 0.9f,1.1f);

    _transLeftArm = glm::vec3(-0.09f, 0.0f, 0.05f);
    _transRightArm = glm::vec3(-0.09f, 0.0f, -0.05f);

    _colorTree = glm::vec3( 0.40f, 0.22f, 0.09f );
    _rotateLeftArmAngle = M_PI / 1.0f;
    _rotateRightArmAngle = M_PI / 4.0f;
    _rotateOuterArmAngle = M_PI / 8.0f;

    _colorLeaves = glm::vec3( 0.0f, 1.0f, 0.0f );

}

void Jammss::drawJammss( glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) {
    glUseProgram( _shaderProgramHandle );

    modelMtx = glm::rotate( modelMtx, -_rotatePlaneAngle, CSCI441::Y_AXIS );
    modelMtx = glm::rotate( modelMtx, _rotatePlaneAngle, CSCI441::Z_AXIS );

    modelMtx = glm::rotate( modelMtx, _rotateBodyAngle,CSCI441::Z_AXIS);

    _drawPlaneBody(modelMtx, viewMtx, projMtx);        // the body of our plane
    _drawUpperArm(true, modelMtx, viewMtx, projMtx);  // the left wing
    _drawUpperArm(false, modelMtx, viewMtx, projMtx); // the right wing
    _drawForeArm(true, modelMtx, viewMtx, projMtx);
    _drawForeArm(false, modelMtx, viewMtx, projMtx);
    _drawHead(modelMtx, viewMtx, projMtx);        // the nose
    _drawLegs(true, modelMtx, viewMtx, projMtx);
    _drawLegs(false, modelMtx, viewMtx, projMtx);// the propeller
}

void Jammss::walkForward() {
    if (_leg1Toggle) {
        _legAngle1 += _legAngleRotationSpeed;
        if (_legAngle1 > GLfloat(M_PI / 4.0f))
            _leg1Toggle = !_leg1Toggle;
    } else {
        _legAngle1 -= _legAngleRotationSpeed;
        if (_legAngle1 < GLfloat(-M_PI/4.0f))
            _leg1Toggle = !_leg1Toggle;
    }
    if (_leg2Toggle) {
        _legAngle2 += _legAngleRotationSpeed;
        if (_legAngle2 > GLfloat(M_PI / 4.0f))
            _leg2Toggle = !_leg2Toggle;
    } else {
        _legAngle2 -= _legAngleRotationSpeed;
        if (_legAngle2 < GLfloat(-M_PI/4.0f))
            _leg2Toggle = !_leg2Toggle;
    }
    if (_leg3Toggle) {
        _legAngle3 += _legAngleRotationSpeed/4.0f;
        if (_legAngle3 > GLfloat(M_PI / 8.0f))
            _leg3Toggle = !_leg3Toggle;
    } else {
        _legAngle3 -= _legAngleRotationSpeed/4.0f;
        if (_legAngle3 < GLfloat(-M_PI/8.0f))
            _leg3Toggle = !_leg3Toggle;
    }
}


void Jammss::_drawPlaneBody(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    modelMtx = glm::scale( modelMtx, _scaleBody );

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_colorBody[0]);

    CSCI441::drawSolidCube( 0.1 );
}

void Jammss::_drawUpperArm(bool isLeftArm, glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    if( isLeftArm ) {
        modelMtx = glm::translate(modelMtx, _transLeftArm);
        modelMtx = glm::rotate(modelMtx, _rotateLeftArmAngle, CSCI441::Z_AXIS);
        modelMtx = glm::rotate(modelMtx, _rotateOuterArmAngle, CSCI441::X_AXIS);
        modelMtx = glm::rotate(modelMtx, _legAngle3, CSCI441::Z_AXIS);
    } else {
        modelMtx = glm::translate(modelMtx, _transRightArm);
        modelMtx = glm::rotate(modelMtx, _rotateRightArmAngle, CSCI441::Z_AXIS);
        modelMtx = glm::rotate(modelMtx, -_rotateOuterArmAngle, CSCI441::X_AXIS);
    }

    modelMtx = glm::scale( modelMtx, _scaleArm );

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_colorBody[0]);

    CSCI441::drawSolidCylinder( 0.05, 0.05, 0.2, 16, 16 );
}

void Jammss::_drawForeArm(bool isLeftArm, glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    if( isLeftArm ) {
        modelMtx = glm::translate(modelMtx, _transLeftArm);
        modelMtx = glm::rotate(modelMtx, _rotateLeftArmAngle, CSCI441::Z_AXIS);
        modelMtx = glm::rotate(modelMtx, _rotateOuterArmAngle, CSCI441::X_AXIS);
        modelMtx = glm::rotate(modelMtx, _legAngle3, CSCI441::Z_AXIS);
        modelMtx = glm::translate(modelMtx, glm::vec3(0.0f, 0.095f, 0.0f));
        modelMtx = glm::rotate(modelMtx, -_rotateOuterArmAngle, CSCI441::X_AXIS);
        modelMtx = glm::rotate(modelMtx, -_rotateLeftArmAngle, CSCI441::Z_AXIS);
        modelMtx = glm::rotate(modelMtx, GLfloat(M_PI/1.0f), CSCI441::Z_AXIS);
        modelMtx = glm::rotate(modelMtx, -_rotateBodyAngle, CSCI441::Z_AXIS);
        modelMtx = glm::scale( modelMtx, _scaleArm );
        _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
        glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_colorBody[0]);
        CSCI441::drawSolidCylinder( 0.1,0.02, 0.2, 16, 16 );
    } else {
        modelMtx = glm::translate(modelMtx, _transRightArm);
        modelMtx = glm::rotate(modelMtx, _rotateRightArmAngle, CSCI441::Z_AXIS);
        modelMtx = glm::rotate(modelMtx, -_rotateOuterArmAngle, CSCI441::X_AXIS);
        modelMtx = glm::translate(modelMtx, glm::vec3(0.0f, 0.095f, 0.0f));
        modelMtx = glm::rotate(modelMtx, _rotateOuterArmAngle, CSCI441::X_AXIS);
        modelMtx = glm::rotate(modelMtx, -_rotateRightArmAngle, CSCI441::Z_AXIS);
        modelMtx = glm::rotate(modelMtx, GLfloat(M_PI/2.0f), CSCI441::X_AXIS);
        modelMtx = glm::translate(modelMtx, glm::vec3(0.0f, -0.03f, 0.0f));
        modelMtx = glm::scale( modelMtx, _scaleArm );
        _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
        glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_colorTree[0]);
        CSCI441::drawSolidCylinder( 0.1,0.08, 0.45, 16, 16 );
        modelMtx = glm::translate(modelMtx, glm::vec3(0.0f, 0.3f, 0.0f));
        _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
        glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_colorLeaves[0]);
        CSCI441::drawSolidCone( 0.2,0.45, 16, 16 );
    }


}


void Jammss::_drawHead(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    //modelMtx = glm::rotate( modelMtx, _rotateNoseAngle, CSCI441::Z_AXIS );
    modelMtx = glm::translate( modelMtx, glm::vec3(-0.12f,0.0f,0.0f));
    modelMtx = glm::scale( modelMtx, _scaleHead );
    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_colorBody[0]);

    CSCI441::drawSolidSphere( 0.05, 16, 16 );
}

void Jammss::_drawLegs(bool isLeftLeg, glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    if( isLeftLeg ) {
        modelMtx = glm::translate(modelMtx, -_transRightArm);
        modelMtx = glm::rotate(modelMtx, _rotateLeftArmAngle, CSCI441::Z_AXIS);
        modelMtx = glm::rotate(modelMtx, -_rotateBodyAngle, CSCI441::Z_AXIS);
        modelMtx = glm::rotate(modelMtx, _legAngle1, CSCI441::Z_AXIS);
    } else {
        modelMtx = glm::translate(modelMtx, -_transLeftArm);
        modelMtx = glm::rotate(modelMtx, _rotateLeftArmAngle, CSCI441::Z_AXIS);
        modelMtx = glm::rotate(modelMtx, -_rotateBodyAngle, CSCI441::Z_AXIS);
        modelMtx = glm::rotate(modelMtx, _legAngle2, CSCI441::Z_AXIS);
    }

    modelMtx = glm::scale( modelMtx, _scaleLeg );

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_colorBody[0]);

    CSCI441::drawSolidCylinder( 0.05,0.03, 0.3, 16, 16 );
}


void Jammss::_computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    // precompute the Model-View-Projection matrix on the CPU
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    // then send it to the shader on the GPU to apply to every vertex
    glProgramUniformMatrix4fv( _shaderProgramHandle, _shaderProgramUniformLocations.mvpMtx, 1, GL_FALSE, &mvpMtx[0][0] );

    glm::mat3 normalMtx = glm::mat3( glm::transpose( glm::inverse( modelMtx )));
    glProgramUniformMatrix3fv( _shaderProgramHandle, _shaderProgramUniformLocations.normalMtx, 1, GL_FALSE, &normalMtx[0][0] );
}
