/*
 * Simple Particle Emitter for generating particle effects
 *
 * Created by He Hao in 2018/05/02
 */  

#ifndef PARTICLE_EMITTER_H
#define PARTICLE_EMITTER_H

#include <vector>

#include <glm/glm.hpp>

#include "GameObject.h"
#include "Texture.h"

struct Particle 
{
    glm::vec3 position;
    glm::vec3 velocity;
    float lifetime;
    float alpha;
    bool  alive;
};

class ParticleEmitter : public GameObject
{
public:
    bool enabled;

    int maxParticles;

    std::vector<Particle> particles;

    ParticleEmitter() { enabled = false; }

    void update(float dt) override {}
};

class SmokeParticleEmitter : public ParticleEmitter
{
public:
    glm::vec3 windDir;

    unsigned int vao, posBuffer;

    Texture texture;

    SmokeParticleEmitter(const char *smokeTexturePath, glm::vec3 wind);

    void update(float dt) override;

    void render(const glm::mat4 &vp, Camera &camera) override;

    int findUnusedParticle();
};

class GunFireParticleEmitter : public ParticleEmitter
{
public:
    unsigned int vao, posBuffer, elapsedTimeBuffer;

    Texture sprite;
    int row, column; // How many rows and columns the sprite have

    GunFireParticleEmitter(const char *gunFireTexturePath, int r, int c);

    void shootParticles(glm::vec3 shootDir);

    void update(float dt) override;

    void render(const glm::mat4 &vp, Camera &camera) override;
};


#endif