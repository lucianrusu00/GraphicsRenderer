#pragma once

#include <KDTree.hpp>
#include <ModelTriangle.h>
#include "BasicShapeDrawer.h"
#include <unordered_map>
#include "glm/gtx/string_cast.hpp"
#include "Camera.h"
#include <glm/gtx/io.hpp>

struct KeyFuncs
{
    size_t operator()(const glm::vec3& k)const
    {
        return std::hash<float>()(k.x) ^ std::hash<float>()(k.y) ^ std::hash<float>()(k.z);
    }

    bool operator()(const glm::vec3& a, const glm::vec3& b)const
    {
        return a.x == b.x && a.y == b.y && a.z == b.z;
    }
};


typedef std::unordered_map<glm::vec3,float,KeyFuncs,KeyFuncs> GlmToFloatMap;
typedef std::unordered_map<glm::vec3,glm::vec3,KeyFuncs,KeyFuncs> GlmToGlmMap;

class PhotonMap{

public:
    KDTree illuminationPoints;
    std::vector<ModelTriangle> triangles;
    BasicShapeDrawer basicShapeDrawer;
    Camera camera;
    std::vector<glm::vec3> lightSources;
    GlmToFloatMap intensityValues;
    GlmToGlmMap photonDirection;

private:
    glm::vec3 getRandomLightRay();
    CanvasPoint getCanvasIntersectionPoint(glm::vec3 vertexPosition);

public:
    PhotonMap();
    PhotonMap(std::vector<ModelTriangle> triangles1, Camera camera1, std::vector<glm::vec3> lightSources1, BasicShapeDrawer *basicShapeDrawer1);
    void generateIlluminationPoints(int pointsNumber = 500);
    float getBaseBrightness(glm::vec3 point, glm::vec3 triangleNormal, double radius);
    float getBaseBrightness(glm::vec3 point);

};