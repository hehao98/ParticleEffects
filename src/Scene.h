/*
 * A class to load models using Assimp
 * Created by He Hao at 2018/4/23
 */

#ifndef SCENE_H
#define SCENE_H


#include <string>
#include <vector>

#include <glm/gtx/projection.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform2.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include <assimp/Importer.hpp>	//OO version Header!

#include "AssimpUtilities.hpp"
#include "GameObject.h"
#include "GL_Constants.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"

class Model : public PbrGameObject
{
public:
    // All the resources loaded in this model will be deallocated
    // in this importer's destructor 
    Assimp::Importer importer;

    // All the model data stores in the scene
    const aiScene *scene;

    // An array to store VAO indices for each mesh
    std::vector<unsigned int> vaoArray;

    // Load model config info from json
    explicit Model(const char *jsonFile);

    void loadModelFromAssimp(const char *file);

    void render(const glm::mat4 &vp, Camera &camera) override;

    void traverseRender(const aiNode* node, const glm::mat4 &vp, glm::mat4 &m,
                               Camera &camera);

    void printInfo()
    { printAiSceneInfo(scene); }
private:
};

// A Scene contains multiple models
class Scene : public GameObject
{
public:
    void render(const glm::mat4 &vp, Camera &camera) override {}
};


#endif