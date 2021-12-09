#include "Renderer.h"
#include "glm/gtx/string_cast.hpp"
#include <glm/gtx/io.hpp>
#include "RaySimulation.h"
#include <limits>

#define MAXFLOAT std::numeric_limits<float>::max()

Renderer::Renderer() {}

Renderer::Renderer(BasicShapeDrawer &basicDrawer1, Camera &camera1) {
    basicDrawer = &basicDrawer1;
    camera = &camera1;
    textureDrawer = new TextureDrawer(*basicDrawer);
}

void Renderer::renderPointCloud(std::vector<ModelTriangle> triangles) {
    basicDrawer->resetDepth2DArray();

    for(int i = 0; i < triangles.size(); i++){
        for(int j = 0; j<3; j++){
            CanvasPoint point = getCanvasIntersectionPoint(triangles[i].vertices[j]);
            basicDrawer->drawPoint(point, triangles[i].colour);
        }
    }
}

void Renderer::renderWireframe(std::vector<ModelTriangle> triangles) {
    basicDrawer->resetDepth2DArray();

    for(int i = 0; i < triangles.size(); i++){
        CanvasPoint point0 = getCanvasIntersectionPoint(triangles[i].vertices[0]);
        CanvasPoint point1 = getCanvasIntersectionPoint(triangles[i].vertices[1]);
        CanvasPoint point2 = getCanvasIntersectionPoint(triangles[i].vertices[2]);

        CanvasTriangle triangle = CanvasTriangle(point0,point1,point2);

        basicDrawer->drawStrokedTriangle(triangle, triangles[i].colour);
    }
}

void Renderer::renderRasterised(std::vector<ModelTriangle> triangles) {
    basicDrawer->resetDepth2DArray();

    for(int i = 0; i < triangles.size(); i++){
        CanvasPoint point0 = getCanvasIntersectionPoint(triangles[i].vertices[0]);
        CanvasPoint point1 = getCanvasIntersectionPoint(triangles[i].vertices[1]);
        CanvasPoint point2 = getCanvasIntersectionPoint(triangles[i].vertices[2]);

        if(triangles[i].textureMap == NULL){
            CanvasTriangle triangle = CanvasTriangle(point0,point1,point2);
            basicDrawer->drawFilledTriangle(triangle,triangles[i].colour);
        }
        else{
            point0.texturePoint = triangles[i].texturePoints[0];
            point1.texturePoint = triangles[i].texturePoints[1];
            point2.texturePoint = triangles[i].texturePoints[2];
            CanvasTriangle triangle = CanvasTriangle(point0,point1,point2);

            textureDrawer->drawTexturedTriangle(triangle, triangles[i].textureMap);

        }

    }
}

void Renderer::renderRayTraced(std::vector<ModelTriangle> triangles, std::vector<glm::vec3> lightSources, int shaderMethod) {

    //TextureMap environmentMaps = TextureMap("InputFiles/environment.ppm");

    BasicShapeDrawer basicShapeDrawer = *basicDrawer;

    int width = basicShapeDrawer.display->width;
    int height = basicShapeDrawer.display->height;

    glm::vec3 cameraPosition = camera->cameraPosition;

    float lightRange = 2.5;

    for(int i = 0; i < width; i++)
        for(int j = 0; j < height; j++){

            glm::vec3 rayDirection = getRayDirection(i, j);
            RayTriangleIntersection closestIntersection = getClosestIntersection(triangles, cameraPosition, rayDirection, -1, "camera");


            // If the mirror ray hit outside the box
            if(closestIntersection.triangleIndex == -2){
                basicDrawer->drawPoint(CanvasPoint(i,j), Colour(147,255,255));

                //printf("I mirrored outside!\n");
            }


            //If the ray form the camera hits something
            if(closestIntersection.distanceFromCamera < MAXFLOAT) {

                float totalBrightness = 0;
                float baseBrightness = 0.2;
                if(photonMap != NULL) baseBrightness = photonMap->getBaseBrightness(closestIntersection.intersectionPoint, closestIntersection.intersectedTriangle.normal, 0.04);

                for (int i = 0; i < lightSources.size(); i++) {

                    float brightness = baseBrightness; // lower threshold
                    glm::vec3 lightSource = lightSources[i];

                    glm::vec3 rayIntersectionToLight = normalize(lightSource - closestIntersection.intersectionPoint);
                    RayTriangleIntersection intersectionPointToLight = getClosestIntersection(triangles,
                                                                                                  closestIntersection.intersectionPoint,
                                                                                                  rayIntersectionToLight,
                                                                                                  closestIntersection.triangleIndex);


                    float distanceIntersectionToLight = glm::dot(
                                closestIntersection.intersectionPoint - lightSource,
                                closestIntersection.intersectionPoint - lightSource);
                    float distanceIntersectionToLightIntersection =
                                intersectionPointToLight.distanceFromCamera *
                                intersectionPointToLight.distanceFromCamera;

                    // if the point gets directly hit by light calculate its brightness
                    if (distanceIntersectionToLight < distanceIntersectionToLightIntersection && distanceIntersectionToLight <= lightRange) {
                        switch(shaderMethod){
                            case 1:
                                brightness += getBrightnessForPoint(closestIntersection.intersectionPoint, closestIntersection.intersectedTriangle.normal, rayIntersectionToLight, lightSource, distanceIntersectionToLight, lightRange);
                                break;
                            case 2:
                                brightness += getGouraudBrightness(closestIntersection, lightSource, lightRange);
                                break;
                            case 3:
                                brightness += getPhongBrightness(closestIntersection, lightSource, lightRange);
                                break;
                            default:
                                brightness += getBrightnessForPoint(closestIntersection.intersectionPoint, closestIntersection.intersectedTriangle.normal, rayIntersectionToLight, lightSource, distanceIntersectionToLight, lightRange);
                        }
                            //printf("brightness2:%f \n", brightness);
                    }
                    totalBrightness += brightness;

                }

                float brightness = totalBrightness / lightSources.size();
                if (brightness > 1) brightness = 1;

                if(closestIntersection.intersectedTriangle.textureMap == NULL)
                    basicShapeDrawer.drawPoint(CanvasPoint(i,j), closestIntersection.intersectedTriangle.colour, brightness);
                else
                    textureDrawer->drawTexturedPixel(closestIntersection.intersectedTriangle, closestIntersection.areaRatios, closestIntersection.intersectionPoint, CanvasPoint(i,j), brightness);
//                    basicShapeDrawer.drawPoint(CanvasPoint(i, j), closestIntersection.intersectedTriangle.colour,
//                                               brightness);

            }

        }

}


//Helper functions

CanvasPoint Renderer::getCanvasIntersectionPoint(glm::vec3 vertexPosition){

    glm::vec3 cameraToVertex = vertexPosition - camera->cameraPosition;
    glm::vec3 adjustedVector = cameraToVertex * camera->cameraOrientation;
    vertexPosition = adjustedVector + camera->cameraPosition;

    float xi = vertexPosition.x - camera->cameraPosition.x;
    float yi = vertexPosition.y - camera->cameraPosition.y;
    float zi = vertexPosition.z - camera->cameraPosition.z;

    float ui = (-camera->focalLength * xi)/zi + basicDrawer->display->width/2.0;
    float vi = (camera->focalLength * yi)/zi + basicDrawer->display->height/2.0;

    return CanvasPoint(ui, vi, -zi);

}

glm::vec3 Renderer::getRayDirection(int i, int j) {

    float imageAspectRatio = (float)basicDrawer->display->width / (float)basicDrawer->display->height;

    float rayDirectionX = (2 * ((i + 0.5) / basicDrawer->display->width) - 1) * imageAspectRatio;
    float rayDirectionY = (1 - 2 * ((j + 0.5) / basicDrawer->display->height));

    //glm::vec3 rayDirection = glm::vec3(rayDirectionX, rayDirectionY, -1) - camera->cameraPosition;
    glm::vec3 rayDirection = normalize( glm::vec3(rayDirectionX, rayDirectionY, -5));

    return normalize(camera->cameraOrientation * rayDirection);
}


float Renderer::getBrightnessForPoint(glm::vec3 intersectionPoint, glm::vec3 normal, glm::vec3 lightRayDirection, glm::vec3 lightSource, float distanceIntersectionToLight, float lightRange){

    float brightness;

    float lightIntensity =  1.0 / (4.0 * M_PI * (distanceIntersectionToLight / lightRange) * (distanceIntersectionToLight / lightRange));
    if(lightIntensity > 1) lightIntensity = 1;

    float angleOfIncidence = glm::dot(normal, lightRayDirection);
    if(angleOfIncidence < 0) angleOfIncidence = 0;

    glm::vec3 viewDirection = glm::normalize(camera->cameraPosition - intersectionPoint);
    glm::vec3 lightIncidenceDirection = glm::normalize(intersectionPoint - lightSource);

    glm::vec3 lightReflectionDirection = lightIncidenceDirection - (2.0f * normal) * (glm::dot(lightIncidenceDirection, normal));
    glm::normalize(lightReflectionDirection);

    float dotProduct = glm::dot(viewDirection, lightReflectionDirection);

    float specularIntensity = pow(dotProduct, 256);

    if(specularIntensity < 0.5) specularIntensity = 0.5;


    brightness = lightIntensity * angleOfIncidence * specularIntensity;


    if(brightness < 0) brightness = 0;
    else if(brightness > 1) brightness = 1;

    return brightness;
}

float Renderer::getGouraudBrightness(RayTriangleIntersection closestIntersection, glm::vec3 lightSource, float lightRange) {

    ModelTriangle intersectedTriangle = closestIntersection.intersectedTriangle;

    std::array<float, 3> verticesBrightness;

    for(int i = 0; i < 3; i++){
        glm::vec3 rayPointToLight = normalize(lightSource - intersectedTriangle.vertices[i]);
        //glm::vec3 rayPointToLight = lightRayDirection;
        float distancePointToLight = glm::dot(intersectedTriangle.vertices[i] - lightSource, intersectedTriangle.vertices[i] - lightSource);
        //float distancePointToLight = distance;
        verticesBrightness[i] = getBrightnessForPoint(intersectedTriangle.vertices[i], intersectedTriangle.verticesNormals[i], rayPointToLight, lightSource, distancePointToLight, lightRange);
    }

    float u = closestIntersection.areaRatios[1];
    float v = closestIntersection.areaRatios[2];
    float w = 1 - u - v;

    return w * verticesBrightness[0] + u * verticesBrightness[1] + v * verticesBrightness[2];
}

float Renderer::getPhongBrightness(RayTriangleIntersection closestIntersection, glm::vec3 lightSource, float lightRange){

    glm::vec3  normal = getNormalForPoint(closestIntersection.areaRatios, closestIntersection.intersectedTriangle.verticesNormals);

    glm::vec3 rayPointToLight = normalize(lightSource - closestIntersection.intersectionPoint);
    float distancePointToLight = glm::dot(closestIntersection.intersectionPoint - lightSource, closestIntersection.intersectionPoint - lightSource);

    return getBrightnessForPoint(closestIntersection.intersectionPoint, normal, rayPointToLight, lightSource, distancePointToLight, lightRange);
}
