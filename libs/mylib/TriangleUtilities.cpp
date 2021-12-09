#include "TriangleUtilities.h"
#include "Interpolate.h"
#include <vector>
#include <limits>

#define MAXFLOAT std::numeric_limits<float>::max()

uint32_t getIntColour(Colour colour, float brightness){
    uint32_t intColour = (255 << 24) + (int(brightness * colour.red) << 16) + (int(brightness * colour.green) << 8) + int(brightness * colour.blue);
    return intColour;
}

void sortTriangleVertexes(CanvasTriangle& triangle){

    //Sort the points by their height
    if(triangle.v0().y > triangle.v1().y) std::swap(triangle.v0(), triangle.v1());
    if(triangle.v0().y > triangle.v2().y) std::swap(triangle.v0(), triangle.v2());
    if(triangle.v1().y > triangle.v2().y) std::swap(triangle.v1(), triangle.v2());

}

CanvasPoint getTriangleSplittingPoint(CanvasTriangle triangle){
    //interpolate the long line and find the point on y = triangle.v1().y
    float yDiff = triangle.v0().y - triangle.v2().y;
    float numberOfPoints = abs(yDiff);

    std::vector<CanvasPoint> points = interpolateCanvasPoints(triangle.v0(), triangle.v2(), numberOfPoints);

    float minDiff = MAXFLOAT;
    int pos = 0;

    for(float i = 0; i < numberOfPoints; i++){
        if(abs(points[i].y - triangle.v1().y) < minDiff){
            minDiff = fabs(points[i].y - triangle.v1().y);
            pos = i;
        }
    }

    return points[pos];
}