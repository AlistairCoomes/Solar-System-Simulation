#include "3DLibraries/engine.h"

Engine::Engine(int ScrWidth, int ScrHeight,const char* name,glm::vec3 cameraStart):  ScrWidth(ScrWidth), ScrHeight(ScrHeight), name(name),
 lastX((float)ScrWidth / 2.0f),lastY((float)ScrHeight / 2.0f), camera(cameraStart){}
 
 
Engine::~Engine(){
    cleanup();
}


bool Engine::Init(){

    firstMouse = true;    
    
    

    glfwInit();

   
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // setting version to 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // telling it to use core profile

    #ifdef __APPLE__
    	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    window = glfwCreateWindow(ScrWidth, ScrHeight, name, NULL, NULL);

    // window creation check
    if(window == NULL){
        std::cout << "ENGINE::Failed to create window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window); //makes the context of the window the context of the current thread

    glfwSetWindowUserPointer(window, this);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // sets the size of the window when it is resized
    glfwSetCursorPosCallback(window, mouse_callback); // process all mouse inputs
    glfwSetScrollCallback(window, scroll_callback); // procces scroller inputs
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  // catches the mouse and locks it to centre of window

    

    // initialise GLAD check ------------------------------------
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "ENGINE::Failed to initialise GLAD" << std::endl;
        return false;
    }

    glEnable(GL_DEPTH_TEST);

    deltaTime = 0.0f;
    lastFrame = (float)glfwGetTime();

    return true;

}

void Engine::resetTimer(){
    deltaTime = 0.0f;
    lastFrame = (float)glfwGetTime();
}

bool Engine::shouldClose() const{
    return glfwWindowShouldClose(window);
}

void Engine::beginFrame() {
    float currentFrame = (float)glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    glClearColor(0.1f,0.1f,0.1f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Engine::endFrame() {
    glfwSwapBuffers(window);
    glfwPollEvents();
}

glm::mat4 Engine::ViewProjection() {
     glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)ScrWidth / (float)ScrHeight, 0.1f, 100.0f);

    glm::mat4 view = camera.GetViewMatrix();

    glm::mat4 viewProjection = projection * view;
    return viewProjection;
}

void Engine::processInput(){

    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
       camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
       camera.ProcessKeyboard(UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
       camera.ProcessKeyboard(DOWN, deltaTime);
    }
}

void Engine::cleanup(){
    if(window){
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}

void Engine::framebuffer_size_callback(GLFWwindow* window, int width, int height){
    Engine* engine = reinterpret_cast<Engine*>(glfwGetWindowUserPointer(window));
    if(engine){
    glViewport(0, 0, width, height);
    }
}

void Engine::mouse_callback(GLFWwindow* window, double xpos, double ypos){
    
    Engine* engine = reinterpret_cast<Engine*>(glfwGetWindowUserPointer(window));

    if (engine) {
        if(engine->firstMouse){
            engine->lastX = xpos;
            engine->lastY = ypos;
            engine->firstMouse = false;
        }
        float xoffset = xpos - engine->lastX;
        float yoffset = engine->lastY - ypos; // y coords range from bottom to top
        engine->lastX = xpos;
        engine->lastY = ypos;
        engine->camera.ProcessMouseMovement(xoffset, yoffset);
    }
}
void Engine::scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    Engine* engine = reinterpret_cast<Engine*>(glfwGetWindowUserPointer(window));
    if(engine){
        engine->camera.ProcessMouseScroll(static_cast<float>(yoffset));
    }
}

