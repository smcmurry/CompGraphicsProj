#ifndef LAB05_JAMMSS_HPP
#define LAB05_JAMMSS_HPP

#include <GL/glew.h>

#include <glm/glm.hpp>

class Jammss {
public:
    //initialize
    Jammss( GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation, GLint materialColorUniformLocation );

    //draw Jammss on screen
    void drawJammss( glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx );

    //move forward with animation
    void walkForward();

    float x;
    float y;
    float angle;

private:

    GLfloat _legAngle1;
    GLfloat _legAngle2;
    GLfloat _legAngle3;
    GLfloat _legAngleRotationSpeed;

    bool _leg1Toggle;
    bool _leg2Toggle;
    bool _leg3Toggle;

    GLuint _shaderProgramHandle;

    struct ShaderProgramUniformLocations {

        GLint mMtx;
        GLint normalMtx;
        GLint materialColor;

    } _shaderProgramUniformLocations;

    GLfloat _rotateBodyAngle;

    glm::vec3 _scaleBody;

    glm::vec3 _scaleArm;
    glm::vec3 _scaleLeg;
    glm::vec3 _scaleHead;

    glm::vec3 _transLeftArm;
    glm::vec3 _transRightArm;

    glm::vec3 _colorTree;
    glm::vec3 _colorBody;
    glm::vec3 _colorLeaves;

    GLfloat _rotateLeftArmAngle;
    GLfloat _rotateRightArmAngle;
    GLfloat _rotateOuterArmAngle;

    void move(float dx, float dy);

    void _drawPlaneBody(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const;

    void _drawUpperArm(bool isLeftArm, glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const;

    void _drawForeArm(bool isLeftArm, glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const;

    void _drawHead(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const;

    void _drawLegs(bool isLeftLeg, glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const;

    void _computeAndSendMatrixUniforms(glm::mat4 modelMtx) const;
};


#endif //LAB05_JAMMSS_HPP
