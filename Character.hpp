//
// Created by luigi on 10/13/2022.
//

#ifndef MIDTERM_PROJECT_CHARACTER_H
#define MIDTERM_PROJECT_CHARACTER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Character {
public:

    virtual void drawCharacter(glm::mat4 viewMtx, glm::mat4 projMtx) = 0;

    void moveForward() {
        //adjust x
        _position[0] += cos(_rotation) * _speed;
        //wrap around to other side of area when leaving
        if (_position[0] > 90.0f) {
            _position[0] = -90.0f;
        } else if (_position[0] < -90.0f) {
            _position[0] = 90.0f;
        }

        //adjust z
        _position[2] -= sin(_rotation) * _speed;
        //wrap around to other side of area when leaving
        if (_position[2] > 90.0f) {
            _position[2] = -90.0f;
        } else if (_position[2] < -90.0f) {
            _position[2] = 90.0f;
        }
    }

    void moveBackward() {
        //adjust x
        _position[0] -= cos(_rotation) * _speed;
        //wrap around to other side of area when leaving
        if (_position[0] > 90.0f) {
            _position[0] = -90.0f;
        } else if (_position[0] < -90.0f) {
            _position[0] = 90.0f;
        }

        //adjust z
        _position[2] += sin(_rotation) * _speed;
        if (_position[2] > 90.0f) {
            _position[2] = -90.0f;
        } else if (_position[2] < -90.0f) {
            _position[2] = 90.0f;
        }
    }

    glm::vec3 calculateFPCam() {
        //use the character's specified camera offset and eye level as well as their current position and rotation
        //to find the proper angle and offset for the first person camera
        return _position + glm::vec3(cos(_rotation) * _cameraOffset, _eyeHeight, -sin(_rotation) * _cameraOffset);
    }

    void setMoving(bool moving) {
        _moving = moving;
        if (not moving) {
            resetAnim();
        } else {
            incrementAnim();
        }
    }

    bool getMoving() {
        return _moving;
    }

    void incrementAnim() {
        _animationTimer++;
    }

    void resetAnim() {
        _animationTimer = 0;
    }

    void turn(bool right) {
        //formula takes boolean and rotates the proper direction
        _rotation += -_rotationRate + 2 * _rotationRate * right;

        //keep rotation value within -2pi to 2pi radians
        if (_rotation > M_PI * 2) {
            _rotation -= M_PI * 2;
        } else if (_rotation < -M_PI * 2) {
            _rotation += M_PI * 2;
        }
    }

    //getters for position and rotation
    glm::vec3 getPosition() {
        return _position;
    }

    GLfloat getRotation() {
        return _rotation;
    }

    bool getJumping() {
        return _jumping;
    }

    void setJumping(bool jumping) {
        _jumping = jumping;
    }

    void setJumpTimer() {
        _jumpTimer = 0.6f;
    }

    GLfloat _rotation;
protected:

    glm::vec3 _position;
    bool _moving;
    // how far the first person camera should be laterally from the model
    float _cameraOffset;
    // how high first person camera should be off the ground
    float _eyeHeight;
    // used to animate parts of a model
    int _animationTimer;
    // how fast the character moves
    float _speed;
    // how fast the character turns
    float _rotationRate;
    float _jumpTimer;
    float _maxJump;
    bool _jumping;


    GLuint _shaderProgramHandle;
    struct ShaderProgramUniformLocations {

        GLint mMtx;
        GLint normalMtx;
        GLint materialColor;
        GLint materialBrightness;

    } _shaderProgramUniformLocations;

    void _computeAndSendMatrixUniforms(glm::mat4 modelMtx) const {
        // precompute the Model-View-Projection matrix on the CPU
        // then send it to the shader on the GPU to apply to every vertex
        glProgramUniformMatrix4fv(_shaderProgramHandle, _shaderProgramUniformLocations.mMtx, 1, GL_FALSE,
                                  &modelMtx[0][0]);

        glm::mat3 normalMtx = glm::mat3(glm::transpose(glm::inverse(modelMtx)));
        glProgramUniformMatrix3fv(_shaderProgramHandle, _shaderProgramUniformLocations.normalMtx, 1, GL_FALSE,
                                  &normalMtx[0][0]);
    };

    // returns the model matrix for a rectangular prism
    glm::mat4 getPrismModelMatrix(glm::mat4 transMatrix,
                                          glm::mat4 rotationMatrix,
                                          glm::vec3 dimensions,
                                          glm::vec3 offset) const {
        glm::mat4 pointTranslation = glm::translate(
                glm::mat4(1.f),
                glm::vec3(offset.x, dimensions.y / 2 + offset.y, offset.z));
        glm::mat4 scale = glm::scale(glm::mat4(1.f), dimensions);
        return transMatrix * rotationMatrix * pointTranslation * scale;
    }
};

#endif //MIDTERM_PROJECT_CHARACTER_H