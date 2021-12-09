//
// Created by Lucian Rusu on 02/12/2021.
//

#include "Material.h"

Material::Material() = default;

Material::Material(Colour colour1) {
    colour = colour1;
    surface = "matte";
}

Material::Material(Colour colour1, std::string texture1, std::string surface1) {
    colour = colour1;
    texture = texture1;
    surface = surface1;
    textureMap = new TextureMap(texture);
}
