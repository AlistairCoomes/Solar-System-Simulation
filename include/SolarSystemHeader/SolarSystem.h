#ifndef SOLARSYSTEM_H
#define SOLARSYSTEM_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <3DLibraries/camera.h>
#include <3DLibraries/shader.h>
#include <3DLibraries/engine.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

enum BodyID{
    Sun,
    Mercury,
    Venus,
    Earth,
    Mars,
    Jupiter,
    Saturn,
    Uranus,
    Neptune,
    BodyCount
};

struct BodyInfo {
    const std::string name;
    glm::vec3 startingPosition;
    float mass;
    float radius;
    glm::vec3 color;
};

extern const BodyInfo BodyLibrary[BodyCount];


const float PI = glm::pi<float>();
const float G = 39.478;


class Body{
    public:
    glm::mat4 modelMatrix;
    std::vector<glm::vec3> trailPoints; // oldest first, cartesian

    glm::vec3 Position = glm::vec3(0.0f);
    glm::vec3 Velocity = glm::vec3(0.0f);
    glm::vec3 Acceleration = glm::vec3(0.0f);

    BodyInfo info;

    unsigned int VBO_body, VAO_body, EBO_body;
    unsigned int indexCount = 0;

    unsigned int VBO_Line, VAO_Line;
    static const int MaxTrailPoints = 180;          // trail covers MaxTrailPoints * TrailStep of orbit
    const float TrailStep = glm::radians(1.0f);     // angle swept around the Sun between trail points

    Shader& BodyShader;
    Shader& LineShader;


    // constructor
    Body(BodyID id);

    // destructor
    ~Body();


    glm::vec3 CartToSphPosition(const glm::vec3 &cartesian);

    // draws the body
    void DrawBody(const glm::mat4& ViewProjection);


    

    private:
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;

    // records the current position into the trail and uploads it to VBO_Line
    void updateTrail();

};


glm::vec3 sphericalToCartesian(glm::vec3 pos);

glm::vec3 gravity(Body& A, Body& B); // B -> A vector Direction, cartesian vector, gets the gravitational attraction of A on B 

void updateGravity(Body& A, Body& B);

glm::vec3 initalVelocity(Body& A, Body& B); // creates a velocity orthogonal to gravitational attraction from sun A = Sun

void VerletIntegration(std::vector<Body>& bodies, float deltaTime);


#endif SOLARSYSTEM_H