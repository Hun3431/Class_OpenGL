#define GL_SILENCE DEPRECATION
#include <GLUT/GLUT.h>  
#include <cmath>
#include <random>
#include <ctime>

#define PI M_PI

int     Width   = 1000;
int     Height  = 1000;

float   sun_radius      = 50.0;
float   mercury_radius  = 12.0;
float   earth_radius    = 20.0;
float   moon_radius     = 7.0;
float   mars_radius     = 35.0;
float   a_radius        = 5.0;
float   b_radius        = 25.0;
float   c_radius        = 15.0;

float   sunRotation     = 0.0;
float   mercuryRotation = 0.0;
float   earthRotation   = 0.0;
float   moonRotation    = 0.0;
float   marsRotation    = 0.0;
float   aRotation       = 0.0;
float   bRotation       = 0.0;
float   cRotation       = 0.0;

float   sun_rotation_speed = 0.033;
float   mercury_rotation_speed = 0.33;
float   earth_rotation_speed = 0.11;
float   moon_rotation_speed = 1.3;
float   mars_rotation_speed = 0.04;
float   a_rotation_speed = 3.0;
float   b_rotation_speed = 0.5;
float   c_rotation_speed = 5.0;

typedef struct{
    float r;
    float g;
    float b;
} Color;

struct Point {
    float x;
    float y;
    Color c;
};

const int star_num = 100;
Point star[star_num];

void init(void) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0 * Width / 2.0, Width / 2.0, -1.0 * Height / 2.0, Height / 2.0);
}

void Draw_Circle(float c_radius) {
   float    delta;
   int      num = 36;

   delta = 2 * PI / num;
   glBegin(GL_POLYGON);
   for (int i = 0; i < num; i++)
      glVertex2f(c_radius*cos(delta*i), c_radius*sin(delta*i));
   glEnd();
}

void Draw_Circle(float x, float y, float c_radius) {
   float    delta;
   int      num = 36;

   delta = 2 * PI / num;
   glBegin(GL_POLYGON);
   for (int i = 0; i < num; i++)
      glVertex2f(x + c_radius*cos(delta*i), y + c_radius*sin(delta*i));
   glEnd();
}

void Draw_Circle_Points(float c_radius) {
    float   delta;
    int     num = int(c_radius) * 2;

    delta = 2 * PI / num;
    glBegin(GL_POINTS);
    for (int i = 0; i < num; i++)
       glVertex2f(c_radius*cos(delta*i), c_radius*sin(delta*i));
    glEnd();
    
}

int b = 0;

void RenderScene(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    
    for(int i = 0; i < star_num; i ++) {
        glColor3f(star[i].c.r, star[i].c.g, star[i].c.b);
//        glColor3f(1.0, 1.0, 1.0);
        Draw_Circle(star[i].x, star[i].y,rand()%3);
    }
    
    sunRotation     +=  sun_rotation_speed;
    earthRotation   +=  earth_rotation_speed;
    moonRotation    +=  moon_rotation_speed;
    marsRotation    +=  mars_rotation_speed;
    mercuryRotation +=  mercury_rotation_speed;
    aRotation       +=  a_rotation_speed;
    bRotation       +=  b_rotation_speed;
    cRotation       +=  c_rotation_speed;
    /***********코드 작성하기 ***************/
    // 태양 : 빨간색의 구
    glPushMatrix();
    glRotatef(sunRotation, 0.0, 0.0, 1.0);
    glColor3f(1.0, 0.0, 0.0);
    Draw_Circle(sun_radius);
    glPopMatrix();

    // 궤도 : 회색의 점선
    glColor3f(1.0, 1.0, 1.0);
    Draw_Circle_Points(100);
    Draw_Circle_Points(300);
    Draw_Circle_Points(450);
    Draw_Circle_Points(360);

    // 수성 : 파란색의 구
    glPushMatrix();
    glRotatef(mercuryRotation, 0.0, 0.0, 1.0);
    glTranslatef(100.0, 0.0, 0.0);
    glColor3f(0.0, 0.0, 1.0);
    Draw_Circle(mercury_radius);
    glPopMatrix();

    // 지구 : 초록색의 구
    glPushMatrix();
    glRotatef(earthRotation, 0.0, 0.0, 1.0);
    glTranslatef(300.0, 0.0, 0.0);
    glColor3f(0.0, 0.5, 0.0);
    Draw_Circle(earth_radius);
    // 달 궤도 : 회색의 점선
    glColor3f(1.0, 1.0, 1.0);
    Draw_Circle_Points(50);

    // 달 : 회색의 구
    glPushMatrix();
    glRotatef(moonRotation, 0.0, 0.0, 1.0);
    glTranslatef(50.0, 0.0, 0.0);
    glColor3f(0.5, 0.5, 0.5);
    Draw_Circle(moon_radius);
    
    glColor3f(1.0, 1.0, 1.0);
    Draw_Circle_Points(20);
    
    glPushMatrix();
    glRotatef(aRotation, 0.0, 0.0, -1.0);
    glTranslatef(20, 0.0, 0.0);
    glColor3f(0.3, 0.8, 0.5);
    Draw_Circle(a_radius);
    
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    //화성
    glPushMatrix();
    glRotatef(marsRotation, 0.0, 0.0, 1.0);
    glTranslatef(450, 0.0, 0.0);
    glColor3f(0.8, 0.3, 0.0);
    Draw_Circle(mars_radius);
    glPopMatrix();
    
    glPushMatrix();
    glRotatef(bRotation, 0.0, 0.0, 1.0);
    glTranslatef(360, 0.0, 0.0);
    glColor3f(0.8, 0.3, 0.8);
    Draw_Circle(b_radius);
    glColor3f(1.0, 1.0, 1.0);
    Draw_Circle_Points(60);
    
    glPushMatrix();
    glRotatef(cRotation, 0.0, 0.0, 1.0);
    glTranslatef(60, 0.0, 0.0);
    glColor3f(0.8, 1.0, 0.8);
    Draw_Circle(c_radius);
    
    glPopMatrix();
    glPopMatrix();
    glFlush();
    glutSwapBuffers();
}

int main(int argc, char** argv) {
    srand(time(NULL));
    for(int i = 0; i < star_num; i ++) {
        Color color = {
            r:static_cast <float> (rand()) / static_cast <float> (RAND_MAX),
            g:static_cast <float> (rand()) / static_cast <float> (RAND_MAX),
            b:static_cast <float> (rand()) / static_cast <float> (RAND_MAX)
        };
        star[i].c = color;
        star[i].x = rand()%1000 - 500;
        star[i].y = rand()%1000 - 500;
    }
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(Width, Height);
    glutCreateWindow("Solar System");
    init();
    glutDisplayFunc(RenderScene);
    glutIdleFunc(RenderScene);
    glutMainLoop();
}

