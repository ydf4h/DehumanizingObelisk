#pragma once

#include <vector>

#include <GLFW/glfw3.h>
#include <glad4.6/glad.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include <shader.hpp>

namespace deoui{
    class UIelem{
    public:
        std::vector<float> vertexData;
        std::vector<unsigned int> indices;
        glm::vec3 position;
        glm::vec3 RGBcolor;
        unsigned int VAO;
        virtual void Draw(Shader* usedShader, glm::mat4 transform);
    };

    typedef UIelem BGrect;

    void genBGrect(BGrect* target, glm::vec3 position, int width, int height);

    class button : public UIelem{
    public:
        GLFWwindow* habitat;
        std::vector<float> AABB;
        bool colX;
        bool colY;
        bool active;
        void pollStatus();
        virtual void Draw(Shader* usedShader, glm::mat4 transform);
    };

    void genButton(button* target, GLFWwindow* habitat, glm::vec3 position, int width, int height);

    class UIlayer{
    public:
        GLFWwindow* habitat;
        std::vector<UIelem*> members;
        Shader* shade;

        UIlayer(GLFWwindow* habitat, Shader* shade);

        virtual void drawLayer(glm::mat4 transform, const GLFWvidmode* video);
    };
}