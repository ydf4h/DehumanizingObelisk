#pragma once

#include <vector>
#include <map>

#include <GLFW/glfw3.h>
#include <glad4.6/glad.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include <shader.hpp>

namespace deoui{
    struct Character {
        unsigned int TextureID;
        glm::ivec2 size;
        glm::ivec2 bearing;
        unsigned int advance;
    };
    inline std::map<char, Character> Characters;

    void loadFont(const char* pathToFont, unsigned int count);

    void renderText(Shader* shade, unsigned int VAO, unsigned int VBO, std::string text, float x, float y, float scale, glm::vec3 color);

    class UIelem{
    public:
        std::vector<float> vertexData;
        std::vector<unsigned int> indices;
        glm::vec3 scale;
        glm::vec3 position;
        int width, height;
        glm::vec3 RGBcolor;
        glm::vec3 outColor;
        unsigned int VAO, VBO;
        unsigned int outVAO, outVBO;
        virtual void Draw(Shader* usedShader, glm::mat4 transform);
    };

    typedef UIelem BGrect;

    void genBGrect(BGrect* target, glm::vec3 position, int width, int height);

    class checkbox : public UIelem{
    public:
        GLFWwindow* habitat;
        std::vector<float> AABB;
        bool colX = 0, colY = 0;
        bool active = 0;
        bool* value;
        unsigned int textVAO;
        unsigned int textVBO;
        std::string name;
        void pollStatus();
        virtual void Draw(Shader* usedShader, Shader* text, glm::mat4 transform);
    };

    void genCheckbox(checkbox* target, GLFWwindow* habitat, bool* value, glm::vec3 position, int width, int height);

    class button : public UIelem{
    public:
        GLFWwindow* habitat;
        std::vector<float> AABB;
        bool colX = 0, colY = 0;
        bool active = 0;
        unsigned int textVAO;
        unsigned int textVBO;
        std::string name;
        void pollStatus();
        virtual void Draw(Shader* usedShader, Shader* text, glm::mat4 transform);
    };

    void genButton(button* target, GLFWwindow* habitat, glm::vec3 position, int width, int height);

    class textbox : public UIelem {
    public:

    };

    class UIlayer{
    public:
        GLFWwindow* habitat;

        BGrect* pane;
        std::vector<button*> buttons;
        std::vector<checkbox*> checkboxes;

        Shader* shade;
        Shader* textShade;

        int width, height;

        unsigned int resizeTex;
        unsigned int resizeVAO;
        glm::vec3 resizePos;
        bool colX = 0, colY = 0;
        std::vector<float> resizeAABB;

        UIlayer(GLFWwindow* habitat, Shader* shade, Shader* textShade, unsigned int width, unsigned int height, unsigned int resizeTex, glm::vec2 position, glm::vec3 RGBcolor, glm::vec3 outColor);

        void addButton(std::string name);
        void addCheckbox(std::string name, bool* value);
        void addTextbox(std::string text);

        void drawLayer(glm::mat4 transform, const GLFWvidmode* video);

        void deleteLayer();
    };
}