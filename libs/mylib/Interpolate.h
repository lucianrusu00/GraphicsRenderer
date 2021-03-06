#pragma once
#include <glm/glm.hpp>
#include "glm/ext.hpp"
#include <CanvasPoint.h>
#include <vector>

std::vector<float> interpolateSingleFloats(float from, float to, float numberOfValues);

std::vector<glm::vec3> interpolateThreeElementValues(glm::vec3 from, glm::vec3 to, float numberOfValues);

std::vector<CanvasPoint> interpolateCanvasPoints(CanvasPoint from, CanvasPoint to, float numberOfPoints);

std::vector<glm::mat3> interpolateOrientationMatrix(glm::mat3 from, glm::mat3 to, float numberOfPoints);