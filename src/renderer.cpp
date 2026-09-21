#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#include <glad4.6/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include <eventHandler.hpp>
#include <OBJ_parser.hpp>
#include <shader.hpp>
#include <DeOui.hpp>
#include <BMP_parser.hpp>

class Camera{
public:
    float pitch = 0.0f;

    //Remember (XYZ) = (RollPitchYaw)

    float yaw = 0.0f;
    float fov = 60.0f;
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraTop = glm::vec3(0.0f, 1.0f, 0.0f);
};
Camera mainCamera;

class DirLight{
public:
    std::string name;

    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    DirLight(std::string name, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular){
        this->name = name;
        this->direction = direction;
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
    }
    void setShaderValues(Shader* affectedShader){
        affectedShader->use();
        affectedShader->setVec3(name + ".direction", this->direction);
        affectedShader->setVec3(name + ".ambient", this->ambient);
        affectedShader->setVec3(name + ".diffuse", this->diffuse);
        affectedShader->setVec3(name + ".specular", this->specular);
    }
};

void drawOutline(Shader* base, Shader* outline, glm::mat4* transform, Model* model, glm::vec3 color, float thickness){
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    glEnable(GL_STENCIL_TEST);
    glEnable(GL_DEPTH_TEST);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
    model->Draw(base, transform);//setup and draw base model

    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    model->updTransform(model->scale + glm::vec3(thickness), glm::vec3(model->position), glm::vec3(model->orientation));
    outline->use();
    outline->setVec3("outlineColor", color);
    model->Draw(outline, transform);//setup and draw outline

    model->resetTransform();
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
    glDisable(GL_STENCIL_TEST);//cleanup and restore base model
}

unsigned int textureFromFile(const char* path){
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

void calculateFPS(float deltaTime, float& accumulation, unsigned int& timesAdded);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void cursor_pos_callback(GLFWwindow* window, double xPos, double yPos);

void processInput(GLFWwindow* window, float deltaTime);

void debug_message_callback(GLenum source, GLenum type, GLuint ID, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

bool firstFrame = 0;

int main(){
    if (!glfwInit()){
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* prim_monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* monVidmode = glfwGetVideoMode(prim_monitor);

    //GLFWwindow* window = glfwCreateWindow(monVidmode->width, monVidmode->height, "DeO3D", NULL, NULL);
    GLFWwindow* window = glfwCreateWindow(640, 480, "DeO3D", NULL, NULL);
    if (!window){
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwWindowHint(GLFW_DEPTH_BITS, 24); //Creates depth for the window

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);

    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD.\n";
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEBUG_OUTPUT);
    //glDebugMessageCallback(debug_message_callback, 0); //Activate this when needed, currently it's just annoying.

    Shader baseShader("../res/shaders/baseVertex.glsl", "../res/shaders/baseFragment.glsl");
    Shader outlineShader("../res/shaders/baseVertex.glsl", "../res/shaders/outlineFragment.glsl");
    Shader uiShader("../res/shaders/uiVertex.glsl", "../res/shaders/uiFragment.glsl");

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetCursorPosCallback(window, cursor_pos_callback);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //Setting this value to 1 makes it so no padding is expected in pixel arrays
    stbi_set_flip_vertically_on_load(1);

    std::string ligname = "dirlight1";
    glm::vec3 ligdir = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);
    DirLight dirlight1(ligname, ligdir, ambient, diffuse, specular);
    dirlight1.setShaderValues(&baseShader);

    glActiveTexture(GL_TEXTURE0);
    unsigned int windowDif = textureFromFile("../res/textures/window.png");

    glActiveTexture(GL_TEXTURE1);
    unsigned int woodDif = textureFromFile("../res/textures/wood.jpg");

    glActiveTexture(GL_TEXTURE2);
    unsigned int grassDif = textureFromFile("../res/textures/grass.png");

    glActiveTexture(GL_TEXTURE3);
    unsigned int channels, texID;
    unsigned char* data = parseBMP("../res/textures/bmp_test.bmp", channels);

    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    delete[] data;


    Model mug("../res/mug.obj");

    std::vector<float> planeVertices = {
        -0.5f, 0.5f, 0.0f,   0.0f, 0.0f, -1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, -1.0f,   0.0f, 0.0f,
        0.5f, -0.5f, 0.0f,   0.0f, 0.0f, -1.0f,   1.0f, 0.0f,
        0.5f, 0.5f, 0.0f,   0.0f, 0.0f, -1.0f,   1.0f, 1.0f
    };

    std::vector<unsigned int> planeIndices = {
        0, 1, 2,
        0, 2, 3
    };

    Mesh plane;
    plane.vertexData = planeVertices;
    plane.indices = planeIndices;
    plane.setupMesh(glm::vec3(1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f));

    Mesh bmpTestPlane;
    bmpTestPlane.vertexData = planeVertices;
    bmpTestPlane.indices = planeIndices;
    bmpTestPlane.setupMesh(glm::vec3(1.0f), glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f));

    float lastFrame = 0.0f; //used for calculating deltaTime

    float accumulation = 0.0f;
    unsigned int timesAdded = 0;

    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST); //Enables stencil buffer
    glDepthFunc(GL_LESS); //This changes which function OpenGL uses for depth testing, look at LearnOpenGL for more.
    //glDepthMask(GL_FALSE); //This disables writing to the depth buffer.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Changes function used for OpenGL blending

    glm::vec3 clearColor = glm::vec3(0.5f, 0.5f, 0.5f);

    deoui::UIlayer mainLayer(window, &uiShader);

    deoui::BGrect background;
    deoui::genBGrect(&background, glm::vec3(50.0f, 50.0f, 0.0f), 300, 300);
    background.RGBcolor = glm::vec3(1.0f, 0.5f, 0.1f);

    deoui::button button1;
    deoui::genButton(&button1, window, glm::vec3(75.0f, 75.0f, 0.0f), 50, 50);
    button1.RGBcolor = glm::vec3(1.0f, 0.0f, 0.0f);

    mainLayer.members.push_back(&background);
    mainLayer.members.push_back(&button1);

    glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    std::vector<glm::vec3> windowPositions = {
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f)
    };

    double lastCPos[2];
    glfwGetCursorPos(window, &lastCPos[0], &lastCPos[1]);

    while (!glfwWindowShouldClose(window)){
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);

        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;//calculates deltatime

        deoEvent::listenAll(window, lastCPos);

        //calculateFPS(deltaTime, accumulation, timesAdded);

        glm::mat4 projection = glm::perspective(glm::radians(mainCamera.fov), 1920.0f / 1200.0f, 0.1f, 1000.0f);
        glm::mat4 view = glm::lookAt(mainCamera.cameraPos, mainCamera.cameraPos + mainCamera.cameraFront, mainCamera.cameraTop);
        glm::mat4 model = glm::mat4(1.0f);

        baseShader.use();
        baseShader.setMat4("projection", projection);
        baseShader.setMat4("view", view);
        baseShader.setMat4("model", model);
        baseShader.setVec3("viewPosition", mainCamera.cameraPos);

        outlineShader.use();
        outlineShader.setMat4("projection", projection);
        outlineShader.setMat4("view", view);
        outlineShader.setMat4("model", model);

        baseShader.use();
        baseShader.setMat4("projection", projection);
        baseShader.setMat4("view", view);
        baseShader.setMat4("model", model);
        baseShader.setVec3("viewPosition", mainCamera.cameraPos);

        baseShader.setInt("diffuseTex", texID - 1);
        glBindTexture(GL_TEXTURE_2D, texID);

        bmpTestPlane.drawMesh(&baseShader, &model);

        /*baseShader.setInt("diffuseTex", woodDif - 1);
        glBindTexture(GL_TEXTURE_2D, woodDif);*/
        drawOutline(&baseShader, &outlineShader, &model, &mug, glm::vec3(1.0f, 0.0f, 0.0f), 0.01f);

        baseShader.setInt("diffuseTex", windowDif - 1);
        glBindTexture(GL_TEXTURE_2D, windowDif);
        glDisable(GL_CULL_FACE);

        /*if(cursorMove.active){
            std::cout << "little buddy" << countup << '\n';
        }
        cursorMove.active = 0;*/

        std::map<float, glm::vec3> sorted;
        for(unsigned int i = 0; i < windowPositions.size(); i++){
            float distance = glm::length(mainCamera.cameraPos - windowPositions[i]);
            sorted[distance] = windowPositions[i];
        }//sort windows in order by distance from viewpoint

        for(std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it){
            plane.updateTransform(glm::vec3(1.0f), it->second, glm::vec3(0.0f));
            plane.drawMesh(&baseShader, &model);
        }//i honestly have no idea what joey does with maps but whatever, and draw windows in order

        button1.pollStatus();

        glm::mat4 uiModel = glm::mat4(1.0f);
        mainLayer.drawLayer(uiModel, monVidmode);

        glEnable(GL_CULL_FACE);

        /*uiShader.use();

        glm::mat4 projectionOrtho = glm::ortho(0.0f, 1920.0f, 1200.0f, 0.0f);
        glm::mat4 model2D = glm::mat4(1.0f);

        uiShader.setMat4("projection", projectionOrtho);
        uiShader.setMat4("model", model2D);

        uiShader.setVec3("currentColor", glm::vec3(0.0f, 0.0f, 0.0f));

        background.Draw(&uiShader, model2D);

        if(button1.pollStatus() == deouiState::TOGGLED_ON){
            uiShader.setVec3("currentColor", glm::vec3(1.0f, 1.0f, 1.0f));
        }

        button1.Draw(&uiShader, model2D);*/

        deoEvent::clearEvents();

        processInput(window, deltaTime);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

float deltaAccumulation = 0.0f;
void calculateFPS(float deltaTime, float& accumulation, unsigned int& timesAdded){
    deltaAccumulation += deltaTime;
    float instantFPS = 1.0f / deltaTime;
    accumulation += instantFPS;
    timesAdded++;
    if(deltaAccumulation >= 1.0f){
        std::cout << "FPS: " << accumulation / timesAdded << '\n';
        deltaAccumulation = 0.0f;
        accumulation = 0.0f;
        timesAdded = 0;
    }
}//keep adding instantfps to accumulation, divide accumulation by timesadded to average fps, if deltaaccumulation over/equal 1.0f then reset to poll each second

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

double lastX;
double lastY;

void cursor_pos_callback(GLFWwindow* window, double xPos, double yPos){
    bool cursorActivated = 1;
    if(!cursorActivated){
        lastX = xPos;
        lastY = yPos;
        cursorActivated = 1;
    }

    float xoffset = xPos - lastX;
    float yoffset = lastY - yPos; 
    lastX = xPos;
    lastY = yPos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    mainCamera.yaw   += xoffset;
    mainCamera.pitch += yoffset;

    if(mainCamera.pitch > 89.0f){
        mainCamera.pitch = 89.0f;
    }
    if(mainCamera.pitch < -89.0f){
        mainCamera.pitch = -89.0f;
    }
    glm::vec3 direction;
    direction.x = cos(glm::radians(mainCamera.yaw)) * cos(glm::radians(mainCamera.pitch));
    direction.y = sin(glm::radians(mainCamera.pitch));
    direction.z = sin(glm::radians(mainCamera.yaw)) * cos(glm::radians(mainCamera.pitch));
    mainCamera.cameraFront = glm::normalize(direction);
}

void debug_message_callback(GLenum source, GLenum type, GLuint ID, GLenum severity, GLsizei length, const GLchar* message, const void* userParam){
    if(type == GL_DEBUG_TYPE_ERROR){
        std::cout << "OpenGL error found: " << message << '\n';
    }
}

void processInput(GLFWwindow* window, float deltaTime){
    float movementSpeed = 1.0f * deltaTime;
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        mainCamera.cameraPos += mainCamera.cameraFront * movementSpeed;
    }if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        mainCamera.cameraPos -= mainCamera.cameraFront * movementSpeed;
    }if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        mainCamera.cameraPos -= glm::normalize(glm::cross(mainCamera.cameraFront, mainCamera.cameraTop)) * movementSpeed;
    }if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        mainCamera.cameraPos += glm::normalize(glm::cross(mainCamera.cameraFront, mainCamera.cameraTop)) * movementSpeed;
    }if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
        mainCamera.cameraPos -= mainCamera.cameraTop * movementSpeed;
    }if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){
        mainCamera.cameraPos += mainCamera.cameraTop * movementSpeed;
    }if(glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS){
        glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, 1920, 1200, 165);
    }if(glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS){
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }if(glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS){
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }if(glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS){
        glEnable(GL_CULL_FACE);
    }if(glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS){
        glDisable(GL_CULL_FACE);
    }
}
