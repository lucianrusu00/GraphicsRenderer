#include <iostream>
#include "Camera.h"
#include "glm/ext.hpp"
#include "glm/gtx/io.hpp"

Camera::Camera() {
    cameraPosition = glm::vec3(0,0,4);

    focalLength = 1100;

    cameraOrientation = glm::mat3(glm::vec3(1.0,0,0),
                                            glm::vec3(0,1.0,0),
                                            glm::vec3(0,0,1.0));

    sideMovement = 0.05;
    rotationMovement = 0.05; // 0.01
}

Camera::Camera(glm::vec3 cameraPosition1, glm::mat3 cameraOrientation1, float focalLength1, float sideMovement1, float rotationMovement1) {
    cameraPosition = cameraPosition1;
    cameraOrientation = cameraOrientation1;
    focalLength = focalLength1;
    sideMovement = sideMovement1;
    rotationMovement = rotationMovement1;
}

void Camera::setCameraPosition(glm::vec3 cameraPosition1) {
    cameraPosition = cameraPosition1;
}

void Camera::setCameraOrientations(glm::mat3 cameraOrientation1) {
    cameraOrientation = cameraOrientation1;
}

void Camera::setFocalLength(float focalLength1) {
    focalLength = focalLength1;
}

glm::vec3 Camera::getCameraPosition() {
    return cameraPosition;
}

glm::mat3 Camera::getCameraOrientation() {
    return cameraOrientation;
}

float Camera::getFocalLength() {
    return focalLength;
}


void Camera::moveLeft() {
    cameraPosition[0] -= sideMovement;
}

void Camera::moveRight() {
    cameraPosition[0] += sideMovement;
}

void Camera::moveUp() {
    cameraPosition[1] += sideMovement;
}

void Camera::moveDown() {
    cameraPosition[1] -= sideMovement;
}

void Camera::moveForward() {
    cameraPosition[2] += sideMovement;
}

void Camera::moveBackward() {
    cameraPosition[2] -= sideMovement;
}

void Camera::rotateAboutX() {
    float theta = rotationMovement;

    float cosTheta = cos(theta);
    float sinTheta = sin(theta);

    glm::vec3 firstColumn = glm::vec3(1,0,0);
    glm::vec3 secondColumn = glm::vec3(0, cosTheta, sinTheta);
    glm::vec3 thirdColumn = glm::vec3(0,-sinTheta, cosTheta);

    glm::mat3 rotMat = glm::mat3(firstColumn,secondColumn,thirdColumn);

    cameraPosition = cameraPosition * rotMat;
}

void Camera::rotateAboutY() {
    float theta = rotationMovement;

    float cosTheta = cos(theta);
    float sinTheta = sin(theta);

    glm::vec3 firstColumn = glm::vec3(cosTheta,0,-sinTheta);
    glm::vec3 secondColumn = glm::vec3(0,1,0);
    glm::vec3 thirdColumn = glm::vec3(sinTheta,0,cosTheta);

    glm::mat3 rotMat = glm::mat3(firstColumn, secondColumn, thirdColumn);

    cameraPosition = cameraPosition * rotMat;
}

void Camera::rotateAboutZ() {
    float theta = rotationMovement;

    float cosTheta = cos(theta);
    float sinTheta = sin(theta);

    glm::vec3 firstColumn = glm::vec3(cosTheta,sinTheta,0);
    glm::vec3 secondColumn = glm::vec3(-sinTheta,cosTheta,0);
    glm::vec3 thirdColumn = glm::vec3(0,0,1);

    glm::mat3 rotMat = glm::mat3(firstColumn,secondColumn,thirdColumn);

    cameraPosition = cameraPosition * rotMat;
}

void Camera::rotateOrientationAboutX(int inverse) {
    float theta = rotationMovement * inverse;

    float cosTheta = cos(theta);
    float sinTheta = sin(theta);

    glm::vec3 firstColumn = glm::vec3(1,0,0);
    glm::vec3 secondColumn = glm::vec3(0, cosTheta, sinTheta);
    glm::vec3 thirdColumn = glm::vec3(0,-sinTheta, cosTheta);

    glm::mat3 rotMat = glm::mat3(firstColumn,secondColumn,thirdColumn);

    cameraOrientation = cameraOrientation * rotMat;
}

void Camera::rotateOrientationAboutY(int inverse) {
    float theta = rotationMovement * inverse;

    float cosTheta = cos(theta);
    float sinTheta = sin(theta);

    glm::vec3 firstColumn = glm::vec3(cosTheta,0,-sinTheta);
    glm::vec3 secondColumn = glm::vec3(0,1,0);
    glm::vec3 thirdColumn = glm::vec3(sinTheta,0,cosTheta);

    glm::mat3 rotMat = glm::mat3(firstColumn,secondColumn,thirdColumn);

    cameraOrientation = cameraOrientation * rotMat;


    std::cout << cameraOrientation << '\n';
}

void Camera::rotateOrientationAboutZ() {
    float theta = rotationMovement;

    float cosTheta = cos(theta);
    float sinTheta = sin(theta);

    glm::vec3 firstColumn = glm::vec3(cosTheta,sinTheta,0);
    glm::vec3 secondColumn = glm::vec3(-sinTheta,cosTheta,0);
    glm::vec3 thirdColumn = glm::vec3(0,0,1);

    glm::mat3 rotMat = glm::mat3(firstColumn,secondColumn,thirdColumn);

    cameraOrientation = cameraOrientation * rotMat;
}

void Camera::lookAt(glm::vec3 pointToLookAt) {

    //Forward is the vector from the camera to pointToLookAt (the opposite direction)
    glm::vec3 Forward = glm::normalize(cameraPosition - pointToLookAt);

    //Right is the vector cross product of vertical (0,1,0) with Forward
    glm::vec3 Right = glm::cross(glm::vec3(0,1,0), Forward);

    //Up is the vector cross product of forward with right
    glm::vec3 Up = glm::cross(Forward, Right);

    cameraOrientation = glm::mat3(Right, Up, Forward);
}

void Camera::orbit(glm::vec3 pointToLookAt) {
    rotateAboutY();
    lookAt(pointToLookAt);
}