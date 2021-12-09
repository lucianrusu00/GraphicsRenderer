#include "RaySimulation.h"
#include <limits>

#define MAXFLOAT std::numeric_limits<float>::max()

RayTriangleIntersection getClosestIntersection(std::vector<ModelTriangle> triangles, glm::vec3 startingPoint, glm::vec3 rayDirection, int jumpTriangleIndex, std::string source) {

    int triangleIndex = -1;
    glm::vec3 closestIntersection = glm::vec3(MAXFLOAT, 0, 0);

    for(int i = 0; i < triangles.size(); i++){
        if(i == jumpTriangleIndex) continue;

        glm::vec3 possibleSolution = getPossibleSolution(triangles[i], startingPoint, rayDirection); //here
        if(isIntersectionValid(possibleSolution) && possibleSolution.x < closestIntersection.x){
            closestIntersection = possibleSolution;
            triangleIndex = i;
        }
    }

    if(triangleIndex < 0 && source == "mirror")
        return RayTriangleIntersection(glm::vec3(), MAXFLOAT, ModelTriangle(), -2, glm::vec3());

    if(triangleIndex < 0)
        return RayTriangleIntersection(glm::vec3(), MAXFLOAT, ModelTriangle(), -1, glm::vec3());

    glm::vec3 hitPoint = getPointOnTriangle(triangles[triangleIndex], closestIntersection); //here

    if(triangles[triangleIndex].surface == "mirror" && !source.empty()){
        glm::vec3 pointNormal = getNormalForPoint(closestIntersection, triangles[triangleIndex].verticesNormals);
        glm::vec3 reflectedRay = getReflectedRayDirection(rayDirection, triangles[triangleIndex].normal); //block
        //glm::vec3 reflectedRay = getReflectedRayDirection(rayDirection, pointNormal); //sphere
        return getClosestIntersection(triangles, hitPoint, reflectedRay, triangleIndex, "mirror");
    }

    if(triangles[triangleIndex].surface == "glass" && !source.empty()){
        glm::vec3 pointNormal = getNormalForPoint(closestIntersection, triangles[triangleIndex].verticesNormals);
        glm::vec3 refractedRay = getRefractionRayDirection(rayDirection, pointNormal, source); // sphere
        //glm::vec3 refractedRay = getRefractionRayDirection(rayDirection, triangles[triangleIndex].normal, source); // block
        return getClosestIntersection(triangles, hitPoint, refractedRay, triangleIndex, "glass");
    }

    return RayTriangleIntersection(hitPoint, closestIntersection.x, triangles[triangleIndex], triangleIndex, closestIntersection);

}

glm::vec3 getPossibleSolution(ModelTriangle triangle, glm::vec3 statingPoint, glm::vec3 rayDirection) {
    glm::vec3 e0 = triangle.vertices[1] - triangle.vertices[0];
    glm::vec3 e1 = triangle.vertices[2] - triangle.vertices[0];
    glm::vec3 SPVector = statingPoint - triangle.vertices[0];
    glm::mat3 DEMatrix(-rayDirection, e0, e1);
    return (glm::inverse(DEMatrix) * SPVector);
}

bool isIntersectionValid(glm::vec3 possibleSolution) {
    if(possibleSolution.y >= 0 && possibleSolution.y <= 1)
        if(possibleSolution.z >= 0 && possibleSolution.z <= 1)
            if(possibleSolution.y + possibleSolution.z <= 1)
                if(possibleSolution.x > 0.004)
                    return true;

    return false;
}

glm::vec3 getPointOnTriangle(ModelTriangle triangle, glm::vec3 closestIntersection) {
    glm::vec3 uTimesP1minusP0 = closestIntersection.y * (triangle.vertices[1] - triangle.vertices[0]);
    glm::vec3 vTimesP2minusP0 = closestIntersection.z * (triangle.vertices[2] - triangle.vertices[0]);

    return triangle.vertices[0] + uTimesP1minusP0 + vTimesP2minusP0;
}

glm::vec3 getNormalForPoint(glm::vec3 areaRatios, std::array<glm::vec3, 3> triangleNormals) {

    float u = areaRatios[1];
    float v = areaRatios[2];
    float w = 1 - u - v;

    return normalize(w * triangleNormals[0] + u * triangleNormals[1] + v * triangleNormals[2]);
}

glm::vec3 getReflectedRayDirection(glm::vec3 initialRay, glm::vec3 triangleNormal) {
    glm::vec3 normal = triangleNormal;
    return normalize(initialRay - (normal + normal) * glm::dot(initialRay, normal));
}

glm::vec3 getRefractionRayDirection(glm::vec3 initialRay, glm::vec3 normal, std::string source) {

    float n = 1.5;

    if(source != "glass"){

        float factorCos = glm::dot(-initialRay, normal);
        if(factorCos > 1) factorCos = 1;

        glm::vec3 perpendicularRay = (1/n) * (initialRay + factorCos * normal);
        glm::vec3 parallelRay = float(-sqrt(fabs(1.0 - glm::dot(perpendicularRay, perpendicularRay)))) * normal;
        return normalize(perpendicularRay + parallelRay);
    }else{

        float factorCos = glm::dot(-initialRay, -normal);
        if(factorCos > 1) factorCos = 1;

        glm::vec3 perpendicularRay = n * (initialRay + factorCos * -normal);
        glm::vec3 parallelRay = float(-sqrt(fabs(1.0 - glm::dot(perpendicularRay, perpendicularRay)))) * -normal;
        return normalize(perpendicularRay + parallelRay);
    }

}
