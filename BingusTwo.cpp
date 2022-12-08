//
// Created by luigi on 10/14/2022.
//
#include "BingusTwo.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <CSCI441/objects.hpp>
#include <CSCI441/OpenGLUtils.hpp>

#ifndef M_PI
#define M_PI 3.14159265
#endif

BingusTwo::BingusTwo(GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation,
                     GLint materialColorUniformLocation, GLint materialBrightnessUniformLocation) {
    glUseProgram(shaderProgramHandle);

    //shader pointers
    _shaderProgramHandle                            = shaderProgramHandle;
    _shaderProgramUniformLocations.mMtx           = mvpMtxUniformLocation;
    _shaderProgramUniformLocations.normalMtx        = normalMtxUniformLocation;
    _shaderProgramUniformLocations.materialColor    = materialColorUniformLocation;

    //character stats
    _position = glm::vec3(-20.f, 0.f, 0.f);
    _rotation = 0;
    _moving = false;
    _cameraOffset = 7.5f;
    _speed = 0.1f;
    _eyeHeight = 17.0f;
    _rotationRate = 0.02f;
    _jumpTimer = 0;
    _maxJump = 10.f;
    _jumping = false;
}

void BingusTwo::drawCharacter(glm::mat4 viewMtx, glm::mat4 projMtx) {
    glUseProgram( _shaderProgramHandle );
    glm::vec3 color = glm::vec3(1.f/255, 1.f/255, 1.f/255);
    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &color[0]);
    GLfloat brightness = 0.1;
    glUniform1fv(_shaderProgramUniformLocations.materialBrightness, 1,
                 &brightness);

    glm::mat4 transMatrix =
            glm::translate(glm::mat4(1.f), _position);

    if (_jumping) {
        transMatrix = glm::translate(transMatrix, glm::vec3(0.f, sin(_jumpTimer / 0.6f * M_PI) * _maxJump, 0.f));
        _jumpTimer -= 0.01;
        if (_jumpTimer <= 0) {
            _jumping = false;
        }
    }

    transMatrix = glm::translate(transMatrix, glm::vec3(0.f, 1.7f, 0.f));

    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.f), _rotation,
                                           CSCI441::Y_AXIS);



    drawBingusTwoHead(viewMtx, projMtx, transMatrix,
                      rotationMatrix);

    color = glm::vec3(1.f/255, 1.f/255, 1.f/255);
    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &color[0]);
    brightness = 0.1;
    glUniform1fv(_shaderProgramUniformLocations.materialBrightness, 1,
                 &brightness);
    drawBingusTwoBody(viewMtx, projMtx, transMatrix,
                      rotationMatrix);
}

void BingusTwo::drawBingusTwoHead(glm::mat4 viewMatrix, glm::mat4 projectionMatrix,
                                  glm::mat4 transMatrix,
                                  glm::mat4 rotationMatrix) const {
    glm::vec3 color = glm::vec3(.776f/255,.702f/255,.592f/255);
    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &color[0]);
    //head
    glm::mat4 modelMatrix =
            getPrismModelMatrix(transMatrix, rotationMatrix, glm::vec3(.3f),
                                glm::vec3(0.f, 0.05, 0.f));
    _computeAndSendMatrixUniforms(modelMatrix);
    CSCI441::drawSolidSphere(1.f, 20, 20);


    color = glm::vec3(.863f/255, .506f/255, .506f/255);
    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &color[0]);

    //nose
    modelMatrix = getPrismModelMatrix(transMatrix, rotationMatrix,
                                      glm::vec3(.1f, .1f, .1f),
                                      glm::vec3(.325f, .20f, 0.f));
    _computeAndSendMatrixUniforms(modelMatrix);
    CSCI441::drawSolidSphere(1.f, 20, 20);

    color = glm::vec3(0.506f/255, 0.604f/255, 0.827f/255);
    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &color[0]);

    //hatfront
    modelMatrix =
            getPrismModelMatrix(transMatrix, rotationMatrix, glm::vec3(.5,.05,.5),
                                glm::vec3(.3f, .37, 0.f));
    _computeAndSendMatrixUniforms(modelMatrix);
    CSCI441::drawSolidCube(1.f);

    //hatmain
    modelMatrix =
            getPrismModelMatrix(transMatrix, rotationMatrix, glm::vec3(.54f,.20f,.54f),
                                glm::vec3(0.f, .35, 0.f));
    _computeAndSendMatrixUniforms(modelMatrix);
    CSCI441::drawSolidCube(1.f);

}

void BingusTwo::drawBingusTwoBody(glm::mat4 viewMatrix, glm::mat4 projectionMatrix,
                                  glm::mat4 transMatrix,
                                  glm::mat4 rotationMatrix) const {

    //body
    glm::mat4 modelMatrix = getPrismModelMatrix(
            transMatrix, rotationMatrix, glm::vec3(.35f, .8f, .5f),
            glm::vec3(0.f, -.8f, 0.f));
    _computeAndSendMatrixUniforms(modelMatrix);
    CSCI441::drawSolidCube(1.f);

    GLfloat myangC = 0;
    GLfloat myangS = 0;

    GLfloat myangCl = 0;
    GLfloat myangSl = 0;
    // funny number to play with
    double lim = 9;

    if (_moving) {
        myangC += (glm::sin(_animationTimer / 9.5f)) * 1.5;
        myangS -= (glm::sin(_animationTimer / 9.5f)) * 1.5;
        myangCl -= glm::sin(glm::cos(_animationTimer / 3.5f)/lim) * 1.5;
        myangSl += glm::sin(glm::cos(_animationTimer / 3.5f)/lim) * 1.5;
    }

    glm::mat4 rotationMatrixC = glm::rotate(glm::mat4(1.f), myangC,
                                            glm::vec3(0.f, 0.f, 1.f));
    glm::mat4 rotationMatrixS = glm::rotate(glm::mat4(1.f), myangS,
                                            glm::vec3(0.f, 0.f, 1.f));

    //right arm
    modelMatrix = getPrismModelMatrix(
            transMatrix, rotationMatrix*rotationMatrixC, glm::vec3(.25f, .7f, .25f),
            glm::vec3(0.f, -.7f, .375f));
    _computeAndSendMatrixUniforms(modelMatrix);
    CSCI441::drawSolidCube(1.f);

    //left arm
    modelMatrix = getPrismModelMatrix(
            transMatrix, rotationMatrix*rotationMatrixS, glm::vec3(.25f, .7f, .25f),
            glm::vec3(0.f, -.7f, -.375f));
    _computeAndSendMatrixUniforms(modelMatrix);
    CSCI441::drawSolidCube(1.f);

    glm::vec3 color = glm::vec3(0.506f/255, 0.604f/255, 0.827f/255);
    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &color[0]);




    glm::mat4 rotationMatrixCl = glm::rotate(glm::mat4(1.f), myangCl,
                                             glm::vec3(0.f, 0.f, 1.f));
    glm::mat4 rotationMatrixSl = glm::rotate(glm::mat4(1.f), myangSl,
                                             glm::vec3(0.f, 0.f, 1.f));

    //right leg
    modelMatrix = getPrismModelMatrix(
            transMatrix, rotationMatrix*rotationMatrixCl, glm::vec3(.25f, .9f, .25f),
            glm::vec3(0.f, -1.7f, .125f));
    _computeAndSendMatrixUniforms(modelMatrix);
    CSCI441::drawSolidCube(1.f);

    //right leg
    modelMatrix = getPrismModelMatrix(
            transMatrix, rotationMatrix*rotationMatrixSl, glm::vec3(.25f, .9f, .25f),
            glm::vec3(0.f, -1.7f, -.125f));
    _computeAndSendMatrixUniforms(modelMatrix);
    CSCI441::drawSolidCube(1.f);
}
