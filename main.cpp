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

  glColor3f(1,1,1);
  glWindowPos2i(5,5);
  utils::Print("%f",gm.getGolfBall()->getPos().y);
  
   glFlush();
  glutSwapBuffers();

}




void initGame(){



  gm = GameManager();

  gm.Lighting();

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

   gluPerspective(90, asp, 0.1, 1500);

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
   glutInitWindowSize(1920,1080);
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
