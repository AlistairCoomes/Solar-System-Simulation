#ifndef SOLARSYSTEM_H
#define SOLARSYSTEM_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

const float PI = glm::pi<float>();
const float G = 39.478;

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;



class Body{
    public:
    glm::mat4 modelMatrix;

    glm::vec3 Position = glm::vec3(0.0f);
    glm::vec3 Velocity = glm::vec3(0.0f);
    glm::vec3 Acceleration = glm::vec3(0.0f);
    float mass, radius;


    unsigned int VBO_body, VAO_body, EBO_body;
    unsigned int indexCount = 0;


    // constructor
    Body(glm::vec3 position = glm::vec3(0.0f,0.0f,0.0f), glm::vec3 velocity = glm::vec3(0.0f,0.0f,0.0f), float mass = 0.0f, float radius=1.0f);

    // destructor
    ~Body();


    glm::vec3 CartToSphPosition(const glm::vec3 &cartesian);

    // draws the body
    void DrawBody(unsigned int ShaderProgram, const glm::mat4& ViewProjection);

    void updateBodyPosition(float deltaTime);

    private:
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;

    
};


enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};


class Camera
{
    public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // Constructor
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 target = glm::vec3(0.0f,0.0f,0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
    // Creates a view matrix
    glm::mat4 GetViewMatrix();

    void setTarget(glm::vec3 target);

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset);

    private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors();
};


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

    Engine(int ScrWidth, int ScrHeight, const char* name, glm::vec3 cameraStart = glm::vec3(0.0f,0.0f,3.0f), glm::vec3 cameraTarget = glm::vec3(0.0f));
    ~Engine();
    bool Init();

    void processInput(); // process all key inputs
    bool shouldClose() const;
    void beginFrame();
    void endFrame();
    glm::mat4 ViewProjection();

    float getDeltaTime() const {return deltaTime;};

    private:
    GLFWwindow* window = nullptr;
    void cleanup();
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height); // resets size of window when changed with mouse
    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
   
};


class Shader
{
public:
    // the program ID
    unsigned int ID;

    // constructor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath);

    // use/activate the shader
    void use();

    // utility uniform functions
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setMat4fv(const std::string &name, glm::mat4 value) const;
    void setVec2(const std::string &name, const glm::vec2 &value) const;
    void setVec2(const std::string &name, float x, float y) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setVec3(const std::string &name, float x, float y, float z) const;
    void setVec4(const std::string &name, const glm::vec4 &value) const;
    void setVec4(const std::string &name, float x, float y, float z, float w) const;
    void setMat2(const std::string &name, const glm::mat2 &mat) const;
    void setMat3(const std::string &name, const glm::mat3 &mat) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;


private: 
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(GLuint shader, std::string type);

};


glm::vec3 sphericalToCartesian(glm::vec3 pos);

glm::vec3 gravity(Body A, Body B); // B -> A vector Direction, cartesian vector

void updatePositions(std::vector<Body> bodies, float DeltaTime);

void updateGravity(std::vector<Body> bodies);

#endif SOLARSYSTEM_H