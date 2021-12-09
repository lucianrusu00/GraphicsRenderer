#pragma once

#include <ModelTriangle.h>
#include "BasicShapeDrawer.h"

class TextureDrawer{
public:
    BasicShapeDrawer basicShapeDrawer;

private:
    void textureSmallTriangle(CanvasTriangle triangle, TextureMap *texture);
    void textureLine(CanvasPoint from, CanvasPoint to, TextureMap *texture);

public:
    TextureDrawer();
    TextureDrawer(BasicShapeDrawer drawer);
    void drawTexturedPixel(ModelTriangle containingTriangle, glm::vec3 areaRatios, glm::vec3 intersectionPoint, CanvasPoint point, float brightness = 1);
    void drawTexturedTriangle(CanvasTriangle triangle, TextureMap *texture);
};
