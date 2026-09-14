#include <vector>

#include <glad4.6/glad.h>

#include <shader.hpp>
#include <model.hpp>
#include <OBJ_parser.hpp>

void Mesh::drawMesh(Shader* usedShader, glm::mat4* transform){
    usedShader->use();

    *transform = glm::mat4(1.0f);
    if(parent != nullptr){
        *transform = glm::translate(*transform, parent->position + this->position);
        *transform = glm::scale(*transform, parent->scale + this->scale);
    }else{
        *transform = glm::translate(*transform, this->position);
        *transform = glm::scale(*transform, this->scale);
    }

    usedShader->setMat4("model", *transform);

    glBindVertexArray(VAO);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);

    glBindVertexArray(0);
}

void Mesh::setupMesh(){
    unsigned int VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), &vertexData[0], GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    glBindVertexArray(0);
}

void Mesh::setupMesh(glm::vec3 sca, glm::vec3 pos, glm::vec3 ori){
    scale = sca;
    position = pos;
    orientation = ori;

    unsigned int VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), &vertexData[0], GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    glBindVertexArray(0);
}

void Mesh::updateTransform(glm::vec3 sca, glm::vec3 pos, glm::vec3 ori){
    scale = sca;
    position = pos;
    orientation = ori;
}

Model::Model(const std::string filepath, glm::vec3 scale, glm::vec3 pos, glm::vec3 ori){
    loadOBJ(filepath, this);

    this->scale = scale;
    this->position = pos;
    this->orientation = ori;

    for(unsigned int i = 0; i < meshes.size(); i++){
        meshes[i].setupMesh();
    }
}

Model::Model(const std::string filepath){
    loadOBJ(filepath, this);

    this->scale = glm::vec3(1.0f);
    this->position = glm::vec3(0.0f);
    this->orientation = glm::vec3(0.0f);

    for(unsigned int i = 0; i < meshes.size(); i++){
        meshes[i].setupMesh();
    }
}

void Model::Draw(Shader* usedShader, glm::mat4* transform){
    usedShader->use();

    *transform = glm::mat4(1.0f);
    *transform = glm::translate(*transform, position);
    *transform = glm::scale(*transform, scale);

    usedShader->setMat4("model", *transform);

    for(unsigned int i = 0; i < meshes.size(); i++){
        meshes[i].drawMesh(usedShader, transform);
    }
}

void Model::updTransform(glm::vec3 scale, glm::vec3 pos, glm::vec3 ori){
    this->scale = scale;
    this->position = pos;
    this->orientation = ori;
}

void Model::resetTransform(){
    this->scale = glm::vec3(1.0f, 1.0f, 1.0f);
    this->position = glm::vec3(0.0f, 0.0f, 0.0f);
    this->orientation = glm::vec3(0.0f, 0.0f, 0.0f);
}