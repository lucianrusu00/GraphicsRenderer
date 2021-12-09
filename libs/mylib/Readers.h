#pragma once

#include <iostream>
#include <unordered_map>
#include <ModelTriangle.h>
#include "Colour.h"
#include "Material.h"

std::unordered_map<std::string, Material> readMaterialOBJ(std::string filename = "InputFiles/textured-cornell-box.mtl");

std::vector<ModelTriangle> readOBJ(std::unordered_map<std::string, Material> materials, float scaling = 1, std::string filename = "InputFiles/textured-cornell-box.obj");

std::vector<ModelTriangle> readOBJ(float scaling = 1, std::string filename = "InputFiles/sphere.obj");