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
 angle = 0;
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
  // glRotatef(angle,0,1,0);
  glTranslatef(pos.x,pos.y,pos.z);

  float specReflection[] = { 0.1f, 0.1f, 0.1f, 1.0f };
  glMaterialfv(GL_FRONT, GL_SPECULAR,specReflection);
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

  glutSolidSphere(radius, 30, 30);

  glPopMatrix();
}

void GolfBall::doGravity(bool val){
  applyGravity = val;
}



void GolfBall::applyPhysics(double deltaTime){

   // gravity
  utils::vec3 gravity = {0,-5.8,0};

  utils::vec3 wind_resistance = {0,0,-2};

  if (applyGravity && vel.y > gravity.y * deltaTime * 10){
    vel.y = vel.y + gravity.y * deltaTime;
  }
  if (applyGravity && vel.z > wind_resistance.z*2*deltaTime){
    vel.z = vel.z + wind_resistance.z * deltaTime;
  }

  pos = pos + vel ;
  if (angle != 0)
  pos.x = pos.z/tan(angle);
  

}

bool GolfBall::getGravity(){
  return applyGravity;
}

void GolfBall::setAngle(float a){
  angle = a;
}






