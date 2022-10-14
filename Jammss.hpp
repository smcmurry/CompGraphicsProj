#ifndef LAB05_JAMMSS_HPP
#define LAB05_JAMMSS_HPP

#include <GL/glew.h>

#include <glm/glm.hpp>

class Jammss {
public:
    /// \desc creates a simple plane that gives the appearance of flight
    /// \param shaderProgramHandle shader program handle that the plane should be drawn using
    /// \param mvpMtxUniformLocation uniform location for the full precomputed MVP matrix
    /// \param normalMtxUniformLocation uniform location for the precomputed Normal matrix
    /// \param materialColorUniformLocation uniform location for the material diffuse color
    Jammss( GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation, GLint materialColorUniformLocation );

    /// \desc draws the model plane for a given MVP matrix
    /// \param modelMtx existing model matrix to apply to plane
    /// \param viewMtx camera view matrix to apply to plane
    /// \param projMtx camera projection matrix to apply to plane
    /// \note internally uses the provided shader program and sets the necessary uniforms
    /// for the MVP and Normal Matrices as well as the material diffuse color
    void drawJammss( glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx );

    /// \desc simulates the plane flying forward by rotating the propeller clockwise
    void walkForward();
    /// \desc simulates the plane flying backward by rotating the propeller counter-clockwise

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


    /// \desc handle of the shader program to use when drawing the plane
    GLuint _shaderProgramHandle;
    /// \desc stores the uniform locations needed for the plan information
    struct ShaderProgramUniformLocations {
        /// \desc location of the precomputed ModelViewProjection matrix
        GLint mMtx;
        /// \desc location of the precomputed Normal matrix
        GLint normalMtx;
        /// \desc location of the material diffuse color
        GLint materialColor;
    } _shaderProgramUniformLocations;

    /// \desc angle to rotate our plane at
    GLfloat _rotatePlaneAngle;

    GLfloat _rotateBodyAngle;


    /// \desc amount to scale the plane's body by
    glm::vec3 _scaleBody;

    glm::vec3 _scaleArm;
    glm::vec3 _scaleLeg;
    glm::vec3 _scaleHead;

    glm::vec3 _transLeftArm;
    glm::vec3 _transRightArm;

    glm::vec3 _colorTree;
    glm::vec3 _colorBody;
    glm::vec3 _colorLeaves;

    /// \desc amount to rotate the plane's wing by
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
