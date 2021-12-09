//
// Created by Lucian Rusu on 25/11/2021.
//

#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include "glm/ext.hpp"
#include <TextureMap.h>
#include <CanvasPoint.h>
#include <Colour.h>
#include <CanvasTriangle.h>
#include <ModelTriangle.h>
#include <unordered_map>
//#include <BasicShapeDrawer.h>
#include <DrawingWindow.h>
#include "Interpolate.h"
#include "Camera.h"

void testInterpolateSingleFloats(){
    std::vector<float> result;
    result = interpolateSingleFloats(8.5, 2.2, 7);
    for(size_t i = 0; i<result.size(); i++)
        std::cout << result[i] << ' ';

    std::cout << std::endl;
}

void testInterpolateThreeElementValues(){
    std::vector<glm::vec3> result;
    glm::vec3 from(1, 4, 9.2);
    glm::vec3 to(4, 1, 9.8);
    result = interpolateThreeElementValues(from, to, 4);
    for(size_t i = 0; i<result.size(); i++)
        std::cout << glm::to_string(result[i]) << ' ';

    std::cout << std::endl;
}

void testVisualVerificationTextureMapping(DrawingWindow &window){
    TextureMap* texture = new TextureMap("InputFiles/texture.ppm");

    CanvasPoint point1 = CanvasPoint(160, 10);
    CanvasPoint point2 = CanvasPoint(300,230);
    CanvasPoint point3 = CanvasPoint(10,150);
    CanvasTriangle canvasTriangle = CanvasTriangle(point1, point2, point3);

    CanvasPoint point4 = CanvasPoint(195, 5);
    CanvasPoint point5 = CanvasPoint(395,380);
    CanvasPoint point6 = CanvasPoint(65,330);
    CanvasTriangle textureTriangle = CanvasTriangle(point4, point5, point6);

    //Do the test below with the texture mapping class
    //showTexturedMapTriangle(canvasTriangle, textureTriangle, *texture, window);

}

void testing(DrawingWindow &window){
    testInterpolateSingleFloats();
    testInterpolateThreeElementValues();
    testVisualVerificationTextureMapping(window);
}