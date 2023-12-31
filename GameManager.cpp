#include "GameManager.h"
#include "CSCIx229.h"
#include "GolfBall.h"
#include "noise.hpp"
#include "utils.hpp"
#include <climits>
#include <cmath>
#if defined(__APPLE__)
#include <GLUT/GLUT.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <random>

GameManager::GameManager() {
  startTime = 1;
  currentScene = utils::AIMING;
  gball = GolfBall();
  gameScore = 0;
  lightpos[0] = 0.0f;
  lightpos[1] = 50.0f;
  lightpos[2] = 200.0f;
  lightpos[3] = 1.0f;
  debugMode = false;
}
float GameManager::get_random() {
  static std::default_random_engine e;
  e.seed(time(0));
  static std::uniform_real_distribution<> dis(0, 1); // range [0, 1)
  return dis(e);
}
void GameManager::init() {
  skybox_tex = LoadTexBMP("cubemap.bmp");
  fairway_tex = LoadTexBMP("fairway_texture.bmp");
  target_tex = LoadTexBMP("targettexture.bmp");
  arrow_tex = LoadTexBMP("arrow.bmp");
  plane_tex = LoadTexBMP("grass.bmp");
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void GameManager::drawPlane() {
  glPushMatrix();
  glTranslatef(0, 0, 0);
  glScalef(25, 5, 5);
  glColor3f(0, 0.5, 0);
  float specReflection[] = {0.6f, 0.6f, 0.6f, 1.0f};
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specReflection);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 1);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, plane_tex);
  glBegin(GL_QUADS);
  glTexCoord2f(0,1);
  glVertex3f(-1, 0, 1);
  glTexCoord2f(0,0);
  glVertex3f(-1, 0, -1);
  glTexCoord2f(1,0);
  glVertex3f(1, 0, -1);
  glTexCoord2f(1,1);
  glVertex3f(1, 0, 1);
  glEnd();
  glPopMatrix();
  glColor3f(1, 1, 1);
}

utils::vec3 ***GameManager::genFairway() {

  float noiseScale = 35;

  utils::vec3 ***buff = new utils::vec3 **[fairway_length];

  float r_offset = get_random() * 8;

  for (size_t i = 0; i < fairway_length; ++i) {
    buff[i] = new utils::vec3 *[fairway_width];
    for (size_t j = 0; j < fairway_width; ++j) {
      buff[i][j] = new utils::vec3[6];
      buff[i][j][0] = (utils::vec3){
          (float)i,
          noise::perlin(((float)i / (float)fairway_length + r_offset),
                        (float)(j - 1) / (float)fairway_width) *
                  noiseScale +
              r_offset,
          (float)j - 1};
      buff[i][j][1] = (utils::vec3){
          (float)i + 1,
          noise::perlin(((float)(i + 1) / (float)fairway_length + r_offset),
                        (float)(j - 1) / (float)fairway_width) *
                  noiseScale +
              r_offset,
          (float)j - 1};
      buff[i][j][2] = (utils::vec3){
          (float)i,
          noise::perlin(((float)i / (float)fairway_length + r_offset),
                        (float)j / (float)fairway_width) *
                  noiseScale +
              r_offset,
          (float)j};

      buff[i][j][3] = (utils::vec3){
          (float)i + 1,
          noise::perlin(((float)(i + 1) / (float)fairway_length + r_offset),
                        (float)(j - 1) / (float)fairway_width) *
                  noiseScale +
              r_offset,
          (float)j - 1};
      buff[i][j][4] = (utils::vec3){
          (float)i + 1,
          noise::perlin(((float)(i + 1) / (float)fairway_length + r_offset),
                        (float)j / (float)fairway_width) *
                  noiseScale +
              r_offset,
          (float)j};
      buff[i][j][5] = (utils::vec3){
          (float)i,
          noise::perlin(((float)i / (float)fairway_length + r_offset),
                        (float)j / (float)fairway_width) *
                  noiseScale +
              r_offset,
          (float)j};
    }
  }

  return buff;
}

utils::vec3 getTriNormal(utils::vec3 p1, utils::vec3 p2, utils::vec3 p3) {

  utils::vec3 A = {.x = p2.x - p1.x, .y = p2.y - p1.y, .z = p2.z - p1.z};

  utils::vec3 B = {.x = p3.x - p1.x, .y = p3.y - p1.y, .z = p3.z - p1.z};

  return (utils::vec3){.x = A.y * B.z - A.z * B.y,
                       .y = A.z * B.x - A.x * B.z,
                       .z = A.x * B.y - A.y * B.x};
}

void drawLight(GLfloat position[]) {

  glPushMatrix();
  glTranslatef(position[0], position[1], position[2]);

  glutSolidSphere(3, 10, 10);
  glPopMatrix();
}

void GameManager::drawFairway(utils::vec3 ***verts) {

  glPushMatrix();
  glTranslatef(-100, -20, 20);

  float specReflection[] = {0.6f, 0.6f, 0.6f, 1.0f};
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specReflection);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 1);

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, fairway_tex);

  glColor3f(1, 1, 1);

  glBegin(GL_TRIANGLES);
  for (size_t i = 0; i < fairway_length; ++i) {
    for (size_t j = 0; j < fairway_width; ++j) {

      utils::vec3 norm =
          getTriNormal(verts[i][j][0], verts[i][j][1], verts[i][j][2]);

      glNormal3f(norm.x, -norm.y, norm.z);

      glTexCoord2f(0, 0);
      glVertex3f(verts[i][j][0].x, verts[i][j][0].y, verts[i][j][0].z);

      glTexCoord2f(1, 0);
      glVertex3f(verts[i][j][1].x, verts[i][j][1].y, verts[i][j][1].z);

      glTexCoord2f(0, 1);
      glVertex3f(verts[i][j][2].x, verts[i][j][2].y, verts[i][j][2].z);

      norm = getTriNormal(verts[i][j][3], verts[i][j][4], verts[i][j][5]);

      glNormal3f(norm.x, -norm.y, norm.z);

      glTexCoord2f(1, 0);
      glVertex3f(verts[i][j][3].x, verts[i][j][3].y, verts[i][j][3].z);

      glTexCoord2f(1, 1);
      glVertex3f(verts[i][j][4].x, verts[i][j][4].y, verts[i][j][4].z);

      glTexCoord2f(0, 1);
      glVertex3f(verts[i][j][5].x, verts[i][j][5].y, verts[i][j][5].z);
    }
  }
  glEnd();
  glPopMatrix();

  glDisable(GL_TEXTURE_2D);
}

void GameManager::setGameState(utils::Scenes scene) {

  switch (scene) {
  case utils::AIMING: {
    arrow_angle = 0;
    should_spin = true;
    spin_rate = utils::PI / 2;
    true_spin_rate = spin_rate;
    gball.doGravity(false);
    gball.setVel((utils::vec3){0, 0, 0});
    gball.setPos((utils::vec3){0, 0.5, 0});
    gball.setAngle(0);
    setCameraPos((utils::vec3){0, 5, -5});
    fairway_width = 500;
    fairway_length = 200;
    fairwayVerts = genFairway();
    genTarget();
    powerRate = 0;
    barPos = 0;
    break;
  }
  case utils::POWER: {
    should_spin = false;
    barPos = 0;
    powerRate = 0.7;
    break;
  }
  case utils::HIT: {
    gball.doGravity(true);
    gball.setPos((utils::vec3){0, 0.5, 0});
    break;
  }
  }

  currentScene = scene;
}

void GameManager::drawArrow() {

  float apos = spinArrow();
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, arrow_tex);
  glEnable(GL_BLEND);
  glBlendEquation(GL_MIN);
  glPushMatrix();
  glTranslatef(0, 0, 0);
  glRotatef(apos * 180, 0, 1, 0);
  glScalef(1, 1, 5);
  glColor3f(1, 1, 1);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0);
  glVertex3f(-0.5, 1, 1); // change
  glTexCoord2f(1, 0);
  glVertex3f(-0.5, 0.5, 0);
  glTexCoord2f(1, 1);
  glVertex3f(0.5, 0.5, 0);
  glTexCoord2f(0, 1);
  glVertex3f(0.5, 1, 1); // change
  glEnd();

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);

  glPopMatrix();
}

float GameManager::spinArrow() {

  if (arrow_angle < 0) {
    true_spin_rate = spin_rate;
  } else if (arrow_angle > utils::PI) {
    true_spin_rate = -spin_rate;
  }
  if (should_spin)
    arrow_angle += true_spin_rate * dt();
  return cos(arrow_angle) / 2;
}

void GameManager::skyBox() {
  // glDepthRange(1, 1);
  glDisable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, skybox_tex);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  glPushMatrix();
  glTranslatef(gball.getPos().x, gball.getPos().y, gball.getPos().z);
  glScalef(1000, 1000, 1000);
  glColor3f(1.0, 1.0, 1.0);
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

  glTexCoord2f(0.749, 0.34);
  glVertex3f(1.0, -1.0, 1.0);

  glTexCoord2f(0.749, 0.659);
  glVertex3f(1.0, 1.0, 1.0);

  // Left Face
  glTexCoord2f(0, 0.659);
  glVertex3f(-1.0, 1.0, 1.0);

  glTexCoord2f(0, 0.34);
  glVertex3f(-1.0, -1.0, 1.0);

  glTexCoord2f(0.25, 0.34);
  glVertex3f(-1.0, -1.0, -1.0);

  glTexCoord2f(0.25, 0.659);
  glVertex3f(-1.0, 1.0, -1.0);

  // Top Face
  glTexCoord2f(0.259, 0.9999);
  glVertex3f(-1.0, 1.0, 1.0);

  glTexCoord2f(0.259, 0.659);
  glVertex3f(-1.0, 1.0, -1.0);

  glTexCoord2f(0.4999, 0.659);
  glVertex3f(1.0, 1.0, -1.0);

  glTexCoord2f(0.4999, 0.9999);
  glVertex3f(1.0, 1.0, 1.0);

  // Bottom Face
  glTexCoord2f(0.259, 0.334);
  glVertex3f(-1.0, -1.0, -1.0);

  glTexCoord2f(0.259, 0.001);
  glVertex3f(-1.0, -1.0, 1.0);

  glTexCoord2f(0.499, 0.001);
  glVertex3f(1.0, -1.0, 1.0);

  glTexCoord2f(0.499, 0.334);
  glVertex3f(1.0, -1.0, -1.0);

  // Back Face
  glTexCoord2f(0.75, 0.659);
  glVertex3f(1.0, 1.0, 1.0);

  glTexCoord2f(0.75, 0.34);
  glVertex3f(1.0, -1.0, 1.0);

  glTexCoord2f(0.99, 0.34);
  glVertex3f(-1.0, -1.0, 1.0);

  glTexCoord2f(0.99, 0.659);
  glVertex3f(-1.0, 1.0, 1.0);

  glEnd();

  glDisable(GL_TEXTURE_2D);
  glEnable(GL_LIGHTING);

  glPopMatrix();
  // glDepthRange(0, 1);
}
void GameManager::moveSelector() {
  if (barPos < 0) {
    powerRate = -powerRate;
    barPos = 0;
  } else if (barPos > 1) {
    powerRate = -powerRate;
    barPos = 1;
  }
  barPos += powerRate * dt();
}

void GameManager::drawPowerBar() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(-1000, 1000, -1000, 1000);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  float width = glutGet(GLUT_WINDOW_WIDTH);
  float height = glutGet(GLUT_WINDOW_HEIGHT);

  glDisable(GL_LIGHTING);

  float barHeight = 500;
  float barWidth = 20;

  float w_margin = 20;
  glBegin(GL_QUADS);
  glColor3f(1, 0, 0);
  glVertex3f(1000 - barWidth - w_margin, barHeight / 2, 0.0f);
  glColor3f(0, 1, 0);
  glVertex3f(1000 - barWidth - w_margin, -barHeight / 2, 0.0f);
  glVertex3f(1000 - 20, -barHeight / 2, 0.0f);
  glColor3f(1, 0, 0);
  glVertex3f(1000 - 20, barHeight / 2, 0.0f);
  glEnd();

  moveSelector();
  float selector_ypos = (barPos * barHeight) - barHeight / 2;

  glBegin(GL_TRIANGLES);
  glColor3f(1, 1, 1);
  glVertex3f(1000 - barWidth - w_margin - 15, selector_ypos + 10, 0.0f);
  glVertex3f(1000 - barWidth - w_margin - 5, selector_ypos, 0.0f);
  glVertex3f(1000 - barWidth - w_margin - 15, selector_ypos - 10, 0.0f);
  glEnd();

  double asp = (height > 0) ? (double)width / height : 1;
  //  Set the viewport to the entire window
  glViewport(0, 0, RES * width, RES * height);

  glMatrixMode(GL_PROJECTION);

  glLoadIdentity();

  gluPerspective(90, asp, 0.1, 1500);

  glMatrixMode(GL_MODELVIEW);

  glLoadIdentity();
  glEnable(GL_LIGHTING);
}

float getDistance(utils::vec3 a, utils::vec3 b) {
  return sqrtf(pow(b.x - a.x, 2) + pow(b.z - a.z, 2));
}

float GameManager::getClosestVertexHeight() {
  utils::vec3 closestHeight = {0};
  float closestDistance = 1000;
  for (int i = 0; i < fairway_length; i++) {
    for (int j = 0; j < fairway_width; j++) {
      for (int k = 0; k < 6; k++) {
        float dist = getDistance(fairwayVerts[i][j][k], targetPos);
        if (dist < closestDistance) {
          closestDistance = dist;
          closestHeight = fairwayVerts[i][j][k];
        }
      }
    }
  }
  return closestHeight.y;
}

void GameManager::drawCollider() {
  glPushMatrix();
  glDisable(GL_LIGHTING);
  glColor3f(1, 1, 1);
  glTranslatef(targetPos.x - 100, targetPos.y - 20, targetPos.z + 20);
  glutWireCube(14);
  glPopMatrix();
  glPushMatrix();
  glTranslatef(gball.getPos().x,gball.getPos().y,gball.getPos().z);
  glutWireCube(2);
  glPopMatrix();
  glEnable(GL_LIGHTING);
}

void GameManager::genTarget() {
  targetPos = (utils::vec3){(fairway_length * get_random()), 0,
                            (fairway_width * get_random())};
  float height = getClosestVertexHeight();
  targetPos.y = height + 7;

  lightpos[0] = targetPos.x - 100;
  lightpos[1] = targetPos.y + 30;
  lightpos[2] = targetPos.z + 20;
}
void GameManager::drawTarget() {
  glPushMatrix();
  glTranslatef(targetPos.x - 100, targetPos.y - 20, targetPos.z + 20);
  glRotatef(45, 1, 0, 0);
  const float twoPI = 2 * utils::PI;
  const float radius = 7;
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, target_tex);
  glBegin(GL_TRIANGLE_FAN);
  glNormal3f(0, 0, -1);
  glTexCoord2f(0.5, 0.5);
  glVertex3f(0, 0, 0);
  for (int i = 0; i <= 50; i++) {
    glTexCoord2f((cos(i * twoPI / 50) + 1) / 2, (sin(i * twoPI / 50) + 1) / 2);
    glVertex3f(radius * cos(i * twoPI / 50), radius * sin(i * twoPI / 50), 0);
  }
  glEnd();

  glDisable(GL_TEXTURE_2D);
  glPopMatrix();
}

bool GameManager::hasCollided() {
  utils::vec3 targc =
      (utils::vec3){targetPos.x - 100, targetPos.y - 20, targetPos.z + 20};
  utils::vec3 gpos = gball.getPos();

  return (targc.x - 7 <= gpos.x + 0.5 && targc.x + 7 >= gpos.x - 0.5 &&
          targc.y - 7 <= gpos.y + 0.5 && targc.y + 7 >= gpos.y - 0.5 &&
          targc.z - 7 <= gpos.z + 0.5 && targc.z + 7 >= gpos.z - 0.5);
}

void GameManager::checkCollisions() {
  if (hasCollided()) {
    gameScore++;
    setGameState(utils::AIMING);
  }
}

void GameManager::draw() {

  if (debugMode)
    drawLight(lightpos);
  Lighting();

  switch (currentScene) {
  case utils::AIMING: {
    skyBox();
    gball.drawBall();
    drawPlane();
    drawFairway(fairwayVerts);
    drawArrow();
    drawTarget();
    if (debugMode)
      drawCollider();
    drawPowerBar();
    break;
  };
  case utils::POWER: {
    skyBox();
    gball.drawBall();
    drawPlane();
    drawFairway(fairwayVerts);
    drawArrow();
    drawTarget();
    if (debugMode)
      drawCollider();
    drawPowerBar();
    break;
  };
  case utils::HIT: {
    skyBox();
    gball.drawBall();
    drawFairway(fairwayVerts);
    drawArrow();
    drawTarget();
    if (debugMode)
      drawCollider();
    break;
  };
  }
}

void GameManager::aimBall() { setGameState(utils::POWER); }

void GameManager::hitBall() {
  utils::vec3 hitVec = {0, 4 * barPos, 5 * barPos};
  gball.setVel(hitVec);
  gball.setAngle(arrow_angle);
  setGameState(utils::HIT);
}

void GameManager::Lighting() {
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_LIGHT0);

  // Create light components
  GLfloat ambientLight[] = {0.8f, 0.2f, 0.2f, 1.0f};
  GLfloat diffuseLight[] = {1.0f, 0.5f, 0.5f, 1.0f};
  GLfloat specularLight[] = {1.0f, 0.3f, 0.3f, 1.0f};

  // Assign created components to GL_LIGHT0
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
  glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
  glLightfv(GL_LIGHT0, GL_POSITION, getLightPos());

  glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
  glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.000001f);
  glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0004f);

  glEnable(GL_LIGHT1);

  GLfloat stationary_pos[] = {0.0, 10, -20, 1.0f};

  GLfloat ambientLight1[] = {0.4f, 0.4f, 0.4f, 1.0f};
  GLfloat diffuseLight1[] = {0.8f, 0.8f, 0.8f, 1.0f};
  GLfloat specularLight1[] = {0.5f, 0.5f, 0.5f, 1.0f};
  // Assign created components to GL_LIGHT0
  glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight1);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight1);
  glLightfv(GL_LIGHT1, GL_SPECULAR, specularLight1);
  glLightfv(GL_LIGHT1, GL_POSITION, stationary_pos);

  glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
  glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.000001f);
  glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0004f);
}

void GameManager::specialCallback(int key) {
  if (debugMode) {
    switch (key) {
    case GLUT_KEY_PAGE_UP: {
      gball.setPos((utils::vec3){gball.getPos().x, gball.getPos().y + 2,
                                 gball.getPos().z});
      break;
    }
    case GLUT_KEY_PAGE_DOWN: {
      gball.setPos((utils::vec3){gball.getPos().x, gball.getPos().y - 2,
                                 gball.getPos().z});
      break;
    }
    case GLUT_KEY_UP: {
      gball.setPos((utils::vec3){gball.getPos().x, gball.getPos().y,
                                 gball.getPos().z + 2});
      break;
    }
    case GLUT_KEY_DOWN: {
      gball.setPos((utils::vec3){gball.getPos().x, gball.getPos().y,
                                 gball.getPos().z - 2});
      break;
    }
    case GLUT_KEY_LEFT: {
      gball.setPos((utils::vec3){gball.getPos().x + 2, gball.getPos().y,
                                 gball.getPos().z});
      break;
    }
    case GLUT_KEY_RIGHT: {
      gball.setPos((utils::vec3){gball.getPos().x - 2, gball.getPos().y,
                                 gball.getPos().z});
      break;
    }
    default:
      break;
    }
  }
}

void GameManager::keyboardCallback(unsigned char key, int x, int y) {
  (void)x;
  (void)y;

  switch (key) {
  case ' ': {

    if (currentScene == utils::AIMING) {
      aimBall();
    } else if (currentScene == utils::POWER) {
      hitBall();
    }
    break;
  }
  case 'g': {
    debugMode = !debugMode;
    break;
  }
  }
}

void GameManager::setCameraPos(utils::vec3 _pos) { cameraPos = _pos; }

utils::Scenes GameManager::getGameState() { return currentScene; }

void GameManager::camera() {
  if (currentScene == utils::HIT) {
    gluLookAt(gball.getPos().x, gball.getPos().y + 5, gball.getPos().z - 10,
              gball.getPos().x, gball.getPos().y, gball.getPos().z, 0, 1, 0);
  } else {
    gluLookAt(gball.getPos().x, gball.getPos().y + 5, gball.getPos().z - 5,
              gball.getPos().x, gball.getPos().y + 2, gball.getPos().z, 0, 1,
              0);
  }
}

void GameManager::calculateDeltaTime() {
  int endTime = glutGet(GLUT_ELAPSED_TIME);
  deltaTime = (endTime - startTime);
  startTime = glutGet(GLUT_ELAPSED_TIME);
}

double GameManager::dt() { return deltaTime / 1000; }

GolfBall *GameManager::getGolfBall() { return &gball; }

void GameManager::applyPhysics() {
  if (currentScene == utils::HIT) {
    if (gball.getPos().y + 30 < targetPos.y ||
        gball.getPos().x < -(float)fairway_length / 2 ||
        gball.getPos().x > (float)fairway_length / 2 ||
        gball.getPos().z > targetPos.z + 20) {
      setGameState(utils::AIMING);
    }
  }
  gball.applyPhysics(dt());
}

GLfloat *GameManager::getLightPos() { return lightpos; }
