//
// Created by 何昊 on 2018/05/02.
//

#include <iostream>
#include <algorithm>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <irrKlang/irrKlang.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include "assimp/Importer.hpp"

#include "Shader.h"
#include "Camera.h"
#include "Scene.h"
#include "BasicShapes.h"
#include "EnvironmentMap.h"
#include "ParticleEmitter.h"

int gScreenWidth = 1280;
int gScreenHeight = 720;

float gDeltaTime = 0.0f;
float gLastFrame = 0.0f;

bool gHideCursor = true;

Camera gCamera;
std::vector<GameObject*> gObjects;

// **********GLFW window related functions**********
// Returns pointer to a initialized window with OpenGL context set up
GLFWwindow *init();
// Sometimes user might resize the window. so the OpenGL viewport should be adjusted as well.
void frameBufferSizeCallback(GLFWwindow *window, int width, int height);
// User input is handled in this function
void processInput(GLFWwindow *window);
// Mouse input is handled in this function
void mouseCallback(GLFWwindow *window, double xpos, double ypos);
void scrollCallback(GLFWwindow *window, double offsetX, double offsetY);
// ********** ImGui Utilities **********
void imGuiInit(GLFWwindow *window);
void imGuiSetup(GLFWwindow *window);

// Core Render Function
void render(SphereSkybox &skybox, std::vector<GameObject*> &objects);

int main()
{
    GLFWwindow *window = init();
    if (window == nullptr) {
        std::cout << "Failed to initialize GLFW and OpenGL!" << std::endl;
        return -1;
    }

    imGuiInit(window);

    //irrklang::ISoundEngine *soundEngine = irrklang::createIrrKlangDevice();
    //soundEngine->play2D("resources/breakout.mp3", true);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);  
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    gCamera.Position = glm::vec3(0.0f, 0.0f, 10.0f);

    std::cout << "Loading Environment Map..." << std::endl;
    EnvironmentMap envMap("resources/Desert_Highway/Road_to_MonumentValley_Env.hdr");

    std::cout << "Loading Skybox..." << std::endl;
    SphereSkybox skybox("resources/Desert_Highway/Road_to_MonumentValley_8k.jpg");

    std::cout << "Loading Models..." << std::endl;
    Shader shader("shaders/PBR.vert", "shaders/PBR.frag");
    Shader particleShader("shaders/Particle.vert", "shaders/Particle.frag", "shaders/Particle.geom");
    Shader gunfireParticleShader("shaders/GunFireParticle.vert", "shaders/GunFireParticle.frag",
                                    "shaders/GunFireParticle.geom");

    Model ak47("resources/ak47.json");
    ak47.transform  = glm::scale(ak47.transform, glm::vec3(0.05f, 0.05f, 0.05f));
    ak47.shader     = shader;
    ak47.setEnvironmentData(envMap);
    ak47.smoothnessFactor = 0.55;
    gObjects.push_back(&ak47);
    std::cout << "Model AK47 Loaded Successfully" << std::endl;

    Model ak47Mag("resources/ak47_magazine.json");
    ak47Mag.transform  = glm::translate(ak47Mag.transform, glm::vec3(0.0f, -0.9f, 0.0f));
    ak47Mag.transform  = glm::rotate(ak47Mag.transform, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    ak47Mag.transform  = glm::scale(ak47Mag.transform, glm::vec3(0.05f, 0.05f, 0.05f));
    ak47Mag.shader     = shader;
    ak47Mag.setEnvironmentData(envMap);
    ak47Mag.smoothnessFactor = 0.55;
    gObjects.push_back(&ak47Mag);
    std::cout << "Model AK47 Magazine Loaded Successfully" << std::endl;

    TexturedQuad terrain("resources/sandy_ground.json");
    terrain.transform = glm::translate(terrain.transform, glm::vec3(0.0f, -5.0f, 0.0f));
    terrain.transform = glm::scale(terrain.transform, glm::vec3(30.0f, 30.0f, 30.0f));
    terrain.transform = glm::rotate(terrain.transform, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    terrain.shader    = shader;
    terrain.setEnvironmentData(envMap);
    gObjects.push_back(&terrain);

    SmokeParticleEmitter smokeEmitter("resources/ParticleCloudWhite.png", glm::vec3(0.0f, 0.0f, 5.0f));
    smokeEmitter.enabled = true;
    smokeEmitter.shader  = particleShader;
    smokeEmitter.transform = glm::translate(smokeEmitter.transform, glm::vec3(0.0f, -5.0f, 0.0f));
    gObjects.push_back(&smokeEmitter);

    GunFireParticleEmitter gunfireEmitter("resources/ParticleAtlas.png", 8, 8);
    gunfireEmitter.enabled = true;
    gunfireEmitter.transform = glm::translate(glm::mat4(1.0f), glm::vec3(-6.5, 0.4, 0.0));
    gunfireEmitter.shader = gunfireParticleShader;
    gObjects.push_back(&gunfireEmitter);

    envMap.brdfShader.use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Game loop
    while (!glfwWindowShouldClose(window)) {
        // Calculate how much time since last frame
        auto currentFrame = (float)glfwGetTime();
        gDeltaTime = currentFrame - gLastFrame;
        gLastFrame = currentFrame;

        processInput(window);

        imGuiSetup(window);

        static double lastTimeShot = 0.0;
        if (glfwGetTime() - lastTimeShot > 2.0) {
            gunfireEmitter.shootParticles(glm::vec3(-1.0, 0.0, 0.0));
            lastTimeShot = glfwGetTime();
        }

        render(skybox, gObjects);

        // Render GUI last
        ImGui::Render();
        if (!gHideCursor)
            ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //soundEngine->drop();
    glfwTerminate();
    return 0;
}

void render(SphereSkybox &skybox, std::vector<GameObject*> &objects) 
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set up view and projection matrix
    glm::mat4 view = gCamera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(gCamera.Zoom),
                                (float)gScreenWidth / gScreenHeight, 0.1f, 1000.0f);
    glm::mat4 vp = projection * view;

    skybox.render(view, projection, gCamera);

    for (auto object : objects) {
        object->update(gDeltaTime);
        object->render(vp, gCamera);
    }
}

void imGuiInit(GLFWwindow *window)
{
    // Setup ImGui binding
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
	ImGui_ImplGlfwGL3_Init(window, true);
	// Setup style
	ImGui::StyleColorsDark();
}

void imGuiSetup(GLFWwindow *window)
{
    struct Transform {
        float position[3];
        float rotation[3];
        float scale[3];
    };
    static Transform transforms[5];
    static bool rotateCamera = true;

    // This call is a must
    ImGui_ImplGlfwGL3_NewFrame();

    {
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                    1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        ImGui::Checkbox("Rotate Camera", &rotateCamera);

        if (ImGui::Button("Close Window")) {
            gHideCursor = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }

        if (ImGui::Button("Quit")) {
            glfwSetWindowShouldClose(window, true);
        }
    }

    if (rotateCamera) {
        double t = glfwGetTime();
        gCamera.Position = glm::vec3(20.0, 10.0, 20.0);
        // quaternions!
        double rotationAngle = t / 3;
        glm::quat rotation;
        rotation.x = 0.0;
        rotation.y = 1.0 * sin(rotationAngle / 2);
        rotation.z = 0.0;
        rotation.w = cos(rotationAngle / 2);
        gCamera.Position = glm::vec3(glm::inverse(rotation) * glm::vec4(gCamera.Position, 1.0) * rotation);

        //gCamera.Position = glm::vec3(20.0 * sin(t/3), 10.0, 20.0 * cos(t/3));
        gCamera.Up = glm::vec3(0.0, 1.0, 0.0);
        gCamera.Front = glm::normalize(-gCamera.Position);
        gCamera.Right = glm::cross(gCamera.Up, gCamera.Front);
    }

    // Update Transforms
    /*
    for (int i = 0; i < 5; ++i) {
        glm::mat4 trans = glm::mat4(1.0);
        trans = glm::translate(trans, glm::vec3(transforms[i].position[0],
                                                transforms[i].position[1],
                                                transforms[i].position[2]));
        trans = glm::rotate(trans, glm::radians(transforms[i].rotation[2]), glm::vec3(0.0, 0.0, 1.0));
        trans = glm::rotate(trans, glm::radians(transforms[i].rotation[1]), glm::vec3(0.0, 1.0, 0.0));
        trans = glm::rotate(trans, glm::radians(transforms[i].rotation[0]), glm::vec3(1.0, 0.0, 0.0));
        trans = glm::scale(trans, glm::vec3(transforms[i].scale[0],
                                            transforms[i].scale[1],
                                            transforms[i].scale[2]));
        gObjects[i]->transform = trans;
    } */
}

GLFWwindow *init()
{
    // Initialization of GLFW context
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Something needed for Mac OS X
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create a window object
    GLFWwindow *window = glfwCreateWindow(gScreenWidth, gScreenHeight, "Window Title", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);

    // Initialize GLAD before calling OpenGL functions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }

    // Tell OpenGL the size of rendering window
    glViewport(0, 0, gScreenWidth * 2, gScreenHeight * 2);

    // Set the windows resize callback function
    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);

    // Set up mouse input
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);

    return window;
}

void frameBufferSizeCallback(GLFWwindow *window, int width, int height)
{
    gScreenWidth = width;
    gScreenHeight = height;
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    // Handle camera movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        gCamera.ProcessKeyboard(FORWARD, gDeltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        gCamera.ProcessKeyboard(LEFT, gDeltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        gCamera.ProcessKeyboard(RIGHT, gDeltaTime);
    }


    // add this to avoid repetitive pressing
    static double timeLastPressed = 0.0;
    if (glfwGetTime() - timeLastPressed < 0.1)
        return;
    timeLastPressed = glfwGetTime();

    // Exit
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        timeLastPressed = glfwGetTime();
        gHideCursor = !gHideCursor;
        if (gHideCursor) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}

void mouseCallback(GLFWwindow *window, double xpos, double ypos)
{
    // Do nothing if the menu is open
    if (!gHideCursor) return;

    // Variables needed to handle mouse input
    static float lastMouseX = 400.0f;
    static float lastMouseY = 300.0f;
    static bool firstMouse = true;

    if (firstMouse) {
        lastMouseX = (float)xpos;
        lastMouseY = (float)ypos;
        firstMouse = false;
    }

    // Calculate mouse movement since last frame
    float offsetX = (float)xpos - lastMouseX;
    float offsetY = (float)ypos - lastMouseY;
    lastMouseX = (float)xpos;
    lastMouseY = (float)ypos;

    gCamera.ProcessMouseMovement(offsetX, offsetY);
}

void scrollCallback(GLFWwindow *window, double offsetX, double offsetY)
{
    gCamera.ProcessMouseScroll((float)offsetY);
}
