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
        else
        {
            // rotate the camera by the distance the mouse moved
            _freeCam->rotate((currMousePosition.x - _mousePosition.x) * 0.005f,
                             (_mousePosition.y - currMousePosition.y) * 0.005f);
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
    _lightingShaderUniformLocations.mvpMatrix = _lightingShaderProgram->getUniformLocation("mvpMatrix");
    _lightingShaderUniformLocations.materialColor = _lightingShaderProgram->getUniformLocation("materialColor");
    _lightingShaderUniformLocations.lightDirection = _lightingShaderProgram->getUniformLocation("lightDirection");
    _lightingShaderUniformLocations.lightColor = _lightingShaderProgram->getUniformLocation("lightColor");
    _lightingShaderUniformLocations.normalMatrix = _lightingShaderProgram->getUniformLocation("normalMatrix");

    _lightingShaderAttributeLocations.vPos = _lightingShaderProgram->getAttributeLocation("vPos");
    _lightingShaderAttributeLocations.vNormal = _lightingShaderProgram->getAttributeLocation("vNormal");
}

void Lab05Engine::_setupBuffers()
{
    CSCI441::setVertexAttributeLocations(_lightingShaderAttributeLocations.vPos, _lightingShaderAttributeLocations.vNormal);


    Engine::MeshData *readData = Engine::readOBJ("./assets.obj");
    // for(int i = 0; i < readData.points[0].size(); i++){
    //     std::cout << "v"<<readData.points[0][i].x <<" "<< readData.points[0][i].y <<" "<< readData.points[0][i].z << std::endl;
    //     std::cout << "n"<<readData.normals[0][i].x <<" "<< readData.normals[0][i].y <<" "<< readData.normals[0][i].z << std::endl;
    // }

    _zennia = new Zennia(_lightingShaderProgram->getShaderProgramHandle(),
                         _lightingShaderUniformLocations.mvpMatrix,
                         _lightingShaderUniformLocations.normalMatrix,
                         _lightingShaderUniformLocations.materialColor,
                         readData);

    _createGroundBuffers();
    _generateEnvironment();
}

void Lab05Engine::_createGroundBuffers()
{
    struct Vertex
    {
        GLfloat x, y, z;
        GLfloat xn, yn, zn;
    };

    Vertex groundQuad[4] = {
        {-1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f},
        {1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f},
        {-1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
        {1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f}};

    GLushort indices[4] = {0, 1, 2, 3};

    _numGroundPoints = 4;

    glGenVertexArrays(1, &_groundVAO);
    glBindVertexArray(_groundVAO);

    GLuint vbods[2]; // 0 - VBO, 1 - IBO
    glGenBuffers(2, vbods);
    glBindBuffer(GL_ARRAY_BUFFER, vbods[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundQuad), groundQuad, GL_STATIC_DRAW);

    glEnableVertexAttribArray(_lightingShaderAttributeLocations.vPos);
    glVertexAttribPointer(_lightingShaderAttributeLocations.vPos, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

    glEnableVertexAttribArray(_lightingShaderAttributeLocations.vNormal);
    glVertexAttribPointer(_lightingShaderAttributeLocations.vNormal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(3 * sizeof(float)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbods[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
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
                glm::mat4 transToSpotMtx = glm::translate(glm::mat4(1.0), glm::vec3(i, 0.0f, j));

                // compute random height
                GLdouble height = powf(getRand(), 2.5) * 10 + 1;
                // scale to building size
                glm::mat4 scaleToHeightMtx = glm::scale(glm::mat4(1.0), glm::vec3(1, height, 1));

                // translate up to grid
                glm::mat4 transToHeight = glm::translate(glm::mat4(1.0), glm::vec3(0, height / 2.0f, 0));

                // compute full model matrix
                glm::mat4 modelMatrix = transToHeight * scaleToHeightMtx * transToSpotMtx;

                // compute random color
                glm::vec3 color(getRand(), getRand(), getRand());
                // store building properties
                BuildingData currentBuilding = {modelMatrix, color};
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

    glm::vec3 lightDir = glm::vec3(-1.0f, -1.0f, -1.0f);
    glm::vec3 lightCol = glm::vec3(1.0f, 1.0f, 1.0f);
    glProgramUniform3fv(_lightingShaderProgram->getShaderProgramHandle(), _lightingShaderUniformLocations.lightDirection, 1, &lightDir[0]);
    glProgramUniform3fv(_lightingShaderProgram->getShaderProgramHandle(), _lightingShaderUniformLocations.lightColor, 1, &lightCol[0]);
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
    glDeleteVertexArrays(1, &_groundVAO);

    fprintf(stdout, "[INFO]: ...deleting VBOs....\n");
    CSCI441::deleteObjectVBOs();

    fprintf(stdout, "[INFO]: ...deleting models..\n");
    _zennia->freeData();
    delete _zennia;
}

//*************************************************************************************
//
// Rendering / Drawing Functions - this is where the magic happens!

void Lab05Engine::_renderScene(glm::mat4 viewMtx, glm::mat4 projMtx) const
{
    // use our lighting shader program
    _lightingShaderProgram->useProgram();

    //// BEGIN DRAWING THE GROUND PLANE ////
    // draw the ground plane
    glm::mat4 groundModelMtx = glm::scale(glm::mat4(1.0f), glm::vec3(WORLD_SIZE, 1.0f, WORLD_SIZE));
    _computeAndSendMatrixUniforms(groundModelMtx, viewMtx, projMtx);

    glm::vec3 groundColor(0.3f, 0.8f, 0.2f);
    glUniform3fv(_lightingShaderUniformLocations.materialColor, 1, &groundColor[0]);

    glBindVertexArray(_groundVAO);
    glDrawElements(GL_TRIANGLE_STRIP, _numGroundPoints, GL_UNSIGNED_SHORT, (void *)0);
    //// END DRAWING THE GROUND PLANE ////

    //// BEGIN DRAWING THE BUILDINGS ////
    for (const BuildingData &currentBuilding : _buildings)
    {
        _computeAndSendMatrixUniforms(currentBuilding.modelMatrix, viewMtx, projMtx);

        glUniform3fv(_lightingShaderUniformLocations.materialColor, 1, &currentBuilding.color[0]);

        CSCI441::drawSolidCube(1.0);
    }
    //// END DRAWING THE BUILDINGS ////

    //// BEGIN DRAWING THE PLANE ////
    glm::mat4 modelMtx(1.0f);
    // draw our plane now
    _zennia->drawZennia(modelMtx, viewMtx, projMtx);
    //// END DRAWING THE PLANE ////
}

void Lab05Engine::_updateScene()
{
    // fly
    if (_keys[GLFW_KEY_W])
    {
        _zennia->flyForward();
        fixCamera();
    }
    if (_keys[GLFW_KEY_S])
    {
        _zennia->flyBackward();
        fixCamera();
    }
    // turn right
    if (_keys[GLFW_KEY_D])
    {
        _freeCam->rotate(_cameraSpeed.y, 0.0f);
        _zennia->angle += _cameraSpeed.y;
        fixCamera();
    }
    // turn left
    if (_keys[GLFW_KEY_A])
    {
        _freeCam->rotate(-_cameraSpeed.y, 0.0f);
        _zennia->angle -= _cameraSpeed.y;
        fixCamera();
    }
}

void Lab05Engine::fixCamera()
{
    _freeCam->recomputeOrientation();
    _freeCam->setPosition(glm::vec3(_zennia->x, 0, _zennia->y) + zoom * (_freeCam->getPosition() - _freeCam->getLookAtPoint()));
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
        _renderScene(glm::lookAt(glm::vec3(_zennia->x, 10, _zennia->y), glm::vec3(_zennia->x, 0, _zennia->y), glm::vec3(1, 0, 0)), projectionMatrix);

        _updateScene();

        glfwSwapBuffers(_window); // flush the OpenGL commands and make sure they get rendered!
        glfwPollEvents();         // check for any events and signal to redraw screen
    }
}

//*************************************************************************************
//
// Private Helper FUnctions

void Lab05Engine::_computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const
{
    // precompute the Model-View-Projection matrix on the CPU
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    // then send it to the shader on the GPU to apply to every vertex
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.mvpMatrix, mvpMtx);

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
