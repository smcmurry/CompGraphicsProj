#ifndef LAB05_LAB05_ENGINE_HPP
#define LAB05_LAB05_ENGINE_HPP

#include "Engine.hpp"
#include <CSCI441/FreeCam.hpp>
#include <CSCI441/OpenGLEngine.hpp>
#include <CSCI441/ShaderProgram.hpp>

#include "Zennia.hpp"
#include "Jammss.hpp"
#include "Plane.hpp"

#include <vector>

class FPEngine : public CSCI441::OpenGLEngine
{
public:
    FPEngine();
    ~FPEngine();

    void run() final;

    /// \desc handle any key events inside the engine
    /// \param key key as represented by GLFW_KEY_ macros
    /// \param action key event action as represented by GLFW_ macros
    void handleKeyEvent(GLint key, GLint action);

    /// \desc handle any mouse button events inside the engine
    /// \param button mouse button as represented by GLFW_MOUSE_BUTTON_ macros
    /// \param action mouse event as represented by GLFW_ macros
    void handleMouseButtonEvent(GLint button, GLint action);

    /// \desc handle any cursor movement events inside the engine
    /// \param currMousePosition the current cursor position
    void handleCursorPositionEvent(glm::vec2 currMousePosition);

    /// \desc value off-screen to represent mouse has not begun interacting with window yet
    static constexpr GLfloat MOUSE_UNINITIALIZED = -9999.0f;

private:
    void _setupGLFW() final;
    void _setupOpenGL() final;
    void _setupShaders() final;
    void _setupBuffers() final;
    void _setupScene() final;

    void _setupTextures() final;
    void _createSkybox();
    void _cleanupBuffers() final;
    void _cleanupShaders() final;
    void _setupCubeFace(GLint TextureTarget, const char* filename);

    void _computeAndSendTransformationMatrices(CSCI441::ShaderProgram* shaderProgram,
                                                            glm::mat4 modelMatrix, glm::mat4 viewMatrix, glm::mat4 projectionMatrix,
                                                            GLint mvpMtxLocation, GLint modelMtxLocation, GLint normalMtxLocation) const;
    /// ********************** SKYBOX ********************** ///
    GLuint skyboxTextureID;
    /// \desc total number of VAOs in our scene
    static constexpr GLuint NUM_VAOS = 1;
    /// \desc used to index through our VAO/VBO/IBO array to give named access
    enum VAO_ID {
        /// \desc Sky box surrounding our scene
        SKY_BOX = 0
    };
    /// \desc VAO for our objects
    GLuint _vaos[NUM_VAOS];
    /// \desc VBO for our objects
    GLuint _vbos[NUM_VAOS];
    /// \desc IBO for our objects
    GLuint _ibos[NUM_VAOS];
    /// \desc the number of points that make up our VAO
    GLsizei _numVAOPoints[NUM_VAOS];

    /// \desc stores the locations of all of our shader attributes, locations
    /// shared by all shader programs being used.
    struct ShaderProgramAttributeLocations {
        /// \desc vertex position location
        GLint vPos;
        /// \desc vertex normal location
        GLint vNormal;
    } _ShaderProgramAttributeLocations;

    /// \desc shader program that performs lighting
    CSCI441::ShaderProgram* _textureShaderProgram = nullptr;   // the wrapper for our shader program
    /// \desc stores the locations of all of our shader uniforms
    struct TextureShaderUniformLocations {
        /// \desc precomputed MVP matrix location
        GLint mvpMatrix;
        GLint texMap;
    } _textureShaderUniformLocations;

    /// ********************** SKYBOX ********************** ///

    /// \desc Number of indices to draw for our skybox
    const GLint numSkyboxIndices = 36;

    /// \desc draws everything to the scene from a particular point of view
    /// \param viewMtx the current view matrix for our camera
    /// \param projMtx the current projection matrix for our camera
    void _renderScene(glm::mat4 viewMtx, glm::mat4 projMtx) const;
    /// \desc handles moving our FreeCam as determined by keyboard input
    void _updateScene();

    /// \desc tracks the number of different keys that can be present as determined by GLFW
    static constexpr GLuint NUM_KEYS = GLFW_KEY_LAST;
    /// \desc boolean array tracking each key state.  if true, then the key is in a pressed or held
    /// down state.  if false, then the key is in a released state and not being interacted with
    GLboolean _keys[NUM_KEYS];

    /// \desc last location of the mouse in window coordinates
    glm::vec2 _mousePosition;
    /// \desc current state of the left mouse button
    GLint _leftMouseButtonState;

    /// \desc the static fixed camera in our world
    CSCI441::FreeCam *_freeCam;
    /// \desc pair of values to store the speed the camera can move/rotate.
    /// \brief x = forward/backward delta, y = rotational delta
    glm::vec2 _cameraSpeed;
    float zoom;

    /// \desc our plane model
    Zennia *_zennia;
    Jammss *_jammss;
    Plane *_plane;


    int heroToggle = 0;
    int cameraToggle = 0;

    /// \desc the size of the world (controls the ground size and locations of buildings)
    static constexpr GLfloat WORLD_SIZE = 55.0f;
    /// \desc the number of points that make up our ground object
    GLsizei _numGroundPoints;

    void fixCamera();

    /// \desc smart container to store information specific to each building we wish to draw
    struct BuildingData
    {
        /// \desc transformations to position and size the building
        glm::mat4 modelMatrix;
        uint32_t buildingId;
    };
    /// \desc information list of all the buildings to draw
    std::vector<BuildingData> _buildings;

    /// \desc generates building information to make up our scene
    void _generateEnvironment();

    /// \desc shader program that performs lighting
    CSCI441::ShaderProgram *_lightingShaderProgram = nullptr; // the wrapper for our shader program
    Engine::BufferData worldMeshes;
    /// \desc stores the locations of all of our shader uniforms
    struct LightingShaderUniformLocations
    {
        /// \desc precomputed MVP matrix location
        GLint mMatrix;
        GLint vMatrix;
        GLint pMatrix;
        /// \desc material diffuse color location
        GLint materialColor;
        GLint normalMatrix;
        GLint ambientColor;
        GLint lightPositions;
        GLint lightDirections;
        GLint lightColors;
        GLint lightSizes;
        GLint lightTypes;
        GLint lightCount;
    } _lightingShaderUniformLocations;
    /// \desc stores the locations of all of our shader attributes
    struct LightingShaderAttributeLocations
    {
        /// \desc vertex position location
        GLint vPos;
        GLint vNormal;

    } _lightingShaderAttributeLocations;

    /*glm::mat4 getPrismModelMatrix(glm::mat4 translateMatrix,
                                  glm::mat4 rotationMatrix,
                                  glm::vec3 dimensions, glm::vec3 offset) const;*/

    glm::vec3 torchPos;
    void _drawTorch(glm::mat4, glm::mat4, glm::mat4) const;

    /// \desc precomputes the matrix uniforms CPU-side and then sends them
    /// to the GPU to be used in the shader for each vertex.  It is more efficient
    /// to calculate these once and then use the resultant product in the shader.
    /// \param modelMtx model transformation matrix
    /// \param viewMtx camera view matrix
    /// \param projMtx camera projection matrix
    void _computeAndSendMatrixUniforms(glm::mat4 modelMtx) const;
};

void lab05_engine_keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void lab05_engine_cursor_callback(GLFWwindow *window, double x, double y);
void lab05_engine_mouse_button_callback(GLFWwindow *window, int button, int action, int mods);

#endif // LAB05_LAB05_ENGINE_HPP
