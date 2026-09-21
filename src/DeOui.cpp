#include <vector>
#include <iostream>

#include <glad4.6/glad.h>

#include <shader.hpp>
#include <DeOui.hpp>
#include <eventHandler.hpp>

void deoui::UIlayer::drawLayer(glm::mat4 transform, const GLFWvidmode* video) {
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(video->width), static_cast<float>(video->height), 0.0f);
    shade->use();
    shade->setMat4("projection", projection);

    for (unsigned int i = 0; i < members.size(); i++) {
        members[i]->Draw(shade, transform);
    }
}

deoui::UIlayer::UIlayer(GLFWwindow* habitat, Shader* shade) {
    this->habitat = habitat;
    this->shade = shade;
}

void deoui::UIelem::Draw(Shader* usedShader, glm::mat4 transform){
    usedShader->use();

    transform = glm::translate(transform, position);
    usedShader->setMat4("model", transform);
    usedShader->setVec3("color", RGBcolor);

    glBindVertexArray(VAO);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);

    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);

    glStencilFunc(GL_EQUAL, 1, 0xFF);
    glStencilMask(0x00);

    glEnable(GL_DEPTH_TEST);

    glBindVertexArray(0);
}

void deoui::genBGrect(BGrect* target, glm::vec3 position, int width, int height){
    target->vertexData = {
        //bottom left
        0.0f, (float)height,
        //top right
        (float)width, 0.0f,
        //top left
        0.0f, 0.0f,
        //bottom right
        (float)width, (float)height
    };

    target->indices = {
        0, 1, 2,
        1, 0, 3
    };

    target->position = position;

    unsigned int VBO, EBO;

    glGenVertexArrays(1, &target->VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(target->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ARRAY_BUFFER, target->vertexData.size() * sizeof(float), &target->vertexData[0], GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, target->indices.size() * sizeof(unsigned int), &target->indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glBindVertexArray(0);
}

void deoui::button::Draw(Shader* usedShader, glm::mat4 transform){
    usedShader->use();

    transform = glm::translate(transform, position);
    usedShader->setMat4("model", transform);
    usedShader->setVec3("color", RGBcolor);

    glBindVertexArray(VAO);

    glDisable(GL_DEPTH_TEST);

    glEnable(GL_STENCIL_TEST);

    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_EQUAL, 1, 0xFF);
    glStencilMask(0xFF);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);

    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);

    glEnable(GL_DEPTH_TEST);

    glBindVertexArray(0);
}

void deoui::genButton(button* target, GLFWwindow* habitat, glm::vec3 position, int width, int height){
    target->habitat = habitat;

    target->vertexData = {
        //bottom left
        0.0f, (float)height,
        //top right
        (float)width, 0.0f,
        //top left
        0.0f, 0.0f,
        //bottom right
        (float)width, (float)height
    };

    target->indices = {
        0, 1, 2,
        1, 0, 3
    };

    target->position = position;

    target->AABB = {
        position.x, position.y,
        position.x + (float)width, position.y + (float)height
    };

    unsigned int VBO, EBO;

    glGenVertexArrays(1, &target->VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(target->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ARRAY_BUFFER, target->vertexData.size() * sizeof(float), &target->vertexData[0], GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, target->indices.size() * sizeof(unsigned int), &target->indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glBindVertexArray(0);
}

void deoui::button::pollStatus(){
    using namespace deoEvent;
    if (MM != nullptr) {
        if (MM->currentPos[0] <= this->AABB[2] && MM->currentPos[0] >= this->AABB[0]) {
            colX = 1;//there is a problem here with window sizing and AABB
        }else {
            colX = 0;
        }
        if (MM->currentPos[1] <= this->AABB[3] && MM->currentPos[1] >= this->AABB[1]) {
            colY = 1;//same problem here
        }else {
            colY = 0;
        }
    }

    if (colX && colY) {
        this->RGBcolor = glm::vec3(0.5f);
    }else {
        this->RGBcolor = glm::vec3(0.0f);
    }

    if (MLC != nullptr && colX && colY) {
        if (MLC->activity == GLFW_RELEASE && LMLC.activity == GLFW_PRESS) {
            int swap = 1 - active;
            active = swap;
        }
    }

    if (active) {
        this->RGBcolor = glm::vec3(1.0f);
    }
}