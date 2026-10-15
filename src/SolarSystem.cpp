#include "SolarSystemHeader/SolarSystem.h"


glm::vec3 sphericalToCartesian(glm::vec3 pos){
    float r = pos.x;
	float theta = pos.y;
	float phi = pos.z;

	float x = r * sin(theta)*cos(phi); 
	float y = r*cos(theta); // in screen coords y is up
	float z = r*sin(theta)*sin(phi);
    return glm::vec3(x,y,z);
}


static Shader& getBodyShader(){
    static Shader shader("ShaderSources/bodyShader.vs",
                         "ShaderSources/bodyShader.fs");
    return shader;
}

static Shader& getLineShader(){
    static Shader shader("ShaderSources/lineShader.vs",
                         "ShaderSources/lineShader.fs");
    return shader;
}


const BodyInfo BodyLibrary[BodyCount] = {
    // Name     Starting Posistion              Mass            Radius      Colour
    {"Sun",     glm::vec3(0.0f),                1.0f,           0.2f,       {1.00f, 0.85f, 0.30f}},
    {"Mercury", glm::vec3(0.387f,0.0f,0.0f),    0.000000166,    0.025f,     {0.55f, 0.53f, 0.50f}},
    {"Venus",   glm::vec3(0.723f,0.0f,0.0f),    0.000003003,    0.045f,     {0.91f, 0.84f, 0.64f}},
    {"Earth",   glm::vec3(1.0f,0.0f,0.0f),      0.000003,       0.05f,      {0.24f, 0.48f, 0.71f}},
    {"Mars",    glm::vec3(1.524f,0.0f,0.0f),    0.000000323,    0.035f,     {0.71f, 0.28f, 0.11f}},
    {"Jupiter", glm::vec3(5.204f,0.0f,0.0f),    0.000955,       0.1f,       {0.78f, 0.55f, 0.29f}},
    {"Saturn",  glm::vec3(9.582f,0.0f,0.0f),    0.000286,       0.085f,     {0.83f, 0.71f, 0.51f}},
    {"Uranus",  glm::vec3(19.201f,0.0f,0.0f),   0.0000437,      0.07f,      {0.49f, 0.91f, 0.91f}},
    {"Neptune", glm::vec3(30.047f,0.0f,0.0f),   0.0000515,      0.065f,     {0.17f, 0.32f, 0.75f}},
};


Body::Body(BodyID id) : modelMatrix(glm::mat4(1.0f)),
info(BodyLibrary[id].name,BodyLibrary[id].startingPosition,BodyLibrary[id].mass, BodyLibrary[id].radius,BodyLibrary[id].color),
BodyShader(getBodyShader()), LineShader(getLineShader()) {

    Position = info.startingPosition;

    int rings = 16;
    int sectors = 32;
    float theta, phi;

    for(unsigned int i = 0;i<=rings;++i){
        for(unsigned int j = 0;j<=sectors;++j){
            theta = PI * (float)i / (float)rings;
            phi = 2 * PI * (float)j / (float)sectors;
            vertices.push_back(glm::vec3(info.radius, theta, phi));
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
    
  
    trailPoints.reserve(MaxTrailPoints);
    trailPoints.push_back(Position);

    // Trail buffers - allocated at max size, filled each frame in updateTrail()
    glGenVertexArrays(1, &VAO_Line);
    glGenBuffers(1, &VBO_Line);

    glBindVertexArray(VAO_Line);
    glBindBuffer(GL_ARRAY_BUFFER,VBO_Line);
    glBufferData(GL_ARRAY_BUFFER, MaxTrailPoints*sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
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



void Body::DrawBody(const glm::mat4& ViewProjection){

    modelMatrix = glm::translate(glm::mat4(1.0f), Position);

    glUseProgram(BodyShader.ID);

    BodyShader.setMat4("model", modelMatrix);
    BodyShader.setMat4("viewProjection", ViewProjection);
    BodyShader.setVec3("BodyColor", info.color);


    glBindVertexArray(VAO_body);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    

    updateTrail();

    // trail is the points so far plus the body's current position
    int pointCount = trailPoints.size() + 1;
    if(pointCount < 2) return;

    glUseProgram(LineShader.ID);

    LineShader.setMat4("model", glm::mat4(1.0f));
    LineShader.setMat4("viewProjection", ViewProjection);
    LineShader.setVec3("TrailColor", info.color);
    LineShader.setInt("PointCount", pointCount);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(VAO_Line);
    glDrawArrays(GL_LINE_STRIP, 0, pointCount);
    glBindVertexArray(0);

    glDisable(GL_BLEND);
}


void Body::updateTrail(){
    // bodies sitting at the origin (the Sun) don't orbit, so have no trail
    if(glm::length2(Position) < 0.0001f){
        trailPoints.clear();
        return;
    }

    // add a point once the body has swept TrailStep around the Sun since the last one
    if(trailPoints.empty() ||
       glm::dot(glm::normalize(trailPoints.back()), glm::normalize(Position)) < std::cos(TrailStep)){
        if(trailPoints.size() >= MaxTrailPoints - 1){ // leave room for the live point
            trailPoints.erase(trailPoints.begin());
        }
        trailPoints.push_back(Position);
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO_Line);
    glBufferSubData(GL_ARRAY_BUFFER, 0, trailPoints.size()*sizeof(glm::vec3), trailPoints.data());
    // final point follows the body so the trail stays attached to it
    glBufferSubData(GL_ARRAY_BUFFER, trailPoints.size()*sizeof(glm::vec3), sizeof(glm::vec3), &Position);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void VerletIntegration(std::vector<Body>& bodies, float deltaTime){
    for(int i=1; i<bodies.size();i++){
        bodies[i].Velocity += 0.5f * bodies[i].Acceleration * deltaTime;
        bodies[i].Position += bodies[i].Velocity * deltaTime;

        updateGravity(bodies[0], bodies[i]);

        bodies[i].Velocity += 0.5f * bodies[i].Acceleration * deltaTime;
    }
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


glm::vec3 gravity(Body& A, Body& B){ 
    float squaredDist = glm::distance2(A.Position, B.Position);
    if (squaredDist < 0.0001f) return glm::vec3(0.0f);
    float g = (G*A.info.mass) / squaredDist;
    glm::vec3 vector = glm::normalize(A.Position - B.Position);
    return g*vector;
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
    float velocity = std::sqrt((G*A.info.mass)/dist);
    return velocity * perpVec;
}

