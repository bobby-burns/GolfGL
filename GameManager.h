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
    GLfloat* getLightPos();
    void Lighting();
  private:
    void skyBox();
    void drawPowerBar();
    void drawFairway(utils::vec3*** verts);
    void drawArrow();
    float spinArrow();
    void moveSelector();
    void hitBall();
    void aimBall();
    GolfBall gball;
    utils::vec3 cameraPos;
    double deltaTime;
    int startTime;
    utils::Scenes currentScene;
    utils::vec3*** fairwayVerts;
    unsigned int fairway_tex;
    unsigned int skybox_tex;
    float arrow_angle;
    float spin_rate;
    float true_spin_rate;
    bool should_spin;
    GLfloat lightpos[4];
    float barPos;
    float powerRate;

    



};
