#pragma once

#include <ModelTriangle.h>
#include <RayTriangleIntersection.h>
#include "BasicShapeDrawer.h"
#include "Camera.h"
#include "TextureDrawer.h"
#include "PhotonMap.h"

class Renderer{

public:
    BasicShapeDrawer *basicDrawer;
    Camera *camera;
    TextureDrawer *textureDrawer;
    PhotonMap *photonMap = NULL;

private:
    CanvasPoint getCanvasIntersectionPoint(glm::vec3 vertexPosition);
    glm::vec3 getRayDirection(int i, int j);
    float getBrightnessForPoint(glm::vec3 point, glm::vec3 normal, glm::vec3 rayDirection, glm::vec3 lightRay, float distToLight, float lightRange);
    float getGouraudBrightness(RayTriangleIntersection closestIntersection, glm::vec3 lightSource, float lightRange);
    float getPhongBrightness(RayTriangleIntersection closestIntersection, glm::vec3 lightSource, float lightRange);

public:
    Renderer();
    Renderer(BasicShapeDrawer &basicDrawer, Camera &camera);
    void renderPointCloud(std::vector<ModelTriangle> triangles);
    void renderWireframe(std::vector<ModelTriangle> triangles);
    void renderRasterised(std::vector<ModelTriangle> triangles);
    void renderRayTraced(std::vector<ModelTriangle> triangles, std::vector<glm::vec3> lightSources = {glm::vec3(0, 0.4, 0)}, int shaderMethod = 1);
};