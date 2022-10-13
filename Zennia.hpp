#ifndef LAB05_Zennia_HPP
#define LAB05_Zennia_HPP
#include "Engine.hpp"

#include <GL/glew.h>

#include <glm/glm.hpp>

class Zennia {
public:
    /// \desc creates a simple Zennia that gives the appearance of flight
    /// \param shaderProgramHandle shader program handle that the Zennia should be drawn using
    /// \param mvpMtxUniformLocation uniform location for the full precomputed MVP matrix
    /// \param normalMtxUniformLocation uniform location for the precomputed Normal matrix
    /// \param materialColorUniformLocation uniform location for the material diffuse color
    Zennia(GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation, GLint materialColorUniformLocation, Engine::MeshData* meshes);

    /// \desc draws the model Zennia for a given MVP matrix
    /// \param modelMtx existing model matrix to apply to Zennia
    /// \param viewMtx camera view matrix to apply to Zennia
    /// \param projMtx camera projection matrix to apply to Zennia
    /// \note internally uses the provided shader program and sets the necessary uniforms
    /// for the MVP and Normal Matrices as well as the material diffuse color
    void drawZennia( glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx );

    /// \desc simulates the Zennia flying forward by rotating the propeller clockwise
    void flyForward();
    /// \desc simulates the Zennia flying backward by rotating the propeller counter-clockwise
    void flyBackward();
    void freeData();
    float x;
    float y;
    float angle;

private:
    /// \desc current angle of rotation for the propeller
    GLfloat _propAngle;
    /// \desc one rotation step
    GLfloat _propAngleRotationSpeed;

    /// \desc handle of the shader program to use when drawing the Zennia
    GLuint _shaderProgramHandle;
    Engine::BufferData meshData;
    GLuint _VBO_SIZE;
    std::vector<GLuint> _IBO_SIZE;
    /// \desc stores the uniform locations needed for the plan information
    struct ShaderProgramUniformLocations {
        /// \desc location of the precomputed ModelViewProjection matrix
        GLint mMtx;
        /// \desc location of the precomputed Normal matrix
        GLint normalMtx;
        /// \desc location of the material diffuse color
        GLint materialColor;
    } _shaderProgramUniformLocations;

    /// \desc angle to rotate our Zennia at
    GLfloat _rotateZenniaAngle;
    GLfloat _rotateWingAngle;

    /// \desc color the Zennia's nose
    glm::vec3 _color;
    /// \desc amount to scale the Zennia's propeller by
    glm::vec3 _scaleProp;
    /// \desc amount to translate the Zennia's propeller by
    glm::vec3 _transProp;
    void move(float dx, float dy);

    /// \desc precomputes the matrix uniforms CPU-side and then sends them
    /// to the GPU to be used in the shader for each vertex.  It is more efficient
    /// to calculate these once and then use the resultant product in the shader.
    /// \param modelMtx model transformation matrix
    /// \param viewMtx camera view matrix
    /// \param projMtx camera projection matrix
    void _computeAndSendMatrixUniforms(glm::mat4 modelMtx) const;
};


#endif //LAB05_Zennia_HPP
