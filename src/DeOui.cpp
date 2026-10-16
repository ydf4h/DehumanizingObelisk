#include <vector>
#include <iostream>
#include <map>

#include <glad4.6/glad.h>

#include <shader.hpp>
#include <DeOui.hpp>
#include <eventHandler.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

void deoui::renderText(Shader* shade, unsigned int VAO, unsigned int VBO, std::string text, float x, float y, float scale, glm::vec3 color) {
    shade->use();
    shade->setVec3("textColor", color);
    shade->setMat4("model", glm::mat4(1.0f));
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
        Character ch = Characters[*c];

        //float xpos = x + ch.bearing.x * scale;
        float xpos = x + ch.bearing.x * scale;
        //bearing y is actually how many pixels go up from the baseline not down
        float ypos = y - ch.bearing.y * scale;

        float w = ch.size.x * scale;
        float h  = ch.size.y * scale;

        float vertices[6][4] = {
            { xpos,     ypos,   0.0f, 0.0f },//top left
            { xpos,     ypos + h,       0.0f, 1.0f },//bottom left
            { xpos + w, ypos + h,       1.0f, 1.0f },//bottom right

            { xpos,     ypos,   0.0f, 0.0f },//top left
            { xpos + w, ypos + h,       1.0f, 1.0f },//bottom right
            { xpos + w, ypos,   1.0f, 0.0f }//top right
        };

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (ch.advance >> 6) * scale;
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
    FT_Set_Pixel_Sizes(face, 0, 48);//sets font width and height parameters, width to 0 allows dynamic width calculation
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

void deoui::UIelem::Draw(Shader* usedShader, glm::mat4 transform){
    usedShader->use();

    if (deoEvent::WRS != nullptr) {
        float pX_Difference = static_cast<float>(deoEvent::WRS->winWidth) / static_cast<float>(deoEvent::WRS->width);
        float pY_Difference = static_cast<float>(deoEvent::WRS->winHeight) / static_cast<float>(deoEvent::WRS->height);
        this->scale = glm::vec3(pX_Difference, pY_Difference, 0.0f);
    }

    transform = glm::mat4(1.0f);
    transform = glm::translate(transform, position);
    usedShader->setMat4("model", transform);

    glEnable(GL_STENCIL_TEST);

    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);

    glBindVertexArray(outVAO);
    usedShader->setVec3("color", outColor);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);

    glBindVertexArray(VAO);
    usedShader->setVec3("color", RGBcolor);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);

    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);

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

    target->scale = glm::vec3(1.0f);
    target->position = position;

    unsigned int EBO;

    glGenVertexArrays(1, &target->VAO);
    glGenBuffers(1, &target->VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(target->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, target->VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ARRAY_BUFFER, target->vertexData.size() * sizeof(float), &target->vertexData[0], GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, target->indices.size() * sizeof(unsigned int), &target->indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glBindVertexArray(0);

    std::vector<float> outV_Data = {
        -1.0f, (float)height + 1.0f,
        (float)width + 1.0f, -1.0f,
        -1.0f, -1.0f,
        (float)width + 1.0f, (float)height + 1.0f
    };
    unsigned int outEBO;

    glGenVertexArrays(1, &target->outVAO);
    glGenBuffers(1, &target->outVBO);
    glGenBuffers(1, &outEBO);

    glBindVertexArray(target->outVAO);
    glBindBuffer(GL_ARRAY_BUFFER, target->outVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, outEBO);

    glBufferData(GL_ARRAY_BUFFER, outV_Data.size() * sizeof(float), &outV_Data[0], GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, target->indices.size() * sizeof(unsigned int), &target->indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glBindVertexArray(0);
}

void deoui::checkbox::Draw(Shader* usedShader, Shader* text, glm::mat4 transform){
    if (deoEvent::WRS != nullptr) {
        float pX_Difference = static_cast<float>(deoEvent::WRS->winWidth) / static_cast<float>(deoEvent::WRS->width);
        float pY_Difference = static_cast<float>(deoEvent::WRS->winHeight) / static_cast<float>(deoEvent::WRS->height);
        this->scale = glm::vec3(pX_Difference, pY_Difference, 0.0f);
    }

    float textScale = height / 48.0f;
    renderText(text, textVAO, textVBO, name, position.x + width + 10.0f, position.y + 48.0f * textScale, textScale, glm::vec3(1.0f, 1.0f, 1.0f));

    transform = glm::mat4(1.0f);
    transform = glm::translate(transform, position);
    usedShader->setMat4("model", transform);
    usedShader->setVec3("color", outColor);

    glBindVertexArray(outVAO);

    glEnable(GL_STENCIL_TEST);

    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_EQUAL, 1, 0xFF);
    glStencilMask(0xFF);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);

    usedShader->setVec3("color", RGBcolor);
    glBindVertexArray(VAO);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);

    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);

    glBindVertexArray(0);
}
void deoui::genCheckbox(checkbox* target, GLFWwindow* habitat, bool* value, glm::vec3 position, int width, int height){
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

    target->width = width;
    target->height = height;

    target->indices = {
        0, 1, 2,
        1, 0, 3
    };

    target->scale = glm::vec3(1.0f);
    target->position = position;

    target->value = value;

    target->AABB = {
        position.x, position.y,
        position.x + (float)width, position.y + (float)height
    };

    target->RGBcolor = glm::vec3(0.0f);
    target->outColor = glm::vec3(1.0f);

    glGenVertexArrays(1, &target->textVAO);
    glGenBuffers(1, &target->textVBO);

    glBindVertexArray(target->textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, target->textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

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

    std::vector<float> outV_Data = {
        -1.0f, (float)height + 1.0f,
        (float)width + 1.0f, -1.0f,
        -1.0f, -1.0f,
        (float)width + 1.0f, (float)height + 1.0f
    };
    unsigned int outVBO, outEBO;

    glGenVertexArrays(1, &target->outVAO);
    glGenBuffers(1, &outVBO);
    glGenBuffers(1, &outEBO);

    glBindVertexArray(target->outVAO);
    glBindBuffer(GL_ARRAY_BUFFER, outVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, outEBO);

    glBufferData(GL_ARRAY_BUFFER, outV_Data.size() * sizeof(float), &outV_Data[0], GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, target->indices.size() * sizeof(unsigned int), &target->indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glBindVertexArray(0);
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
        *this->value = 1;
    }else {
        *this->value = 0;
    }
}

void deoui::button::Draw(Shader* usedShader, Shader* text, glm::mat4 transform) {
    if (deoEvent::WRS != nullptr) {
        float pX_Difference = static_cast<float>(deoEvent::WRS->winWidth) / static_cast<float>(deoEvent::WRS->width);
        float pY_Difference = static_cast<float>(deoEvent::WRS->winHeight) / static_cast<float>(deoEvent::WRS->height);
        this->scale = glm::vec3(pX_Difference, pY_Difference, 0.0f);
    }

    float textScale = height / 48.0f;
    renderText(text, textVAO, textVBO, name, position.x + width + 10.0f, position.y + height, textScale, glm::vec3(1.0f, 1.0f, 1.0f));

    transform = glm::mat4(1.0f);
    transform = glm::translate(transform, position);
    usedShader->setMat4("model", transform);
    usedShader->setVec3("color", outColor);

    glBindVertexArray(outVAO);

    glEnable(GL_STENCIL_TEST);

    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_EQUAL, 1, 0xFF);
    glStencilMask(0xFF);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);

    usedShader->setVec3("color", RGBcolor);
    glBindVertexArray(VAO);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);

    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);

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

    target->width = width;
    target->height = height;

    target->indices = {
        0, 1, 2,
        1, 0, 3
    };

    target->scale = glm::vec3(1.0f);
    target->position = position;

    target->AABB = {
        position.x, position.y,
        position.x + (float)width, position.y + (float)height
    };

    target->RGBcolor = glm::vec3(0.0f);
    target->outColor = glm::vec3(1.0f);

    glGenVertexArrays(1, &target->textVAO);
    glGenBuffers(1, &target->textVBO);

    glBindVertexArray(target->textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, target->textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

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

    std::vector<float> outV_Data = {
        -1.0f, (float)height + 1.0f,
        (float)width + 1.0f, -1.0f,
        -1.0f, -1.0f,
        (float)width + 1.0f, (float)height + 1.0f
    };
    unsigned int outVBO, outEBO;

    glGenVertexArrays(1, &target->outVAO);
    glGenBuffers(1, &outVBO);
    glGenBuffers(1, &outEBO);

    glBindVertexArray(target->outVAO);
    glBindBuffer(GL_ARRAY_BUFFER, outVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, outEBO);

    glBufferData(GL_ARRAY_BUFFER, outV_Data.size() * sizeof(float), &outV_Data[0], GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, target->indices.size() * sizeof(unsigned int), &target->indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glBindVertexArray(0);
}
void deoui::button::pollStatus() {
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
            active = 1;
        }else if (MLC->activity == GLFW_PRESS) {
            this->RGBcolor = glm::vec3(1.0f);
        }
    }

    if (active) {
        this->RGBcolor = glm::vec3(1.0f);
        active = 0;
    }
}

void deoui::UIlayer::drawLayer(glm::mat4 transform, const GLFWvidmode* video) {
    glDisable(GL_DEPTH_TEST);

    if (deoEvent::WRS != nullptr) {
        glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(deoEvent::WRS->width), static_cast<float>(deoEvent::WRS->height), 0.0f);
        textShade->use();
        textShade->setMat4("projection", projection);
        shade->use();
        shade->setMat4("projection", projection);
    }

    unsigned int lastX = 0, lastY = 0;
    int diffX = 0, diffY = 0;
    if (deoEvent::MM != nullptr) {
        if (deoEvent::MM->currentPos[0] >= this->resizeAABB[0] && deoEvent::MM->currentPos[0] <= this->resizeAABB[2]) {
            this->colX = 1;
        }else {
            this->colX = 0;
        }
        if (deoEvent::MM->currentPos[1] >= this->resizeAABB[1] && deoEvent::MM->currentPos[1] <= this->resizeAABB[3]) {
            this->colY = 1;
        }else {
            this->colY = 0;
        }
        diffX = deoEvent::MM->currentPos[0] - deoEvent::MM->lastPos[0];
        diffY = deoEvent::MM->currentPos[1] - deoEvent::MM->lastPos[1];
    }
    if (this->colX && this->colY) {
        this->pane->outColor = glm::vec3(1.0f, 0.0f, 0.0f);
        if (deoEvent::MLC != nullptr) {
            if (deoEvent::MLC->activity == GLFW_PRESS) {
                if (this->width + diffX >= 30) {
                    this->width += diffX;
                }
                if (this->height + diffY >= 30) {
                    this->height += diffY;
                }

                this->pane->vertexData = {//bgrect
                    0.0f, (float)this->height,
                    (float)this->width, 0.0f,
                    0.0f, 0.0f,
                    (float)this->width, (float)this->height
                };
                std::vector<float> outV_Data = {//outline
                    -1.0f, (float)this->height + 1.0f,
                    (float)this->width + 1.0f, -1.0f,
                    -1.0f, -1.0f,
                    (float)this->width + 1.0f, (float)this->height + 1.0f
                };
                glBindVertexArray(this->pane->VAO);//bgrect
                glBindBuffer(GL_ARRAY_BUFFER, this->pane->VBO);
                glBufferSubData(GL_ARRAY_BUFFER, 0, this->pane->vertexData.size() * sizeof(float), &this->pane->vertexData[0]);
                glBindVertexArray(this->pane->outVAO);//outline
                glBindBuffer(GL_ARRAY_BUFFER, this->pane->outVBO);
                glBufferSubData(GL_ARRAY_BUFFER, 0, outV_Data.size() * sizeof(float), &outV_Data[0]);

                this->resizePos = glm::vec3(this->pane->position.x + this->width - 20.0f, this->pane->position.y + this->height - 20.0f, 0.0f);//update position for resize box
                this->resizeAABB = {//update aabb for resize box
                    this->resizePos.x, this->resizePos.y,
                    this->resizePos.x + 20.0f, this->resizePos.y + 20.0f
                };
            }
        }
    }else {
        this->pane->outColor = glm::vec3(1.0f);
    }

    pane->Draw(shade, transform);

    transform = glm::translate(transform, this->resizePos);
    textShade->use();
    textShade->setVec3("textColor", pane->outColor);
    textShade->setMat4("model", transform);
    glBindVertexArray(resizeVAO);
    glBindTexture(GL_TEXTURE_2D, resizeTex);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

    for (unsigned int i = 0; i < buttons.size(); i++) {
        buttons[i]->pollStatus();
        buttons[i]->Draw(shade, textShade, transform);
    }
    for (unsigned int i = 0; i < checkboxes.size(); i++) {
        checkboxes[i]->pollStatus();
        checkboxes[i]->Draw(shade, textShade, transform);
    }

    glEnable(GL_DEPTH_TEST);
}
void deoui::UIlayer::addButton(std::string name) {
    button* target = new button;
    glm::vec3 position = glm::vec3(this->pane->position.x + 10, this->pane->position.y + 10 + (35 * this->buttons.size()) + (35 * this->checkboxes.size()), 0.0f);
    target->name = name;
    genButton(target, this->habitat, position, 25, 25);
    this->buttons.push_back(target);
}
void deoui::UIlayer::addCheckbox(std::string name, bool* value) {
    checkbox* target = new checkbox;
    glm::vec3 position = glm::vec3(this->pane->position.x + 10, this->pane->position.y + 10 + (35 * this->buttons.size()) + (35 * this->checkboxes.size()), 0.0f);
    target->name = name;
    genCheckbox(target, this->habitat, value, position, 25, 25);
    this->checkboxes.push_back(target);
}
deoui::UIlayer::UIlayer(GLFWwindow* habitat, Shader* shade, Shader* textShade, unsigned int width, unsigned int height, unsigned int resizeTex, glm::vec2 position, glm::vec3 RGBcolor, glm::vec3 outColor) {
    BGrect* mainPane = new BGrect;
    genBGrect(mainPane, glm::vec3(position.x, position.y, 0.0f), width, height);
    mainPane->RGBcolor = RGBcolor;
    mainPane->outColor = outColor;
    this->pane = mainPane;
    this->width = width;
    this->height = height;

    this->habitat = habitat;

    this->shade = shade;
    this->textShade = textShade;

    this->resizeTex = resizeTex;//texture for resize box thing
    std::vector<float> resizeV_Data = {//create vertex data for resize capability
        0.0f, 0.0f,     0.0f, 1.0f,//top left
        0.0f, 20.0f,     0.0f, 0.0f,//bottom left
        20.0f, 20.0f,      1.0f, 0.0f,//bottom right
        20.0f, 0.0f,     1.0f, 1.0f//top right
    };
    std::vector<unsigned int> resizeIndices = {//create indices for resize rectangle
        0, 1, 2,
        0, 2, 3
    };
    glm::vec3 resizePosition = glm::vec3(mainPane->position.x + this->width - 20.0f, mainPane->position.y + this->height - 20.0f, 0.0f);
    this->resizePos = resizePosition;
    this->resizeAABB = {
        resizePosition.x, resizePosition.y,
        resizePosition.x + 20.0f, resizePosition.y + 20.0f
    };
    unsigned int VBO, EBO;
    glGenVertexArrays(1, &resizeVAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(resizeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ARRAY_BUFFER, resizeV_Data.size() * sizeof(float), &resizeV_Data[0], GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, resizeIndices.size() * sizeof(unsigned int), &resizeIndices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)0);
}
void deoui::UIlayer::deleteLayer() {
    delete pane;
    for (unsigned int i = 0; i < buttons.size(); i++) {
        delete buttons[i];
    }
    for (unsigned int i = 0; i < checkboxes.size(); i++) {
        delete checkboxes[i];
    }
}