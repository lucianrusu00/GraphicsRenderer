#include "PhotonMap.h"
#include "RaySimulation.h"
#include <limits>

#define MAXFLOAT std::numeric_limits<float>::max()

PhotonMap::PhotonMap() = default;

PhotonMap::PhotonMap(std::vector<ModelTriangle> triangles1, Camera camera1, std::vector<glm::vec3> lightSources1, BasicShapeDrawer *basicShapeDrawer1) {
    triangles = triangles1;
    basicShapeDrawer = *basicShapeDrawer1;
    lightSources = lightSources1;
    camera = camera1;
    camera.focalLength = 200;
    camera.cameraPosition.z = 1;
}


//Helper functions

void PhotonMap::generateIlluminationPoints(int pointsNumber) {
    for(ModelTriangle triangle : triangles){
        CanvasTriangle tri = CanvasTriangle(getCanvasIntersectionPoint(triangle.vertices[0]), getCanvasIntersectionPoint(triangle.vertices[1]), getCanvasIntersectionPoint(triangle.vertices[2]));
        basicShapeDrawer.drawStrokedTriangle(tri, Colour(40,40,40));
    }

    std::vector<point_t> points;

    for(int i = 0; i < pointsNumber; i++){
        glm::vec3 lightSource = lightSources[rand()%lightSources.size()]; // get a random light source
        glm::vec3 randomRay = glm::normalize(getRandomLightRay() - lightSource);

        float lightIntensity = 1;

        glm::vec3 jumpingPoint = lightSource;
        glm::vec3 jumpingRay = randomRay;
        int jumpTriangle = -1;

        while(lightIntensity > 0.05){
            RayTriangleIntersection closestIntersection = getClosestIntersection(triangles, jumpingPoint, jumpingRay, jumpTriangle, "camera");
            jumpTriangle = closestIntersection.triangleIndex;
            if(closestIntersection.distanceFromCamera < MAXFLOAT){
                point_t newPoint = {closestIntersection.intersectionPoint.x, closestIntersection.intersectionPoint.y, closestIntersection.intersectionPoint.z};


                intensityValues.insert({closestIntersection.intersectionPoint, lightIntensity});
                photonDirection.insert({closestIntersection.intersectionPoint, jumpingRay});

                points.push_back(newPoint);
                basicShapeDrawer.drawPoint(getCanvasIntersectionPoint(closestIntersection.intersectionPoint), Colour(138*lightIntensity,43*lightIntensity,226*lightIntensity));


                lightIntensity /= 2;
                jumpingRay = glm::normalize(getRandomLightRay() - closestIntersection.intersectionPoint);
                while(glm::dot(closestIntersection.intersectedTriangle.normal, jumpingRay) <= 0) jumpingRay = glm::normalize(getRandomLightRay() - closestIntersection.intersectionPoint);

                if(rand()%6 == 3) lightIntensity = 0; // Russian roulette

                jumpingPoint = closestIntersection.intersectionPoint;
            }
            else lightIntensity = 0;
        }
    }

    std::cout << points.size() << '\n';

    basicShapeDrawer.display->renderFrame();

    illuminationPoints = KDTree(points);
}

glm::vec3 PhotonMap::getRandomLightRay() {
    //float random = LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO)));
    float x = -1 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1- -1)));
    float y = -1 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1- -1)));
    float z = -1 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1- -1)));

    return glm::normalize(glm::vec3(x,y,z));

}

CanvasPoint PhotonMap::getCanvasIntersectionPoint(glm::vec3 vertexPosition){

    glm::vec3 cameraToVertex = vertexPosition - camera.cameraPosition;
    glm::vec3 adjustedVector = cameraToVertex * camera.cameraOrientation;
    vertexPosition = adjustedVector + camera.cameraPosition;

    float xi = vertexPosition.x - camera.cameraPosition.x;
    float yi = vertexPosition.y - camera.cameraPosition.y;
    float zi = vertexPosition.z - camera.cameraPosition.z;

    float ui = (-camera.focalLength * xi)/zi + basicShapeDrawer.display->width/2.0;
    float vi = (camera.focalLength * yi)/zi + basicShapeDrawer.display->height/2.0;

    return CanvasPoint(ui, vi, -zi);

}

float PhotonMap::getBaseBrightness(glm::vec3 point, glm::vec3 triangleNormal, double radius){

    point_t searchPoint = {point[0], point[1], point[2]};
    std::vector<std::vector<double>> neighboringPoints = illuminationPoints.neighborhood_points(searchPoint, radius);

    if(neighboringPoints.size() == 0) return 0;

    float totalBrightness = 0;

    int numberOfSJFL = 0;

    for(int i = 0; i < neighboringPoints.size(); i++){
        glm::vec3 neighboringPoint = glm::vec3(neighboringPoints[i][0],neighboringPoints[i][1],neighboringPoints[i][2]);
        float cosAngle = glm::dot(triangleNormal, glm::normalize(-photonDirection[neighboringPoint]));
        if(cosAngle > 0){
            numberOfSJFL++;
            totalBrightness += (intensityValues[neighboringPoint] * cosAngle);
        }
    }

    if(numberOfSJFL == 0)
        return 0;

    return totalBrightness / numberOfSJFL;
}

float PhotonMap::getBaseBrightness(glm::vec3 point) {
    point_t searchPoint = {point[0], point[1], point[2]};
    std::vector<double> neighboringPoint = illuminationPoints.nearest_point(searchPoint);

    glm::vec3 neighbour = glm::vec3(neighboringPoint[0], neighboringPoint[1], neighboringPoint[2]);

    return intensityValues[neighbour];
}

