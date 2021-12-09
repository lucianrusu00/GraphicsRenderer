#include "TextureDrawer.h"
#include <glm/gtx/io.hpp>
#include "TriangleUtilities.h"
#include "Interpolate.h"

TextureDrawer::TextureDrawer() {}

TextureDrawer::TextureDrawer(BasicShapeDrawer drawer) {
    basicShapeDrawer = drawer;
}

void TextureDrawer::drawTexturedPixel(ModelTriangle containingTriangle, glm::vec3 areaRatios, glm::vec3 intersectionPoint, CanvasPoint point, float brightness) {

    glm::vec2 point0 = glm::vec2(containingTriangle.texturePoints[0].x, containingTriangle.texturePoints[0].y);
    glm::vec2 point1 = glm::vec2(containingTriangle.texturePoints[1].x, containingTriangle.texturePoints[1].y);
    glm::vec2 point2 = glm::vec2(containingTriangle.texturePoints[2].x, containingTriangle.texturePoints[2].y);

    float u = areaRatios[1];
    float v = areaRatios[2];
    float w = 1 - u - v;

    float width = containingTriangle.textureMap->width;

    glm::vec2 textureCorrespondingPoint = w * point0 + u * point1 + v * point2;

    uint32_t color = containingTriangle.textureMap->pixels[int(round(textureCorrespondingPoint.y) * width + round(textureCorrespondingPoint.x))];

    float colorRed = ((color >> 16) & 0xff) * brightness;
    float colorGreen = ((color >> 8) & 0xff) * brightness;
    float colorBlue = (color & 0xff) * brightness;

    basicShapeDrawer.drawPoint(point, Colour(colorRed, colorGreen, colorBlue));

}

void TextureDrawer::drawTexturedTriangle(CanvasTriangle triangle, TextureMap *texture) {
    sortTriangleVertexes(triangle);

    CanvasPoint splittingPoint = getTriangleSplittingPoint(triangle);

    CanvasTriangle smallUpperTriangle = CanvasTriangle(triangle.v0(), triangle.v1(), splittingPoint);
    textureSmallTriangle(smallUpperTriangle, texture);

    CanvasTriangle smallLowerTriangle = CanvasTriangle(triangle.v2(), triangle.v1(), splittingPoint);
    textureSmallTriangle(smallLowerTriangle, texture);

    textureLine(triangle.v1(), splittingPoint, texture);

}

// Helper functions

void TextureDrawer::textureSmallTriangle(CanvasTriangle triangle, TextureMap *texture) {
    //Go from v0 (the point on the different height) to v1 and from v0 to splitting point(v2)
    int numberOfPoints = int(fabs(triangle.v0().y - triangle.v1().y)) + 1;

    std::vector<CanvasPoint> v0Tov1 = interpolateCanvasPoints(triangle.v0(), triangle.v1(), numberOfPoints);
    std::vector<CanvasPoint> v0Tov2 = interpolateCanvasPoints(triangle.v0(), triangle.v2(), numberOfPoints);

    for(int i = 0; i < numberOfPoints; i++){
        textureLine(v0Tov1[i], v0Tov2[i], texture);
    }

}

void TextureDrawer::textureLine(CanvasPoint from, CanvasPoint to, TextureMap *texture) {
    int numberOfPoints = int(fmax(fabs(from.x - to.x), fabs(from.y - to.y))) + 1;

    std::vector<CanvasPoint> pointsLine = interpolateCanvasPoints(from, to, numberOfPoints);

    for(int i = 0; i < pointsLine.size(); i++){
        CanvasPoint p = pointsLine[i];

        uint32_t intColour = texture->pixels[(round(p.texturePoint.y) * texture->width + round(p.texturePoint.x))];

        if(basicShapeDrawer.Depth2DArray!=NULL && basicShapeDrawer.inBounds(p.x,p.y) && 1/p.depth > basicShapeDrawer.Depth2DArray[(int)round(p.x)][(int)round(p.y)]){
            basicShapeDrawer.display->setPixelColour(round(p.x), round(p.y), intColour);
            basicShapeDrawer.Depth2DArray[(int)round(p.x)][(int)round(p.y)] = 1/p.depth;
        }
        else if(basicShapeDrawer.Depth2DArray == NULL)
            basicShapeDrawer.display->setPixelColour(round(p.x), round(p.y), intColour);
    }
}
