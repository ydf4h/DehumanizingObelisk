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

    class BGrect{
    public:
        std::vector<float> vertexData;//main rect vertices
        std::vector<unsigned int> indices;//main rect indices
        unsigned int VAO, VBO;//main rect vao and vertex buffer

        glm::vec3 RGBcolor;//main rect color

        glm::vec2 position;//main rect position in pixels
        int width, height;//main rect width and height in pixels

        BGrect(glm::vec2 position, int width, int height);

        void Draw(Shader* usedShader, glm::mat4 transform);
    };

    class checkbox{
    public:
        std::vector<float> vertexData;
        std::vector<unsigned int> indices;
        unsigned int VAO;

        glm::vec3 RGBcolor;

        glm::vec2 position;

        std::vector<float> AABB;//aabb for collision detection with cursor
        bool colX = 0, colY = 0;//if the cursor collides with AABB on x and y axis
        bool* value;//boolean value that changes on checkbox activation

        unsigned int textVAO, textVBO;//text box
        std::string text;//text displayed to right of checkbox

        checkbox(bool* value, glm::vec2 position, std::string name);

        void pollStatus();

        void Draw(Shader* usedShader, Shader* text, glm::mat4 transform);
    };

    class button{
    public:
        std::vector<float> vertexData;
        std::vector<unsigned int> indices;
        unsigned int VAO;

        glm::vec3 RGBcolor;

        glm::vec2 position;

        std::vector<float> AABB;
        bool colX = 0, colY = 0;
        bool active = 0;

        unsigned int textVAO, textVBO;
        std::string text;

        button(glm::vec2 position, std::string name);

        void pollStatus();

        void Draw(Shader* usedShader, Shader* text, glm::mat4 transform);
    };

    class UIlayer;

    class paneHandle{
    public:
        std::vector<float> paneVertexData;
        std::vector<unsigned int> paneIndices;
        unsigned int paneVAO, paneVBO;

        glm::vec2 panePosition;
        int paneWidth;

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

        paneHandle(UIlayer* parent, glm::vec2 position, int width, std::string name);

        void pollStatus();

        bool isMouseHover();

        bool isActive();

        void Draw(Shader* usedShader, Shader* text, glm::mat4 transform);
    };

    class resizeHandle{
    public:
        UIlayer* parent;

        std::vector<float> vertexData;
        std::vector<unsigned int> indices;
        unsigned int VAO;

        glm::vec2 position;

        std::vector<float> AABB;
        bool colX = 0, colY = 0;
        bool resizeActive = 0;
        glm::vec2 sizeDiff;

        unsigned int resizeTex;
        glm::vec3 RGBcolor;

        resizeHandle(UIlayer* parent, glm::vec2 position, std::string pathToTex);

        void pollStatus();

        void Draw(Shader* usedShader, glm::mat4 transform);
    };

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

        floatInField(glm::vec2 position, float* value);

        void Draw(Shader* usedShader, Shader* textShader, glm::mat4 transform);

        void pollStatus();
    };

    class vec3InField{
    public:
        floatInField* fields[3];
        glm::vec2 position;
        glm::vec3* vectorValue;

        vec3InField(glm::vec2 position, glm::vec3* value);

        void Draw(Shader* usedShader, Shader* textShader, glm::mat4 transform);

        void pollStatus();

        void updPositions(int diffX, int diffY);
    };

    class UIlayer{
    public:
        BGrect* pane;//solid color background rectangle for the layer, provides no functionality

        std::vector<button*> buttons;//all buttons on the layer
        std::vector<checkbox*> checkboxes;//all checkboxes on the layer
        std::vector<floatInField*> floatFields;//all float in fields on the layer
        std::vector<vec3InField*> vec3Fields;//all 3d vector in fields on the layer

        Shader* shade;//shader for opaque pane rendering
        Shader* textShade;//shader for text rendering

        int width, height;

        unsigned int outVAO, outVBO;//outline vertex array ID
        std::vector<float> outV_Data;//outline vertex data
        std::vector<unsigned int> outIndices;//outline indices

        std::vector<float> layerAABB;//the AABB for all of what is revealed of the layer currently (dependent on if it is collapsed)
        bool colX = 0, colY = 0;//current collision status with the cursor in the layerAABB of the layer
        bool overlapping = 0;//whether the cursor is currently hovering over a layer with higher precedence
        unsigned int layerPrecedence;//lower number = higher precedence, basically just index in the allLayers vector

        resizeHandle* rHandle;//resize handle for the layer
        paneHandle* handle;//handle for the layer, allows collapsing, closing, and moving of the layer with a title included

        UIlayer(Shader* shade, Shader* textShade, unsigned int width, unsigned int height, glm::vec2 position, std::string name);//constructor

        glm::vec2 nextElemPos();//retrieve 2d coordinates for ui element positions based off of number of other elements already present in the vectors

        void addButton(std::string name);
        void addCheckbox(std::string name, bool* value);
        void addFloatInField(float* value);
        void addVec3InField(glm::vec3* value);

        void pollStatus();//poll the status of every element contained in a UIlayer
        void pollPrecedence();//update the layer's precedence value
        void drawLayer(glm::mat4 transform);//draw the UIlayer

        void deleteLayer();//deletes a UIlayer and every UI element that is contained in it
    };
    inline std::vector<UIlayer*> allLayers;//every single UIlayer that is currently active

    void UIlayerDraw(UIlayer*& target, glm::mat4 transform);//draw a single UIlayer with nullptr checking and status polling
    void renderUI(glm::mat4 transform);//render every UIlayer that is currently in the allLayers vector

    void cleanUpRes();//delete all layers in the allLayers vector
}