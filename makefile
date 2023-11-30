# Example 21
EXE=main

# Main target
all: $(EXE)

#  Msys/MinGW
ifeq "$(OS)" "Windows_NT"
CFLG=-O3 -Wall -DUSEGLEW
LIBS=-lfreeglut -lglew32 -lglu32 -lopengl32 -lm
CLEAN=rm -f *.exe *.o *.a
else
#  OSX
ifeq "$(shell uname)" "Darwin"
RES=$(shell uname -r|sed -E 's/(.).*/\1/'|tr 12 21)
CFLG= -Wall -Wno-deprecated-declarations -DRES=$(RES) 
LIBS=-framework GLUT -framework OpenGL
#  Linux/Unix/Solaris
else
CFLG=-O3 -Wall
LIBS=-lglut -lGLU -lGL -lm
endif
#  OSX/Linux/Unix/Solaris
CLEAN=rm -f $(EXE) *.o *.a
endif

# Dependencies
main.o: main.cpp
GolfBall.o: GolfBall.cpp
GameManager.o: GameManager.cpp
loadtexbmp.o: loadtexbmp.c
# Compile rules
.c.o:
	gcc -c $(CFLG)  $<
.cpp.o:
	g++ -c $(CFLG) -std=c++17 $<

#  Link
main:main.o GolfBall.o GameManager.o loadtexbmp.o
	gcc $(CFLG) -o $@ $^ -lstdc++ $(LIBS)

#  Clean
clean:
	$(CLEAN)
