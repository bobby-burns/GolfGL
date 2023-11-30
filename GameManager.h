#pragma once

#include "utils.hpp"
#include "GolfBall.h"
#include <memory>

class GameManager {
  public:
    GameManager();
    
    GolfBall* getGolfBall();

    void setCameraPos(utils::vec3 pos);
    void setGameState(utils::Scenes scene);

    void calculateDeltaTime();
    void applyPhysics();

    void specialCallback(int key);

    // get delta time
    double dt();
    void draw();
    void camera();
  private:
    GolfBall gball;
    utils::vec3 cameraPos;
    double deltaTime;
    int startTime;
    utils::Scenes currentScene;
    utils::vec3*** fairwayVerts;
    



};
