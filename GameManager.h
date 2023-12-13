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
    utils::Scenes getGameState();

    void calculateDeltaTime();
    void applyPhysics();

    void specialCallback(int key);
    void keyboardCallback(unsigned char key,int x,int y);
    void checkCollisions();
    // get delta time
    double dt();
    void draw();
    void camera();
    GLfloat* getLightPos();
    void Lighting();
    void init();
    int gameScore;
    bool debugMode;
  private:
    float get_random();
    void skyBox();
    void drawPowerBar();
    void drawPlane();
    int fairway_length;
    int fairway_width;
    utils::vec3*** fairwayVerts;
    utils::vec3*** genFairway();
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
    unsigned int fairway_tex;
    unsigned int skybox_tex;
    unsigned int target_tex;
    unsigned int arrow_tex;
    unsigned int plane_tex;
    float arrow_angle;
    float spin_rate;
    float true_spin_rate;
    bool should_spin;
    GLfloat lightpos[4];
    float barPos;
    float powerRate;

    utils::vec3 targetPos;
    void genTarget();
    void drawTarget();
    float getClosestVertexHeight();
    void drawCollider();

    bool hasCollided();
    



};
