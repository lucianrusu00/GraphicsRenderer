#include "Readers.h"
#include <fstream>
#include <Utils.h>
#include <glm/gtx/io.hpp>

std::unordered_map<std::string, Material> readMaterialOBJ(std::string filename){

    std::unordered_map<std::string, Material> materials;
    std::ifstream materialOBJFile(filename);
    std::string lineMaterialOBJ;

    while(getline(materialOBJFile, lineMaterialOBJ)){
        std::vector<std::string> elements = split(lineMaterialOBJ, ' ');
        if(elements.size()){
            if(elements[0] == "newmtl"){
                Material thisMaterial = Material();
                if(elements[1] == "Mirror") thisMaterial.surface = "mirror";
                if(elements[1] == "Glass") thisMaterial.surface = "glass";
                std::string colorLine;
                getline(materialOBJFile, colorLine);
                std::vector<std::string> colors = split(colorLine, ' ');
                int r = (int) roundf(std::stof(colors[1]) * 255);
                int g = (int) roundf(std::stof(colors[2]) * 255);
                int b = (int) roundf(std::stof(colors[3]) * 255);
                thisMaterial.colour = Colour(r,g,b);
                std::string textureLine;
                getline(materialOBJFile, textureLine);
                if(textureLine.size()){
                    std::vector<std::string> textures = split(textureLine, ' ');
                    if(textures[0] == "map_Kd"){
                        thisMaterial.texture = textures[1];
                        std::string relativeFilePath = "InputFiles/" + textures[1];
                        thisMaterial.textureMap = new TextureMap(relativeFilePath);
                    }

                }
                materials.insert({elements[1], thisMaterial});
            }
        }
    }

//    std::unordered_map<std::string, Colour>::iterator  it;
//    for(it = materials.begin(); it!= materials.end(); it++){
//        std::cout << it -> first << ':' << it -> second << '\n';
//    }

    materialOBJFile.close();

    return materials;
}

std::vector<ModelTriangle> readOBJ(std::unordered_map<std::string, Material> materials, float scaling, std::string filename){
    std::vector<ModelTriangle> triangles;

    std::ifstream OBJFile(filename);
    std::string lineOBJ;

    std::vector<glm::vec3> vertexes;
    vertexes.push_back(glm::vec3(0,0,0));

    std::vector<glm::vec2> textureVertexes;
    textureVertexes.push_back(glm::vec2(0,0));

    std::string lastTexture;
    bool hasTexture = false;

    while(getline(OBJFile, lineOBJ)){
        std::vector<std::string> elements = split(lineOBJ, ' ');

        if(elements.size()) {
            if(elements[0] == "usemtl"){
                lastTexture = elements[1];
                if(materials.at(lastTexture).texture != "") hasTexture = true;
                else hasTexture = false;
            }
            if(elements[0] == "v"){
                float a = std::stof(elements[1]) * scaling;
                float b = std::stof(elements[2]) * scaling;
                float c = std::stof(elements[3]) * scaling;
                vertexes.push_back(glm::vec3(a,b,c));
            }
            else if(elements[0] == "vt"){
                float x = std::stof(elements[1]);
                float y = std::stof(elements[2]);
                textureVertexes.push_back(glm::vec2(x,y));
            }
            else if (elements[0] == "f"){
                ModelTriangle triangle;
                if(!hasTexture){
                    std::string integerString1 = elements[1];
                    integerString1.pop_back();
                    int a = std::stoi(integerString1);
                    std::string integerString2 = elements[2];
                    integerString2.pop_back();
                    int b = std::stoi(integerString2);
                    std::string integerString3 = elements[3];
                    integerString3.pop_back();
                    int c = std::stoi(integerString3);

                    Colour color = materials.at(lastTexture).colour;

                    triangle = ModelTriangle(vertexes[a], vertexes[b], vertexes[c], color);
                    triangle.textureMap = NULL;

                }
                else{
                    std::vector<std::string> vertexNTextureVertexA = split(elements[1], '/');
                    int a = std::stoi(vertexNTextureVertexA[0]);
                    int ta = std::stoi(vertexNTextureVertexA[1]);
                    std::vector<std::string> vertexNTextureVertexB = split(elements[2], '/');
                    int b = std::stoi(vertexNTextureVertexB[0]);
                    int tb = std::stoi(vertexNTextureVertexB[1]);
                    std::vector<std::string> vertexNTextureVertexC = split(elements[3], '/');
                    int c = std::stoi(vertexNTextureVertexC[0]);
                    int tc = std::stoi(vertexNTextureVertexC[1]);

                    printf("a: %d/%d  b: %d/%d   c: %d/%d \n", a, ta, b, tb, c, tc);

                    Material currentMaterial = materials.at(lastTexture);
                    Colour color = currentMaterial.colour;
                    printf("With:%d   Height:%d \n", currentMaterial.textureMap->width, currentMaterial.textureMap->height);

                    triangle = ModelTriangle(vertexes[a], vertexes[b], vertexes[c], color);
                    float posXForA = textureVertexes[ta].x * currentMaterial.textureMap->width;
                    float posYForA = currentMaterial.textureMap->height - textureVertexes[ta].y * currentMaterial.textureMap->height;
                    //float posYForA = textureVertexes[ta].y * currentMaterial.textureMap->height;
                    triangle.texturePoints[0] = TexturePoint(posXForA, posYForA);
//                    printf("TexturePointRatio1: %f x %f\n", textureVertexes[ta].x, textureVertexes[ta].y);
//                    printf("TexturePoint1: %f x %f\n", posXForA, posYForA);

                    float posXForB = textureVertexes[tb].x * currentMaterial.textureMap->width;
                    float posYForB = currentMaterial.textureMap->height - textureVertexes[tb].y * currentMaterial.textureMap->height;
                    //float posYForB = textureVertexes[tb].y * currentMaterial.textureMap->height;
                    triangle.texturePoints[1] = TexturePoint(posXForB, posYForB);
//                    printf("TexturePointRatio2: %f x %f\n", textureVertexes[tb].x, textureVertexes[tb].y);
//                    printf("TexturePoint2: %f x %f\n", posXForB, posYForB);

                    float posXForC = textureVertexes[tc].x * currentMaterial.textureMap->width;
                    float posYForC = currentMaterial.textureMap->height - textureVertexes[tc].y * currentMaterial.textureMap->height;
                    //float posYForC = textureVertexes[tc].y * currentMaterial.textureMap->height;
                    triangle.texturePoints[2] = TexturePoint(posXForC, posYForC);
//                    printf("TexturePointRatio3: %f x %f\n", textureVertexes[tc].x, textureVertexes[tc].y);
//                    printf("TexturePoint3: %f x %f\n", posXForC, posYForC);

                    triangle.textureMap = currentMaterial.textureMap;
                }
                triangle.surface = materials.at(lastTexture).surface;
                triangles.push_back(triangle);

            }
        }

    }

//    std::cout << "Here come the triangles \n";
//    for(int i = 0; i < triangles.size(); i++)
//        std::cout << triangles[i] << ' ' << triangles[i].colour << '\n';
//    std::cout << '\n';

    OBJFile.close();

    for(int i = 0; i < triangles.size(); i++){
        triangles[i].normal = (glm::cross(triangles[i].vertices[1] - triangles[i].vertices[0], triangles[i].vertices[2] - triangles[i].vertices[0]));
        triangles[i].normal = normalize(triangles[i].normal);
    }

    for(int i = 0; i < triangles.size(); i++){
        for(int j = 0; j < 3; j++){
            float neighbouringTriangles = 0;
            glm::vec3 vertexNormal = glm::vec3(0,0,0);
            for(int k = 0; k < triangles.size(); k++){
                for(int p = 0; p < 3; p++){
                    if(triangles[k].vertices[p].x == triangles[i].vertices[j].x && triangles[k].vertices[p].y == triangles[i].vertices[j].y && triangles[k].vertices[p].z == triangles[i].vertices[j].z) {
                        neighbouringTriangles++;
                        vertexNormal = vertexNormal + (triangles[k].normal);
                    }

                }
            }
            triangles[i].verticesNormals[j] = glm::normalize(vertexNormal / neighbouringTriangles);
        }
    }


    return triangles;
}

std::vector<ModelTriangle> readOBJ(float scaling, std::string filename){
    std::vector<ModelTriangle> triangles;

    std::ifstream OBJFile(filename);
    std::string lineOBJ;

    std::vector<glm::vec3> vertexes;
    vertexes.push_back(glm::vec3(0,0,0));

    while(getline(OBJFile, lineOBJ)){
        std::vector<std::string> elements = split(lineOBJ, ' ');

        if(elements.size()) {
            if(elements[0] == "v"){
                float a = std::stof(elements[1]) * scaling;
                float b = std::stof(elements[2]) * scaling;
                float c = std::stof(elements[3]) * scaling;
                vertexes.push_back(glm::vec3(a,b,c));
            }
            else if (elements[0] == "f"){
                std::string integerString1 = elements[1];
                integerString1.pop_back();
                int a = std::stoi(integerString1);
                std::string integerString2 = elements[2];
                integerString2.pop_back();
                int b = std::stoi(integerString2);
                std::string integerString3 = elements[3];
                integerString3.pop_back();
                int c = std::stoi(integerString3);

                Colour color = Colour(255,0,0); // The default color is red

                ModelTriangle triangle = ModelTriangle(vertexes[a], vertexes[b], vertexes[c], color);
                triangles.push_back(triangle);

            }
        }

    }

//    for(int i = 0; i < triangles.size(); i++)
//        std::cout << triangles[i] << ' ' << triangles[i].colour << '\n';
//    std::cout << '\n';

    OBJFile.close();

    for(int i = 0; i < triangles.size(); i++){
        triangles[i].normal = glm::normalize(glm::cross(triangles[i].vertices[1] - triangles[i].vertices[0], triangles[i].vertices[2] - triangles[i].vertices[0]));
    }

    for(int i = 0; i < triangles.size(); i++){
        for(int j = 0; j < 3; j++){
            float neighbouringTriangles = 0;
            glm::vec3 vertexNormal = glm::vec3(0,0,0);
            for(int k = 0; k < triangles.size(); k++){
                for(int p = 0; p < 3; p++){
                    if(triangles[k].vertices[p] == triangles[i].vertices[j]){
                        neighbouringTriangles++;
                        vertexNormal = vertexNormal + triangles[k].normal;
                    }

                }
            }
            triangles[i].verticesNormals[j] = glm::normalize(vertexNormal / neighbouringTriangles);
            //std::cout << "triangle " << i << " vertex " << j << " is " << triangles[i].verticesNormals[j] << '\n';
        }
    }


    return triangles;
}

