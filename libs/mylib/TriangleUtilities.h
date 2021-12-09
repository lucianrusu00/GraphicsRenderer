#pragma once

#include <cstdint>
#include <Colour.h>
#include <CanvasTriangle.h>

uint32_t getIntColour(Colour colour, float brightness = 1);

void sortTriangleVertexes(CanvasTriangle& triangle);

CanvasPoint getTriangleSplittingPoint(CanvasTriangle triangle);
