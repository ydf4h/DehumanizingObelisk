#include <vector>
#include <iostream>

#include <glad4.6/glad.h>

#include <shader.hpp>
#include <DeOui.hpp>

void deoui::sUIlayer(const GLFWvidmode* video, Shader* pShader){
    //Be sure that the shader used here is not used for anything other than UI or it will be messed up
    glm::mat4 projection = glm::ortho(0, video->width, video->height, 0);

    pShader->use();
    pShader->setMat4("projection", projection);
}

void deoui::BGrect::Draw(Shader* usedShader, glm::mat4 transform){
    usedShader->use();

    transform = glm::translate(transform, position);
    usedShader->setMat4("model", transform);

    glBindVertexArray(VAO);

    glDisable(GL_DEPTH_TEST);

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

    glBindVertexArray(VAO);

    glDisable(GL_DEPTH_TEST);

    glEnable(GL_STENCIL_TEST);

    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_EQUAL, 1, 0xFF);
    glStencilMask(0x00);

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

bool deoui::button::pollStatus(){
    double xPos;
    double yPos;
    glfwGetCursorPos(habitat, &xPos, &yPos);

    bool collX = 0;
    bool collY = 0;

    if(xPos <= AABB[3] && xPos >= AABB[0]){
        collX = 1;
    }

    if(yPos <= AABB[2] && yPos >= AABB[1]){
        collY = 1;
    }

    //check fi cursor in box and check press if no then check release because toggle and activr state so toggle off on and compare past state to current

    GLenum clickState = glfwGetMouseButton(habitat, GLFW_MOUSE_BUTTON_1);

    if(collX && collY){
        if(prevClick == GLFW_PRESS && clickState == GLFW_RELEASE){
            if(!toggled){
                toggled = 1;
            }else{
                toggled = 0;
            }
        }
    }

    prevClick = clickState;

    /*if(status == deouiState::DISABLING && glfwGetMouseButton(habitat, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE){
        status = deouiState::TOGGLED_OFF;
    }else if(status == deouiState::ACTIVE && glfwGetMouseButton(habitat, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE){
        status = deouiState::TOGGLED_ON;
    }else if(status == deouiState::TOGGLED_ON){
        if(glfwGetMouseButton(habitat, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS){
            status = deouiState::DISABLING;
        }else{
            status = deouiState::TOGGLED_ON;
        }
    }else if(status == deouiState::TOGGLED_OFF){
        if(glfwGetMouseButton(habitat, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS){
            status = deouiState::ACTIVE;
        }else{
            status = deouiState::TOGGLED_OFF;
        }
    }else if(collX && collY){
        if(glfwGetMouseButton(habitat, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS){
            status = deouiState::ACTIVE;
        }else{
            status = deouiState::HOVER;
        }
    }else{
        status = deouiState::STILL;
    }*/

    return toggled;
}