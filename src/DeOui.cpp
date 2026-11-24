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



void deoui::UIelem::Draw(Shader* usedShader, glm::mat4 transform){
    usedShader->use();

    transform = glm::mat4(1.0f);
    transform = glm::translate(transform, position);
    usedShader->setMat4("model", transform);

    glBindVertexArray(VAO);
    usedShader->setVec3("color", RGBcolor);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);

    glBindVertexArray(0);
}
void deoui::genBGrect(UIelem* target, glm::vec3 position, int width, int height){
    target->vertexData = {
        0.0f, 0.0f,
        0.0f, (float)height,
        (float)width, (float)height,
        (float)width, 0.0f
    };
    target->indices = {
        0, 1, 2,
        0, 2, 3
    };
    unsigned int EBO;
    setupGLrectangle(&target->VAO, &target->VBO, &EBO, target->vertexData, target->indices);

    target->width = width;
    target->height = height;

    target->position = position;
}



void deoui::checkbox::Draw(Shader* usedShader, Shader* textShade, glm::mat4 transform){
    if (text.size() != 0) {
        float textScale = height / 15.0f;
        renderText(textShade, textVAO, textVBO, text, position.x + width + 5.0f, position.y + height, textScale, glm::vec3(1.0f, 1.0f, 1.0f));
    }

    transform = glm::mat4(1.0f);
    transform = glm::translate(transform, position);
    usedShader->setMat4("model", transform);

    usedShader->setVec3("color", RGBcolor);
    glBindVertexArray(VAO);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);

    glBindVertexArray(0);
}
void deoui::genCheckbox(checkbox* target, bool* value, glm::vec3 position, int width, int height, std::string name){
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

    target->width = width;
    target->height = height;

    target->indices = {
        0, 1, 2,
        1, 0, 3
    };

    target->position = position;

    target->value = value;

    target->AABB = {
        position.x, position.y,
        position.x + (float)width, position.y + (float)height
    };

    target->RGBcolor = glm::vec3(0.0f, 0.0f, 0.25f);

    target->text = name;

    std::vector<unsigned int> textIndices = {
        0, 1, 2,
        0, 3, 2
    };
    unsigned int textEBO;
    setupGLtexRectangle(&target->textVAO, &target->textVBO, &textEBO, textIndices);

    unsigned int VBO, EBO;
    setupGLrectangle(&target->VAO, &VBO, &EBO, target->vertexData, target->indices);
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
        float textScale = height / 15.0f;
        renderText(textShade, textVAO, textVBO, text, position.x + width + 5.0f, position.y + height, textScale, glm::vec3(1.0f, 1.0f, 1.0f));
    }

    transform = glm::mat4(1.0f);
    transform = glm::translate(transform, position);
    usedShader->setMat4("model", transform);

    usedShader->setVec3("color", RGBcolor);
    glBindVertexArray(VAO);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);

    glBindVertexArray(0);
}
void deoui::genButton(button* target, glm::vec3 position, int width, int height, std::string name){
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

    target->width = width;
    target->height = height;

    target->indices = {
        0, 1, 2,
        1, 0, 3
    };

    target->position = position;

    target->AABB = {
        position.x, position.y,
        position.x + (float)width, position.y + (float)height
    };

    target->RGBcolor = glm::vec3(0.0f, 0.0f, 0.25f);

    target->text = name;
    std::vector<unsigned int> textIndices = {
        0, 1, 2,
        0, 3, 2
    };
    unsigned int textEBO;
    setupGLtexRectangle(&target->textVAO, &target->textVBO, &textEBO, textIndices);

    unsigned int VBO, EBO;
    setupGLrectangle(&target->VAO, &VBO, &EBO, target->vertexData, target->indices);
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
    transform = glm::translate(transform, this->position);

    usedShader->use();
    usedShader->setVec3("color", this->RGBcolor);
    usedShader->setMat4("model", transform);

    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, (void*)0);

    this->close->Draw(usedShader, text, transform);
    this->collapse->Draw(usedShader, text, transform);

    float textScale = this->height / 20.0f;
    renderText(text, this->textVAO, this->textVBO, this->name, this->position.x + 5.0f, this->position.y + this->height - 2.0f, textScale, glm::vec3(1.0f));
}
void deoui::genPaneHandle(paneHandle* target, UIlayer* parent, glm::vec2 position, int width, int height, std::string name) {
    target->close = new button;
    target->collapse = new checkbox;
    target->parent = parent;
    genButton(target->close, glm::vec3(position.x + (float)width - 15.0f, position.y, 0.0f), 15, 15, "");
    genCheckbox(target->collapse, &target->collapsed, glm::vec3(position.x, position.y, 0.0f), 15, 15, "");
    target->width = width - 30;
    target->height = height;
    target->RGBcolor = glm::vec3(0.125f);
    target->position = glm::vec3(position.x + 15.0f, position.y, 0.0f);

    target->paneAABB = {
        target->position.x, target->position.y,
        target->position.x + static_cast<float>(target->width), target->position.y + static_cast<float>(target->height)
    };

    target->name = name;
    std::vector<unsigned int> textIndices = {
        0, 1, 2,
        0, 3, 2
    };
    unsigned int textEBO;
    setupGLtexRectangle(&target->textVAO, &target->textVBO, &textEBO, textIndices);

    target->vertexData = {
        0.0f, 0.0f,
        0.0f, static_cast<float>(target->height),
        static_cast<float>(target->width), static_cast<float>(target->height),
        static_cast<float>(target->width), 0.0f
    };
    target->indices = {
        0, 1, 2,
        0, 2, 3
    };
    unsigned int EBO;
    setupGLrectangle(&target->VAO, &target->VBO, &EBO, target->vertexData, target->indices);
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

        this->position.x += this->posDiff.x;
        this->position.y += this->posDiff.y;

        this->close->position.x += this->posDiff.x;
        this->close->position.y += this->posDiff.y;

        this->collapse->position.x += this->posDiff.x;
        this->collapse->position.y += this->posDiff.y;

        if (diffX != 0 || diffY != 0) {
            updRectAABB(&this->paneAABB, this->width, this->height, this->position);
            updRectAABB(&this->close->AABB, this->close->width, this->close->height, this->close->position);
            updRectAABB(&this->collapse->AABB, this->collapse->width, this->collapse->height, this->collapse->position);
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
            updRectAABB(&this->parent->buttons[i]->AABB, this->parent->buttons[i]->width, this->parent->buttons[i]->height, this->parent->buttons[i]->position);
        }

        for (unsigned int i = 0; i < this->parent->checkboxes.size(); i++) {
            this->parent->checkboxes[i]->position.x += this->parent->handle->posDiff.x;
            this->parent->checkboxes[i]->position.y += this->parent->handle->posDiff.y;
            updRectAABB(&this->parent->checkboxes[i]->AABB, this->parent->checkboxes[i]->width, this->parent->checkboxes[i]->height, this->parent->checkboxes[i]->position);
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



void deoui::genResizeHandle(resizeHandle* target, UIlayer* parent, glm::vec2 position, std::string pathToTex) {
    target->parent = parent;

    target->vertexData = {
        0.0f, 0.0f,   0.0f, 1.0f,
        0.0f, 20.0f,   0.0f, 0.0f,
        20.0f, 20.0f,   1.0f, 0.0f,
        20.0f, 0.0f,   1.0f, 1.0f
    };
    target->indices = {
        0, 1, 2,
        0, 2, 3
    };
    target->RGBcolor = glm::vec3(0.0f, 0.0f, 0.25f);
    target->position = glm::vec3(position, 0.0f);
    target->width = 20;
    target->height = 20;

    unsigned int EBO;
    setupGLtexRectangle(&target->VAO, &target->VBO, &EBO, target->vertexData, target->indices);

    target->resizeTex = textureFromFile(pathToTex.c_str());

    target->AABB = {
        position.x, position.y,
        position.x + static_cast<float>(target->width), position.y + static_cast<float>(target->width)
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

            this->parent->handle->width += diffX;
            this->parent->handle->vertexData = {
                0.0f, 0.0f,
                0.0f, static_cast<float>(this->parent->handle->height),
                static_cast<float>(this->parent->handle->width), static_cast<float>(this->parent->handle->height),
                static_cast<float>(this->parent->handle->width), 0.0f
            };
            updGLrectangle(this->parent->handle->VAO, this->parent->handle->VBO, this->parent->handle->vertexData);
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

        updRectAABB(&this->parent->handle->paneAABB, this->parent->handle->width, this->parent->handle->height, this->parent->handle->position);
        updRectAABB(&this->parent->handle->close->AABB, this->parent->handle->close->width, this->parent->handle->close->height, this->parent->handle->close->position);
        updRectAABB(&this->parent->rHandle->AABB, this->parent->rHandle->width, this->parent->rHandle->height, this->parent->rHandle->position);
    }
}
void deoui::resizeHandle::Draw(Shader* usedShader, glm::mat4 transform) {
    transform = glm::mat4(1.0f);
    transform = glm::translate(transform, this->position);
    usedShader->use();
    usedShader->setVec3("textColor", this->RGBcolor);
    usedShader->setMat4("model", transform);
    glBindTexture(GL_TEXTURE_2D, this->resizeTex);
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, (void*)0);
}



void deoui::genFloatInField(floatInField* target, glm::vec2 position, float* value) {
    target->vertexData = {
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
    setupGLrectangle(&target->VAO, &target->VBO, &EBO, target->vertexData, indices);
    target->position = position;

    unsigned int textEBO;
    setupGLtexRectangle(&target->textVAO, &target->textVBO, &textEBO, indices);

    target->RGBcolor = glm::vec3(0.5f);

    target->AABB = {
        position.x, position.y,
        position.x + 60.0f, position.y + 20.0f
    };
    target->value = value;

    target->displayValue = std::to_string(*value);
    target->displayValue = trimFSTR(target->displayValue);
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



void deoui::genVec3InField(vec3InField* target, glm::vec2 position, glm::vec3* value) {
    target->position = position;
    target->vectorValue = value;

    for (unsigned char i = 0; i < 3; i++) {
        target->fields[i] = new floatInField;
        glm::vec2 currentPos = glm::vec2(target->position.x + (65.0f * i), target->position.y);
        if (i == 0) {
            genFloatInField(target->fields[i], currentPos, &target->vectorValue->x);
        }else if (i == 1) {
            genFloatInField(target->fields[i], currentPos, &target->vectorValue->y);
        }else {
            genFloatInField(target->fields[i], currentPos, &target->vectorValue->z);
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



void deoui::UIlayer::drawLayer(glm::mat4 transform, bool overlapping) {
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
    transform = glm::translate(transform, glm::vec3(this->handle->position.x - 15.0f, this->handle->position.y, 0.0f));
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
    button* target = new button;
    glm::vec2 position = this->nextElemPos();
    genButton(target, glm::vec3(position, 0.0f), 15, 15, name);
    this->buttons.push_back(target);
}
void deoui::UIlayer::addCheckbox(std::string name, bool* value) {
    checkbox* target = new checkbox;
    glm::vec2 position = this->nextElemPos();
    genCheckbox(target, value, glm::vec3(position, 0.0f), 15, 15, name);
    this->checkboxes.push_back(target);
}
void deoui::UIlayer::addFloatInField(float* value) {
    floatInField* target = new floatInField;
    glm::vec2 position = this->nextElemPos();
    genFloatInField(target, position, value);
    this->floatFields.push_back(target);
}
void deoui::UIlayer::addVec3InField(glm::vec3* value) {
    vec3InField* target = new vec3InField;
    glm::vec2 position = this->nextElemPos();
    genVec3InField(target, position, value);
    this->vec3Fields.push_back(target);
}
deoui::UIlayer::UIlayer(Shader* shade, Shader* textShade, unsigned int width, unsigned int height, glm::vec2 position, std::string name) {
    this->pane = new BGrect;
    genBGrect(this->pane, glm::vec3(position.x, position.y + 15.0f, 0.0f), width, height);
    this->pane->RGBcolor = glm::vec3(0.0f);
    this->width = width;
    this->height = height;

    this->handle = new paneHandle;
    genPaneHandle(this->handle, this, glm::vec3(position, 0.0f), width, 15, name);

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

    this->rHandle = new resizeHandle;
    glm::vec2 rHandlePos = glm::vec2(position.x + static_cast<float>(width) - 20.0f, position.y + static_cast<float>(height) - 5.0f);
    genResizeHandle(this->rHandle, this, rHandlePos, "../res/textures/uiThings/resize.png");

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
        bool lastCollapseStat = target->handle->collapsed;//used to check if pane was just collapsed/uncollapsed
        target->handle->pollStatus();
        if (!target->handle->collapsed) {
            target->rHandle->pollStatus();
            if (lastCollapseStat != target->handle->collapsed) {
                target->outV_Data = {
                    -1.0f, -1.0f,
                    -1.0f, target->height + 16.0f,
                    target->width + 1.0f, target->height + 16.0f,
                    target->width + 1.0f, -1.0f
                };
                updGLrectangle(target->outVAO, target->outVBO, target->outV_Data);
                updRectAABB(&target->layerAABB, target->pane->width, target->pane->height + 15, target->handle->collapse->position);
            }//checks if pane was just uncollapsed
            for (unsigned int i = 0; i < target->buttons.size(); i++) {
                target->buttons[i]->pollStatus();
            }
            for (unsigned int i = 0; i < target->checkboxes.size(); i++) {
                target->checkboxes[i]->pollStatus();
            }
            for (unsigned int i = 0; i < target->floatFields.size(); i++) {
                target->floatFields[i]->pollStatus();
            }
            for (unsigned int i = 0; i < target->vec3Fields.size(); i++) {
                target->vec3Fields[i]->pollStatus();
            }
        }else if (lastCollapseStat != target->handle->collapsed) {
            target->outV_Data = {
                -1.0f, -1.0f,
                -1.0f, target->handle->height + 1.0f,
                target->handle->width + 31.0f, target->handle->height + 1.0f,
                target->handle->width + 31.0f, -1.0f
            };
            updGLrectangle(target->outVAO, target->outVBO, target->outV_Data);
            updRectAABB(&target->layerAABB, target->pane->width, 15, target->handle->collapse->position);
        }//checks if pane was just collapsed

        bool overlapping = 0;
        updCurInAABB(target->layerAABB, deoEvent::mainCursor->currentPos, &target->colX, &target->colY);
        if (target->colX && target->colY) {
            if (deoEvent::MLC != nullptr) {
                if (deoEvent::MLC->activity == GLFW_PRESS && deoEvent::LMLC.activity == GLFW_RELEASE) {
                    if (target->layerPrecedence != 0) {
                        for (unsigned int i = 0; i < target->layerPrecedence; i++) {//search through all layers with higher precedence
                            if (allLayers[i]->colX && allLayers[i]->colY) {//will be true if mouse is hovering over layer with higher precedence
                                overlapping = 1;
                                break;
                            }
                        }
                        if (!overlapping) {
                            UIlayer* overwriter = target;
                            UIlayer* overwritten = allLayers[0];

                            std::vector<UIlayer*> uninvolved;
                            for (unsigned int i = 0; i < allLayers.size(); i++) {
                                if (i != overwriter->layerPrecedence && i != overwritten->layerPrecedence) {
                                    uninvolved.push_back(allLayers[i]);
                                }
                            }

                            allLayers.clear();
                            allLayers.push_back(overwriter);
                            allLayers.push_back(overwritten);

                            overwriter->layerPrecedence = 0;
                            overwritten->layerPrecedence = 1;
                            for (unsigned int i = 0; i < uninvolved.size(); i++) {
                                uninvolved[i]->layerPrecedence = i + 2;
                                allLayers.push_back(uninvolved[i]);
                            }
                        }
                    }
                }
            }
        }

        target->drawLayer(transform, overlapping);
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