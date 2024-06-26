//---------------------------------------
// Program: surface4.cpp
// Purpose: Use Phong shading to display
//          random wave surface model.
// Author:  John Gauch
// Date:    Spring 2012
//---------------------------------------
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef MAC
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Transformation variables 
#define ROTATE 1
#define TRANSLATE 2
int xangle = 0;
int yangle = 0;
int zangle = 0;
int xpos = 0;
int ypos = 0;
int zpos = 0;
int mode = ROTATE;

// Surface variables
#define SIZE 500
float Px[SIZE][SIZE];
float Py[SIZE][SIZE];
float Pz[SIZE][SIZE];
float Nx[SIZE][SIZE];
float Ny[SIZE][SIZE];
float Nz[SIZE][SIZE];
#define STEP 0.1

#include "shading.cpp"
#include <fstream>
#include <iostream>
using namespace std;

static float R[SIZE][SIZE];
static float G[SIZE][SIZE];
static float B[SIZE][SIZE];

int displayMode = 0;

//---------------------------------------
// Initialize Depth Information
//---------------------------------------
void readDepth()
{
   ifstream file("penny-depth.txt");

   // Initialize depth information
   for (int i = 0; i < SIZE; i++) {
      for (int j = 0; j < SIZE; j++) {
         file >> Pz[i][j];
         // cout << Pz[i][j] << "\n";
      }
   }
   file.close();
}

//---------------------------------------
// Initialize Color Information
//---------------------------------------
void readColor()
{
   ifstream file("penny-image.txt");

   // Initialize color information
   for (int i = 0; i < SIZE; i++) {
      for (int j = 0; j < SIZE; j++) {
         file >> R[i][j];
         // cout << R[i][j] << "\n";
         file >> G[i][j];
         // cout << G[i][j] << "\n";
         file >> B[i][j];
         // cout << Pz[i][j] << "\n";
      }
   }

   file.close();
}

//---------------------------------------
// Initialize surface points
//---------------------------------------
void init_surface(float Xmin, float Xmax, float Ymin, float Ymax)
{
   // Initialize (x,y,z) coordinates of surface
   for (int i = 0; i < SIZE; i++)
   for (int j = 0; j < SIZE; j++)
   {
      Px[i][j] = Xmin + i * (Xmax - Xmin) / SIZE;
      Py[i][j] = Ymin + j * (Ymax - Ymin) / SIZE;
   }
}

//---------------------------------------
// Initialize surface normals
//---------------------------------------
void init_normals()
{
   // Initialize surface normals
   for (int i=0; i<SIZE-1; i++)
   for (int j=0; j<SIZE-1; j++)
   {
      // Get tangents S and T
      float Sx = Px[i+1][j] - Px[i][j];
      float Sy = Py[i+1][j] - Py[i][j];
      float Sz = Pz[i+1][j] - Pz[i][j];
      float Tx = Px[i][j+1] - Px[i][j];
      float Ty = Py[i][j+1] - Py[i][j];
      float Tz = Pz[i][j+1] - Pz[i][j];
      
      // Calculate N = S cross T
      Nx[i][j] = Sy*Tz - Sz*Ty;
      Ny[i][j] = Sz*Tx - Sx*Tz;
      Nz[i][j] = Sx*Ty - Sy*Tx;

      // Make unit vector
      float length = sqrt(Nx[i][j]*Nx[i][j]
                   + Ny[i][j]*Ny[i][j]
                   + Nz[i][j]*Nz[i][j]);
      if (length > 0)
      {
         Nx[i][j] /= length;
         Ny[i][j] /= length;
         Nz[i][j] /= length;
      }
   }

   // Handle last row and column
   for (int i=0; i<SIZE; i++)
   {
      Nx[i][SIZE-1] = Nx[i][SIZE-2];
      Ny[i][SIZE-1] = Ny[i][SIZE-2];
      Nz[i][SIZE-1] = Nz[i][SIZE-2];
      Nx[SIZE-1][i] = Nx[SIZE-2][i];
      Ny[SIZE-1][i] = Ny[SIZE-2][i];
      Nz[SIZE-1][i] = Nz[SIZE-2][i];
   }
}

//---------------------------------------
// Init function for OpenGL
//---------------------------------------
void init()
{
   // Initialize OpenGL
   glClearColor(0.0, 0.0, 0.0, 1.0);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   float radius = 1.2;
   glOrtho(-radius, radius, -radius, radius, -radius, radius);
   glEnable(GL_DEPTH_TEST);

   readColor();
   readDepth();

   // Initialize material properties
   init_material(Ka, Kd, Ks, 100 * Kp, 149, 82, 37);

   // Initialize smooth shading
   glShadeModel(GL_SMOOTH);

   // Initialize surface
   init_surface(-1.0, 1.0, -1.0, 1.0);
   init_normals();
}

//---------------------------------------
// Display callback for OpenGL
//---------------------------------------
void display()
{
   // Incrementally rotate objects
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glTranslatef(xpos / 500.0, ypos / 500.0, zpos / 500.0);
   glRotatef(xangle, 1.0, 0.0, 0.0);
   glRotatef(yangle, 0.0, 1.0, 0.0);
   glRotatef(zangle, 0.0, 0.0, 1.0);

   //Wire Mode
   if(displayMode == 0) {
      for(int i = 0; i < SIZE - 1; i++) {
         for(int j = 0; j < SIZE - 1; j++) {
            glBegin(GL_LINE_LOOP);
	         glVertex3f(Px[i][j], Py[i][j], Pz[i][j] * 0.001);
	         glVertex3f(Px[i + 1][j], Py[i + 1][j], Pz[i + 1][j] * 0.001);
	         glVertex3f(Px[i + 1][j + 1], Py[i + 1][j + 1], Pz[i + 1][j + 1] * 0.001);
	         glVertex3f(Px[i][j + 1], Py[i][j + 1], Pz[i][j + 1] * 0.001);
	         glEnd();
         }
      }
   } 
   //RGB Mode
   else if(displayMode == 1) {
      for(int i = 0; i < SIZE - 1; i++) {
         for(int j = 0; j < SIZE - 1; j++) {
            glColor3f(R[i][j] / 255, G[i][j] / 255, B[i][j] / 255);
	         glBegin(GL_POLYGON);
	         glVertex3f(Px[i][j], Py[i][j], Pz[i][j] * 0.001);
	         glVertex3f(Px[i + 1][j], Py[i + 1][j], Pz[i + 1][j] * 0.001);
	         glVertex3f(Px[i + 1][j + 1], Py[i + 1][j + 1], Pz[i + 1][j + 1] * 0.001);
	         glVertex3f(Px[i][j + 1], Py[i][j + 1], Pz[i][j + 1] * 0.001);
	         glEnd();
         }
      }
   }
   //Phong Shading Mode
   else if(displayMode == 2) {
      glEnable(GL_LIGHTING);
      glEnable(GL_LIGHT0);
      glEnable(GL_NORMALIZE);

      GLfloat ambientLight[] = {0.2f, 0.2f, 0.2f, 1.0f};
      GLfloat diffuseLight[] = {0.8f, 0.8f, 0.8, 1.0f};
      GLfloat specularLight[] = {1.0f, 1.0f, 1.0f, 1.0f};
      GLfloat position[] = {1.5f, 1.0f, 4.0f, 1.0f};

      glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
      glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
      glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
      glLightfv(GL_LIGHT0, GL_POSITION, position);

      GLfloat mat_ambient[] = { 128.0f/255.0f, 85.0f/255.0f, 66.0f/255.0f, 1.0f };
      GLfloat mat_diffuse[] = { 128.0f/255.0f, 85.0f/255.0f, 66.0f/255.0f, 1.0f };
      GLfloat mat_specular[] = { 128.0f/255.0f, 85.0f/255.0f, 66.0f/255.0f, 1.0f };

      glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
      glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
      glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);

      for(int i = 0; i < SIZE - 1; i++) {
         for(int j = 0; j < SIZE - 1; j++) {
            glBegin(GL_POLYGON);
	         glNormal3f(Nx[i][j], Ny[i][j], Nz[i][j]);
	         glVertex3f(Px[i][j], Py[i][j], Pz[i][j] * 0.001);
	         glNormal3f(Nx[i + 1][j], Ny[i + 1][j], Nz[i + 1][j]);
	         glVertex3f(Px[i + 1][j], Py[i + 1][j], Pz[i + 1][j] * 0.001);
	         glNormal3f(Nx[i + 1][j + 1], Ny[i + 1][j + 1], Nz[i + 1][j + 1]);
	         glVertex3f(Px[i + 1][j + 1], Py[i + 1][j + 1], Pz[i + 1][j + 1] * 0.001);
	         glNormal3f(Nx[i][j + 1], Ny[i][j + 1], Nz[i][j + 1]);
	         glVertex3f(Px[i][j + 1], Py[i][j + 1], Pz[i][j + 1] * 0.001);
	         glEnd();
         }
      }

      glDisable(GL_LIGHTING);
      glDisable(GL_LIGHT0);
      glDisable(GL_NORMALIZE);
   }
   else {
      return;
   }

   glFlush();
}

//---------------------------------------
// Keyboard callback for OpenGL
//---------------------------------------
void keyboard(unsigned char key, int x, int y)
{
   // Determine if we are in ROTATE or TRANSLATE mode
   if ((key == 'r') || (key == 'R'))
   {
      printf("Type x y z to decrease or X Y Z to increase ROTATION angles.\n");
      mode = ROTATE;
   }
   else if ((key == 't') || (key == 'T'))
   {
      printf
	 ("Type x y z to decrease or X Y Z to increase TRANSLATION distance.\n");
      mode = TRANSLATE;
   }

   if(key == '0') {
      printf("Wire Mode\n");
      displayMode = 0;
   } else if(key == '1') {
      printf("RGB Mode\n");
      displayMode = 1;
   } else if(key == '2') {
      printf("Phong Shading Mode\n");
      displayMode = 2;
   }

   // Handle ROTATE
   if (mode == ROTATE)
   {
      if (key == 'x')
	 xangle -= 5;
      else if (key == 'y')
	 yangle -= 5;
      else if (key == 'z')
	 zangle -= 5;
      else if (key == 'X')
	 xangle += 5;
      else if (key == 'Y')
	 yangle += 5;
      else if (key == 'Z')
	 zangle += 5;
   }

   // Handle TRANSLATE
   if (mode == TRANSLATE)
   {
      if (key == 'x')
	 xpos -= 5;
      else if (key == 'y')
	 ypos -= 5;
      else if (key == 'z')
	 zpos -= 5;
      else if (key == 'X')
	 xpos += 5;
      else if (key == 'Y')
	 ypos += 5;
      else if (key == 'Z')
	 zpos += 5;
   }

   // Handle material properties
   if (key == 'a')
      Ka -= STEP;
   if (key == 'd')
      Kd -= STEP;
   if (key == 's')
      Ks -= STEP;
   if (key == 'p')
      Kp -= STEP;
   if (key == 'A')
      Ka += STEP;
   if (key == 'D')
      Kd += STEP;
   if (key == 'S')
      Ks += STEP;
   if (key == 'P')
      Kp += STEP;
   if (Ka < 0)
      Ka = 0;
   if (Kd < 0)
      Kd = 0;
   if (Ks < 0)
      Ks = 0;
   if (Kp < STEP)
      Kp = STEP;

   glutPostRedisplay();
}

//---------------------------------------
// Mouse callback for OpenGL
//---------------------------------------
void mouse(int button, int state, int x, int y)
{
   // Handle mouse down
   static int xdown, ydown;
   if (state == GLUT_DOWN)
   {
      xdown = x;
      ydown = y;
   }

   // Handle ROTATE
   if ((mode == ROTATE) && (state == GLUT_UP))
   {
      xangle += (y - ydown);
      yangle -= (x - xdown);
      zangle = 0;
      glutPostRedisplay();
   }

   // Handle TRANSLATE
   if ((mode == TRANSLATE) && (state == GLUT_UP))
   {
      xpos += (x - xdown);
      ypos -= (y - ydown);
      glutPostRedisplay();
   }
}

//---------------------------------------
// Main program
//---------------------------------------
int main(int argc, char *argv[])
{
   // Create OpenGL window
   glutInit(&argc, argv);
   glutInitWindowSize(500, 500);
   glutInitWindowPosition(250, 250);
   glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH);
   glutCreateWindow("Surface");
   init();
   printf("Type r to enter ROTATE mode or t to enter TRANSLATE mode.\n");
   printf("Type 1, 2, 3 to change display mode.\n");

   // Specify callback function
   glutDisplayFunc(display);
   glutKeyboardFunc(keyboard);
   glutMouseFunc(mouse);
   glutMainLoop();
   return 0;
}