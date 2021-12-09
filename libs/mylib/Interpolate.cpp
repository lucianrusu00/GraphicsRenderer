#include "Interpolate.h"

std::vector<float> interpolateSingleFloats(float from, float to, float numberOfValues){

    std::vector<float> array;

    float step = (to - from)/(numberOfValues);
    for(float i = 0; i < numberOfValues; i++) array.push_back(from + (step * i));

    return array;
}

std::vector<glm::vec3> interpolateThreeElementValues(glm::vec3 from, glm::vec3 to, float numberOfValues){
    if(numberOfValues < 2)
        return {from, to};

    std::vector<glm::vec3> result;
    std::vector<float> resultRed = interpolateSingleFloats(from.x,to.x, numberOfValues);
    std::vector<float> resultGreen = interpolateSingleFloats(from.y,to.y, numberOfValues);
    std::vector<float> resultBlue = interpolateSingleFloats(from.z,to.z, numberOfValues);

    for(float i = 0; i < numberOfValues; i++) result.push_back(glm::vec3(resultRed[i], resultGreen[i], resultBlue[i]));

    return result;
}

std::vector<CanvasPoint> interpolateCanvasPoints(CanvasPoint from, CanvasPoint to, float numberOfPoints){
    if(numberOfPoints < 1)
        numberOfPoints = 1;

    std::vector<CanvasPoint> result;

    std::vector<float> resultX = interpolateSingleFloats(from.x, to.x, numberOfPoints);
    std::vector<float> resultY = interpolateSingleFloats(from.y, to.y, numberOfPoints);
    std::vector<float> resultDepth = interpolateSingleFloats(from.depth, to.depth, numberOfPoints);
    std::vector<float> resultXTexture = interpolateSingleFloats(from.texturePoint.x, to.texturePoint.x, numberOfPoints);
    std::vector<float> resultYTexture = interpolateSingleFloats(from.texturePoint.y, to.texturePoint.y, numberOfPoints);

    for(float i = 0; i < numberOfPoints; i++){
        CanvasPoint point = CanvasPoint(resultX[i], resultY[i], resultDepth[i]);
        point.texturePoint.x = resultXTexture[i];
        point.texturePoint.y = resultYTexture[i];
        result.push_back(point);
    }

    return result;
}