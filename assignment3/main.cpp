#include <bits/stdc++.h>
//GLEW
#include<GL/glew.h>
//GLFW
#include <GLFW/glfw3.h>
#include <GL/glut.h>
#define GLM_ENABLE_EXPERIMENTAL
//OpenGL Math libs
#include <glm/glm.hpp>
#include<glm/vec3.hpp>
#include<glm/vec4.hpp>
#include<glm/mat4x4.hpp>
#include<glm/ext.hpp>
#include<glm/gtx/string_cast.hpp>
// #define pi 3.14
// #define deg 180
using namespace std;

/*
** Create a single component texture map
*/
GLuint tex1;

void make_tex(void)
{
    unsigned char data[256][256][3];
    for (int y = 0; y < 255; y++) {
  for (int x = 0; x < 255; x++) {
      unsigned char *p = data[y][x];
      p[0] = p[1] = p[2] = (x ^ y) & 8 ? 0 : 255;
  }
    }
    glGenTextures(1, &tex1);
    glBindTexture(GL_TEXTURE_2D, tex1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, (const GLvoid *) data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
GLfloat plane[4];
GLfloat v0[3], v1[3], v2[3];

GLfloat *make_texture(int maxs, int maxt)
{
    int s, t;
    static GLfloat *texture;

    texture = (GLfloat *)malloc(maxs * maxt * sizeof(GLfloat));
    for(t = 0; t < maxt; t++) {
  for(s = 0; s < maxs; s++) {
      texture[s + maxs * t] = ((s >> 4) & 0x1) ^ ((t >> 4) & 0x1);
  }
    }
    return texture;
}

enum {SPHERE = 1,SPHERE2 , SPHERE3, CONE, LIGHT, LEFTWALL, FLOOR};

enum {X, Y, Z, W};
enum {A, B, C, D};
/* create a matrix that will project the desired shadow */
void
shadowmatrix(GLfloat shadowMat[4][4],
       GLfloat groundplane[4],
       GLfloat lightpos[4])
{
  GLfloat dot;

  /* find dot product between light position vector and ground plane normal */
  dot = groundplane[X] * lightpos[X] +
        groundplane[Y] * lightpos[Y] +
        groundplane[Z] * lightpos[Z] +
        groundplane[W] * lightpos[W];

  shadowMat[0][0] = dot - lightpos[X] * groundplane[X];
  shadowMat[1][0] = 0.f - lightpos[X] * groundplane[Y];
  shadowMat[2][0] = 0.f - lightpos[X] * groundplane[Z];
  shadowMat[3][0] = 0.f - lightpos[X] * groundplane[W];

  shadowMat[X][1] = 0.f - lightpos[Y] * groundplane[X];
  shadowMat[1][1] = dot - lightpos[Y] * groundplane[Y];
  shadowMat[2][1] = 0.f - lightpos[Y] * groundplane[Z];
  shadowMat[3][1] = 0.f - lightpos[Y] * groundplane[W];

  shadowMat[X][2] = 0.f - lightpos[Z] * groundplane[X];
  shadowMat[1][2] = 0.f - lightpos[Z] * groundplane[Y];
  shadowMat[2][2] = dot - lightpos[Z] * groundplane[Z];
  shadowMat[3][2] = 0.f - lightpos[Z] * groundplane[W];

  shadowMat[X][3] = 0.f - lightpos[W] * groundplane[X];
  shadowMat[1][3] = 0.f - lightpos[W] * groundplane[Y];
  shadowMat[2][3] = 0.f - lightpos[W] * groundplane[Z];
  shadowMat[3][3] = dot - lightpos[W] * groundplane[W];

}

/* find the plane equation given 3 points */
void
findplane(GLfloat plane[4],
    GLfloat v0[3], GLfloat v1[3], GLfloat v2[3])
{
  GLfloat vec0[3], vec1[3];

  /* need 2 vectors to find cross product */
  vec0[X] = v1[X] - v0[X];
  vec0[Y] = v1[Y] - v0[Y];
  vec0[Z] = v1[Z] - v0[Z];

  vec1[X] = v2[X] - v0[X];
  vec1[Y] = v2[Y] - v0[Y];
  vec1[Z] = v2[Z] - v0[Z];
  

  /* find cross product to get A, B, and C of plane equation */
  plane[A] = vec0[Y] * vec1[Z] - vec0[Z] * vec1[Y];
  plane[B] = -(vec0[X] * vec1[Z] - vec0[Z] * vec1[X]);
  plane[C] = vec0[X] * vec1[Y] - vec0[Y] * vec1[X];

  plane[D] = -(plane[A] * v0[X] + plane[B] * v0[Y] + plane[C] * v0[Z]);
}

void sphere(void)
{
    glPushMatrix();
    glTranslatef(0.0f, -50.f, -370.f);
    glCallList(SPHERE);
    glPopMatrix();
}

void sphere2(void)
{
    glPushMatrix();
    glTranslatef(-70.0f, -50.f, -370.f);
    glCallList(SPHERE2);
    glPopMatrix();
}

void sphere3(void)
{
    glPushMatrix();
    glTranslatef(50.0f, -50.f, -370.f);
    glCallList(SPHERE3);
    glPopMatrix();
}

enum {NONE, SHADOW};

int rendermode = NONE;

void
menu(int mode)
{
  rendermode = mode;
  glutPostRedisplay();
}
GLfloat rightwallshadow[4][4];
GLfloat leftwallshadow[4][4];
GLfloat floorshadow[4][4];
GLfloat lightx = 50.f;
GLfloat lightpos[] = {0.f, 100.f, -320.f, 1.f};

void
redraw(void)
{

   /* material properties for objects in scene */
    static GLfloat wall_mat[] = {1.f,1.f, 1.f, 1.f};
    static GLfloat sphere_mat[] = {1.f, 1.0f, 1.f, 1.f};
    static GLfloat sphere_mat_ambient[] = {0.1f,0.1f,0.1f, 0.1f};
    static GLfloat sphere_mat_diffuse[] = {0.1f,0.1f,0.1f, 0.1f};


    static GLfloat clear[] = {0.f,0.f,0.f,0.f};
    // static GLfloat sphere_mat[] = {1.f, .5f, 0.f, 1.f};
    static GLfloat sphere2_mat[] = {0.0f, 0.0f,1.f, 1.f};
    static GLfloat sphere3_mat[] = {0.0f, 0.0f, 1.f, 100.f};

    static GLfloat sphere_mat3_ambient[] = {0.f,0.f,0.f,0.f, 0.25f};
    static GLfloat sphere_mat3_diffuse[] = {0.01f,0.01f,0.f, 0.25f};
    static GLfloat sphere_mat3_specular[] = {0.5f,0.5f,0.5f,0.25f};
    static GLfloat gold[]={50.0};

    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

    /*
    ** Note: wall verticies are ordered so they are all front facing
    ** this lets me do back face culling to speed things up.
    */
 

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, wall_mat);

        /* floor */
    /* make the floor textured */
    // glEnable(GL_TEXTURE_2D);

    /*
    ** Since we want to turn texturing on for floor only, we have to
    ** make floor a separate glBegin()/glEnd() sequence. You can't
    ** turn texturing on and off between begin and end calls
    */
    glBegin(GL_QUADS);
    glNormal3f(0.f, 1.f, 0.f);
    glTexCoord2i(0, 0);
    glVertex3f(-100.f, -100.f, -320.f);
    glTexCoord2i(1, 0);
    glVertex3f( 100.f, -100.f, -320.f);
    glTexCoord2i(1, 1);
    glVertex3f( 100.f, -100.f, -520.f);
    glTexCoord2i(0, 1);
    glVertex3f(-100.f, -100.f, -520.f);
    glEnd();

      glDisable(GL_DEPTH_TEST);
      glDisable(GL_LIGHTING);
      glColor3f(0.f, 0.f, 0.f); /* shadow color */

      glPushMatrix();
      glMultMatrixf((GLfloat *)floorshadow);
      //cone();
      glPopMatrix();

      glPushMatrix();
      glMultMatrixf((GLfloat *)floorshadow);
      sphere();
      sphere2();
      sphere3();
      glPopMatrix();
      glEnable(GL_DEPTH_TEST);
      glEnable(GL_LIGHTING);

    /* walls */
      glEnable(GL_STENCIL_TEST);
      glStencilFunc(GL_ALWAYS, 1, 0);
      glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    glBegin(GL_QUADS);
    /* left wall */
    glNormal3f(1.f, 0.f, 0.f);
    glVertex3f(-100.f, -100.f, -320.f);
    glVertex3f(-100.f, -100.f, -520.f);
    glVertex3f(-100.f,  100.f, -520.f);
    glVertex3f(-100.f,  100.f, -320.f);
    glEnd();

      glStencilFunc(GL_EQUAL, 1, 1);
      glDisable(GL_DEPTH_TEST);
      glDisable(GL_LIGHTING);
      glColor3f(0.f, 0.f, 0.f); /* shadow color */
      glDisable(GL_DEPTH_TEST);
      glPushMatrix();
      glMultMatrixf((GLfloat *)leftwallshadow);
      glMultMatrixf((GLfloat *)rightwallshadow);
      sphere();
      sphere2();
      sphere3();
      //cone();
      glPopMatrix();
      glEnable(GL_DEPTH_TEST);
      glDisable(GL_STENCIL_TEST);
      glEnable(GL_DEPTH_TEST);
      glEnable(GL_LIGHTING);
    

    glBegin(GL_QUADS);
    /* right wall */
    glNormal3f(-1.f, 0.f, 0.f);
    glVertex3f( 100.f, -100.f, -320.f);
    glVertex3f( 100.f,  100.f, -320.f);
    glVertex3f( 100.f,  100.f, -520.f);
    glVertex3f( 100.f, -100.f, -520.f);

    /* ceiling */
    glNormal3f(0.f, -1.f, 0.f);
    glVertex3f(-100.f,  100.f, -320.f);
    glVertex3f(-100.f,  100.f, -520.f);
    glVertex3f( 100.f,  100.f, -520.f);
    glVertex3f( 100.f,  100.f, -320.f);

    /* back wall */
    glNormal3f(0.f, 0.f, 1.f);
    glVertex3f(-100.f, -100.f, -520.f);
    glVertex3f( 100.f, -100.f, -520.f);
    glVertex3f( 100.f,  100.f, -520.f);
    glVertex3f(-100.f,  100.f, -520.f);
    glEnd();

    glPushMatrix();
    glTranslatef(lightpos[X], lightpos[Y], lightpos[Z]);
    glDisable(GL_LIGHTING);
    glColor3f(255.0f, 1.0f, 1.0f);
    glCallList(LIGHT);
    glEnable(GL_LIGHTING);
    glPopMatrix();

    // glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, cone_mat);

    // cone();

    glMaterialfv(GL_FRONT, GL_AMBIENT, sphere3_mat);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, sphere3_mat);
    glMaterialfv(GL_FRONT, GL_SPECULAR,sphere_mat3_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS,gold);
    sphere3();

    glMaterialfv(GL_FRONT,GL_AMBIENT,clear);
    glMaterialfv(GL_FRONT,GL_DIFFUSE,clear);
    glMaterialfv(GL_FRONT,GL_SPECULAR,clear);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, sphere_mat);
    sphere();

    glMaterialfv(GL_FRONT,GL_AMBIENT,clear);
    glMaterialfv(GL_FRONT,GL_DIFFUSE,clear);
    glMaterialfv(GL_FRONT,GL_SPECULAR,clear);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, sphere2_mat);
    sphere2();

  

    glutSwapBuffers(); /* high end machines may need this */
}

void key(unsigned char key, int x, int y)
{
    if(key == '\033')
  exit(0);

}

const int TEXDIM = 0;
/* Parse arguments, and set up interface between OpenGL and window system */
int main(int argc, char *argv[])
{
    GLfloat *tex;
    GLUquadricObj *sphere,*sphere1, *cone, *base;
    GLfloat plane[4];
    GLfloat v0[3], v1[3], v2[3];

    glutInit(&argc, argv);
    glutInitWindowSize(1024, 1024);
    glutInitDisplayMode(GLUT_RGBA|GLUT_DEPTH|GLUT_STENCIL|GLUT_DOUBLE);
    (void)glutCreateWindow("Projecting balls");
    glutDisplayFunc(redraw);
    glutKeyboardFunc(key);

    /* draw a perspective scene */
    glMatrixMode(GL_PROJECTION);
    glFrustum(-100., 100., -100., 100., 320., 640.); 
    glMatrixMode(GL_MODELVIEW);

    /* turn on features */
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    /* make shadow matricies */

      /* 3 points on floor */
      v0[X] = -100.f; v0[Y] = -100.f; v0[Z] = -320.f;
      v1[X] =  100.f; v1[Y] = -100.f; v1[Z] = -320.f;
      v2[X] =  100.f; v2[Y] = -100.f; v2[Z] = -520.f;
      

      findplane(plane, v0, v1, v2);
      shadowmatrix(floorshadow, plane, lightpos);

      /* 3 points on left wall */
      v0[X] = -100.f; v0[Y] = -100.f; v0[Z] = -320.f;
      v1[X] = -100.f; v1[Y] = -100.f; v1[Z] = -520.f;
      v2[X] = -100.f; v2[Y] =  100.f; v2[Z] = -520.f;

      findplane(plane, v0, v1, v2);
      shadowmatrix(leftwallshadow, plane, lightpos);

      /* 3 points on right wall */
      v0[X] = -100.f; v0[Y] = -100.f; v0[Z] = -320.f;
      v1[X] = -100.f; v1[Y] = -100.f; v1[Z] = -520.f;
      v2[X] = -100.f; v2[Y] =  100.f; v2[Z] = -520.f;

      findplane(plane, v0, v1, v2);
      shadowmatrix(rightwallshadow, plane, lightpos);

    /* place light 0 in the right place */
    glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

    /* remove back faces to speed things up */
    glCullFace(GL_BACK);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    /* make display lists for sphere and cone; for efficiency */

    glNewList(SPHERE3, GL_COMPILE);
    sphere = gluNewQuadric();
    //glColor4f(1,1,1,1); 
    gluSphere(sphere, 20.f, 40, 20);

    gluDeleteQuadric(sphere);
    glEndList();

    glNewList(SPHERE2, GL_COMPILE);
    sphere = gluNewQuadric();
    gluSphere(sphere, 20.f, 20, 20);
    gluDeleteQuadric(sphere);
    glEndList();

    glNewList(SPHERE, GL_COMPILE);
    make_tex();
    sphere = gluNewQuadric();
    glEnable(GL_TEXTURE_2D);
    gluQuadricDrawStyle(sphere, GLU_FILL);
    glBindTexture(GL_TEXTURE_2D, tex1);
    gluQuadricTexture(sphere, GL_TRUE);
    gluQuadricNormals(sphere, GLU_SMOOTH);
    gluSphere(sphere, 20.f, 20, 20);
    glDisable(GL_TEXTURE_2D);
    gluDeleteQuadric(sphere);

    glNewList(LIGHT, GL_COMPILE);
    sphere = gluNewQuadric();
    gluSphere(sphere, 5.f, 20, 20);
    gluDeleteQuadric(sphere);
    glEndList();
    glNewList(FLOOR, GL_COMPILE);
    glEndList();
    glNewList(LEFTWALL, GL_COMPILE);
    glEndList();
    glutMainLoop();

}

