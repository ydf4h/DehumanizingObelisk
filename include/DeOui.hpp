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
    unsigned int textureFromFile(const char* path);//load texture data from a file into opengl

    void setupGLrectangle(unsigned int* VAO, unsigned int* VBO, unsigned int* EBO, const std::vector<float> vertices, const std::vector<unsigned int> indices);
    void setupGLrectangle(unsigned int* VAO, unsigned int* VBO, const std::vector<float> vertices);//basic rectangle

    void updGLrectangle(unsigned int VAO, unsigned int VBO, const std::vector<float> vertices);//update vertex attributes for a rectangle

    void setupGLtexRectangle(unsigned int* VAO, unsigned int* VBO, unsigned int* EBO, const std::vector<float> vertices, const std::vector<unsigned int> indices);//create full
    void setupGLtexRectangle(unsigned int* VAO, unsigned int* VBO, unsigned int* EBO, const std::vector<unsigned int> indices);//no vdata yet just indices first
    void setupGLtexRectangle(unsigned int* VAO, unsigned int* VBO);//textured rectangle w/o indices create but don't add data yet

    void updRectAABB(std::vector<float> *AABB, int width, int height, glm::vec2 position);//updates rect AABB based off of width, height, position
    void updCurInAABB(const std::vector<float> AABB, const double curPos[2], bool* colX, bool* colY);//updates cursor collision status for rect AABB

    std::string trimFSTR(std::string target);//trims off trailing zeros in a float represented by a string

    struct Character {
        unsigned int TextureID;
        glm::ivec2 size;
        glm::ivec2 bearing;
        unsigned int advance;
    };//contains texture id and metric info for glyphs to render them
    inline std::map<char, Character> Characters;//contains all loaded characters
    void loadFont(const char* pathToFont, unsigned int count);//loads a certain font with specified character count into Characters map
    void renderText(Shader* shade, unsigned int VAO, unsigned int VBO, std::string text, float x, float y, float scale, glm::vec3 color);//renders string

    struct BMPfont {
        unsigned int TextureID;
        unsigned int glyphVAO, glyphVBO;

        float xAdvance, yAdvance;
    };
    inline BMPfont mainFont;
    void loadBMPfont(const char* pathToFont, const int charDimensions[], int charCount, const int BMPdimensions[]);
    void renderBMPfont(Shader* shade, std::string text, glm::vec2 position, glm::mat4 transform, glm::vec3 color);

    class UIelem{
    public:
        std::vector<float> vertexData;//main rect vertices
        std::vector<unsigned int> indices;//outline and main rect indices
        unsigned int VAO, VBO;//main rect vao and vertex buffer

        glm::vec3 RGBcolor;//main rect color

        glm::vec3 position;//main rect position in pixels
        int width, height;//main rect width and height in pixels

        virtual void Draw(Shader* usedShader, glm::mat4 transform);//draw main rect and outline, virtual for various types
    };
    typedef UIelem BGrect;
    void genBGrect(BGrect* target, glm::vec3 position, int width, int height);

    class checkbox : public UIelem{
    public:
        std::vector<float> AABB;//aabb for collision detection with cursor
        bool colX = 0, colY = 0;//if the cursor collides with AABB on x and y axis
        bool* value;//boolean value that changes on checkbox activation
        unsigned int textVAO, textVBO;//text box
        std::string text;//text displayed to right of checkbox
        void pollStatus();
        void Draw(Shader* usedShader, Shader* text, glm::mat4 transform);
    };
    void genCheckbox(checkbox* target, bool* value, glm::vec3 position, int width, int height, std::string name);

    class button : public UIelem{
    public:
        std::vector<float> AABB;
        bool colX = 0, colY = 0;
        bool active = 0;
        unsigned int textVAO;
        unsigned int textVBO;
        std::string text;
        void pollStatus();
        void Draw(Shader* usedShader, Shader* text, glm::mat4 transform);
    };
    void genButton(button* target, glm::vec3 position, int width, int height, std::string name);

    class UIlayer;

    class paneHandle : public UIelem {
    public:
        UIlayer* parent;
        std::vector<float> paneAABB;
        bool colX = 0, colY = 0;
        bool moveActive = 0;
        glm::vec2 posDiff;
        button* close;
        checkbox* collapse;
        bool collapsed = 0;
        std::string name;
        unsigned int textVAO;
        unsigned int textVBO;
        void pollStatus();
        bool isMouseHover();
        bool isActive();
        void Draw(Shader* usedShader, Shader* text, glm::mat4 transform);
    };
    void genPaneHandle(paneHandle* target, UIlayer* parent, glm::vec2 position, int width, int height, std::string name);

    class resizeHandle : public UIelem {
    public:
        UIlayer* parent;
        std::vector<float> AABB;
        bool colX = 0, colY = 0;
        bool resizeActive = 0;
        glm::vec2 sizeDiff;
        unsigned int resizeTex;
        void pollStatus();
        void Draw(Shader* usedShader, glm::mat4 transform);
    };
    void genResizeHandle(resizeHandle* target, UIlayer* parent, glm::vec2 position, std::string pathToTex);

    class floatInField {
    public:
        unsigned int VAO, VBO;
        std::vector<float> vertexData;
        glm::vec2 position;

        unsigned int textVAO, textVBO;

        glm::vec3 RGBcolor;

        std::vector<float> AABB;
        bool colX = 0, colY = 0;
        bool active = 0;
        float* value;

        std::string displayValue;

        void Draw(Shader* usedShader, Shader* textShader, glm::mat4 transform);

        void pollStatus();
    };
    void genFloatInField(floatInField* target, glm::vec2 position, float* value);

    class vec3InField{
    public:
        floatInField* fields[3];
        glm::vec2 position;
        glm::vec3* vectorValue;

        void Draw(Shader* usedShader, Shader* textShader, glm::mat4 transform);

        void pollStatus();

        void updPositions(int diffX, int diffY);
    };
    void genVec3InField(vec3InField* target, glm::vec2 position, glm::vec3* value);

    class UIlayer{
    public:
        BGrect* pane;
        std::vector<button*> buttons;
        std::vector<checkbox*> checkboxes;
        std::vector<floatInField*> floatFields;
        std::vector<vec3InField*> vec3Fields;
        paneHandle* handle;

        Shader* shade;//shader for opaque pane rendering
        Shader* textShade;//shader for text rendering

        int width, height;

        unsigned int outVAO, outVBO;
        std::vector<float> outV_Data;
        std::vector<unsigned int> outIndices;

        std::vector<float> layerAABB;
        bool colX = 0, colY = 0;
        unsigned int layerPrecedence;//lower number = higher precedence, basically just index in the allLayers vector

        resizeHandle* rHandle;

        UIlayer(Shader* shade, Shader* textShade, unsigned int width, unsigned int height, glm::vec2 position, std::string name);

        glm::vec2 nextElemPos();//retrieve 2d coordinates for ui element positions based off of number of other elements already present in the vectors

        void addButton(std::string name);
        void addCheckbox(std::string name, bool* value);
        void addFloatInField(float* value);
        void addVec3InField(glm::vec3* value);

        void drawLayer(glm::mat4 transform, bool overlapping);

        void deleteLayer();
    };

    inline std::vector<UIlayer*> allLayers;

    void UIlayerDraw(UIlayer*& target, glm::mat4 transform);

    void renderUI(glm::mat4 transform);

    void cleanUpRes();
}