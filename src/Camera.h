/*
 * Simple FPS Camera implementled using quaternions
 * 
 * Created by He Hao at 2018/5/1
 */ 

#ifndef PROJECT_CAMERA_H
#define PROJECT_CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <vector>

// Defines several possible options for camera movement.
// Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float SPEED      =  2.5f;
const float SENSITIVTY =  0.1f;
const float ZOOM       =  45.0f;

// An abstract camera class that processes input and calculates
// the corresponding Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // Camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // Camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 up       = glm::vec3(0.0f, 1.0f, 0.0f),
           glm::vec3 front    = glm::vec3(0.0f, 0.0f, -1.0f))
            : Front(front),
              MovementSpeed(SPEED),
              MouseSensitivity(SENSITIVTY),
              Zoom(ZOOM)
    {
        Position = position;
        WorldUp  = up;
        Up       = up;
        Right    = glm::cross(Front, Up);
    }

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }
 
    // Processes input received from any keyboard-like input system.
    // Accepts input parameter in the form of camera defined ENUM
    // (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }

    // Processes input received from a mouse input system.
    // Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        // For xoffset, the rotation vector is Up vector,
        // the rotation angle is xoffset(in degrees)
        glm::quat rotationX;
        float rotationAngleX = glm::radians(xoffset);
        rotationX.x = Up.x * sin(rotationAngleX / 2);
        rotationX.y = Up.y * sin(rotationAngleX / 2);
        rotationX.z = Up.z * sin(rotationAngleX / 2);
        rotationX.w = cos(rotationAngleX / 2);

        // Then calculate rotation on y axis
        float rotationAngleY = glm::radians(yoffset);
        glm::quat rotationY;
        rotationY.x = Right.x * sin(rotationAngleY / 2);
        rotationY.y = Right.y * sin(rotationAngleY / 2);
        rotationY.z = Right.z * sin(rotationAngleY / 2);
        rotationY.w = cos(rotationAngleY / 2);

        glm::quat rotation = rotationX * rotationY;

        Up    = glm::inverse(rotation) * glm::vec4(Up, 0.0) * rotation;
        Front = glm::inverse(rotation) * glm::vec4(Front, 0.0) * rotation;
        Up    = glm::normalize(Up);
        Front = glm::normalize(Front); 
        Right = glm::normalize(glm::cross(Front, Up));
    }

    // Processes input received from a mouse scroll-wheel event.
    // Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        if (Zoom >= 1.0f && Zoom <= 45.0f)
            Zoom -= yoffset;
        if (Zoom <= 1.0f)
            Zoom = 1.0f;
        if (Zoom >= 45.0f)
            Zoom = 45.0f;
    }
};

#endif //PROJECT_CAMERA_H
