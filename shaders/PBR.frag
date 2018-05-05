#version 330 core

out vec4 fragColor;

in vec4 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in mat3 TBNMatrix;
in vec3 TangentCamPos;
in vec3 TangentLightPos;
in vec3 TangentFragPos;

// Material Parameters
// albedo is the material's ambient color
uniform sampler2D albedoMap;
uniform bool albedoIsSRGB;

uniform sampler2D normalMap;
uniform bool normalIsSRGB;

// metallic parameter in channel red
// smoothness parameter in channel alpha
uniform sampler2D metallicSmoothnessMap;
uniform bool metallicSmoothnessIsSRGB;

// Ambient Occulusion
uniform sampler2D aoMap;
uniform bool hasAO;
uniform bool aoIsSRGB;

// height map for parallax mapping
uniform sampler2D heightMap;
uniform bool hasHeightMap;

// smoothness will be multiplied by this factor
// to avoid all 1 situation
uniform float smoothnessFactor;

// Precaculated environment maps
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D   brdfLUT;  

// lights
uniform int  lightCount;
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);

vec3 normalMapping(vec2 texCoord);
vec2 parallaxMapping(vec2 texCoord, vec3 viewDir);

void main()
{
    // ********** Get Necessary Data From Textures **********
    vec2 texCoord = TexCoord;
    if (hasHeightMap) {
        texCoord = parallaxMapping(TexCoord, normalize(TangentCamPos - TangentFragPos.xyz));
        if(texCoord.x > 1.0 || texCoord.y > 1.0 || texCoord.x < 0.0 || texCoord.y < 0.0)
            discard;
    }

    vec3 albedo = texture(albedoMap, texCoord).rgb;
    if (albedoIsSRGB) { // The lighting must be done in linear space
        albedo.r = pow(albedo.r, 2.2);
        albedo.g = pow(albedo.g, 2.2);
        albedo.b = pow(albedo.b, 2.2);
    }

    vec3 normal = normalMapping(texCoord);

    vec4 metallicSmoothness = texture(metallicSmoothnessMap, texCoord);
    if (metallicSmoothnessIsSRGB) {
        metallicSmoothness.r = pow(metallicSmoothness.r, 2.2);
        metallicSmoothness.a = pow(metallicSmoothness.a, 2.2);
    }
    float metallic  = metallicSmoothness.r;
    float roughness = 1 - smoothnessFactor * metallicSmoothness.a;

    float ao = texture(aoMap, texCoord).r;
    if (aoIsSRGB) {
        ao = pow(ao, 2.2);
    }

    // ********** Do the Lighting **********
    vec3 N = normalize(normal);
    vec3 V = normalize(TangentCamPos - TangentFragPos.xyz);
    vec3 R = reflect(-V, N);   
 
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
	           
    // reflectance equation
    vec3 Lo = vec3(0.0);

    for(int i = 0; i < lightCount; ++i) {
        // Transform Light Positon to Tangent Space
        vec3 lightPos = TBNMatrix * lightPositions[i];

        // calculate per-light radiance
        vec3 L = normalize(lightPos - TangentFragPos.xyz);
        if (i == 0) L = normalize(lightPos); // Directional Light
        vec3 H = normalize(V + L);
        float distance    = length(lightPos - TangentFragPos.xyz);
        float attenuation = 1.0 / (distance * distance);
        if (i == 0) attenuation = 1;         // Directional Light
        vec3 radiance     = lightColors[i] * attenuation;       
        
        // cook-torrance brdf
        float NDF = DistributionGGX(N, H, roughness);        
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);       
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;	  
        
        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
        vec3 specular     = numerator / denominator;  
            
        // add to outgoing radiance Lo
        float NdotL = max(dot(N, L), 0.0);                
        Lo += (kD * albedo / PI + specular) * radiance * NdotL; 
    }   
  
    // calculate diffuse from irradiance map
    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	  
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse = irradiance * albedo;

    // calculate specular from prefilter map and BRDF LUT map
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = 1.0 * textureLod(prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    vec3 ambient = (kD * diffuse + specular);
    if (hasAO) ambient = (kD * diffuse + specular) * ao;

	vec3 color = ambient + Lo;

    // map HDR to LDR using tone mapping
    color = color / (color + vec3(1.0));
    // gamma correction
    color = pow(color, vec3(1.0/2.2));
    
    fragColor = vec4(color, 1.0);
}  

// Normal Distribution Function used in BRDF
// uses the Trowbridge-Reitz GGX normal distribution
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

// GeometrySmith helper function
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
  
// The Geometry function used in BRDF
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

// An approximation of the fresnel equation
// cosTheta = normal dot view
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}   

vec3 normalMapping(vec2 texCoord)
{
    // Lighting is already done in tangent space
    vec3 n = texture(normalMap, texCoord).xyz;
    if (normalIsSRGB) {
        n.x = pow(n.x, 2.2);
        n.y = pow(n.y, 2.2);
        n.z = pow(n.z, 2.2);
    }
    return normalize(n * 2.0 - 1.0);
}

// Parallax occulusion maping
vec2 parallaxMapping(vec2 texCoord, vec3 viewDir)
{
    float height_scale = 0.05;
    const float numLayers = 10;
    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;
    vec2 P = viewDir.xy * height_scale; 
    vec2 deltaTexCoord = P / numLayers;

    // get initial values
    vec2  currentTexCoord      = texCoord;
    float currentDepthMapValue = 1 - texture(heightMap, currentTexCoord).r;
    
    while(currentLayerDepth < currentDepthMapValue)
    {
        currentTexCoord -= deltaTexCoord;
        currentDepthMapValue = 1 - texture(heightMap, currentTexCoord).r;  
        currentLayerDepth += layerDepth;  
    }

    vec2 prevTexCoord = currentTexCoord + deltaTexCoord;

    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = (1 - texture(heightMap, prevTexCoord).r) - currentLayerDepth + layerDepth;
 
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoord = prevTexCoord * weight + currentTexCoord * (1.0 - weight);

    return finalTexCoord;
}
