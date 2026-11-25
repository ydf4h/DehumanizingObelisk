#include <vector>
#include <iostream>
#include <map>
#include <sstream>

#include <glad4.6/glad.h>

#include <shader.hpp>
#include <DeOui.hpp>
#include <eventHandler.hpp>
#include <time.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <stb_image/stb_image.h>

unsigned int deoui::textureFromFile(const char* path){
    std::string filename = std::string(path);

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height,
                                    &nrComponents, 0);

    if (data){
        GLenum format;
        if (nrComponents == 1){
            format = GL_RED;
        }else if (nrComponents == 3){
            format = GL_RGB;
        }else if (nrComponents == 4){
            format = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }else{
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}



void deoui::setupGLrectangle(unsigned int* VAO, unsigned int* VBO, unsigned int* EBO, const std::vector<float> vertices, const std::vector<unsigned int> indices) {
    glGenVertexArrays(1, VAO);
    glGenBuffers(1, VBO);
    glGenBuffers(1, EBO);
    glBindVertexArray(*VAO);
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glBindVertexArray(0);
}
void deoui::setupGLrectangle(unsigned int* VAO, unsigned int* VBO, const std::vector<float> vertices) {
    glGenVertexArrays(1, VAO);
    glGenBuffers(1, VBO);
    glBindVertexArray(*VAO);
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glBindVertexArray(0);
}



void deoui::updGLrectangle(unsigned int VAO, unsigned int VBO, const std::vector<float> vertices) {
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), &vertices[0]);
    glBindVertexArray(0);
}



void deoui::setupGLtexRectangle(unsigned int* VAO, unsigned int* VBO, unsigned int* EBO, const std::vector<float> vertices, const std::vector<unsigned int> indices) {
    glGenVertexArrays(1, VAO);
    glGenBuffers(1, VBO);
    glGenBuffers(1, EBO);
    glBindVertexArray(*VAO);
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindVertexArray(0);
}
void deoui::setupGLtexRectangle(unsigned int* VAO, unsigned int* VBO, unsigned int* EBO, const std::vector<unsigned int> indices) {
    glGenVertexArrays(1, VAO);
    glGenBuffers(1, VBO);
    glGenBuffers(1, EBO);
    glBindVertexArray(*VAO);
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
    glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), NULL, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindVertexArray(0);
}
void deoui::setupGLtexRectangle(unsigned int* VAO, unsigned int* VBO) {
    glGenVertexArrays(1, VAO);
    glGenBuffers(1, VBO);
    glBindVertexArray(*VAO);
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), NULL, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindVertexArray(0);
}



void deoui::updRectAABB(std::vector<float> *AABB, int width, int height, glm::vec2 position) {
    *AABB = {
        position.x, position.y,
        position.x + static_cast<float>(width), position.y + static_cast<float>(height)
    };
}
void deoui::updCurInAABB(const std::vector<float> AABB, const double curPos[2], bool* colX, bool* colY) {
    if (curPos[0] < AABB[2] && curPos[0] > AABB[0]) {
        *colX = 1;
    }else {
        *colX = 0;
    }
    if (curPos[1] < AABB[3] && curPos[1] > AABB[1]) {
        *colY = 1;
    }else {
        *colY = 0;
    }
}


std::string deoui::trimFSTR(std::string target) {
    unsigned int floatPos = target.find('.');
    unsigned int lastNumPos = floatPos + 2;
    for (unsigned int i = floatPos+2; i < target.size(); i++) {
        if (target[i] != '0') {
            lastNumPos = i + 1;
        }
    }
    target.erase(lastNumPos);

    return target;
}



void deoui::renderText(Shader* shade, unsigned int VAO, unsigned int VBO, std::string text, float x, float y, float scale, glm::vec3 color) {
    shade->use();
    shade->setVec3("textColor", color);
    shade->setMat4("model", glm::mat4(1.0f));
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
        Character currentChar = Characters[*c];

        float xPos = x + currentChar.bearing.x * scale;
        float yPos = y - currentChar.bearing.y * scale;

        float charWidth = currentChar.size.x * scale;
        float charHeight = currentChar.size.y * scale;

        std::vector<float> vertexData = {
            xPos, yPos,                            0.0f, 0.0f,
            xPos, yPos + charHeight,               0.0f, 1.0f,
            xPos + charWidth, yPos + charHeight,   1.0f, 1.0f,

            xPos + charWidth, yPos,                1.0f, 0.0f
        };

        glBindTexture(GL_TEXTURE_2D, currentChar.TextureID);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertexData.size() * sizeof(float), &vertexData[0]);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

        x += (currentChar.advance >> 6) * scale;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
void deoui::loadFont(const char* pathToFont, unsigned int count) {
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {//initialize freetype library
        std::cout << "Failed to initialize FreeType.\n";
    }
    FT_Face face;
    if (FT_New_Face(ft, pathToFont, 0, &face)) {//load font "face" or where glyph data is stored
        std::cout << "Failed to load font face.\n";
    }
    FT_Set_Pixel_Sizes(face, 0, 15);//sets font width and height parameters, width to 0 allows dynamic width calculation
    for (unsigned char c = 0; c < count; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cout << "Failed to load glyph no. " << c << '\n';
            continue;
        }
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        deoui::Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)
        };
        deoui::Characters.insert(std::pair<char, Character>(c, character));
    }
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}



void deoui::loadBMPfont(const char* pathToFont, const int charDimensions[], int charCount, const int BMPdimensions[]) {
    mainFont.TextureID = textureFromFile(pathToFont);
    std::cout << mainFont.TextureID << '\n';

    mainFont.xAdvance = (float)charDimensions[0] / (float)BMPdimensions[0];
    mainFont.yAdvance = (float)charDimensions[1] / (float)BMPdimensions[1];

    unsigned int glyphEBO;
    std::vector<float> vertexData = {
        0.0f, 0.0f,                                                                     0.0f, 0.0f,
        0.0f, static_cast<float>(charDimensions[1]),                                    0.0f, mainFont.yAdvance,
        static_cast<float>(charDimensions[0]), static_cast<float>(charDimensions[1]),   mainFont.xAdvance, mainFont.yAdvance,
        static_cast<float>(charDimensions[0]), 0.0f,                                    mainFont.xAdvance, 0.0f
    };
    std::vector<unsigned int> indices = {
        0, 1, 2,
        0, 2, 3
    };
    setupGLtexRectangle(&mainFont.glyphVAO, &mainFont.glyphVBO, &glyphEBO, vertexData, indices);
}
void deoui::renderBMPfont(Shader* shade, std::string text, glm::vec2 position, glm::mat4 transform, glm::vec3 color) {
    shade->use();
    shade->setVec3("textColor", color);
    transform = glm::mat4(1.0f);

    glBindVertexArray(mainFont.glyphVAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mainFont.TextureID);

    glm::vec3 currentPos = glm::vec3(position, 0.0f);
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
        transform = glm::mat4(1.0f);
        transform = glm::translate(transform, currentPos);
        shade->setMat4("model", transform);

        unsigned int curCharIndex = static_cast<unsigned int>(*c) - 32;
        unsigned int row = curCharIndex / 12;
        unsigned int column = curCharIndex % 12;
        std::vector<float> vertexData = {
            0.0f, 0.0f,            mainFont.xAdvance * (float)column, mainFont.yAdvance * (float)(8 - row),
            0.0f, 15.0f,           mainFont.xAdvance * (float)column, mainFont.yAdvance * (float)(7 - row),
            15.0f, 15.0f,          mainFont.xAdvance * (float)(column + 1), mainFont.yAdvance * (float)(7 - row),
            15.0f, 0.0f,           mainFont.xAdvance * (float)(column + 1), mainFont.yAdvance * (float)(8 - row)
        };

        glBindBuffer(GL_ARRAY_BUFFER, mainFont.glyphVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertexData.size() * sizeof(float), &vertexData[0]);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

        currentPos.x += 15.0f;
    }
}



void deoui::BGrect::Draw(Shader* usedShader, glm::mat4 transform){
    usedShader->use();

    transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(position, 0.0f));
    usedShader->setMat4("model", transform);

    glBindVertexArray(VAO);
    usedShader->setVec3("color", RGBcolor);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);

    glBindVertexArray(0);
}
deoui::BGrect::BGrect(glm::vec2 position, int width, int height) {
    this->vertexData = {
        0.0f, 0.0f,
        0.0f, (float)height,
        (float)width, (float)height,
        (float)width, 0.0f
    };
    this->indices = {
        0, 1, 2,
        0, 2, 3
    };
    unsigned int EBO;
    setupGLrectangle(&this->VAO, &this->VBO, &EBO, this->vertexData, this->indices);

    this->width = width;
    this->height = height;

    this->position = position;
}



void deoui::checkbox::Draw(Shader* usedShader, Shader* textShade, glm::mat4 transform){
    if (text.size() != 0) {
        renderText(textShade, textVAO, textVBO, text, position.x + 15.0f + 5.0f, position.y + 15.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    }

    transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(position, 0.0f));
    usedShader->setMat4("model", transform);

    usedShader->setVec3("color", RGBcolor);
    glBindVertexArray(VAO);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);

    glBindVertexArray(0);
}
deoui::checkbox::checkbox(bool* value, glm::vec2 position, std::string name) {
    this->vertexData = {
        //bottom left
        0.0f, 15.0f,
        //top right
        15.0f, 0.0f,
        //top left
        0.0f, 0.0f,
        //bottom right
        15.0f, 15.0f
    };

    this->indices = {
        0, 1, 2,
        1, 0, 3
    };

    this->position = position;

    this->value = value;

    this->AABB = {
        position.x, position.y,
        position.x + 15.0f, position.y + 15.0f
    };

    this->RGBcolor = glm::vec3(0.0f, 0.0f, 0.25f);

    this->text = name;

    std::vector<unsigned int> textIndices = {
        0, 1, 2,
        0, 3, 2
    };
    unsigned int textEBO;
    setupGLtexRectangle(&this->textVAO, &this->textVBO, &textEBO, textIndices);

    unsigned int VBO, EBO;
    setupGLrectangle(&this->VAO, &VBO, &EBO, this->vertexData, this->indices);
}
void deoui::checkbox::pollStatus(){
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
        this->RGBcolor = glm::vec3(0.0f, 0.0f, 0.25f);
    }

    if (MLC != nullptr && colX && colY) {
        bool colX2 = 0, colY2 = 0;//to check if first left click with cursor was in the AABB
        updCurInAABB(this->AABB, mainCursor->firstLClickPos, &colX2, &colY2);
        if (MLC->activity == GLFW_PRESS) {
            if (colX2 && colY2) {
                this->RGBcolor = glm::vec3(1.0f);
            }
        }
        if (MLC->activity == GLFW_RELEASE && LMLC.activity == GLFW_PRESS) {
            if (colX2 && colY2) {
                int swap = 1 - *value;
                *value = swap;
            }
        }
    }

    if (*value) {
        this->RGBcolor = glm::vec3(1.0f);
    }
}



void deoui::button::Draw(Shader* usedShader, Shader* textShade, glm::mat4 transform) {
    if (text.size() != 0) {
        renderText(textShade, textVAO, textVBO, text, position.x + 15.0f + 5.0f, position.y + 15.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    }

    transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(position, 0.0f));
    usedShader->setMat4("model", transform);

    usedShader->setVec3("color", RGBcolor);
    glBindVertexArray(VAO);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);

    glBindVertexArray(0);
}
deoui::button::button(glm::vec2 position, std::string name){
    this->vertexData = {
        //bottom left
        0.0f, 15.0f,
        //top right
        15.0f, 0.0f,
        //top left
        0.0f, 0.0f,
        //bottom right
        15.0f, 15.0f
    };

    this->indices = {
        0, 1, 2,
        1, 0, 3
    };

    this->position = position;

    this->AABB = {
        position.x, position.y,
        position.x + 15.0f, position.y + 15.0f
    };

    this->RGBcolor = glm::vec3(0.0f, 0.0f, 0.25f);

    this->text = name;
    std::vector<unsigned int> textIndices = {
        0, 1, 2,
        0, 3, 2
    };
    unsigned int textEBO;
    setupGLtexRectangle(&this->textVAO, &this->textVBO, &textEBO, textIndices);

    unsigned int VBO, EBO;
    setupGLrectangle(&this->VAO, &VBO, &EBO, this->vertexData, this->indices);
}
void deoui::button::pollStatus() {
    using namespace deoEvent;

    active = 0;

    if (MM != nullptr) {
        updCurInAABB(this->AABB, MM->currentPos, &this->colX, &this->colY);
    }

    if (colX && colY) {
        this->RGBcolor = glm::vec3(0.5f);
    }else {
        this->RGBcolor = glm::vec3(0.0f, 0.0f, 0.25f);
    }

    if (MLC != nullptr && colX && colY) {
        bool colX2 = 0, colY2 = 0;//to check for if the first left click with the cursor was in the AABB
        updCurInAABB(this->AABB, mainCursor->firstLClickPos, &colX2, &colY2);
        if (MLC->activity == GLFW_PRESS) {
            if (colX2 && colY2) {
                this->RGBcolor = glm::vec3(1.0f);
            }
        }
        if (MLC->activity == GLFW_RELEASE && LMLC.activity == GLFW_PRESS) {
            if (colX2 && colY2) {
                active = 1;
            }
        }
    }

    if (active) {
        this->RGBcolor = glm::vec3(1.0f);
    }
}



void deoui::paneHandle::Draw(Shader* usedShader, Shader* text, glm::mat4 transform) {
    transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(this->panePosition, 0.0f));

    usedShader->use();
    usedShader->setVec3("color", glm::vec3(0.125f));
    usedShader->setMat4("model", transform);

    glBindVertexArray(this->paneVAO);
    glDrawElements(GL_TRIANGLES, this->paneIndices.size(), GL_UNSIGNED_INT, (void*)0);

    this->close->Draw(usedShader, text, transform);
    this->collapse->Draw(usedShader, text, transform);

    float textScale = 15.0f / 20.0f;
    renderText(text, this->textVAO, this->textVBO, this->name, this->panePosition.x + 5.0f, this->panePosition.y + 15.0f - 2.0f, textScale, glm::vec3(1.0f));
}
deoui::paneHandle::paneHandle(UIlayer* parent, glm::vec2 position, int width, std::string name) {
    this->close = new button(glm::vec2(position.x + (float)width - 15.0f, position.y), "");
    this->collapse = new checkbox(&this->collapsed, glm::vec2(position.x, position.y), "");
    this->parent = parent;
    this->paneWidth = width - 30;
    this->panePosition = glm::vec3(position.x + 15.0f, position.y, 0.0f);

    this->paneAABB = {
        this->panePosition.x, this->panePosition.y,
        this->panePosition.x + static_cast<float>(this->paneWidth), this->panePosition.y + 15.0f
    };

    this->name = name;
    std::vector<unsigned int> textIndices = {
        0, 1, 2,
        0, 3, 2
    };
    unsigned int textEBO;
    setupGLtexRectangle(&this->textVAO, &this->textVBO, &textEBO, textIndices);

    this->paneVertexData = {
        0.0f, 0.0f,
        0.0f, 15.0f,
        static_cast<float>(this->paneWidth), 15.0f,
        static_cast<float>(this->paneWidth), 0.0f
    };
    this->paneIndices = {
        0, 1, 2,
        0, 2, 3
    };
    unsigned int EBO;
    setupGLrectangle(&this->paneVAO, &this->paneVBO, &EBO, this->paneVertexData, this->paneIndices);
}
bool deoui::paneHandle::isMouseHover() {
    updCurInAABB(this->paneAABB, deoEvent::mainCursor->currentPos, &this->colX, &this->colY);
    return this->colX && this->colY;
}
bool deoui::paneHandle::isActive() {
    if (this->colX && this->colY) {
        if (deoEvent::MLC != nullptr) {
            if (deoEvent::LMLC.activity == GLFW_RELEASE && deoEvent::MLC->activity == GLFW_PRESS) {
                this->moveActive = 1;
            }
        }
    }
    if (deoEvent::MLC != nullptr) {
        if (deoEvent::MLC->activity == GLFW_RELEASE) {
            this->moveActive = 0;
        }
    }
    return this->moveActive;
}
void deoui::paneHandle::pollStatus() {
    bool mouseHover = this->isMouseHover();
    bool isActive = this->isActive();
    int diffX = 0, diffY = 0;
    if (isActive) {
        if (deoEvent::MM != nullptr) {
            diffX = deoEvent::MM->currentPos[0] - deoEvent::MM->lastPos[0];
            diffY = deoEvent::MM->currentPos[1] - deoEvent::MM->lastPos[1];
        }
        this->posDiff = glm::vec2(static_cast<float>(diffX), static_cast<float>(diffY));

        this->panePosition.x += this->posDiff.x;
        this->panePosition.y += this->posDiff.y;

        this->close->position.x += this->posDiff.x;
        this->close->position.y += this->posDiff.y;

        this->collapse->position.x += this->posDiff.x;
        this->collapse->position.y += this->posDiff.y;

        if (diffX != 0 || diffY != 0) {
            updRectAABB(&this->paneAABB, this->paneWidth, 15.0f, this->panePosition);
            updRectAABB(&this->close->AABB, 15.0f, 15.0f, this->close->position);
            updRectAABB(&this->collapse->AABB, 15.0f, 15.0f, this->collapse->position);
        }
        
        this->parent->pane->position.x += this->parent->handle->posDiff.x;
        this->parent->pane->position.y += this->parent->handle->posDiff.y;

        this->parent->rHandle->position.x += this->parent->handle->posDiff.x;
        this->parent->rHandle->position.y += this->parent->handle->posDiff.y;
        updRectAABB(&this->parent->rHandle->AABB, 20, 20, this->parent->rHandle->position);

        if (!this->parent->handle->collapsed) {
            updRectAABB(&this->parent->layerAABB, this->parent->pane->width, this->parent->pane->height + 15, this->parent->handle->collapse->position);
        }else {
            updRectAABB(&this->parent->layerAABB, this->parent->pane->width, 15, this->parent->handle->collapse->position);
        }

        for (unsigned int i = 0; i < this->parent->buttons.size(); i++) {
            this->parent->buttons[i]->position.x += this->parent->handle->posDiff.x;
            this->parent->buttons[i]->position.y += this->parent->handle->posDiff.y;
            updRectAABB(&this->parent->buttons[i]->AABB, 15.0f, 15.0f, this->parent->buttons[i]->position);
        }

        for (unsigned int i = 0; i < this->parent->checkboxes.size(); i++) {
            this->parent->checkboxes[i]->position.x += this->parent->handle->posDiff.x;
            this->parent->checkboxes[i]->position.y += this->parent->handle->posDiff.y;
            updRectAABB(&this->parent->checkboxes[i]->AABB, 15.0f, 15.0f, this->parent->checkboxes[i]->position);
        }

        for (unsigned int i = 0; i < this->parent->floatFields.size(); i++) {
            this->parent->floatFields[i]->position.x += this->parent->handle->posDiff.x;
            this->parent->floatFields[i]->position.y += this->parent->handle->posDiff.y;
            updRectAABB(&this->parent->floatFields[i]->AABB, 40, 20, this->parent->floatFields[i]->position);
        }

        for (unsigned int i = 0; i < this->parent->vec3Fields.size(); i++) {
            this->parent->vec3Fields[i]->updPositions(this->parent->handle->posDiff.x, this->parent->handle->posDiff.y);
        }
    }
    if (deoEvent::MLC != nullptr) {
        if (deoEvent::MLC->activity == GLFW_RELEASE) {
            this->moveActive = 0;
        }
    }

    this->close->pollStatus();
    this->collapse->pollStatus();
}



deoui::resizeHandle::resizeHandle(UIlayer* parent, glm::vec2 position, std::string pathToTex) {
    this->parent = parent;

    this->vertexData = {
        0.0f, 0.0f,   0.0f, 1.0f,
        0.0f, 20.0f,   0.0f, 0.0f,
        20.0f, 20.0f,   1.0f, 0.0f,
        20.0f, 0.0f,   1.0f, 1.0f
    };
    this->indices = {
        0, 1, 2,
        0, 2, 3
    };
    this->RGBcolor = glm::vec3(0.0f, 0.0f, 0.25f);
    this->position = glm::vec3(position, 0.0f);

    unsigned int VBO, EBO;
    setupGLtexRectangle(&this->VAO, &VBO, &EBO, this->vertexData, this->indices);

    this->resizeTex = textureFromFile(pathToTex.c_str());

    this->AABB = {
        position.x, position.y,
        position.x + 20.0f, position.y + 20.0f
    };
}
void deoui::resizeHandle::pollStatus() {
    if (deoEvent::MM != nullptr) {
        if (deoEvent::MM->currentPos[0] < this->AABB[2] && deoEvent::MM->currentPos[0] > this->AABB[0]) {
            this->colX = 1;
        }else {
            this->colX = 0;
        }
        if (deoEvent::MM->currentPos[1] < this->AABB[3] && deoEvent::MM->currentPos[1] > this->AABB[1]) {
            this->colY = 1;
        }else {
            this->colY = 0;
        }
    }
    if (this->colX && this->colY) {
        this->RGBcolor = glm::vec3(0.5f);
        if (deoEvent::MLC != nullptr) {
            if (deoEvent::LMLC.activity == GLFW_RELEASE && deoEvent::MLC->activity == GLFW_PRESS) {
                this->resizeActive = 1;
            }
        }
    }else {
        this->RGBcolor = glm::vec3(0.0f, 0.0f, 0.25f);
    }
    if (this->resizeActive) {
        this->RGBcolor = glm::vec3(1.0f);
        if (deoEvent::MLC != nullptr) {
            if (deoEvent::MLC->activity == GLFW_RELEASE) {
                this->resizeActive = 0;
                if (this->colX && this->colY) {
                    this->RGBcolor = glm::vec3(0.5f);
                }else {
                    this->RGBcolor = glm::vec3(0.0f, 0.0f, 0.25f);
                }
            }
        }
        if (deoEvent::MM != nullptr) {
            this->sizeDiff.x = deoEvent::MM->currentPos[0] - (this->position.x + 20);
            this->sizeDiff.y = deoEvent::MM->currentPos[1] - (this->position.y + 20);
        }else {
            this->sizeDiff.x = 0.0f;
            this->sizeDiff.y = 0.0f;
        }
        
        int diffX = this->sizeDiff.x; int diffY = this->sizeDiff.y;
        if (diffX + this->parent->width >= 45) {
            this->parent->width += diffX;
            this->parent->pane->width = this->parent->width;

            this->parent->handle->paneWidth += diffX;
            this->parent->handle->paneVertexData = {
                0.0f, 0.0f,
                0.0f, 15.0f,
                static_cast<float>(this->parent->handle->paneWidth), 15.0f,
                static_cast<float>(this->parent->handle->paneWidth), 0.0f
            };
            updGLrectangle(this->parent->handle->paneVAO, this->parent->handle->paneVBO, this->parent->handle->paneVertexData);
            this->parent->handle->close->position.x += diffX;

            this->parent->rHandle->position.x += diffX;
        }
        if (diffY + this->parent->height >= 30) {
            this->parent->height += diffY;
            this->parent->pane->height = this->parent->height;

            this->parent->rHandle->position.y += diffY;
        }
        this->parent->pane->vertexData = {
            0.0f, 0.0f,
            0.0f, static_cast<float>(this->parent->pane->height),
            static_cast<float>(this->parent->pane->width), static_cast<float>(this->parent->pane->height),
            static_cast<float>(this->parent->pane->width), 0.0f
        };
        updGLrectangle(this->parent->pane->VAO, this->parent->pane->VBO, this->parent->pane->vertexData);
        this->parent->outV_Data = {
            -1.0f, -1.0f,
            -1.0f, this->parent->pane->height + 16.0f,
            this->parent->pane->width + 1.0f, this->parent->pane->height + 16.0f,
            this->parent->pane->width + 1.0f, -1.0f
        };
        updGLrectangle(this->parent->outVAO, this->parent->outVBO, this->parent->outV_Data);

        updRectAABB(&this->parent->layerAABB, this->parent->pane->width, this->parent->pane->height + 15, this->parent->handle->collapse->position);

        updRectAABB(&this->parent->handle->paneAABB, this->parent->handle->paneWidth, 15.0f, this->parent->handle->panePosition);
        updRectAABB(&this->parent->handle->close->AABB, 15.0f, 15.0f, this->parent->handle->close->position);
        updRectAABB(&this->parent->rHandle->AABB, 20.0f, 20.0f, this->parent->rHandle->position);
    }
}
void deoui::resizeHandle::Draw(Shader* usedShader, glm::mat4 transform) {
    transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(this->position, 0.0f));
    usedShader->use();
    usedShader->setVec3("textColor", this->RGBcolor);
    usedShader->setMat4("model", transform);
    glBindTexture(GL_TEXTURE_2D, this->resizeTex);
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, (void*)0);
}



deoui::floatInField::floatInField(glm::vec2 position, float* value) {
    this->vertexData = {
        0.0f, 0.0f,
        0.0f, 20.0f,
        60.0f, 20.0f,
        60.0f, 0.0f
    };
    std::vector<unsigned int> indices = {
        0, 1, 2,
        0, 2, 3
    };
    unsigned int EBO;
    setupGLrectangle(&this->VAO, &this->VBO, &EBO, this->vertexData, indices);
    this->position = position;

    unsigned int textEBO;
    setupGLtexRectangle(&this->textVAO, &this->textVBO, &textEBO, indices);

    this->RGBcolor = glm::vec3(0.5f);

    this->AABB = {
        position.x, position.y,
        position.x + 60.0f, position.y + 20.0f
    };
    this->value = value;

    this->displayValue = std::to_string(*value);
    this->displayValue = trimFSTR(this->displayValue);
}
void deoui::floatInField::Draw(Shader* usedShader, Shader* textShader, glm::mat4 transform) {
    usedShader->use();
    transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(this->position, 0.0f));
    usedShader->setMat4("model", transform);
    usedShader->setVec3("color", this->RGBcolor);

    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

    renderText(textShader, this->textVAO, this->textVBO, this->displayValue, this->position.x + 2.0f, this->position.y + 15.0f, 1.0f, glm::vec3(1.0f));
}
void deoui::floatInField::pollStatus() {
    if (deoEvent::MM != nullptr) {
        updCurInAABB(this->AABB, deoEvent::mainCursor->currentPos, &this->colX, &this->colY);
    }
    if (this->colX && this->colY && deoEvent::MLC != nullptr) {
        if (deoEvent::MLC->activity == GLFW_PRESS) {
            bool colX2 = 0, colY2 = 0;
            updCurInAABB(this->AABB, deoEvent::mainCursor->firstLClickPos, &colX2, &colY2);
            if (colX2 && colY2) {
                this->active = 1;
            }
        }
    }
    if (!this->colX || !this->colY && deoEvent::MLC != nullptr) {
        if (deoEvent::MLC->activity == GLFW_PRESS) {
            bool colX2 = 0, colY2 = 0;
            updCurInAABB(this->AABB, deoEvent::mainCursor->firstLClickPos, &colX2, &colY2);
            if (!colX2 || !colY2) {
                this->active = 0;
            }
        }
    }
    if (this->active) {
        this->RGBcolor = glm::vec3(0.25f);//active background field color
        if (deoEvent::BS != nullptr) {
            if (deoEvent::BS->activity == GLFW_PRESS && deoEvent::LBS.activity == GLFW_RELEASE && this->displayValue.size() != 0) {//check if backspace was just pressed and that displayValue isn't empty
                this->displayValue.erase(this->displayValue.size() - 1, this->displayValue.size());//remove last value
            }
        }
        if (deoEvent::CIN != nullptr) {
            std::stringstream charStream;
            charStream << this->displayValue << deoEvent::CIN->c;//shove the character into the stringstream
            this->displayValue = charStream.str();//update the displayValue to match the new data
        }
        if (deoEvent::ENT != nullptr) {
            if (deoEvent::ENT->activity == GLFW_PRESS) {
                this->active = 0;
                for (unsigned int i = 0; i < this->displayValue.size(); i++) {
                    if (this->displayValue[i] < 48 || this->displayValue[i] > 57) {//check if current character is not one of the numbers 0-9, so outside range of 48-57 decimal ascii code
                        if (this->displayValue[i] != 46 && this->displayValue[i] != 45) {//check if current character is not a hyphen (negative sign) and it is not a period (decimal point)
                            this->displayValue = "0.0";//reset displayValue to 0.0
                            *this->value = 0.0f;//reset real value to 0.0f
                            break;//stop iterating through the string
                        }
                    }else if (i + 1 == this->displayValue.size()) {//see if none of the characters have failed the non-number test and if the end has been reached
                        *this->value = std::atof(this->displayValue.c_str());//update real value
                    }
                }
            }
        }
    }else {
        this->RGBcolor = glm::vec3(0.5f);//inactive background field color
        if (deoTime::waitFor(1.0f)) {
            if (std::atof(this->displayValue.c_str()) != *this->value) {
                this->displayValue = std::to_string(*this->value);
                this->displayValue = trimFSTR(this->displayValue);
            }
        }
    }
}



deoui::vec3InField::vec3InField(glm::vec2 position, glm::vec3* value) {
    this->position = position;
    this->vectorValue = value;

    for (unsigned char i = 0; i < 3; i++) {
        glm::vec2 currentPos = glm::vec2(this->position.x + (65.0f * i), this->position.y);
        if (i == 0) {
            this->fields[i] = new floatInField(currentPos, &this->vectorValue->x);
        }else if (i == 1) {
            this->fields[i] = new floatInField(currentPos, &this->vectorValue->y);
        }else {
            this->fields[i] = new floatInField(currentPos, &this->vectorValue->z);
        }
    }
}
void deoui::vec3InField::Draw(Shader* usedShader, Shader* textShader, glm::mat4 transform) {
    for (unsigned char i = 0; i < 3; i++) {
        this->fields[i]->Draw(usedShader, textShader, transform);
    }
}
void deoui::vec3InField::pollStatus() {
    for (unsigned char i = 0; i < 3; i++) {
        this->fields[i]->pollStatus();
    }
}
void::deoui::vec3InField::updPositions(int diffX, int diffY) {
    this->position.x += diffX;
    this->position.y += diffY;
    for (unsigned char i = 0; i < 3; i++) {
        this->fields[i]->position.x += diffX;
        this->fields[i]->position.y += diffY;
        updRectAABB(&this->fields[i]->AABB, 40, 20, this->fields[i]->position);
    }
}



void deoui::UIlayer::drawLayer(glm::mat4 transform) {
    glDisable(GL_DEPTH_TEST);//everything should render sequentially and stack on top of each other
    glDisable(GL_STENCIL_TEST);

    if (deoEvent::WRS != nullptr) {//changes projection matrix according to window size
        glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(deoEvent::WRS->width), static_cast<float>(deoEvent::WRS->height), 0.0f);
        textShade->use();
        textShade->setMat4("projection", projection);
        shade->use();
        shade->setMat4("projection", projection);
    }

    glBindVertexArray(this->outVAO);
    shade->use();
    shade->setVec3("color", glm::vec3(1.0f));
    transform = glm::translate(transform, glm::vec3(this->handle->panePosition.x - 15.0f, this->handle->panePosition.y, 0.0f));
    shade->setMat4("model", transform);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

    if (!this->handle->collapsed) {
        pane->Draw(shade, transform);

        rHandle->Draw(textShade, transform);

        for (unsigned int i = 0; i < buttons.size(); i++) {
            buttons[i]->Draw(shade, textShade, transform);
        }
        for (unsigned int i = 0; i < checkboxes.size(); i++) {
            checkboxes[i]->Draw(shade, textShade, transform);
        }
        for (unsigned int i = 0; i < floatFields.size(); i++) {
            floatFields[i]->Draw(shade, textShade, transform);
        }
        for (unsigned int i = 0; i < vec3Fields.size(); i++) {
            vec3Fields[i]->Draw(shade, textShade, transform);
        }
    }

    this->handle->Draw(this->shade, this->textShade, transform);

    glEnable(GL_DEPTH_TEST);
}
void deoui::UIlayer::pollStatus() {
    bool lastCollapseStat = this->handle->collapsed;//used to check if pane was just collapsed/uncollapsed
    if (!this->overlapping) {//cursor is not hovering over another layer with higher precedence
        this->handle->pollStatus();
        if (!this->handle->collapsed) {//layer is not collapsed
            this->rHandle->pollStatus();
            if (lastCollapseStat != this->handle->collapsed) {//checks if pane was just uncollapsed
                this->outV_Data = {
                    -1.0f, -1.0f,
                    -1.0f, this->height + 16.0f,
                    this->width + 1.0f, this->height + 16.0f,
                    this->width + 1.0f, -1.0f
                };
                updGLrectangle(this->outVAO, this->outVBO, this->outV_Data);
                updRectAABB(&this->layerAABB, this->pane->width, this->pane->height + 15, this->handle->collapse->position);
            }
            for (unsigned int i = 0; i < this->buttons.size(); i++) {
                this->buttons[i]->pollStatus();
            }
            for (unsigned int i = 0; i < this->checkboxes.size(); i++) {
                this->checkboxes[i]->pollStatus();
            }
            for (unsigned int i = 0; i < this->floatFields.size(); i++) {
                this->floatFields[i]->pollStatus();
            }
            for (unsigned int i = 0; i < this->vec3Fields.size(); i++) {
                this->vec3Fields[i]->pollStatus();
            }
        }else if (lastCollapseStat != this->handle->collapsed) {//layer is collapsed and was just collapsed
            this->outV_Data = {
                -1.0f, -1.0f,
                -1.0f, 16.0f,
                this->handle->paneWidth + 31.0f, 16.0f,
                this->handle->paneWidth + 31.0f, -1.0f
            };//update outline vertex data
            updGLrectangle(this->outVAO, this->outVBO, this->outV_Data);
            updRectAABB(&this->layerAABB, this->pane->width, 15, this->handle->collapse->position);
        }//checks if pane was just collapsed
    }
}
void deoui::UIlayer::pollPrecedence() {
    this->overlapping = 0;//reset overlapping value
    for (unsigned int i = 0; i < this->layerPrecedence; i++) {//search through all layers with higher precedence (if any)
        if (allLayers[i]->colX && allLayers[i]->colY) {//will be true if mouse is hovering over layer with higher precedence
            this->overlapping = 1;//update overlapping value
            break;//stop search
        }
    }
    updCurInAABB(this->layerAABB, deoEvent::mainCursor->currentPos, &this->colX, &this->colY);//check if the cursor is currently inside the layerAABB of the layer
    if (this->colX && this->colY) {//if cursor is inside this layer's AABB
        if (deoEvent::MLC != nullptr) {
            if (deoEvent::MLC->activity == GLFW_PRESS && deoEvent::LMLC.activity == GLFW_RELEASE) {//mouse left click has just started being held down
                if (this->layerPrecedence != 0) {//not the highest precedence layer currently
                    if (!this->overlapping) {//cursor not hovering over layer with higher precedence
                        UIlayer* overwriter = this;//current layer, will be shoved to front
                        UIlayer* overwritten = allLayers[0];//current highest precedence layer, will be shoved back one

                        std::vector<UIlayer*> uninvolved;//all layers that will not be affected currently
                        for (unsigned int i = 0; i < allLayers.size(); i++) {
                            if (i != overwriter->layerPrecedence && i != overwritten->layerPrecedence) {
                                uninvolved.push_back(allLayers[i]);//add uninvolved layer to the uninvolved vector
                            }
                        }

                        allLayers.clear();//clear all layers from the allLayers vector
                        allLayers.push_back(overwriter);//first, have the overwriter at index 0
                        allLayers.push_back(overwritten);//then, have the overwritten layer at index 1

                        overwriter->layerPrecedence = 0;//change layer precedence of overwriter
                        overwritten->layerPrecedence = 1;//change layer precedence of overwritten
                        for (unsigned int i = 0; i < uninvolved.size(); i++) {//add all uninvolved layers back to the allLayers vector
                            uninvolved[i]->layerPrecedence = i + 2;//update layer precedence of each uninvolved layer
                            allLayers.push_back(uninvolved[i]);//add uninvolved layer to the final allLayers vector
                        }
                    }
                }
            }
        }
    }
}
glm::vec2 deoui::UIlayer::nextElemPos() {
    glm::vec2 finalPos = this->pane->position;
    finalPos.x += 5.0f;
    finalPos.y += 5.0f;
    for (unsigned int i = 0; i < this->buttons.size(); i++) {
        finalPos.y += 20.0f;
    }
    for (unsigned int i = 0; i < this->checkboxes.size(); i++) {
        finalPos.y += 20.0f;
    }
    for (unsigned int i = 0; i < this->floatFields.size(); i++) {
        finalPos.y += 25.0f;
    }
    for (unsigned int i = 0; i < this->vec3Fields.size(); i++) {
        finalPos.y += 25.0f;
    }
    return finalPos;
}
void deoui::UIlayer::addButton(std::string name) {
    glm::vec2 position = this->nextElemPos();
    button* target = new button(position, name);
    this->buttons.push_back(target);
}
void deoui::UIlayer::addCheckbox(std::string name, bool* value) {
    glm::vec2 position = this->nextElemPos();
    checkbox* target = new checkbox(value, position, name);
    this->checkboxes.push_back(target);
}
void deoui::UIlayer::addFloatInField(float* value) {
    glm::vec2 position = this->nextElemPos();
    floatInField* target = new floatInField(position, value);
    this->floatFields.push_back(target);
}
void deoui::UIlayer::addVec3InField(glm::vec3* value) {
    glm::vec2 position = this->nextElemPos();
    vec3InField* target = new vec3InField(position, value);
    this->vec3Fields.push_back(target);
}
deoui::UIlayer::UIlayer(Shader* shade, Shader* textShade, unsigned int width, unsigned int height, glm::vec2 position, std::string name) {
    this->pane = new BGrect(glm::vec2(position.x, position.y + 15.0f), width, height);
    this->pane->RGBcolor = glm::vec3(0.0f);
    this->width = width;
    this->height = height;

    this->handle = new paneHandle(this, position, width, name);

    this->outV_Data = {
        -1.0f, -1.0f,
        -1.0f, height + 16.0f,
        width + 1.0f, height + 16.0f,
        width + 1.0f, -1.0f
    };
    this->outIndices = {
        0, 1, 2,
        0, 2, 3
    };
    unsigned int outEBO;
    setupGLrectangle(&this->outVAO, &this->outVBO, &outEBO, this->outV_Data, this->outIndices);

    this->layerAABB = {
        position.x, position.y,
        position.x + width, position.y + 15.0f + height
    };

    this->shade = shade;
    this->textShade = textShade;

    glm::vec2 rHandlePos = glm::vec2(position.x + static_cast<float>(width) - 20.0f, position.y + static_cast<float>(height) - 5.0f);
    this->rHandle = new resizeHandle(this, rHandlePos, "../res/textures/uiThings/resize.png");

    this->layerPrecedence = allLayers.size();
    allLayers.push_back(this);
}
void deoui::UIlayer::deleteLayer() {
    delete pane;
    delete handle->close;
    delete handle->collapse;
    delete handle;
    for (unsigned int i = 0; i < buttons.size(); i++) {
        delete buttons[i];
    }
    for (unsigned int i = 0; i < checkboxes.size(); i++) {
        delete checkboxes[i];
    }
    for (unsigned int i = 0; i < floatFields.size(); i++) {
        delete floatFields[i];
    }
}



void deoui::UIlayerDraw(UIlayer*& target, glm::mat4 transform) {
    if (target != nullptr) {
        target->pollPrecedence();
        target->pollStatus();

        target->drawLayer(transform);
    }
}



void deoui::renderUI(glm::mat4 transform) {
    std::vector<UIlayer*> intactLayers = allLayers;
    std::vector<UIlayer*>::reverse_iterator v;
    for (v = intactLayers.rbegin(); v != intactLayers.rend(); v++) {
        UIlayerDraw(*v, transform);
    }
    std::vector<UIlayer*>::const_iterator vR;
    for (vR = allLayers.begin(); vR != allLayers.end(); vR++) {
        UIlayer* deref = *vR;
        if (deref->handle->close->active) {
            for (unsigned int i = deref->layerPrecedence + 1; i < allLayers.size(); i++) {
                allLayers[i]->layerPrecedence--;
            }
            deref->deleteLayer();
            delete deref;
            allLayers.erase(vR);
            vR--;
        }
    }
}



void deoui::cleanUpRes() {
    for (unsigned int i = 0; i < allLayers.size(); i++) {
        if (allLayers[i] != nullptr) {
            allLayers[i]->deleteLayer();
            delete allLayers[i];
            allLayers[i] = nullptr;
        }
    }
}