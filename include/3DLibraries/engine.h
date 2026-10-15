#ifndef ENGINE_H
#define ENGINE_H


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <3DLibraries/camera.h>
#include <3DLibraries/shader.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

class Engine{

    public:
    int ScrWidth, ScrHeight;
    const char* name;

    // Camera Initalisations
    Camera camera;
    float lastX;
    float lastY;
    bool firstMouse;

    float deltaTime; // Time between current frame and last
    float lastFrame; // time of last frame

    Engine(int ScrWidth, int ScrHeight, const char* name, glm::vec3 cameraStart = glm::vec3(0.0f));
    ~Engine();
    bool Init();

    void processInput(); // process all key inputs
    bool shouldClose() const;
    void beginFrame();
    void endFrame();
    void resetTimer(); // restart frame timing (call after slow loading)
    glm::mat4 ViewProjection();

    float getDeltaTime() const {return deltaTime;};

    private:
    GLFWwindow* window = nullptr;
    void cleanup();
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height); // resets size of window when changed with mouse
    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
   
};


#endif //ENGINE_H