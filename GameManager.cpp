#include "GameManager.h"
#include "utils.hpp"
#include "noise.hpp"
#include "GolfBall.h"
#include "CSCIx229.h"
#if defined(__APPLE__)
#include <GLUT/GLUT.h>
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#endif
#include <cstddef>
#include <cstdio>
#include <memory>

GameManager::GameManager() {
  startTime = 1;
  currentScene = utils::AIMING;
  gball = GolfBall();
}


void drawPlane(){
  glPushMatrix();
  glTranslatef(0, 0, 0);
  glScalef(25,5,5);
  glColor3f(0, 1, 0);
  glBegin(GL_QUADS);
  glVertex3f(-1,0,-1);
  glVertex3f(1,0,-1);
  glVertex3f(1,0,1);
  glVertex3f(-1,0,1);
  glEnd();
  glPopMatrix();
}


utils::vec3*** genFairway(int width, int length){

  float noiseScale = 17;

  utils::vec3*** buff = new utils::vec3 **[length];

  for (size_t i = 0; i < length; ++i){
    buff[i] = new utils::vec3 *[width];
    for (size_t j = 0; j < width; ++j){
      buff[i][j] = new utils::vec3 [6];
      buff[i][j][0] = (utils::vec3){(float)i,noise::perlin(((float)i/(float)length), (float)(j-1)/(float)width)*noiseScale,(float)j-1};
      buff[i][j][1] = (utils::vec3){(float)i+1,noise::perlin(((float)(i+1)/(float)length), (float)(j-1)/(float)width)*noiseScale,(float)j-1};
      buff[i][j][2] = (utils::vec3){(float)i,noise::perlin(((float)i/(float)length), (float)j/(float)width)*noiseScale,(float)j};

      buff[i][j][3] = (utils::vec3){(float)i+1, noise::perlin(((float)(i+1)/(float)length), (float)(j-1)/(float)width)*noiseScale,(float)j-1};
      buff[i][j][4] = (utils::vec3){(float)i+1,noise::perlin(((float)(i+1)/(float)length), (float)j/(float)width)*noiseScale,(float)j};
      buff[i][j][5] = (utils::vec3){(float)i,noise::perlin(((float)i/(float)length), (float)j/(float)width)*noiseScale,(float)j};



    }
  }

  return buff;


}

utils::vec3 getTriNormal(utils::vec3 p1,utils::vec3 p2, utils::vec3 p3){

  utils::vec3 A = {
    .x = p2.x - p1.x,
    .y = p2.y - p1.y,
    .z = p2.z - p1.z
  };

  utils::vec3 B = {
    .x = p3.x - p1.x,
    .y = p3.y - p1.y,
    .z = p3.z - p1.z
  };

  return (utils::vec3){
    .x = A.y * B.z - A.z * B.y,
      .y = A.z * B.x - A.x * B.z,
      .z = A.x * B.y - A.y * B.x
  };

}



void GameManager::drawFairway(utils::vec3*** verts) {


  glPushMatrix();
  glTranslatef(-50, -10, 20);

  int width = 500;
  int length = 100;



  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, fairway_tex);

  glBegin(GL_TRIANGLES);
  float specReflection[] = { 0.8f, 0.8f, 0.8f, 1.0f };
  glMaterialfv(GL_FRONT, GL_SPECULAR, specReflection);
  for (size_t i = 0; i < length; ++i){
    for (size_t j = 0; j < width; ++j){


      utils::vec3 norm = getTriNormal(verts[i][j][0], verts[i][j][1], verts[i][j][2]);

      glNormal3f(norm.x, norm.y, norm.z);

      glTexCoord2f(0,0);
      glVertex3f(verts[i][j][0].x, verts[i][j][0].y, verts[i][j][0].z);

      glTexCoord2f(1,0);
      glVertex3f(verts[i][j][1].x, verts[i][j][1].y, verts[i][j][1].z);

      glTexCoord2f(0,1);
      glVertex3f(verts[i][j][2].x, verts[i][j][2].y, verts[i][j][2].z);


      norm = getTriNormal(verts[i][j][3], verts[i][j][4], verts[i][j][5]);

      glNormal3f(norm.x, norm.y, norm.z);

      glTexCoord2f(1,0);
      glVertex3f(verts[i][j][3].x, verts[i][j][3].y, verts[i][j][3].z);

      glTexCoord2f(1,1);
      glVertex3f(verts[i][j][4].x, verts[i][j][4].y, verts[i][j][4].z);

      glTexCoord2f(0,1);
      glVertex3f(verts[i][j][5].x, verts[i][j][5].y, verts[i][j][5].z);


    }
  }
  glEnd();
  glPopMatrix();

  glDisable(GL_TEXTURE_2D);
}

void GameManager::setGameState(utils::Scenes scene){

  switch(scene){
    case utils::AIMING: {
                          skybox_tex = LoadTexBMP("cubemap.bmp");
                          fairway_tex = LoadTexBMP("fairway_texture.bmp");
                          gball.doGravity(false);
                          setCameraPos((utils::vec3){0,5,-10});
                          fairwayVerts = genFairway(500, 100);
                          break;
                        }
  }


  currentScene = scene;

}
void GameManager::skyBox(){
  // glDepthRange(1, 1);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, skybox_tex);
  glPushMatrix();
  glScalef(100, 100, 100);
  glTranslatef(0.0, 0.0, 0.0);
  glColor3f(1.0,1.0,1.0);
  glBegin(GL_QUADS);
    // Front face
    glTexCoord2f(0.251, 0.666);
    glVertex3f(-1.0, 1.0, -1.0);

    glTexCoord2f(0.251, 0.333);
    glVertex3f(-1.0, -1.0, -1.0);

    glTexCoord2f(0.499, 0.333);
    glVertex3f(1.0, -1.0, -1.0);

    glTexCoord2f(0.499, 0.666);
    glVertex3f(1.0, 1.0, -1.0);


    // Right Face
    glTexCoord2f(0.49, 0.659);
    glVertex3f(1.0, 1.0, -1.0);

    glTexCoord2f(0.49, 0.34);
    glVertex3f(1.0, -1.0, -1.0);

    glTexCoord2f(0.74, 0.34);
    glVertex3f(1.0, -1.0, 1.0);

    glTexCoord2f(0.74, 0.659);
    glVertex3f(1.0, 1.0, 1.0);


    // Left Face
    glTexCoord2f(0, 0.659);
    glVertex3f(-1.0,1.0,1.0);

    glTexCoord2f(0, 0.34);
    glVertex3f(-1.0,-1.0,1.0);

    glTexCoord2f(0.25, 0.34);
    glVertex3f(-1.0,-1.0,-1.0);

    glTexCoord2f(0.25, 0.659);
    glVertex3f(-1.0,1.0,-1.0);


    // Top Face
    glTexCoord2f(0.259,0.9999);
    glVertex3f(-1.0,1.0,1.0);

    glTexCoord2f(0.259,0.659);
    glVertex3f(-1.0,1.0,-1.0);

    glTexCoord2f(0.4999,0.659);
    glVertex3f(1.0,1.0,-1.0);

    glTexCoord2f(0.4999,0.9999);
    glVertex3f(1.0,1.0,1.0);

    // Bottom Face
    glTexCoord2f(0.259, 0.34);
    glVertex3f(-1.0, -1.0, -1.0);

    glTexCoord2f(0.259, 0.01);
    glVertex3f(-1.0, -1.0, 1.0);

    glTexCoord2f(0.49, 0.01);
    glVertex3f(1.0, -1.0, 1.0);

    glTexCoord2f(0.49, 0.34);
    glVertex3f(1.0, -1.0, -1.0);

    
    // Back Face 
    glTexCoord2f(0.75, 0.659);
    glVertex3f(1.0,1.0,1.0);

    glTexCoord2f(0.75, 0.34);
    glVertex3f(1.0,-1.0,1.0);

    glTexCoord2f(0.99, 0.34);
    glVertex3f(-1.0,-1.0,1.0);

    glTexCoord2f(0.99, 0.659);
    glVertex3f(-1.0,1.0,1.0);

    
  glEnd();
  
  glDisable(GL_TEXTURE_2D);

  glPopMatrix();
  // glDepthRange(0, 1);
}

void GameManager::draw() {

  switch(currentScene){
    case utils::AIMING: {
                          skyBox();
                          gball.drawBall();
                          drawPlane();
                          drawFairway(fairwayVerts);
                          break;
                        };
  }
}

void GameManager::specialCallback(int key){
  switch (key) {
    case GLUT_KEY_RIGHT: {
                           cameraPos.x -= 1;
                           break;
                         }
    case GLUT_KEY_LEFT: {
                          cameraPos.x += 1;
                          break;
                        }
    case GLUT_KEY_UP: {
                        cameraPos.z += 1;
                        break;
                      }
    case GLUT_KEY_DOWN: {
                          cameraPos.z -= 1;
                          break;
                        }
    default:
                        break;
  }
}

void GameManager::setCameraPos(utils::vec3 _pos){
  cameraPos = _pos;
}

void GameManager::camera(){
  gluLookAt(cameraPos.x, cameraPos.y, cameraPos.z, 0,3,0, 0, 1, 0);
}

void GameManager::calculateDeltaTime() {
  int endTime = glutGet(GLUT_ELAPSED_TIME);
  deltaTime = (endTime - startTime) ;
  startTime = glutGet(GLUT_ELAPSED_TIME);
}

double GameManager::dt(){
  return deltaTime/1000;
}

GolfBall* GameManager::getGolfBall(){
  return &gball;
}

void GameManager::applyPhysics(){
  gball.applyPhysics(dt());
}




