#pragma once

#include "utils.hpp"

class GolfBall {
  
  public:
    GolfBall(float radius = 2);
    // Position getter and setter
    utils::vec3 getPos();
    void setPos(utils::vec3 pos);

    // Velocity getter and setter
    utils::vec3 getVel();
    void setVel(utils::vec3 vel);

    //OpenGL
    void drawBall();

    void applyPhysics(double deltaTime);
    void doGravity(bool val);
  private:
    utils::vec3 pos;
    utils::vec3 vel;
    float radius;
    bool applyGravity;


};
