#include "BasicShapeDrawer.h"
#include "Interpolate.h"

BasicShapeDrawer::BasicShapeDrawer() {}

BasicShapeDrawer::BasicShapeDrawer(DrawingWindow &window, float **Depth2DArray1) {
    display = &window;
    Depth2DArray = Depth2DArray1;
}

void BasicShapeDrawer::clearCanvas() {
    display->clearPixels();
}

void BasicShapeDrawer::drawPoint(CanvasPoint point, Colour colour, float brightness) {
    display->setPixelColour(point.x, point.y, getIntColour(colour, brightness));
}

void BasicShapeDrawer::drawLine(CanvasPoint from, CanvasPoint to, Colour colour) {
    float numberOfPoints = std::max(std::max(fabs(from.x - to.x), fabs(from.y - to.y)), fabs(from.depth- to.depth)) + 10;

    std::vector<CanvasPoint> pointsLine = interpolateCanvasPoints(from, to, numberOfPoints);

    uint32_t intColour = (255 << 24) + (colour.red << 16) + (colour.green << 8) + colour.blue;

    for(float i = 0; i < numberOfPoints; i++){
        CanvasPoint p = pointsLine[i];
        if(Depth2DArray!=NULL && inBounds(p.x,p.y) && 1/p.depth > Depth2DArray[(int)round(p.x)][(int)round(p.y)]){
            display->setPixelColour(round(p.x), round(p.y), intColour);
            Depth2DArray[(int)round(p.x)][(int)round(p.y)] = 1/p.depth;
        }
        else if(Depth2DArray == NULL)
            display->setPixelColour(round(p.x), round(p.y), intColour);
    }
}

//void BasicShapeDrawer::drawLine(CanvasPoint from, CanvasPoint to, Colour colour) {
//    float yDiff = to.y - from.y;
//    float xDiff = to.x - from.x;
//    float zDiff = to.depth - from.depth;
//
//    float numberOfSteps = std::max(std::max(abs(xDiff), abs(yDiff)),abs(zDiff)) + 1;
//
//    float xStepSize = xDiff/numberOfSteps;
//    float yStepSize = yDiff/numberOfSteps;
//    float zStepSize = zDiff/numberOfSteps;
//
//    uint32_t intColour = (255 << 24) + (colour.red << 16) + (colour.green << 8) + colour.blue;
//
//    for(float i=0.0; i<numberOfSteps; i++){
//        float x = from.x + (xStepSize*i);
//        float y = from.y + (yStepSize*i);
//        float z = from.depth + (zStepSize*i);
//
//        display->setPixelColour(round(x), round(y), intColour);
//    }
//
//}

void BasicShapeDrawer::drawStrokedTriangle(CanvasTriangle triangle, Colour colour) {
    drawLine(triangle.v0(), triangle.v1(), colour);
    drawLine(triangle.v1(), triangle.v2(), colour);
    drawLine(triangle.v2(), triangle.v0(), colour);
}

void BasicShapeDrawer::drawFilledTriangle(CanvasTriangle triangle, Colour colour) {
    sortTriangleVertexes(triangle);

    drawStrokedTriangle(triangle, colour);

    CanvasPoint splittingPoint = getTriangleSplittingPoint(triangle);

    drawLine(triangle.v1(), splittingPoint, colour);

    //V2 is going to be the splitting point


    //Split the triangle into an upper one and a lower one, split by a horizontal line starting from the middle [...]
    //[...] point (with regard to height) to the line formed by the other two points. And fill the two triangles.
    CanvasTriangle smallUpperTriangle = CanvasTriangle(triangle.v0(), triangle.v1(), splittingPoint);
    fillSmallTriangle(smallUpperTriangle, colour);

    CanvasTriangle smallLowerTriangle = CanvasTriangle(triangle.v2(), triangle.v1(), splittingPoint);
    fillSmallTriangle(smallLowerTriangle, colour);
}

void BasicShapeDrawer::resetDepth2DArray() {
    for(int i = 0; i < display->width; i++)
        for(int j = 0; j < display->height; j++)
            Depth2DArray[i][j] = 0;
}


//Helper, private functions

void BasicShapeDrawer::fillSmallTriangle(CanvasTriangle triangle, Colour colour) {

    //Go from v0 (the point on the different height) to v1 and from v0 to splitting point(v2)

    float numberOfPoints = std::max(abs(triangle.v0().y - triangle.v1().y), abs(triangle.v0().y - triangle.v2().y)) + 2;

    std::vector<CanvasPoint> v0Tov1 = interpolateCanvasPoints(triangle.v0(), triangle.v1(), numberOfPoints);
    std::vector<CanvasPoint> v0Tov2 = interpolateCanvasPoints(triangle.v0(), triangle.v2(), numberOfPoints);

    for(float i = 0; i < numberOfPoints; i++){
        drawLine(v0Tov1[i], v0Tov2[i], colour);
    }

}

bool BasicShapeDrawer::inBounds(float x, float y){
    x = (int)roundf(x);
    y = (int)roundf(y);
    if(x >= 0 && x < display->width && y >= 0 && y < display->height)
        return true;
    return false;
}
