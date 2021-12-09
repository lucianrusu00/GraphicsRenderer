#pragma once

#include "RayTriangleIntersection.h"

RayTriangleIntersection getClosestIntersection(std::vector<ModelTriangle> triangles, glm::vec3 startingPoint, glm::vec3 rayDirection, int jumpTriangleIndex = -1, std::string source = "");

glm::vec3 getPossibleSolution(ModelTriangle triangle, glm::vec3 startingPoint, glm::vec3 rayDirection);

bool isIntersectionValid(glm::vec3 possibleSolution);

glm::vec3 getPointOnTriangle(ModelTriangle triangle, glm::vec3 closestIntersection);

glm::vec3 getNormalForPoint(glm::vec3 areaRatios, std::array<glm::vec3, 3> triangleNormals);

glm::vec3 getReflectedRayDirection(glm::vec3 initialRay, glm::vec3 triangleNormal);

glm::vec3 getRefractionRayDirection(glm::vec3 initialRay, glm::vec3 normal, std::string source);