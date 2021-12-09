#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <vector>
#include <glm/glm.hpp>
#include "glm/ext.hpp"
#include "glm/gtx/io.hpp"
#include <CanvasPoint.h>
#include <Colour.h>
#include <ModelTriangle.h>
#include <unordered_map>
#include <BasicShapeDrawer.h>
#include <Renderer.h>
#include <Interpolate.h>
#include "Camera.h"
#include "Readers.h"
#include "PhotonMap.h"

#define WIDTH 640 //640
#define HEIGHT 480 //480

std::tuple<CanvasTriangle, Colour> getRandomPointsAndColour(DrawingWindow window){
    CanvasPoint v0 = CanvasPoint(rand()%window.width, rand()%window.height);
    CanvasPoint v1 = CanvasPoint(rand()%window.width, rand()%window.height);
    CanvasPoint v2 = CanvasPoint(rand()%window.width, rand()%window.height);
    CanvasTriangle strokedTriangle = CanvasTriangle(v0, v1, v2);
    Colour colour = Colour(rand()%256, rand()%256, rand()%256);
    return std::make_tuple(strokedTriangle, colour);
}

void addStrokedTriangle(BasicShapeDrawer *basicDrawer){
    std::tuple<CanvasTriangle, Colour> pair = getRandomPointsAndColour(*basicDrawer->display);
    CanvasTriangle triangle = std::get<0>(pair);
    Colour colour = std::get<1>(pair);
    basicDrawer->drawStrokedTriangle(triangle, colour);
}

void addFilledTriangle(BasicShapeDrawer *basicDrawer){
    std::tuple<CanvasTriangle, Colour> pair = getRandomPointsAndColour(*basicDrawer->display);
    CanvasTriangle triangle = std::get<0>(pair);
    Colour colour = std::get<1>(pair);
    basicDrawer->drawFilledTriangle(triangle, colour);
}

void handleEvent(SDL_Event event, Camera *camera, bool &orbiting, bool &run, BasicShapeDrawer *basicDrawer, std::vector<glm::vec3> &lightSources) {
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_LEFT) camera->moveLeft();
        else if (event.key.keysym.sym == SDLK_RIGHT) camera->moveRight();
        else if (event.key.keysym.sym == SDLK_UP) camera->moveUp();
        else if (event.key.keysym.sym == SDLK_DOWN) camera->moveDown();
        else if (event.key.keysym.sym == SDLK_l) camera->moveForward();
        else if (event.key.keysym.sym == SDLK_p) camera->moveBackward();
        else if(event.key.keysym.sym == SDLK_o) camera->rotateAboutX();
        else if(event.key.keysym.sym == SDLK_k) camera->rotateAboutY();
        else if(event.key.keysym.sym == SDLK_m) camera->rotateAboutZ();
        else if(event.key.keysym.sym == SDLK_q) orbiting = !orbiting;
        else if (event.key.keysym.sym == SDLK_u) addStrokedTriangle(basicDrawer);
        else if (event.key.keysym.sym == SDLK_f) addFilledTriangle(basicDrawer);
        else if (event.key.keysym.sym == SDLK_v) run = false;
        else if(event.key.keysym.sym == SDLK_g){
            for(int i = 0 ; i < lightSources.size(); i++)
                lightSources[i].x -= 0.1; // Move right
        }
        else if(event.key.keysym.sym == SDLK_j) {
            for(int i = 0 ; i < lightSources.size(); i++)
                lightSources[i].x += 0.1; // Move Left
        }
        else if(event.key.keysym.sym == SDLK_b) {
            for(int i = 0 ; i < lightSources.size(); i++)
                lightSources[i].y += 0.1; // Move up
        }
        else if(event.key.keysym.sym == SDLK_n) {
            for(int i = 0 ; i < lightSources.size(); i++)
                lightSources[i].y -= 0.1; // Move down
        }
        else if(event.key.keysym.sym == SDLK_y) {
            for(int i = 0 ; i < lightSources.size(); i++)
                lightSources[i].z -= 0.1; // Move forwards
        }
        else if(event.key.keysym.sym == SDLK_h) {
            for(int i = 0 ; i < lightSources.size(); i++)
                lightSources[i].z += 0.1; // Move backwards
        }
    } else if (event.type == SDL_MOUSEBUTTONDOWN) {
        basicDrawer->display->savePPM("output.ppm");
        basicDrawer->display->saveBMP("output.bmp");
    }
}

std::vector<glm::vec3> getMultipleLightSources(glm::vec3 centerLightSource){
    std::vector<glm::vec3> lightSources;
    lightSources.push_back(centerLightSource);

    int intermediatePoints = 5;

    for(int i = 1; i<= intermediatePoints; i++){
        glm::vec3 aboveLight = centerLightSource;
        aboveLight.y += 0.1/intermediatePoints * i;
        lightSources.push_back(aboveLight);
    }

    for(int i = 1; i<= intermediatePoints; i++){
        glm::vec3 belowLight = centerLightSource;
        belowLight.y -= 0.1/intermediatePoints * i;
        lightSources.push_back(belowLight);
    }

    for(int i = 1; i<= intermediatePoints; i++){
        glm::vec3 leftLight = centerLightSource;
        leftLight.x -= (0.1/intermediatePoints * i);
        lightSources.push_back(leftLight);
    }

    for(int i = 1; i<= intermediatePoints; i++){
        glm::vec3 rightLight = centerLightSource;
        rightLight.x += (0.1/intermediatePoints * i);
        lightSources.push_back(rightLight);
    }

    for(int i = 1; i<= intermediatePoints; i++){
        glm::vec3 forwardLight = centerLightSource;
        forwardLight.z += (0.1/intermediatePoints * i);
        lightSources.push_back(forwardLight);
    }

    for(int i = 1; i<= intermediatePoints; i++){
        glm::vec3 backwardLight = centerLightSource;
        backwardLight.z -= (0.1/intermediatePoints * i);
        lightSources.push_back(backwardLight);
    }

    return lightSources;
}

int main(int argc, char *argv[]) {
    DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
    SDL_Event event;

    float **Depth2DArray = new float*[WIDTH];
    for(int i = 0; i < WIDTH; i++){
        Depth2DArray[i] = new float[HEIGHT]();
    }

    BasicShapeDrawer *basicDrawer = new BasicShapeDrawer(window, Depth2DArray);
    Camera *camera = new Camera();

    Renderer *renderer = new Renderer(*basicDrawer, *camera);


    //std::vector<ModelTriangle> triangles = readOBJ(0.17); // Read for sphere

    std::unordered_map<std::string, Material> materials = readMaterialOBJ(); //Read for box
    std::vector<ModelTriangle> triangles = readOBJ(materials, 0.17,"InputFiles/modified2.obj");
    //std::vector<ModelTriangle> triangles = readOBJ(materials,0.17);

    bool orbiting = false;

    bool run = true;

    int mode = 3;

    //std::vector<glm::vec3> lightSources = {glm::vec3(-0.4, 0.4, 1)}; // Light source for sphere
    //std::vector<glm::vec3> lightSources = getMultipleLightSources(glm::vec3(0, 0.36, 0.1)); // Light source for box
    std::vector<glm::vec3> lightSources = {glm::vec3(0,0.4,0.2)};

    PhotonMap *photonMap = new PhotonMap(triangles, *camera, lightSources, basicDrawer);
    photonMap->generateIlluminationPoints(50000);
    renderer->photonMap = photonMap;




//    CanvasPoint point0 = CanvasPoint(160,10);
//    point0.texturePoint = TexturePoint(195,5);
//    CanvasPoint point1 = CanvasPoint(300,230);
//    point1.texturePoint = TexturePoint(395,380);
//    CanvasPoint point2 = CanvasPoint(10, 150);
//    point2.texturePoint = TexturePoint(65,330);

    //TextureDrawer textureDrawer = TextureDrawer(*basicDrawer);
    //textureDrawer.drawTexturedTriangle(CanvasTriangle(point0, point1, point2), new TextureMap("InputFiles/texture.ppm"));

    int totalFrames = 120;

    std::vector<glm::vec3> cameraPositions = interpolateThreeElementValues(camera->cameraPosition, glm::vec3(0,0,0), totalFrames);

//    for(int i = 0; i < totalFrames; i++){
//        basicDrawer->display->clearPixels();
//        camera->cameraPosition = cameraPositions[i];
//        renderer->renderRayTraced(triangles, lightSources, 3);
//        basicDrawer->display->renderFrame();
//        std::string filename = "outputAnimation/"+ std::to_string(i) + "_.ppm";
//        basicDrawer->display->savePPM(filename);
//        std::cout << i << '\n';
//    }



// Normal RUN
    while (run) {
        // We MUST poll for events - otherwise the window will freeze !
        if (window.pollForInputEvents(event)) handleEvent(event, camera, orbiting, run, basicDrawer, lightSources);
        //std::cout << camera->cameraPosition << '\n';

        switch (mode) {
            case 0:
                basicDrawer->clearCanvas();
                renderer->renderPointCloud(triangles);
                break;
            case 1:
                basicDrawer->clearCanvas();
                renderer->renderWireframe(triangles);
                break;
            case 2:
                basicDrawer->clearCanvas();
                renderer->renderRasterised(triangles);
                break;
            case 3:
                basicDrawer->clearCanvas();
                renderer->renderRayTraced(triangles, lightSources);
                break;
            case 4:
                basicDrawer->clearCanvas();
                renderer->renderRayTraced(triangles, lightSources, 2);
                break; // Gourand
            case 5:
                basicDrawer->clearCanvas();
                renderer->renderRayTraced(triangles, lightSources, 3);
                break;
            case 6:
                break;
            default:
                basicDrawer->clearCanvas();
                renderer->renderRayTraced(triangles);
        }


        if(orbiting){
            camera->orbit(glm::vec3(0,0,0));
        }

        window.renderFrame();
        //run = false;
    }

    for(int i = 0; i < WIDTH; i++)
        delete [] Depth2DArray[i];

    delete [] Depth2DArray;

    delete basicDrawer;
    delete camera;
    delete renderer->textureDrawer;
    delete renderer;
}
