
#include "ParticleEmitter.h"

#include "GL_Constants.h"
#include <glad/glad.h>
#include <map>

static unsigned int quadVAO, quadVBO;
static const float quadVertices[] = {
            // Positions    // Normals        // Texture coordinates
        -0.5f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
         0.5f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, 0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
         0.5f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
         0.5f, 0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    };

static void initQuadVaoAndVbo()
{
    // init only once
    static bool flag = false;
    if (flag) return;
    flag = true;

    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(VertexAttribLocations::vPos);
    glVertexAttribPointer(VertexAttribLocations::vPos, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(VertexAttribLocations::vNormal);
    glVertexAttribPointer(VertexAttribLocations::vNormal, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(VertexAttribLocations::vTexCoord);
    glVertexAttribPointer(VertexAttribLocations::vTexCoord, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
}

SmokeParticleEmitter::SmokeParticleEmitter(const char *smokeTexturePath, glm::vec3 wind)
    : texture(smokeTexturePath), windDir(wind)
{
    maxParticles = 1000;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &posBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, posBuffer);
    glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(glm::vec3), nullptr, GL_STREAM_DRAW);
}

void SmokeParticleEmitter::update(float dt)
{
    if (!enabled) return;

    for (int i = 0; i < 3; ++i) {
        glm::vec3 offset;
        offset.x = ((rand() % 1000) / 1000.0f) * 4;
        offset.y = ((rand() % 1000) / 1000.0f) * 4;
        offset.z = ((rand() % 1000) / 1000.0f) * 4;

        Particle p;
        p.alive     = true;
        p.lifetime  = 1.0f;
        p.alpha     = 1.0f;
        p.position  = glm::vec3(this->transform[3][0], this->transform[3][1], this->transform[3][2]);
        p.velocity  = windDir + offset + glm::vec3(0.0, 5.0, 0.0);
        
        if (particles.size() < maxParticles) {
            particles.push_back(p);
        } else {
            int index = findUnusedParticle();
            particles[index] = p;
        }
    }

    // Update particle positions and alive data
    for (auto &p : particles) {
        if (!p.alive) continue;
        p.alpha -= dt / p.lifetime;
        p.lifetime -= dt;
        if (p.lifetime <= 0) p.alive = false;
        p.position += p.velocity * dt;
    }
}

void SmokeParticleEmitter::render(const glm::mat4 &vp, Camera &camera)
{
    if (!enabled) return;

    // Render all the living particles
    glm::vec3 *living = new glm::vec3[maxParticles];
    int size = 0;
    for (auto &p : particles) {
        if (p.alive) {
            living[size++] = p.position;
        }
    }
    if (size <= 0) return;

    // Sort all the particles according to distance from camera
    std::map<float, glm::vec3> sortedLiving;
    for (int i = 0; i < size; ++i) {
        glm::vec3 pos = living[i];
        float disToCam = glm::length(pos - camera.Position);
        sortedLiving.insert(std::make_pair(disToCam, living[i]));
    }
    size = 0;
    for (auto it = sortedLiving.rbegin(); it != sortedLiving.rend(); ++it) {
        living[size++] = it->second;
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    shader.use();
    shader.setMat4("vp", vp);
    shader.setVec3("camPos", camera.Position);
    shader.setInt("sprite", TextureChannel::sprite);
    texture.useTextureUnit(TextureChannel::sprite);

    // Config instanced array
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, posBuffer);
    glBufferData(GL_ARRAY_BUFFER, size * sizeof(glm::vec3), &living[0], GL_STREAM_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glVertexAttribDivisor(0, 1);

    glDrawArraysInstanced(GL_POINTS, 0, 1, size);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    delete[] living;
}

int SmokeParticleEmitter::findUnusedParticle() {
    static int lastUsedParticle = 0;
    for(int i = lastUsedParticle; i < maxParticles; i++){
        if (!particles[i].alive){
            lastUsedParticle = i;
            return i;
        }
    }

    for(int i = 0; i< lastUsedParticle; i++){
        if (!particles[i].alive){
            lastUsedParticle = i;
            return i;
        }
    }

    return 0; // All particles are taken, override the first one
}

GunFireParticleEmitter::GunFireParticleEmitter(const char *gunFireTexturePath, int r, int c)
    : sprite(gunFireTexturePath), row(r), column(c)
{
    maxParticles = 20;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &posBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, posBuffer);
    glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(glm::vec3), nullptr, GL_STREAM_DRAW);
    glGenBuffers(1, &elapsedTimeBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, elapsedTimeBuffer);
    glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(float), nullptr, GL_STREAM_DRAW);
}

void GunFireParticleEmitter::shootParticles(glm::vec3 shootDir)
{
    particles.clear();
    for (int i = 0; i < maxParticles; ++i) {
        glm::vec3 offset;
        offset.x = ((rand() % 1000) / 1000.0f) ;
        offset.y = ((rand() % 1000) / 1000.0f) ;
        offset.z = ((rand() % 1000) / 1000.0f) ;

        Particle p;
        p.alive = true;
        p.lifetime = 0.5;
        p.velocity = glm::normalize(shootDir) * 1.0f + offset;
        p.position = glm::vec3(this->transform[3][0], this->transform[3][1], this->transform[3][2]);
        particles.push_back(p);
    }
}

void GunFireParticleEmitter::update(float dt)
{
    if (!enabled) return;
    for (auto &p : particles) {
        if (!p.alive) continue;
        p.alpha -= dt / p.lifetime;
        p.lifetime -= dt;
        if (p.lifetime <= 0) p.alive = false;
        p.position += p.velocity * dt;
    }
}

void GunFireParticleEmitter::render(const glm::mat4 &vp, Camera &camera)
{
    if (!enabled) return;

    // Render all the living particles
    Particle *living = new Particle[maxParticles];
    float *posBufferData = new float[maxParticles * 3];
    float *timeBufferData = new float[maxParticles];
    int size = 0;
    for (auto &p : particles) {
        if (p.alive) {
            living[size++] = p;
        }
    }
    if (size <= 0) return;

    // Sort all the particles according to distance from camera
    std::map<float, Particle> sortedLiving;
    for (int i = 0; i < size; ++i) {
        glm::vec3 pos = living[i].position;
        float disToCam = glm::length(pos - camera.Position);
        sortedLiving.insert(std::make_pair(disToCam, living[i]));
    }
    size = 0;
    for (auto it = sortedLiving.rbegin(); it != sortedLiving.rend(); ++it) {
        posBufferData[3 * size + 0] = it->second.position.x;
        posBufferData[3 * size + 1] = it->second.position.y;
        posBufferData[3 * size + 2] = it->second.position.z;
        timeBufferData[size] = 0.5 - it->second.lifetime;
        size++;
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    shader.use();
    shader.setMat4("vp", vp);
    shader.setVec3("camPos", camera.Position);
    shader.setFloat("lifetime", 0.5);
    shader.setInt("sprite", TextureChannel::sprite);
    sprite.useTextureUnit(TextureChannel::sprite);
    shader.setInt("spriteRow", row);
    shader.setInt("spriteColumn", column);

    // Config instanced array
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, posBuffer);
    glBufferData(GL_ARRAY_BUFFER, size * sizeof(glm::vec3), posBufferData, GL_STREAM_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glVertexAttribDivisor(0, 1);

    glBindBuffer(GL_ARRAY_BUFFER, elapsedTimeBuffer);
    glBufferData(GL_ARRAY_BUFFER, size * sizeof(float), timeBufferData, GL_STREAM_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
    glVertexAttribDivisor(1, 1);

    glDrawArraysInstanced(GL_POINTS, 0, 1, size);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    delete[] living;
    delete[] posBufferData;
    delete[] timeBufferData;
}
