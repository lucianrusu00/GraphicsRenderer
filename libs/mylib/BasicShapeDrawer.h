#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <DrawingWindow.h>
#include <CanvasTriangle.h>
#include <Colour.h>
#include "TriangleUtilities.h"

class BasicShapeDrawer {

public:
    DrawingWindow* display;
    float **Depth2DArray;

private:
    void fillSmallTriangle(CanvasTriangle triangle, Colour colour);

public:
    BasicShapeDrawer();
    BasicShapeDrawer(DrawingWindow &window, float **Depth2DArray);
    bool inBounds(float x, float y);
    void clearCanvas();
    void resetDepth2DArray();
    void drawFilledTriangle(CanvasTriangle triangle, Colour colour = Colour{255,255,255});
    void drawStrokedTriangle(CanvasTriangle triangle, Colour colour = Colour{255,255,255});
    void drawLine(CanvasPoint from, CanvasPoint to, Colour colour = Colour{255,255,255});
    void drawPoint(CanvasPoint point, Colour colour = Colour{255,255,255}, float brightness = 1);
};
