#pragma once

#include <glm/glm.hpp>
#include "glm/ext.hpp"

class Camera{

public:
    glm::vec3 cameraPosition;
    glm::mat3 cameraOrientation;
    float focalLength;
    float sideMovement;
    float rotationMovement;

private:

public:
    Camera();
    Camera(glm::vec3 cameraPosition, glm::mat3 cameraOrientation, float focalLength, float sideMovement = 0.05, float rotationMovement = 0.01);
    void setCameraPosition(glm::vec3 cameraPosition);
    void setCameraOrientations(glm::mat3 cameraOrientation);
    void setFocalLength(float focalLength);
    glm::vec3 getCameraPosition();
    glm::mat3 getCameraOrientation();
    float getFocalLength();
    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();
    void moveBackward();
    void moveForward();
    void rotateAboutX();
    void rotateAboutY();
    void rotateAboutZ();
    void rotateOrientationAboutX(int inverse = 1);
    void rotateOrientationAboutY(int inverse = 1);
    void rotateOrientationAboutZ();
    void lookAt(glm::vec3 pointToLookAt);
    void orbit(glm::vec3 pointToLookAt);

};