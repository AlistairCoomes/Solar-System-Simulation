#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include<SolarSystemHeader/SolarSystem.h>

#include <iostream>

enum Bodies{
    Sun,
    //Mercury,
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
    Engine engine(800,600,"Solar System Simulation", glm::vec3(0.0f,5.0f,5.0f));
    engine.Init();

    
    std::vector<Body> bodies;
    std::vector<Shader> shaders;

    bodies.reserve(9);
    shaders.reserve(9);

    // Sun[0]
    bodies.emplace_back(glm::vec3(0.0f), 1.0f, 0.2f); 
    shaders.emplace_back("ShaderSources/shader.vs","ShaderSources/FragmentShaders/SunShader.fs");

    // Mercury[1] Something wrong verlet helps but still not circular
    //bodies.emplace_back(glm::vec3(0.387f,0.0f,0.0f), 0.000000166, 0.025f); 
    //shaders.emplace_back("ShaderSources/shader.vs","ShaderSources/FragmentShaders/EarthShader.fs");

    // Venus
    bodies.emplace_back(glm::vec3(0.723f,0.0f,0.0f), 0.000003003, 0.045f); 
    shaders.emplace_back("ShaderSources/shader.vs","ShaderSources/FragmentShaders/MarsShader.fs");

    // Earth
    bodies.emplace_back(glm::vec3(1.0f,0.0f,0.0f), 0.000003, 0.05f); 
    shaders.emplace_back("ShaderSources/shader.vs","ShaderSources/FragmentShaders/EarthShader.fs");

    // Mars
    bodies.emplace_back(glm::vec3(1.524f,0.0f,0.0f), 0.000000323, 0.035f);
    shaders.emplace_back("ShaderSources/shader.vs","ShaderSources/FragmentShaders/MarsShader.fs");

    // Jupiter
    bodies.emplace_back(glm::vec3(5.204f,0.0f,0.0f), 0.000955, 0.1f);
    shaders.emplace_back("ShaderSources/shader.vs","ShaderSources/FragmentShaders/JupiterShader.fs");

    // Saturn
    bodies.emplace_back(glm::vec3(9.582f,0.0f,0.0f), 0.000286, 0.085f);
    shaders.emplace_back("ShaderSources/shader.vs","ShaderSources/FragmentShaders/JupiterShader.fs");

    // Uranus
    bodies.emplace_back(glm::vec3(19.201f,0.0f,0.0f), 0.0000437, 0.07f);
    shaders.emplace_back("ShaderSources/shader.vs","ShaderSources/FragmentShaders/NeptuneShader.fs");

    // Neptune
    bodies.emplace_back(glm::vec3(30.047f,0.0f,0.0f), 0.0000515, 0.065f);
    shaders.emplace_back("ShaderSources/shader.vs","ShaderSources/FragmentShaders/NeptuneShader.fs");

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
            updateGravity(bodies[0], bodies[i]);
        }
        
        VerletIntegration(bodies, deltaTime);
        

        //std::cout << bodies[Mercury].Position.x << "," << bodies[Mercury].Position.y << "," << bodies[Mercury].Position.z << "\n";
        engine.endFrame();
    }
    return 0;
}
