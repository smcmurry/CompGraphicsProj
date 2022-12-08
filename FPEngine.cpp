#include "FPEngine.hpp"

#include <CSCI441/objects.hpp>
#include <CSCI441/SimpleShader.hpp>
#include "Plane.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include "stb_image.h"

//*************************************************************************************
//
// Helper Functions

#ifndef M_PI
#define M_PI 3.14159265
#endif

/// \desc Simple helper function to return a random number between 0.0f and 1.0f.
GLfloat getRand()
{
    return (GLfloat)rand() / (GLfloat)RAND_MAX;
}

//*************************************************************************************
//
// Public Interface

FPEngine::FPEngine()
    : CSCI441::OpenGLEngine(4, 1,
                            640, 480,
                            "FP: There And Back Again")
{

    for (auto &_key : _keys)
        _key = GL_FALSE;

    _mousePosition = glm::vec2(MOUSE_UNINITIALIZED, MOUSE_UNINITIALIZED);
    _leftMouseButtonState = GLFW_RELEASE;
    zoom = 3.0;
    torchPos = glm::vec3(10.0, 0.0, 0.0);
}

FPEngine::~FPEngine()
{
    delete _freeCam;
}

void FPEngine::handleKeyEvent(GLint key, GLint action)
{
    if (key != GLFW_KEY_UNKNOWN)
        _keys[key] = ((action == GLFW_PRESS) || (action == GLFW_REPEAT));

    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        // quit!
        case GLFW_KEY_C: //switch cameras
            if (cameraToggle == 0)
                cameraToggle = 1;
            else if (cameraToggle == 1)
                cameraToggle = 2;
            else
                cameraToggle = 0;
            fixCamera();
            break;
        case GLFW_KEY_X: // switch heroes
            heroToggle++;
            if (heroToggle == 3)
                heroToggle = 0;
            fixCamera();
            break;
        case GLFW_KEY_Q:
        case GLFW_KEY_ESCAPE:
            setWindowShouldClose();
            break;
        default:
            break; // suppress CLion warning
        }
    }
}

void FPEngine::handleMouseButtonEvent(GLint button, GLint action)
{
    // if the event is for the left mouse button
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        // update the left mouse button's state
        _leftMouseButtonState = action;
    }
}

void FPEngine::handleCursorPositionEvent(glm::vec2 currMousePosition)
{
    // if mouse hasn't moved in the window, prevent camera from flipping out
    if (_mousePosition.x == MOUSE_UNINITIALIZED)
    {
        _mousePosition = currMousePosition;
    }

    // if the left mouse button is being held down while the mouse is moving
    if (_leftMouseButtonState == GLFW_PRESS)
    {
        if (_keys[GLFW_KEY_LEFT_SHIFT]) // zoom when moving mouse while pressing shift
        {
            zoom += (currMousePosition.y - _mousePosition.y) * 0.01;
            if (zoom < 0.1)
                zoom = 0.1;
        }
        else {
            // rotate the camera by the distance the mouse moved
            if (cameraToggle != 2) {
            _freeCam->rotate((currMousePosition.x - _mousePosition.x) * 0.005f,
                             (_mousePosition.y - currMousePosition.y) * 0.005f);
            }
        }
        fixCamera();
    }

    // update the mouse position
    _mousePosition = currMousePosition;
}

//*************************************************************************************
//
// Engine Setup

void FPEngine::_setupGLFW()
{
    CSCI441::OpenGLEngine::_windowResizable = true;
    CSCI441::OpenGLEngine::_setupGLFW();

    // set our callbacks
    glfwSetKeyCallback(_window, lab05_engine_keyboard_callback);
    glfwSetMouseButtonCallback(_window, lab05_engine_mouse_button_callback);
    glfwSetCursorPosCallback(_window, lab05_engine_cursor_callback);
}

void FPEngine::_setupOpenGL()
{
    glEnable(GL_DEPTH_TEST); // enable depth testing
    glDepthFunc(GL_LESS);    // use less than depth test

    glEnable(GL_BLEND);                                // enable blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // use one minus blending equation

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // clear the frame buffer to black
}

void FPEngine::_setupShaders()
{
    _lightingShaderProgram = new CSCI441::ShaderProgram("shaders/FP.v.glsl", "shaders/FP.f.glsl");
    _lightingShaderUniformLocations.mMatrix = _lightingShaderProgram->getUniformLocation("mMatrix");
    _lightingShaderUniformLocations.vMatrix = _lightingShaderProgram->getUniformLocation("vMatrix");
    _lightingShaderUniformLocations.pMatrix = _lightingShaderProgram->getUniformLocation("pMatrix");
    _lightingShaderUniformLocations.materialColor = _lightingShaderProgram->getUniformLocation("materialColor");
    _lightingShaderUniformLocations.ambientColor = _lightingShaderProgram->getUniformLocation("ambientColor");
    _lightingShaderUniformLocations.normalMatrix = _lightingShaderProgram->getUniformLocation("normalMatrix");
    _lightingShaderUniformLocations.lightColors = _lightingShaderProgram->getUniformLocation("lightColors");
    _lightingShaderUniformLocations.lightDirections = _lightingShaderProgram->getUniformLocation("lightDirections");
    _lightingShaderUniformLocations.lightPositions = _lightingShaderProgram->getUniformLocation("lightPositions");
    _lightingShaderUniformLocations.lightSizes = _lightingShaderProgram->getUniformLocation("lightSizes");
    _lightingShaderUniformLocations.lightTypes = _lightingShaderProgram->getUniformLocation("lightTypes");
    _lightingShaderUniformLocations.lightCount = _lightingShaderProgram->getUniformLocation("lightCount");

    _lightingShaderAttributeLocations.vPos = _lightingShaderProgram->getAttributeLocation("vPos");

    _lightingShaderAttributeLocations.vNormal = _lightingShaderProgram->getAttributeLocation("vNormal");
    //***************************************************************************
    // Setup Texture Shader Program
    _textureShaderProgram = new CSCI441::ShaderProgram("shaders/skyboxShader.v.glsl", "shaders/skyboxShader.f.glsl");
    _textureShaderUniformLocations.mvpMatrix                = _textureShaderProgram->getUniformLocation("mvpMatrix");
    _textureShaderUniformLocations.texMap                   = _textureShaderProgram->getUniformLocation("texMap");
}

void FPEngine::_setupBuffers()
{
    CSCI441::setVertexAttributeLocations(_lightingShaderAttributeLocations.vPos, _lightingShaderAttributeLocations.vNormal);


    Engine::MeshData *readData = Engine::readOBJ("./assets.obj");

    _zennia = new Zennia(_lightingShaderProgram->getShaderProgramHandle(),
                         _lightingShaderUniformLocations.mMatrix,
                         _lightingShaderUniformLocations.normalMatrix,
                         _lightingShaderUniformLocations.materialColor,
                         readData);
    _jammss = new Jammss(_lightingShaderProgram->getShaderProgramHandle(),
            _lightingShaderUniformLocations.mMatrix,
            _lightingShaderUniformLocations.normalMatrix,
            _lightingShaderUniformLocations.materialColor);

    _plane = new Plane(_lightingShaderProgram->getShaderProgramHandle(),
                         _lightingShaderUniformLocations.mMatrix,
                         _lightingShaderUniformLocations.normalMatrix,
                         _lightingShaderUniformLocations.materialColor);

    Engine::MeshData *worldData = Engine::readOBJ("./world_meshes.obj");
    worldMeshes = Engine::_getVao(*worldData);
    _generateEnvironment();
}

void FPEngine::_generateEnvironment()
{
    //******************************************************************
    // parameters to make up our grid size and spacing, feel free to
    // play around with this
    const GLfloat GRID_WIDTH = WORLD_SIZE * 1.8f;
    const GLfloat GRID_LENGTH = WORLD_SIZE * 1.8f;
    const GLfloat GRID_SPACING_WIDTH = 1.0f;
    const GLfloat GRID_SPACING_LENGTH = 1.0f;
    // precomputed parameters based on above
    const GLfloat LEFT_END_POINT = -GRID_WIDTH / 2.0f - 5.0f;
    const GLfloat RIGHT_END_POINT = GRID_WIDTH / 2.0f + 5.0f;
    const GLfloat BOTTOM_END_POINT = -GRID_LENGTH / 2.0f - 5.0f;
    const GLfloat TOP_END_POINT = GRID_LENGTH / 2.0f + 5.0f;
    //******************************************************************

    srand(time(0)); // seed our RNG

    // psych! everything's on a grid.
    for (int i = LEFT_END_POINT; i < RIGHT_END_POINT; i += GRID_SPACING_WIDTH)
    {
        for (int j = BOTTOM_END_POINT; j < TOP_END_POINT; j += GRID_SPACING_LENGTH)
        {
            // don't just draw a building ANYWHERE.
            if (i % 2 && j % 2 && getRand() < 0.4f)
            {
                // translate to spot
                glm::mat4 transToSpotMtx = glm::translate(glm::mat4(1.0), glm::vec3(i+getRand(), 0.0f, j+getRand()));

                // compute random height
                GLdouble height = getRand() * 0.5 + 1;
                // scale to randomly
                glm::mat4 scaleToHeightMtx = glm::scale(glm::mat4(1.0), glm::vec3(1, height, 1));

                // randomly rotate
                glm::mat4 randomRotate = glm::rotate(glm::mat4(1.0), getRand() * 360, glm::vec3(0, 1, 0));

                // compute full model matrix
                glm::mat4 modelMatrix = scaleToHeightMtx * transToSpotMtx*randomRotate;

                // store building properties
                BuildingData currentBuilding = {modelMatrix, (uint32_t) (4*getRand())}; //get random model id
                _buildings.emplace_back(currentBuilding);
            }
        }
    }
}

void FPEngine::_setupScene()
{
    _freeCam = new CSCI441::FreeCam();
    _freeCam->setPosition(glm::vec3(60.0f, 1.0f, 30.0f));
    _freeCam->setTheta(-M_PI / 3.0f);
    _freeCam->setPhi(M_PI / 2.0f);
    _freeCam->recomputeOrientation();
    _cameraSpeed = glm::vec2(0.25f, 0.02f);

    //Send all the lights to the gpu
    std::vector<glm::vec3> lightPositions = {glm::vec3(0), glm::vec3(0.0, 0.1, 0.0), glm::vec3(0.0, 1.0, 0.0), torchPos};
    std::vector<glm::vec3> lightDirections = {glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0), glm::vec3(0.0f, -0.1f, 1.0f), glm::vec3(0)};
    std::vector<glm::vec3> lightColors = {glm::vec3(5.f, 6.f, 5.0f), glm::vec3(500.0f, 250.0f, 250.0f), glm::vec3(2000.f, 2000.f, 5000.f), glm::vec3(150.f, 150.f, 150.f)};
    std::vector<uint32_t> lightTypes = {0, 1, 2, 1};
    std::vector<float> lightSizes = {0.f, 0.f, 1.f, 0.f};
    uint32_t numLights = lightDirections.size();
    glProgramUniform3f(_lightingShaderProgram->getShaderProgramHandle(), _lightingShaderUniformLocations.ambientColor, 0.1, 0.1, 0.1);
    glProgramUniform3fv(_lightingShaderProgram->getShaderProgramHandle(), _lightingShaderUniformLocations.lightPositions, numLights, &lightPositions[0][0]);
    glProgramUniform3fv(_lightingShaderProgram->getShaderProgramHandle(), _lightingShaderUniformLocations.lightDirections, numLights, &lightDirections[0][0]);
    glProgramUniform3fv(_lightingShaderProgram->getShaderProgramHandle(), _lightingShaderUniformLocations.lightColors, numLights, &lightColors[0][0]);
    glProgramUniform1uiv(_lightingShaderProgram->getShaderProgramHandle(), _lightingShaderUniformLocations.lightTypes, numLights, &lightTypes[0]);
    glProgramUniform1fv(_lightingShaderProgram->getShaderProgramHandle(), _lightingShaderUniformLocations.lightSizes, numLights, &lightSizes[0]);
    glProgramUniform1ui(_lightingShaderProgram->getShaderProgramHandle(), _lightingShaderUniformLocations.lightCount, numLights);
    fixCamera();
}

void FPEngine::_setupTextures() {
    glGenTextures(1, &skyboxTextureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    _createSkybox();
    _setupCubeFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X, "assets/textures/Yokohama2/posx.jpg");
    _setupCubeFace(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, "assets/textures/Yokohama2/negx.jpg");
    _setupCubeFace(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, "assets/textures/Yokohama2/posy.jpg");
    _setupCubeFace(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, "assets/textures/Yokohama2/negy.jpg");
    _setupCubeFace(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, "assets/textures/Yokohama2/posz.jpg");
    _setupCubeFace(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, "assets/textures/Yokohama2/negz.jpg");
}

void FPEngine::_setupCubeFace(GLint TextureTarget, const char* filename)
{
    //stbi_set_flip_vertically_on_load(true);

    // will hold image parameters after load
    GLint imageWidth, imageHeight, imageChannels;
    // load image from file
    GLubyte* data = stbi_load( filename, &imageWidth, &imageHeight, &imageChannels, 0);
    if (data)
    {
        const GLint STORAGE_TYPE = (imageChannels == 4 ? GL_RGBA : GL_RGB);

        glTexImage2D(TextureTarget,
                     0,
                     STORAGE_TYPE,
                     imageWidth,
                     imageHeight,
                     0,
                     STORAGE_TYPE,
                     GL_UNSIGNED_BYTE,
                     data
        );
        stbi_image_free(data);
    }
    else
    {
        std::cerr << "Could not open file " << filename << std::endl;
        exit(-1);
    }
}

void FPEngine::_createSkybox()
{
    const int numSkyboxVertices = 8;
    GLfloat skyboxVertices[3 * numSkyboxVertices] =
            {
                    //   Coordinates
                    -1.0f, -1.0f,  1.0f, // Front bottom left   0
                    1.0f, -1.0f,  1.0f,  // Front bottom right  1
                    1.0f, -1.0f, -1.0f,  // Back bottom right   2
                    -1.0f, -1.0f, -1.0f, // Back bottom left   3
                    -1.0f,  1.0f,  1.0f, // Front top left    4
                    1.0f,  1.0f,  1.0f, // Front top right    5
                    1.0f,  1.0f, -1.0f, // Back top right    6
                    -1.0f,  1.0f, -1.0f // Back top left     7
            };
    GLuint skyboxIndices[] =
            {
                    1, 2, 6,
                    6, 5, 1,
                    // Left
                    0, 4, 7,
                    7, 3, 0,
                    // Top
                    4, 5, 6,
                    6, 7, 4,
                    // Bottom
                    0, 3, 2,
                    2, 1, 0,
                    // Back
                    0, 1, 5,
                    5, 4, 0,
                    // Front
                    3, 7, 6,
                    6, 2, 3
            };

    glGenVertexArrays(1, &_vaos[SKY_BOX]);
    glGenBuffers(1, &_vbos[SKY_BOX]);
    glGenBuffers(1, &_ibos[SKY_BOX]);
    glBindVertexArray(_vaos[SKY_BOX]);
    glBindBuffer(GL_ARRAY_BUFFER, _vbos[SKY_BOX]);
    glBufferData(GL_ARRAY_BUFFER, 3*numSkyboxVertices * sizeof(GLfloat), &skyboxVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibos[SKY_BOX]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numSkyboxIndices * sizeof(GLuint), &skyboxIndices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(_ShaderProgramAttributeLocations.vPos);
    glVertexAttribPointer(_ShaderProgramAttributeLocations.vPos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*) 0);

}
//*************************************************************************************
//
// Engine Cleanup

void FPEngine::_cleanupShaders()
{
    fprintf(stdout, "[INFO]: ...deleting Shaders.\n");
    delete _lightingShaderProgram;
}

void FPEngine::_cleanupBuffers()
{
    fprintf(stdout, "[INFO]: ...deleting VAOs....\n");
    CSCI441::deleteObjectVAOs();

    fprintf(stdout, "[INFO]: ...deleting VBOs....\n");
    CSCI441::deleteObjectVBOs();

    fprintf(stdout, "[INFO]: ...deleting models..\n");
    _zennia->freeData();
    delete _zennia;
    delete _jammss;
    delete _plane;
}

//*************************************************************************************
//
// Rendering / Drawing Functions - this is where the magic happens!

void FPEngine::_renderScene(glm::mat4 viewMtx, glm::mat4 projMtx) const
{
    // if either shader program is null, do not continue any further to prevent run time errors
    if(!_lightingShaderProgram || !_textureShaderProgram) {
        return;
    }
    //***************************************************************************
    // draw the Skybox
    glDepthMask(GL_FALSE);
    _textureShaderProgram->useProgram();
    GLfloat skyboxScale = 800.0f;
    glm::mat4 skyBoxModelMtx = glm::mat4(1.0f);
    skyBoxModelMtx = glm::scale(skyBoxModelMtx, glm::vec3(skyboxScale, skyboxScale, skyboxScale));
    _computeAndSendTransformationMatrices( _textureShaderProgram,
                                           skyBoxModelMtx, viewMtx, projMtx,
                                           _textureShaderUniformLocations.mvpMatrix,
                                           -1,
                                           -1);
    glBindVertexArray(_vaos[SKY_BOX]);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);
    glDrawElements(GL_TRIANGLES, numSkyboxIndices, GL_UNSIGNED_INT, 0);
    glDepthMask(GL_TRUE);

    //***************************************************************************
    // use our lighting shader program
    _lightingShaderProgram->useProgram();
    // update viewMtx from camera position
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.vMatrix, viewMtx);
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.pMatrix, projMtx);

    //// BEGIN DRAWING THE GROUND PLANE ////
    // draw the ground plane
    glm::mat4 groundModelMtx = glm::scale(glm::mat4(1.0f), glm::vec3(WORLD_SIZE, 1.0f, WORLD_SIZE));
    _computeAndSendMatrixUniforms(groundModelMtx);

    glm::vec3 groundColor(1.0f, 1.0f, 1.0f);
    glUniform3fv(_lightingShaderUniformLocations.materialColor, 1, &groundColor[0]);
    Engine::drawObj(worldMeshes, 4);
    //// END DRAWING THE GROUND PLANE ////

    //// BEGIN DRAWING THE BUILDINGS ////
    for (const BuildingData &currentBuilding : _buildings)
    {
        _computeAndSendMatrixUniforms(currentBuilding.modelMatrix);
        //choose color based on which model we are drawing
        glUniform3fv(_lightingShaderUniformLocations.materialColor, 1, &glm::vec3(0.1, 0.5, 0.1)[0]);
        if(currentBuilding.buildingId == 0)
            glUniform3fv(_lightingShaderUniformLocations.materialColor, 1, &glm::vec3(0.1, 0.5, 0.1)[0]);
        else if(currentBuilding.buildingId == 1)
            glUniform3fv(_lightingShaderUniformLocations.materialColor, 1, &glm::vec3(0.5, 0.5, 0.1)[0]);
        else if(currentBuilding.buildingId == 2)
            glUniform3fv(_lightingShaderUniformLocations.materialColor, 1, &glm::vec3(0.1, 0.1, 1.0)[0]);
        else if(currentBuilding.buildingId == 3)
            glUniform3fv(_lightingShaderUniformLocations.materialColor, 1, &glm::vec3(0.1, 1.0, 0.1)[0]);

        Engine::drawObj(worldMeshes, currentBuilding.buildingId);
    }
    //// END DRAWING THE BUILDINGS ////

    glm::mat4 modelMtx(1.0f);
    // don't draw the character in first person
    if (heroToggle != 0 || cameraToggle != 2)
        _zennia->drawZennia(modelMtx, viewMtx, projMtx);
    if (heroToggle != 1 || cameraToggle != 2)
        _jammss->drawJammss(modelMtx, viewMtx, projMtx);
    if (heroToggle != 2 || cameraToggle != 2) {
        auto t1 = glm::translate(modelMtx, _plane->getPosition());
        t1 = glm::scale(t1, glm::vec3(3, 3, 3));
        _plane->drawPlane(t1, viewMtx, projMtx);
    }
    //// END DRAWING THE PLANE ////

    //// Animate Torch Light ////
    _drawTorch(modelMtx, viewMtx, projMtx);
}

void FPEngine::_updateScene()
{
    // fly
    GLfloat theta;
    float movementSpeed;
    if (_keys[GLFW_KEY_W])
    {
        if (cameraToggle == 0 || cameraToggle == 2) {
            if (heroToggle == 0){
                _zennia->flyForward();
                fixCamera();
            }
            else if (heroToggle == 1){
                _jammss->walkForward();
                fixCamera();
            }
            else if (heroToggle == 2) {
                theta = _plane->getOrientation().theta;
                movementSpeed = 0.15;
                _plane->flyForward(glm::vec3(movementSpeed*cos(-theta), 0.0, movementSpeed*sin(-theta)));
                fixCamera();
            }
        }
        if (cameraToggle == 1) {
            _freeCam->rotate(0.0f, _cameraSpeed.y);
        }
    }
    if (_keys[GLFW_KEY_S])
    {
        if (cameraToggle == 0 || cameraToggle == 2) {
            if (heroToggle == 0)
                _zennia->flyBackward();
            else if (heroToggle == 1)
                _jammss->walkForward();
            else if (heroToggle == 2) {
                theta = _plane->getOrientation().theta;;
                movementSpeed = 0.15;
                _plane->flyBackward(glm::vec3(-movementSpeed*cos(-theta),0.0, -movementSpeed*sin(-theta)));
            }
            fixCamera();
        }
        if (cameraToggle == 1) {
            _freeCam->rotate(0.0f, -_cameraSpeed.y);
        }
    }
    // turn right
    if (_keys[GLFW_KEY_D])
    {
        if (cameraToggle == 0 || cameraToggle == 2) {
           if(cameraToggle == 0)  _freeCam->rotate(_cameraSpeed.y, 0.0f);
            if (heroToggle == 0)
                _zennia->angle += _cameraSpeed.y;
            else if (heroToggle == 1)
                _jammss->angle += _cameraSpeed.y;
            else if (heroToggle == 2)
                _plane->rotate(-0.1);
            fixCamera();
        }
        if (cameraToggle == 1) {
            _freeCam->rotate(_cameraSpeed.y, 0.0f);
        }
    }
    // turn left
    if (_keys[GLFW_KEY_A])
    {
        if (cameraToggle == 0 || cameraToggle == 2) {
            if(cameraToggle == 0) _freeCam->rotate(-_cameraSpeed.y, 0.0f);
            if (heroToggle == 0)
                _zennia->angle -= _cameraSpeed.y;
            else if (heroToggle == 1)
                _jammss->angle -= _cameraSpeed.y;
            else if (heroToggle == 2)
                _plane->rotate(0.1);
            fixCamera();
        }
        if (cameraToggle == 1) {
            _freeCam->rotate(-_cameraSpeed.y, 0.0f);
        }
    }
    if( _keys[GLFW_KEY_SPACE] ) {
        // go backward if shift held down
        if (cameraToggle == 1) {
            if (_keys[GLFW_KEY_LEFT_SHIFT] || _keys[GLFW_KEY_RIGHT_SHIFT]) {
                _freeCam->moveBackward(_cameraSpeed.x);
            }
                // go forward
            else {
                _freeCam->moveForward(_cameraSpeed.x);
            }
        }
    }

}

void FPEngine::_computeAndSendTransformationMatrices(CSCI441::ShaderProgram* shaderProgram,
                                               glm::mat4 modelMatrix, glm::mat4 viewMatrix, glm::mat4 projectionMatrix,
                                               GLint mvpMtxLocation, GLint modelMtxLocation, GLint normalMtxLocation) const {
    // ensure our shader program is not null
    if( shaderProgram ) {
        // precompute the MVP matrix CPU side
        glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
        // precompute the Normal matrix CPU side
        glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelMatrix)));

        // send the matrices to the shader
        shaderProgram->setProgramUniform(mvpMtxLocation, mvpMatrix);
        if (modelMtxLocation != -1)
        {
            shaderProgram->setProgramUniform(modelMtxLocation, modelMatrix);
        }
        if (normalMtxLocation != -1)
        {
            shaderProgram->setProgramUniform(normalMtxLocation, normalMatrix);
        }
    }
}


// resets the camera position to be correct given the camera type
void FPEngine::fixCamera()
{
    _freeCam->recomputeOrientation();
    if (heroToggle == 0) {
        if (cameraToggle == 0) {
            _freeCam->setPosition(glm::vec3(_zennia->x, 0, _zennia->y) +
                                  zoom * (_freeCam->getPosition() - _freeCam->getLookAtPoint()));
        }
        if (cameraToggle == 2) {
            _freeCam->setPosition(glm::vec3(_zennia->x, 1.0, _zennia->y));
            _freeCam->setPhi(M_PI / 2);
            _freeCam->setTheta(_zennia->angle + M_PI/2);
        }

    } else if (heroToggle == 1){
        if (cameraToggle == 0) {
            _freeCam->setPosition(
                    glm::vec3(_jammss->x, 0, _jammss->y) +
                    zoom * (_freeCam->getPosition() - _freeCam->getLookAtPoint()));
        }
        if (cameraToggle == 2) {
            _freeCam->setPosition(glm::vec3(_jammss->x, 1.0, _jammss->y));
            _freeCam->setPhi(M_PI / 2);
            _freeCam->setTheta(_jammss->angle - M_PI/2);
        }
    } else if (heroToggle == 2){
        if (cameraToggle == 0) {
            _freeCam->setPosition(
                    glm::vec3(_plane->getPosition().x, _plane->getPosition().y, _plane->getPosition().z) +
                    zoom * (_freeCam->getPosition() - _freeCam->getLookAtPoint()));
        }
        if (cameraToggle == 2) {
            _freeCam->setPosition(glm::vec3(_plane->getPosition().x, _plane->getPosition().y + 1.0f, _plane->getPosition().z));
            _freeCam->setPhi(M_PI/2);
            _freeCam->setTheta(-_plane->getOrientation().theta + M_PI/2);
        }
    }
    _freeCam->recomputeOrientation();
}

void FPEngine::run()
{
    //  This is our draw loop - all rendering is done here.  We use a loop to keep the window open
    //	until the user decides to close the window and quit the program.  Without a loop, the
    //	window will display once and then the program exits.
    while (!glfwWindowShouldClose(_window))
    {                                                       // check if the window was instructed to be closed
        glDrawBuffer(GL_BACK);                              // work with our back frame buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the current color contents and depth buffer in the window

        // Get the size of our framebuffer.  Ideally this should be the same dimensions as our window, but
        // when using a Retina display the actual window can be larger than the requested window.  Therefore,
        // query what the actual size of the window we are rendering to is.
        GLint framebufferWidth, framebufferHeight;
        glfwGetFramebufferSize(_window, &framebufferWidth, &framebufferHeight);

        // update the viewport - tell OpenGL we want to render to the whole window
        glViewport(0, 0, framebufferWidth, framebufferHeight);

        // set the projection matrix based on the window size
        // use a perspective projection that ranges
        // with a FOV of 45 degrees, for our current aspect ratio, and Z ranges from [0.001, 10000].
        glm::mat4 projectionMatrix = glm::perspective(45.0f, (GLfloat)framebufferWidth / (GLfloat)framebufferHeight, 0.001f, 10000.0f);

        // set up our look at matrix to position our camera
        glm::mat4 viewMatrix = _freeCam->getViewMatrix();

        // draw everything to the window
        glViewport(0, 0, framebufferWidth, framebufferHeight);
        _renderScene(viewMatrix, projectionMatrix);
        glClear(GL_DEPTH_BUFFER_BIT); // clear the current color contents and depth buffer in the window
        //draw minimap
        glViewport(0, 0, framebufferWidth / 3, framebufferHeight / 3);
        if (heroToggle == 0)
            _renderScene(glm::lookAt(glm::vec3(_zennia->x, 10, _zennia->y), glm::vec3(_zennia->x, 0, _zennia->y), glm::vec3(1, 0, 0)), projectionMatrix);
        else if (heroToggle == 1)
            _renderScene(glm::lookAt(glm::vec3(_jammss->x, 10, _jammss->y), glm::vec3(_jammss->x, 0, _jammss->y), glm::vec3(1, 0, 0)), projectionMatrix);
        else if (heroToggle == 2)
            _renderScene(glm::lookAt(glm::vec3(_plane->getPosition().x, 10, _plane->getPosition().y), glm::vec3(_plane->getPosition().x, 0, _plane->getPosition().y), glm::vec3(1, 0, 0)), projectionMatrix);
        _updateScene();

        glfwSwapBuffers(_window); // flush the OpenGL commands and make sure they get rendered!
        glfwPollEvents();         // check for any events and signal to redraw screen
    }
}

//*************************************************************************************
//
// Private Helper FUnctions

void FPEngine::_computeAndSendMatrixUniforms(glm::mat4 modelMtx) const
{
    // then send it to the shader on the GPU to apply to every vertex
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.mMatrix, modelMtx);

    glm::mat3 normalMtx = glm::mat3(glm::transpose(glm::inverse(modelMtx)));
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.normalMatrix, normalMtx);
}

/*glm::mat4 Engine::getPrismModelMatrix(glm::mat4 translateMatrix,
                                      glm::mat4 rotationMatrix,
                                      glm::vec3 dimensions,
                                      glm::vec3 offset) const {
    glm::mat4 pointTranslation = glm::translate(
            glm::mat4(1.f),
            glm::vec3(offset.x, dimensions.y / 2 + offset.y, offset.z));
    glm::mat4 scale = glm::scale(glm::mat4(1.f), dimensions);
    return translateMatrix * rotationMatrix * pointTranslation * scale;
}*/

void FPEngine::_drawTorch(glm::mat4 modelMatrix, glm::mat4 viewMatrix, glm::mat4 projMatrix) const {
    glm::vec3 color = glm::vec3(112 / 255.f, 58 / 255.f, 29 / 255.f);
    glUniform3fv(_lightingShaderUniformLocations.materialColor, 1, &color[0]);
    glm::mat4 translateToSpot = glm::translate(modelMatrix, torchPos);
    _computeAndSendMatrixUniforms(translateToSpot);
    CSCI441::drawSolidCylinder(0.2f, 0.2f, 2.f, 20, 20);

    color = glm::vec3(232 / 255.f, 47 / 255.f, 23 / 255.f);
    glUniform3fv(_lightingShaderUniformLocations.materialColor, 1, &color[0]);
    //_computeAndSendMatrixUniforms(modelMatrix);
    translateToSpot = glm::translate(translateToSpot, glm::vec3(0.0, 2.0, 0.0));
    _computeAndSendMatrixUniforms(translateToSpot);
    glm::vec3 pointLightColor =
            glm::vec3((150 + 100*glm::sin(3*glfwGetTime())),
                      (150 + 100*glm::sin(3*glfwGetTime())),
                      (150 + 100*glm::sin(3*glfwGetTime())));
    std::vector<glm::vec3> lightColors = {glm::vec3(5.f, 6.f, 5.0f), glm::vec3(500.0f, 250.0f, 250.0f), glm::vec3(2000.f, 2000.f, 5000.f), pointLightColor};
    uint32_t numLights = lightColors.size();
    glProgramUniform3fv(_lightingShaderProgram->getShaderProgramHandle(), _lightingShaderUniformLocations.lightColors, numLights, &lightColors[0][0]);
    CSCI441::drawSolidSphere(0.6f + (glm::sin(3*glfwGetTime())/5.f) , 20, 20);
};

//*************************************************************************************
//
// Callbacks

void lab05_engine_keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    auto engine = (FPEngine *)glfwGetWindowUserPointer(window);

    // pass the key and action through to the engine
    engine->handleKeyEvent(key, action);
}

void lab05_engine_cursor_callback(GLFWwindow *window, double x, double y)
{
    auto engine = (FPEngine *)glfwGetWindowUserPointer(window);

    // pass the cursor position through to the engine
    engine->handleCursorPositionEvent(glm::vec2(x, y));
}

void lab05_engine_mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    auto engine = (FPEngine *)glfwGetWindowUserPointer(window);

    // pass the mouse button and action through to the engine
    engine->handleMouseButtonEvent(button, action);
}