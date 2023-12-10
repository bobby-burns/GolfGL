#include "CSCIx229.h"
#include "GameManager.h"
#include "utils.hpp"
#include "GolfBall.h"
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
#include <iostream>

GameManager gm;


static void display() {
  
  gm.calculateDeltaTime();


  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();

  glEnable(GL_DEPTH_TEST);
  

  gm.camera();
  gm.draw();

  glWindowPos2i(5,5);
  utils::Print("%f",gm.getGolfBall()->getPos().y);
  
   glFlush();
  glutSwapBuffers();

}

void initLighting(){
  glEnable(GL_LIGHT0);
  glClearColor(0.0,0.0,0.0,0.0);
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);

    // Create light components
    GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat diffuseLight[] = { 0.8f, 0.8f, 0.8, 1.0f };
    GLfloat specularLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat position[] = { 0.0f, 10.0f, -5.0f, 1.0f };

    // Assign created components to GL_LIGHT0
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
}



void initGame(){



  gm = GameManager();

  initLighting();

  gm.setGameState(utils::AIMING);
  

}

void specialC(int key, int x, int y){
  gm.specialCallback(key);
  glutPostRedisplay();
}

void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   double asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, RES*width,RES*height);

   glMatrixMode(GL_PROJECTION);

   glLoadIdentity();

   gluPerspective(90, asp, 0.1, 200);

   glMatrixMode(GL_MODELVIEW);


   glLoadIdentity();

}


static void update(){
  
  gm.applyPhysics();


  glutPostRedisplay();


}

int main(int argc,char* argv[])
{
   //  Initialize GLUT
   glutInit(&argc,argv);

   //  Request double buffered, true color window with Z buffering at 800x600
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(800,600);
   glutCreateWindow("Robert Burns");
#ifdef USEGLEW
   //  Initialize GLEW
   if (glewInit()!=GLEW_OK)
     printf("Error initializing GLEW\n");

#endif
    
    
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutIdleFunc(update);
   glutSpecialFunc(specialC);
   glEnable(GL_LIGHTING);
   glEnable(GL_NORMALIZE);
   initGame();
   glutMainLoop();
   return 0;
}
