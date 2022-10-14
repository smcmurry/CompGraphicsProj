#include "Lab05Engine.hpp"

#include <CSCI441/objects.hpp>
#include <CSCI441/SimpleShader.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

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

Lab05Engine::Lab05Engine()
    : CSCI441::OpenGLEngine(4, 1,
                            640, 480,
                            "Lab05: Flight Simulator v0.41 alpha")
{

    for (auto &_key : _keys)
        _key = GL_FALSE;

    _mousePosition = glm::vec2(MOUSE_UNINITIALIZED, MOUSE_UNINITIALIZED);
    _leftMouseButtonState = GLFW_RELEASE;
    zoom = 3.0;
}

Lab05Engine::~Lab05Engine()
{
    delete _freeCam;
}

void Lab05Engine::handleKeyEvent(GLint key, GLint action)
{
    if (key != GLFW_KEY_UNKNOWN)
        _keys[key] = ((action == GLFW_PRESS) || (action == GLFW_REPEAT));

    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        // quit!
        case GLFW_KEY_C:
            if (cameraToggle == 0)
                cameraToggle = 1;
            else if (cameraToggle == 1)
                cameraToggle = 2;
            else
                cameraToggle = 0;
            break;
        case GLFW_KEY_X:
            if (heroToggle == 0)
                heroToggle = 1;
            else
                heroToggle = 0;
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

void Lab05Engine::handleMouseButtonEvent(GLint button, GLint action)
{
    // if the event is for the left mouse button
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        // update the left mouse button's state
        _leftMouseButtonState = action;
    }
}

void Lab05Engine::handleCursorPositionEvent(glm::vec2 currMousePosition)
{
    // if mouse hasn't moved in the window, prevent camera from flipping out
    if (_mousePosition.x == MOUSE_UNINITIALIZED)
    {
        _mousePosition = currMousePosition;
    }

    // if the left mouse button is being held down while the mouse is moving
    if (_leftMouseButtonState == GLFW_PRESS)
    {
        if (_keys[GLFW_KEY_LEFT_SHIFT])
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

void Lab05Engine::_setupGLFW()
{
    CSCI441::OpenGLEngine::_windowResizable = true;
    CSCI441::OpenGLEngine::_setupGLFW();

    // set our callbacks
    glfwSetKeyCallback(_window, lab05_engine_keyboard_callback);
    glfwSetMouseButtonCallback(_window, lab05_engine_mouse_button_callback);
    glfwSetCursorPosCallback(_window, lab05_engine_cursor_callback);
}

void Lab05Engine::_setupOpenGL()
{
    glEnable(GL_DEPTH_TEST); // enable depth testing
    glDepthFunc(GL_LESS);    // use less than depth test

    glEnable(GL_BLEND);                                // enable blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // use one minus blending equation

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // clear the frame buffer to black
}

void Lab05Engine::_setupShaders()
{
    _lightingShaderProgram = new CSCI441::ShaderProgram("shaders/lab05.v.glsl", "shaders/lab05.f.glsl");
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
}

void Lab05Engine::_setupBuffers()
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

    Engine::MeshData *worldData = Engine::readOBJ("./world_meshes.obj");
    worldMeshes = Engine::_getVao(*worldData);
    _generateEnvironment();
}

void Lab05Engine::_generateEnvironment()
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
                // scale to building size
                glm::mat4 scaleToHeightMtx = glm::scale(glm::mat4(1.0), glm::vec3(1, height, 1));

                // randomly rotate
                glm::mat4 randomRotate = glm::rotate(glm::mat4(1.0), getRand() * 360, glm::vec3(0, 1, 0));

                // compute full model matrix
                glm::mat4 modelMatrix = scaleToHeightMtx * transToSpotMtx*randomRotate;

                // store building properties
                BuildingData currentBuilding = {modelMatrix, (uint32_t) (4*getRand())};
                _buildings.emplace_back(currentBuilding);
            }
        }
    }
}

void Lab05Engine::_setupScene()
{
    _freeCam = new CSCI441::FreeCam();
    _freeCam->setPosition(glm::vec3(60.0f, 1.0f, 30.0f));
    _freeCam->setTheta(-M_PI / 3.0f);
    _freeCam->setPhi(M_PI / 2.0f);
    _freeCam->recomputeOrientation();
    _cameraSpeed = glm::vec2(0.25f, 0.02f);

    std::vector<glm::vec3> lightPositions = {glm::vec3(0), glm::vec3(0.0, 0.1, 0.0), glm::vec3(0.0, 1.0, 0.0)};
    std::vector<glm::vec3> lightDirections = {glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0), glm::vec3(0.0f, -0.1f, 1.0f)};
    std::vector<glm::vec3> lightColors = {glm::vec3(5.f, 6.f, 5.0f), glm::vec3(500.0f, 250.0f, 250.0f), glm::vec3(2000.f, 2000.f, 5000.f)};
    std::vector<uint32_t> lightTypes = {0, 1, 2};
    std::vector<float> lightSizes = {0.f, 0.f, 1.f};
    uint32_t numLights = lightDirections.size();
    glProgramUniform3f(_lightingShaderProgram->getShaderProgramHandle(), _lightingShaderUniformLocations.ambientColor, 0.1, 0.1, 0.1);
    glProgramUniform3fv(_lightingShaderProgram->getShaderProgramHandle(), _lightingShaderUniformLocations.lightPositions, numLights, &lightPositions[0][0]);
    glProgramUniform3fv(_lightingShaderProgram->getShaderProgramHandle(), _lightingShaderUniformLocations.lightDirections, numLights, &lightDirections[0][0]);
    glProgramUniform3fv(_lightingShaderProgram->getShaderProgramHandle(), _lightingShaderUniformLocations.lightColors, numLights, &lightColors[0][0]);
    glProgramUniform1uiv(_lightingShaderProgram->getShaderProgramHandle(), _lightingShaderUniformLocations.lightTypes, numLights, &lightTypes[0]);
    glProgramUniform1fv(_lightingShaderProgram->getShaderProgramHandle(), _lightingShaderUniformLocations.lightSizes, numLights, &lightSizes[0]);
    glProgramUniform1ui(_lightingShaderProgram->getShaderProgramHandle(), _lightingShaderUniformLocations.lightCount, numLights);
}

//*************************************************************************************
//
// Engine Cleanup

void Lab05Engine::_cleanupShaders()
{
    fprintf(stdout, "[INFO]: ...deleting Shaders.\n");
    delete _lightingShaderProgram;
}

void Lab05Engine::_cleanupBuffers()
{
    fprintf(stdout, "[INFO]: ...deleting VAOs....\n");
    CSCI441::deleteObjectVAOs();

    fprintf(stdout, "[INFO]: ...deleting VBOs....\n");
    CSCI441::deleteObjectVBOs();

    fprintf(stdout, "[INFO]: ...deleting models..\n");
    _zennia->freeData();
    delete _zennia;
    delete _jammss;
}

//*************************************************************************************
//
// Rendering / Drawing Functions - this is where the magic happens!

void Lab05Engine::_renderScene(glm::mat4 viewMtx, glm::mat4 projMtx) const
{
    // use our lighting shader program
    _lightingShaderProgram->useProgram();
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

    //// BEGIN DRAWING THE PLANE ////
    glm::mat4 modelMtx(1.0f);
    // draw our plane now
    _zennia->drawZennia(modelMtx, viewMtx, projMtx);
    _jammss->drawJammss(modelMtx, viewMtx, projMtx);
    //// END DRAWING THE PLANE ////
}

void Lab05Engine::_updateScene()
{
    // fly
    if (_keys[GLFW_KEY_W])
    {
        if (cameraToggle == 0 || cameraToggle == 2) {
            if (heroToggle)
                _zennia->flyForward();
            else
                _jammss->walkForward();
            fixCamera();
        }
        if (cameraToggle == 1) {
            _freeCam->rotate(0.0f, _cameraSpeed.y);
        }
    }
    if (_keys[GLFW_KEY_S])
    {
        if (cameraToggle == 0 || cameraToggle == 2) {
            if (heroToggle)
                _zennia->flyBackward();
            else
                _jammss->walkForward();
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
            _freeCam->rotate(_cameraSpeed.y, 0.0f);
            if (heroToggle)
                _zennia->angle += _cameraSpeed.y;
            else
                _jammss->angle += _cameraSpeed.y;
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
            _freeCam->rotate(-_cameraSpeed.y, 0.0f);
            if (heroToggle)
                _zennia->angle -= _cameraSpeed.y;
            else
                _jammss->angle -= _cameraSpeed.y;
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

void Lab05Engine::fixCamera()
{
    _freeCam->recomputeOrientation();
    if (heroToggle) {
        if (cameraToggle == 0) {
            _freeCam->setPosition(glm::vec3(_zennia->x, 0, _zennia->y) +
                                  zoom * (_freeCam->getPosition() - _freeCam->getLookAtPoint()));
        }
        if (cameraToggle == 2) {
            _freeCam->setPosition(glm::vec3(_zennia->x, 1.0f, _zennia->y));
            _freeCam->setLookAtPoint(glm::vec3(_zennia->x, 1.0f, _zennia->y) + glm::vec3(cos(_zennia->angle), 0.0f, sin(_zennia->angle)));
        }

    } else {
        if (cameraToggle == 0) {
            _freeCam->setPosition(
                    glm::vec3(_jammss->x, 0, _jammss->y) +
                    zoom * (_freeCam->getPosition() - _freeCam->getLookAtPoint()));
        }
        if (cameraToggle == 2) {
            _freeCam->setPosition(glm::vec3(_jammss->x, 1.0f, _jammss->y));
            _freeCam->setLookAtPoint(glm::vec3(_jammss->x, 1.0f, _jammss->y) + glm::vec3(cos(_jammss->angle), 0.0f, sin(_jammss->angle)));
        }
    }
    _freeCam->recomputeOrientation();
}

void Lab05Engine::run()
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
        // with a FOV of 45 degrees, for our current aspect ratio, and Z ranges from [0.001, 1000].
        glm::mat4 projectionMatrix = glm::perspective(45.0f, (GLfloat)framebufferWidth / (GLfloat)framebufferHeight, 0.001f, 1000.0f);

        // set up our look at matrix to position our camera
        glm::mat4 viewMatrix = _freeCam->getViewMatrix();

        // draw everything to the window
        glViewport(0, 0, framebufferWidth, framebufferHeight);
        _renderScene(viewMatrix, projectionMatrix);
        glClear(GL_DEPTH_BUFFER_BIT); // clear the current color contents and depth buffer in the window
        glViewport(0, 0, framebufferWidth / 3, framebufferHeight / 3);
        if (cameraToggle != 2 || !heroToggle)
            _renderScene(glm::lookAt(glm::vec3(_zennia->x, 10, _zennia->y), glm::vec3(_zennia->x, 0, _zennia->y), glm::vec3(1, 0, 0)), projectionMatrix);
        if (cameraToggle != 2 || heroToggle)
            _renderScene(glm::lookAt(glm::vec3(_jammss->x, 10, _jammss->y), glm::vec3(_jammss->x, 0, _jammss->y), glm::vec3(1, 0, 0)), projectionMatrix);
        _updateScene();

        glfwSwapBuffers(_window); // flush the OpenGL commands and make sure they get rendered!
        glfwPollEvents();         // check for any events and signal to redraw screen
    }
}

//*************************************************************************************
//
// Private Helper FUnctions

void Lab05Engine::_computeAndSendMatrixUniforms(glm::mat4 modelMtx) const
{
    // then send it to the shader on the GPU to apply to every vertex
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.mMatrix, modelMtx);

    glm::mat3 normalMtx = glm::mat3(glm::transpose(glm::inverse(modelMtx)));
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.normalMatrix, normalMtx);
}

//*************************************************************************************
//
// Callbacks

void lab05_engine_keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    auto engine = (Lab05Engine *)glfwGetWindowUserPointer(window);

    // pass the key and action through to the engine
    engine->handleKeyEvent(key, action);
}

void lab05_engine_cursor_callback(GLFWwindow *window, double x, double y)
{
    auto engine = (Lab05Engine *)glfwGetWindowUserPointer(window);

    // pass the cursor position through to the engine
    engine->handleCursorPositionEvent(glm::vec2(x, y));
}

void lab05_engine_mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    auto engine = (Lab05Engine *)glfwGetWindowUserPointer(window);

    // pass the mouse button and action through to the engine
    engine->handleMouseButtonEvent(button, action);
}
