#pragma once
#include <glm/glm.hpp>
#include "glm/ext.hpp"
#include <CanvasPoint.h>

std::vector<float> interpolateSingleFloats(float from, float to, float numberOfValues);

std::vector<glm::vec3> interpolateThreeElementValues(glm::vec3 from, glm::vec3 to, float numberOfValues);

std::vector<CanvasPoint> interpolateCanvasPoints(CanvasPoint from, CanvasPoint to, float numberOfPoints);