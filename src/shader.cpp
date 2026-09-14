#include <iostream>
#include <sstream>
#include <fstream>

#include <glad4.6/glad.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include <shader.hpp>

Shader::Shader(const char* vertexPath, const char* fragmentPath){
    std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try{
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            vShaderFile.close();
            fShaderFile.close();

            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch(std::ifstream::failure e){
            std::cout << "Shader file not successfully read.\n";
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();

        unsigned int vertex, fragment;
        int success;
        char infoLog[512];

        vertex = glCreateShader(GL_VERTEX_SHADER);
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(vertex);
        glCompileShader(fragment);
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if(!success){
            glGetShaderInfoLog(vertex, 512, NULL, infoLog);
            std::cout << "Vertex shader compilation error: " << infoLog << '\n';
        }
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if(!success){
            glGetShaderInfoLog(fragment, 512, NULL, infoLog);
            std::cout << "Fragment shader compilation error: " << infoLog << '\n';
        }
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertex);
        glAttachShader(shaderProgram, fragment);
        glLinkProgram(shaderProgram);
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if(!success){
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            std::cout << "Shader program linking error: " << infoLog << '\n';
        }
        glDeleteShader(vertex);
        glDeleteShader(fragment);
}

void Shader::printActiveUniforms(){
    GLint uniformCount;
    glGetProgramiv(this->shaderProgram, GL_ACTIVE_UNIFORMS, &uniformCount);

    std::cout << "Program " << this->shaderProgram << " has "
              << uniformCount << " active uniforms:\n";

    GLchar name[256];
    GLsizei length;
    GLint size;
    GLenum type;

    for (GLint i = 0; i < uniformCount; i++){
        glGetActiveUniform(
            this->shaderProgram,
            i,
            sizeof(name),
            &length,
            &size,
            &type,
            name
        );

        std::cout << i << ": " << name
                  << " (location "
                  << glGetUniformLocation(this->shaderProgram, name)
                  << ")\n";
    }
}//This is a debugging tool packed into a function when uniforms aren't playing along nicely. OpenGL hands over the uniforms it thinks are in the program.

void Shader::use(){
    glUseProgram(shaderProgram);
}

void Shader::setBool(const std::string& name, bool value) const{
    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), (int)value);
}
void Shader::setFloat(const std::string &name, float value) const{
    glUseProgram(shaderProgram);
    glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), value);
}
void Shader::setInt(const std::string &name, int value) const{
    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
}
void Shader::setVec3(const std::string &name, glm::vec3 value) const{
    glUseProgram(shaderProgram);
    glUniform3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, glm::value_ptr(value));
}
void Shader::setMat4(const std::string &name, glm::mat4 value) const{
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, false, glm::value_ptr(value));
}
void Shader::setMat3(const std::string &name, glm::mat3 value) const{
    glUseProgram(shaderProgram);
    glUniformMatrix3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, false, glm::value_ptr(value));
}