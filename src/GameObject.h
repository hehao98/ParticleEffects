// abstract class that tag all renderable types
#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <glm/gtx/projection.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform2.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include <json.hpp>

#include "Camera.h"
#include "Shader.h"
#include "Texture.h"
#include "EnvironmentMap.h"

class GameObject 
{
public:
    glm::mat4 transform;

    Shader shader;

    GameObject() { transform = glm::mat4(1.0f); }

    // All GameObjects MUST implement their own render function
    virtual void render(const glm::mat4 &vp, Camera &camera) = 0;

    // Some GameObjects can implement update function that are called each frame
    virtual void update(float dt) {}
};

class PbrGameObject : public GameObject
{
public:
    // Materials
    Texture albedo;
    bool albedoIsSRGB;

	Texture metallicSmoothness;
    bool metallicSmoothnessIsSRGB;
    float smoothnessFactor;

	Texture normal;
    bool normalIsSRGB;

	bool hasAO;
    Texture ao;
    bool aoIsSRGB;

    bool hasHeightMap;
    Texture heightMap;
    float heightMapScale;
    bool heightMapIsSRGB;

	// The following three are cubemap textures
    // used for environment mapping
    // MUST be manually set before rendering call
	unsigned int irradiance;
	unsigned int prefilter;
	unsigned int brdfLUT;

    // light[0] is directional light
    // need to be manually set in order to function
    int lightCount;
    glm::vec3 lightPositions[4];
    glm::vec3 lightColors[4];

    PbrGameObject(const char *jsonFile)
    {
        nlohmann::json j;

        std::ifstream inFile(jsonFile);
        if (inFile.good()) {
            inFile >> j;
        } else {
            std::cerr << "Failed to load json file " << jsonFile << std::endl;
        }

        albedo                   = Texture(j["albedo_map_path"].get<std::string>().c_str());
        albedoIsSRGB             = j["albedo_map_is_srgb"].get<bool>();
        metallicSmoothness       = Texture(j["metallic_smoothness_map_path"].get<std::string>().c_str());
        metallicSmoothnessIsSRGB = j["metallic_smoothness_map_is_srgb"].get<bool>();
        normal                   = Texture(j["normal_map_path"].get<std::string>().c_str());
        normalIsSRGB             = j["normal_map_is_srgb"].get<bool>();
        hasAO                    = j["has_ao"].get<bool>();
        ao                       = Texture(j["ao_map_path"].get<std::string>().c_str());
        aoIsSRGB                 = j["ao_map_is_srgb"].get<bool>();
        hasHeightMap             = j["has_height_map"].get<bool>();
        heightMap                = Texture(j["height_map_path"].get<std::string>().c_str());
        heightMapScale           = j["height_map_scale"].get<float>();
        heightMapIsSRGB          = j["height_map_is_srgb"].get<bool>();

        smoothnessFactor = 1.0;
        irradiance = 0;
	    prefilter  = 0;
        brdfLUT    = 0;
        lightCount = 0;
    }

    void setEnvironmentData(EnvironmentMap &envMap)
    {
        irradiance = envMap.envCubemap;
        prefilter  = envMap.prefilterMap;
        brdfLUT    = envMap.brdfLUT;
    }
};

#endif