#pragma once

#include <Colour.h>
#include <TextureMap.h>

class Material{

public:
    Colour colour;
    std::string texture;
    std::string surface;
    TextureMap* textureMap;

private:

public:
    Material();
    Material(Colour colour1);
    Material(Colour colour1, std::string texture1, std::string surface1);

};
