#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include<SolarSystemHeader/SolarSystem.h>

#include <iostream>

enum Bodies{
    Sun,
    Earth
};

int main(){
    Engine engine(800,600,"Solar System Simulation", glm::vec3(0.0f,0.0f,3.0f));
    engine.Init();

    
    std::vector<Body> bodies;
    std::vector<Shader> shaders;

    bodies.reserve(2);
    shaders.reserve(2);

    // Sun[0]
    bodies.emplace_back(glm::vec3(0.0f), glm::vec3(0.0f), 1.0f, 0.2f); 
    shaders.emplace_back("ShaderSources/shader.vs","ShaderSources/SunShader.fs");

    // Earth[1]
    bodies.emplace_back(glm::vec3(1.0f,0.0f,0.0f),glm::vec3(0.0f,0.0f,0.0f), 0.000003f, 0.05f); // (0,6.28,0)
    shaders.emplace_back("ShaderSources/shader.vs","ShaderSources/EarthShader.fs");
    
    bodies[Earth].Acceleration = glm::vec3(-0.1f,0.0f,0.0f);


    while(!engine.shouldClose()){
        engine.beginFrame();
        engine.processInput();

        glm::mat4 viewProjection = engine.ViewProjection();
        float deltaTime = engine.getDeltaTime();

        
        for(unsigned int i=0; i<bodies.size();i++){
            bodies[i].DrawBody(shaders[i].ID, viewProjection);
        }
        /*
        updateGravity(bodies[Sun], bodies[Earth]);
        
        for(unsigned int j=1;j<bodies.size();j++){
            bodies[j].updateBodyPosition(deltaTime);
        }
        */

        //updatePositions(bodies, deltaTime);
        //glm::vec3 gravacc = gravity(bodies[Sun], bodies[Earth]);
        //std::cout << gravacc.x << "," << gravacc.y <<"," << gravacc.z << std::endl;
        engine.endFrame();
    }
    return 0;
}
