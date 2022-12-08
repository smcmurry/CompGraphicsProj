//
// Created by luigi on 10/14/2022.
//

#ifndef MIDTERM_PROJECT_BINGUSTWO_H
#define MIDTERM_PROJECT_BINGUSTWO_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Character.hpp"

class BingusTwo : public Character {
public:

    BingusTwo(GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation,
              GLint materialColorUniformLocation, GLint materialBrightnessUniformLocation );

    void drawCharacter(glm::mat4 viewMtx, glm::mat4 projMtx) override;

private:

    //hierarchical functions called in drawCharacter
    void drawBingusTwoHead(glm::mat4, glm::mat4, glm::mat4, glm::mat4) const;
    void drawBingusTwoBody(glm::mat4, glm::mat4, glm::mat4, glm::mat4) const;

};

#endif //MIDTERM_PROJECT_BINGUSTWO_H
