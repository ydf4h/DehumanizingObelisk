#pragma once

#include <vector>

#include <shader.hpp>

class Model;

class Mesh{
public:
    std::vector<float> vertexData;
    std::vector<unsigned int> indices;

    Model* parent = nullptr;

    glm::vec3 scale = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 orientation = glm::vec3(0.0f, 0.0f, 0.0f);//relative to parent model, if not initialized then will all be glm::vec3(0, 0, 0)

    unsigned int VAO;

    void setupMesh();
    void setupMesh(glm::vec3 sca, glm::vec3 pos, glm::vec3 ori);

    void drawMesh(Shader* usedShader, glm::mat4* transform);

    void updateTransform(glm::vec3 sca, glm::vec3 pos, glm::vec3 ori);
};

class Model{
public:
    std::vector<Mesh> meshes;

    glm::vec3 scale;
    glm::vec3 position;
    glm::vec3 orientation;

    Model(const std::string filepath, glm::vec3 size, glm::vec3 pos, glm::vec3 ori);
    Model(const std::string filepath);

    void Draw(Shader* usedShader, glm::mat4* transform);

    void updTransform(glm::vec3 scale, glm::vec3 pos, glm::vec3 ori);

    void resetTransform();
};