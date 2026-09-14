#pragma once

#include <vector>

#include <GLFW/glfw3.h>
#include <glad4.6/glad.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include <shader.hpp>

namespace deoui{
    class UIlayer{
    public:
        
    };

    void sUIlayer(const GLFWvidmode* window, Shader* pShader);

    class BGrect{
    public:
        std::vector<float> vertexData;
        std::vector<unsigned int> indices;

        glm::vec3 position;

        unsigned int VAO;

        void Draw(Shader* usedShader, glm::mat4 transform);
    };

    void genBGrect(BGrect* target, glm::vec3 position, int width, int height);

    class button{
    public:
        GLFWwindow* habitat;

        std::vector<float> vertexData;
        std::vector<unsigned int> indices;

        glm::vec3 position;

        std::vector<float> AABB;

        unsigned int VAO;

        bool toggled;

        GLenum prevClick;

        void Draw(Shader* usedShader, glm::mat4 transform);

        bool pollStatus();
    };

    void genButton(button* target, GLFWwindow* habitat, glm::vec3 position, int width, int height);
}