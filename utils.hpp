#pragma once
#include <iostream>
#include <fstream>

// GLEW _MUST_ be included first
#ifdef USEGLEW
#include <GL/glew.h>
#endif
//  Get all GL prototypes
#define GL_GLEXT_PROTOTYPES
//  Select SDL, SDL2, GLFW or GLUT
#if defined(SDL2)
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#elif defined(SDL)
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#elif defined(GLFW)
#include <GLFW/glfw3.h>
#elif defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
//  Make sure GLU and GL are included
#ifdef __APPLE__
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
// Tell Xcode IDE to not gripe about OpenGL deprecation
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif
//  Default resolution
//  For Retina displays compile with -DRES=2
#ifndef RES
#define RES 1
#endif



namespace utils {

  enum Scenes {
    AIMING,
    POWER,
    HIT

  };

  static const float PI = 3.1415926;

  typedef struct vec3 {
    float x;
    float y;
    float z;

    friend vec3 operator+(vec3 v1, const vec3& v2){
      v1.x += v2.x;
      v1.y += v2.y;
      v1.z += v2.z;
      return v1;
    }

    friend vec3 operator*(vec3 v1, const float scalar){
      v1.x *= scalar;
      v1.y *= scalar;
      v1.z *= scalar;
      return v1;
    }
  } vec3;


  
#define LEN 8192  //  Maximum length of text string
static inline void Print(const char* format , ...)
{
   char    buf[LEN];
   char*   ch=buf;
   va_list args;
   //  Turn the parameters into a character string
   va_start(args,format);
   vsnprintf(buf,LEN,format,args);
   va_end(args);
   //  Display the characters one at a time at the current raster position
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}

}
