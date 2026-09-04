#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include<SolarSystemHeader/SolarSystem.h>

#include <iostream>

enum Bodies{
    Sun,
    Mercury,
    Venus,
    Earth,
    Mars,
    Jupiter,
    Saturn,
    Uranus,
    Neptune
};

//--------------------------------------------//
// Masses in Solar Masses                     //
// Distances in AU                            //
// T = 1 year                                 //
//--------------------------------------------//

int main(){
    Engine engine(800,600,"Solar System Simulation", glm::vec3(0.0f,0.0f,40.0f));
    engine.Init();

    
    std::vector<Body> bodies;
    std::vector<Shader> shaders;

    bodies.reserve(9);
    shaders.reserve(9);

    // Sun[0]
    bodies.emplace_back(glm::vec3(0.0f), 1.0f, 0.2f); 
    shaders.emplace_back("ShaderSources/shader.vs","ShaderSources/FragmentShaders/SunShader.fs");

    // Mercury[1] Something wrong
    bodies.emplace_back(glm::vec3(0.387f,0.0f,0.0f), 0.000000166f, 0.025f); 
    shaders.emplace_back("ShaderSources/shader.vs","ShaderSources/FragmentShaders/EarthShader.fs");

    // Venus[2]
    bodies.emplace_back(glm::vec3(0.723f,0.0f,0.0f), 0.000003003f, 0.045f); 
    shaders.emplace_back("ShaderSources/shader.vs","ShaderSources/FragmentShaders/MarsShader.fs");

    // Earth[3]
    bodies.emplace_back(glm::vec3(1.0f,0.0f,0.0f), 0.000003f, 0.05f); 
    shaders.emplace_back("ShaderSources/shader.vs","ShaderSources/FragmentShaders/EarthShader.fs");

    // Mars[4]
    bodies.emplace_back(glm::vec3(1.524f,0.0f,0.0f), 0.000000323f, 0.035f);
    shaders.emplace_back("ShaderSources/shader.vs","ShaderSources/FragmentShaders/MarsShader.fs");

    // Jupiter[5]
    bodies.emplace_back(glm::vec3(5.204f,0.0f,0.0f), 0.000955f, 0.1f);
    shaders.emplace_back("ShaderSources/shader.vs","ShaderSources/FragmentShaders/MarsShader.fs");

    // Saturn[6]
    bodies.emplace_back(glm::vec3(9.582f,0.0f,0.0f), 0.000286f, 0.085f);
    shaders.emplace_back("ShaderSources/shader.vs","ShaderSources/FragmentShaders/MarsShader.fs");

    // Uranus[7]
    bodies.emplace_back(glm::vec3(19.201f,0.0f,0.0f), 0.0000437f, 0.07f);
    shaders.emplace_back("ShaderSources/shader.vs","ShaderSources/FragmentShaders/MarsShader.fs");

    // Neptune[8]
    bodies.emplace_back(glm::vec3(30.047f,0.0f,0.0f), 0.0000515f, 0.065f);
    shaders.emplace_back("ShaderSources/shader.vs","ShaderSources/FragmentShaders/MarsShader.fs");

    // Initial Velocities
    for(int i=1;i<bodies.size();i++){
        bodies[i].Velocity = initalVelocity(bodies[Sun], bodies[i]);
    }


    while(!engine.shouldClose()){
        engine.beginFrame();
        engine.processInput();

        glm::mat4 viewProjection = engine.ViewProjection();
        float deltaTime = glm::min(engine.getDeltaTime(), 0.05f);

        
        for(int i=0; i<bodies.size();i++){
            bodies[i].DrawBody(shaders[i].ID, viewProjection);
        }
        
        for(int i=1; i<bodies.size();i++){
            updateGravity(bodies[Sun], bodies[i]);
        }

        for(int j=0;j<bodies.size();j++){
            bodies[j].updateBodyPosition(deltaTime);
        }
        

        //updatePositions(bodies, deltaTime);
        //glm::vec3 gravacc = gravity(bodies[Sun], bodies[Earth]);
        //std::cout << gravacc.x << "," << gravacc.y <<"," << gravacc.z << "\n";
        engine.endFrame();
    }
    return 0;
}
