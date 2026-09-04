#include "SolarSystem.h"


glm::vec3 sphericalToCartesian(glm::vec3 pos){
    float r = pos.x;
	float theta = pos.y;
	float phi = pos.z;

	float x = r * sin(theta)*cos(phi); 
	float y = r*cos(theta); // in screen coords y is up
	float z = r*sin(theta)*sin(phi);
    return glm::vec3(x,y,z);
}



Body::Body(glm::vec3 position, double mass, float radius) : modelMatrix(glm::mat4(1.0f)), Position(position), mass(mass), radius(radius){

    int rings = 16;
    int sectors = 32;
    float theta, phi;

    for(unsigned int i = 0;i<=rings;++i){
        for(unsigned int j = 0;j<=sectors;++j){
            theta = PI * (float)i / (float)rings;
            phi = 2 * PI * (float)j / (float)sectors;
            vertices.push_back(glm::vec3(radius, theta, phi));
        }
    }

    for (unsigned int i = 0; i < rings; ++i) {
        for (unsigned int j = 0; j < sectors; ++j) {
            unsigned int cur  = i * (sectors + 1) + j;
            unsigned int next = (i + 1) * (sectors + 1) + j;

            
            if(i == 0){
                // triangle 1
                indices.push_back(cur);
                indices.push_back(next);
                indices.push_back(next + 1);
            }
            else if(i == rings -1){
                // triangle 1
                indices.push_back(cur);
                indices.push_back(next);
                indices.push_back(cur + 1);
            }
            else{
            // triangle 1
            indices.push_back(cur);
            indices.push_back(next);
            indices.push_back(cur + 1);
            // Triangle 2
            indices.push_back(cur + 1);
            indices.push_back(next);
            indices.push_back(next + 1);
            }
        }
    }
    indexCount = indices.size();
    
  
    if(position.x > 5.0f){
            LineSegments = 200;
        }
    
    for (int i = 0; i < LineSegments; ++i) {
        float angle = (2.0f * PI * i) / LineSegments;
        ringPoints.push_back(glm::vec3(position.x, PI/2.0f, angle));
    }
    
    // Ring buffers
    glGenVertexArrays(1, &VAO_Line);
    glGenBuffers(1, &VBO_Line);

    glBindVertexArray(VAO_Line);
    glBindBuffer(GL_ARRAY_BUFFER,VBO_Line);
    glBufferData(GL_ARRAY_BUFFER, ringPoints.size()*sizeof(glm::vec3), &ringPoints[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0); 

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    // Generate Buffers
    glGenVertexArrays(1, &VAO_body);
    glGenBuffers(1, &VBO_body);
    glGenBuffers(1, &EBO_body);

    // bind Buffers    
    glBindVertexArray(VAO_body);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_body);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_body);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Position Attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0); 
    glEnableVertexAttribArray(0); 

    // Unbind
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

}


Body::~Body(){
    glDeleteVertexArrays(1, &VAO_body);
    glDeleteBuffers(1,&VBO_body);
    glDeleteBuffers(1,&EBO_body);
    glDeleteVertexArrays(1, &VAO_Line);
    glDeleteBuffers(1,&VBO_Line);
}



void Body::DrawBody(unsigned int ShaderProgram, const glm::mat4& ViewProjection){

    modelMatrix = glm::translate(glm::mat4(1.0f), Position);

    glUseProgram(ShaderProgram);

    unsigned int modelLoc = glGetUniformLocation(ShaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &modelMatrix[0][0]);

    unsigned int VPLoc = glGetUniformLocation(ShaderProgram, "viewProjection");
    glUniformMatrix4fv(VPLoc, 1, GL_FALSE, &ViewProjection[0][0]);

    glBindVertexArray(VAO_body);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    Shader LineShader("ShaderSources/shader.vs","ShaderSources/FragmentShaders/lineShader.fs");

    glUseProgram(LineShader.ID);

    modelMatrix = glm::mat4(1.0f);

    modelLoc = glGetUniformLocation(LineShader.ID, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &modelMatrix[0][0]);

    VPLoc = glGetUniformLocation(LineShader.ID, "viewProjection");
    glUniformMatrix4fv(VPLoc, 1, GL_FALSE, &ViewProjection[0][0]);

    glLineWidth(0.2f);

    glBindVertexArray(VAO_Line);
    glDrawArrays(GL_LINE_LOOP, 0, LineSegments);
    glBindVertexArray(0);
}



void Body::updateBodyPosition(float deltaTime){
    Velocity += Acceleration * deltaTime;
    Position += Velocity * deltaTime;
}

glm::vec3 Body::CartToSphPosition(const glm::vec3 &cartesian){
    float r = glm::length(cartesian);
    if(r < 0.0001f) return glm::vec3(0.0f);

    float theta = std::acos(glm::clamp(cartesian.y / r, -1.0f, 1.0f));
    float phi = std::atan2(cartesian.z, cartesian.x);
    if(phi < 0.0f){
        phi += 2.0f * PI;
    }

    return glm::vec3(r,theta,phi);
}

/*
void DrawRings(const glm::mat4& ViewProjection, std::vector<Body>& bodies){

    Shader LineShader("ShaderSources/shader.vs","ShaderSources/FragmentShaders/lineShader.fs");

    unsigned int VBO_Line, VAO_Line;
    int LineSegments = 100;


    for(int i=1; i<bodies.size();i++){
        if(bodies[i].Position.x > 5.0f){
            LineSegments = 200;
        }
        
        for (int i = 0; i < LineSegments; ++i) {
            float angle = (2.0f * PI * i) / LineSegments;
            bodies[i].ringPoints.push_back(glm::vec3(bodies[i].Position.x, PI/2.0f, angle));
        }

        glGenVertexArrays(1, &VAO_Line);
        glGenBuffers(1, &VBO_Line);

        glBindVertexArray(VAO_Line);
        glBindBuffer(GL_ARRAY_BUFFER,VBO_Line);
        glBufferData(GL_ARRAY_BUFFER, ringPoints.size()*sizeof(glm::vec3), &ringPoints[0],GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0); 

        glBindBuffer(GL_ARRAY_BUFFER,0);
        glBindVertexArray(0);

        glUseProgram(LineShader.ID);

        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f));

        unsigned int modelLoc = glGetUniformLocation(LineShader.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &modelMatrix[0][0]);

        unsigned int VPLoc = glGetUniformLocation(LineShader.ID, "viewProjection");
        glUniformMatrix4fv(VPLoc, 1, GL_FALSE, &ViewProjection[0][0]);

        glBindVertexArray(VAO_Line);
        glDrawArrays(GL_LINE_LOOP, 0, LineSegments);
        glBindVertexArray(0);


        glDeleteVertexArrays(1, &VAO_Line);
        glDeleteBuffers(1,&VBO_Line);
    }
}
*/


glm::vec3 gravity(Body& A, Body& B){ 
    float squaredDist = glm::distance2(A.Position, B.Position);
    if (squaredDist < 0.0001f) return glm::vec3(0.0f);
    float g = (G*A.mass) / squaredDist;
    glm::vec3 vector = glm::normalize(A.Position - B.Position);
    return g*vector;
}

void updatePositions(std::vector<Body> bodies, float DeltaTime){

    bodies[1].Acceleration = gravity(bodies[0],bodies[1]);
    bodies[1].Velocity += bodies[1].Acceleration * DeltaTime;
    bodies[1].Position += bodies[1].Velocity * DeltaTime;
}

void updateGravity(Body& A, Body& B){
    A.Acceleration = gravity(B, A);
    B.Acceleration = gravity(A, B);
}

glm::vec3 initalVelocity(Body& A, Body& B){
    float dist = glm::distance(A.Position, B.Position);
    glm::vec3 up = glm::vec3(0.0f,1.0f,0.0f);
    glm::vec3 gravVector = A.Position - B.Position;
    glm::vec3 perpVec = glm::cross(up,gravVector);
    perpVec = glm::normalize(perpVec);
    float velocity = std::sqrt((G*A.mass)/dist);
    return velocity * perpVec;
}


Engine::Engine(int ScrWidth, int ScrHeight, const char* name, glm::vec3 cameraStart, glm::vec3 cameraTarget):  ScrWidth(ScrWidth), ScrHeight(ScrHeight), name(name),
 lastX((float)ScrWidth / 2.0f),lastY((float)ScrHeight / 2.0f){
    camera.Position = cameraStart;
    camera.setTarget(cameraTarget);
 }
 
 
Engine::~Engine(){
    cleanup();
}


bool Engine::Init(){

    firstMouse = true;    
    deltaTime = 0.0f;
    lastFrame = 0.0f;

    glfwInit();

   
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // setting version to 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // telling it to use core profile

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


    return true;

}

bool Engine::shouldClose() const{
    return glfwWindowShouldClose(window);
}

void Engine::beginFrame() {
    float currentFrame = (float)glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    glClearColor(0.0f,0.0f,0.0f,1.0f);
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

Shader::Shader(const char* vertexPath, const char* fragmentPath){
    
    // retrieve the vertex/fragment source code from filepath ------------------------------------------------------------------------
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // ensure ifstream objects can throw exceptions
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        // close file handlers
        vShaderFile.close();
        fShaderFile.close();

        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch(std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // compile shaders ----------------------------------------------------------------------------------------
    unsigned int vertex, fragment;

    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    // compile error check
    checkCompileErrors(vertex, "VERTEX");

    // fragment shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    // Shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    // linking error check
    checkCompileErrors(ID, "PROGRAM");

    // delete shaders as they are linked 
    glDeleteShader(vertex);
    glDeleteShader(fragment);

}

void Shader::use(){
    glUseProgram(ID);
}

void Shader::setBool(const std::string &name, bool value) const{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
// ------------------------------------------------------------------------
void Shader::setInt(const std::string &name, int value) const{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::setFloat(const std::string &name, float value) const{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::setMat4fv(const std::string &name, glm::mat4 value) const{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

// GLM functions
// ------------------------------------------------------------------------
void Shader::setVec2(const std::string &name, const glm::vec2 &value) const
{ 
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
}
void Shader::setVec2(const std::string &name, float x, float y) const
{ 
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y); 
}
// ------------------------------------------------------------------------
void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{ 
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
}
void Shader::setVec3(const std::string &name, float x, float y, float z) const
{ 
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z); 
}
// ------------------------------------------------------------------------
void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
{ 
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
}
void Shader::setVec4(const std::string &name, float x, float y, float z, float w) const
{ 
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w); 
}
// ------------------------------------------------------------------------
void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

// Compile error checker type = "PROGRAM" or "SHADER"
void Shader::checkCompileErrors(GLuint shader, std::string type){
     GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM){
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;

    setTarget(target);
    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix(){
    return glm::lookAt(Position, Position + Front, Up);
}

void Camera::setTarget(glm::vec3 target){
    glm::vec3 direction = glm::normalize(target - Position);

    Pitch = glm::degrees(asin(direction.y));
    Yaw   = glm::degrees(atan2(direction.z, direction.x));

    updateCameraVectors();
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime){
    float velocity = MovementSpeed * deltaTime;
    if (direction == FORWARD)
        Position += Front * velocity;
    if (direction == BACKWARD)
        Position -= Front * velocity;
    if (direction == LEFT)
        Position -= Right * velocity;
    if (direction == RIGHT)
        Position += Right * velocity;
    if (direction == UP)
        Position += Up * velocity;
    if (direction == DOWN)
        Position -= Up * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw   += xoffset;
    Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
    Zoom -= (float)yoffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
}

void Camera::updateCameraVectors()
{
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up    = glm::normalize(glm::cross(Right, Front));
}
