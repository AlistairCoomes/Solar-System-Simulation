#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include<SolarSystemHeader/SolarSystem.h>


//--------------------------------------------//
// Masses in Solar Masses                     //
// Distances in AU                            //
//--------------------------------------------//

std::vector<BodyID> toDraw = {Sun, Mercury, Venus, Earth, Mars, Jupiter, Saturn, Uranus, Neptune};

int main(){
    Engine engine(800,600,"Solar System Simulation", glm::vec3(0.0f,5.0f,5.0f));
    engine.Init();

    
    std::vector<Body> bodies;

    bodies.reserve(toDraw.size());

    for(int i = 0; i<toDraw.size();i++){
        bodies.emplace_back(toDraw[i]);
    }
    

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
            bodies[i].DrawBody(viewProjection);
            updateGravity(bodies[0], bodies[i]);
        }
        
        VerletIntegration(bodies, deltaTime);
        
        engine.endFrame();
    }
    return 0;
}
