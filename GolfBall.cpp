#include "GolfBall.h"
#include "utils.hpp"
#if defined(__APPLE__)
#include <GLUT/GLUT.h>
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#endif
#include <cstdio>

GolfBall::GolfBall(float _radius){
 radius = _radius;
 utils::vec3 empty = {0,0,0};
 pos = empty;
 vel = empty;
 applyGravity = true;
}

utils::vec3 GolfBall::getPos(){
  return pos;
}

void GolfBall::setPos(utils::vec3 _pos){
  pos = _pos;
}

utils::vec3 GolfBall::getVel(){
  return vel;
}

void GolfBall::setVel(utils::vec3 _vel){
  vel = _vel;
}

void GolfBall::drawBall(){
  glPushMatrix();

  glColor3f(0.8, 0.8, 0.8);
  glTranslatef(pos.x,pos.y,pos.z);

  glutSolidSphere(radius, 30, 30);

  glPopMatrix();
}

void GolfBall::doGravity(bool val){
  applyGravity = val;
}



void GolfBall::applyPhysics(double deltaTime){

   // gravity
  utils::vec3 gravity = {0,-9.8,0};

  if (applyGravity && vel.y > -9.8){
    vel = vel + gravity;
  }

  pos = pos + (vel * (float)deltaTime);
  

}




