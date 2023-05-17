#include <GLUT/GLUT.h>
#include <math.h>
#include <iostream>

#define    width             400
#define    height            600
#define    PI                3.1415
#define    polygon_num        50

//시작 지점 좌표
int left = -200;
int bottom = -300;

float zoom = 1;

int        mouse_motion = 0;

float    radius = 20.0;

typedef struct _Point {
    float    x;
    float    y;
} Point;

Point    circle_center;


/// 윈도우 상의 기준 축을 그려주는 함수 
void Modeling_Axis(void) {
    glLineWidth(2.0);
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex2i(0, 0);
    glVertex2i(1.0 * width, 0);

    glColor3f(0.0, 0.0, 1.0);
    glVertex2f(0.0, 0.0);
    glVertex2f(0.0, 1.0 * height);
    glEnd();
}

/// 윈도우 상의 도형을 그리는 함수
void    Modeling_Circle(Point CC) {
    float    delta;

    glColor3f(1.0, 0.0, 0.0);
    glPointSize(3.0);
    delta = 2 * PI / polygon_num;
    glBegin(GL_POLYGON);
    for (int i = 0; i < polygon_num; i++)
        glVertex2f(CC.x + radius * cos(delta * i), CC.y + radius * sin(delta * i));
    glEnd();

    CC = { 30, 20 };
    glColor3f(0.0, 0.0, 1.0);
    glPointSize(3.0);
    delta = 2 * PI / polygon_num;
    glBegin(GL_POLYGON);
    for (int i = 0; i < polygon_num; i++)
        glVertex2f(CC.x + radius * cos(delta * i), CC.y + radius * sin(delta * i));
    glEnd();

    CC = { 70, -20 };
    glColor3f(1.0, 0.0, 1.0);
    glPointSize(3.0);
    delta = 2 * PI / polygon_num;
    glBegin(GL_POLYGON);
    for (int i = 0; i < polygon_num; i++)
        glVertex2f(CC.x + radius * cos(delta * i), CC.y + radius * sin(delta * i));
    glEnd();

    CC = { -10, 50 };
    glColor3f(0, 1.0, 0.0);
    glPointSize(3.0);
    delta = 2 * PI / polygon_num;
    glBegin(GL_POLYGON);
    for (int i = 0; i < polygon_num; i++)
        glVertex2f(CC.x + radius * cos(delta * i), CC.y + radius * sin(delta * i));
    glEnd();

    CC = { -60, -90 };
    glColor3f(1.0, 1.0, 1.0);
    glPointSize(3.0);
    delta = 2 * PI / polygon_num;
    glBegin(GL_POLYGON);
    for (int i = 0; i < polygon_num; i++)
        glVertex2f(CC.x + radius * cos(delta * i), CC.y + radius * sin(delta * i));
    glEnd();
}

void RenderScene(void) {
    glClearColor(1.0, 1.0, 0.0, 0.0); 
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // 좌측 하단의 좌표를 기준으로 잡고
    gluOrtho2D(left * zoom, (width + left) * zoom, bottom * zoom, (height + bottom) * zoom); // mouse2()


    Modeling_Axis();
    Modeling_Circle(circle_center);

    glFlush();
}


void mouse1(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        zoom *= 0.9;

        left += (x - width / 2) * zoom; // (x - width/2) x의 좌표 확인 : -는 왼쪽 +는 오른쪽, 차이가 크면 클수록 중심에서 먼곳을 클릭함
        bottom -= (y - width / 2) * zoom;
    }
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        zoom *= 1.1;

        left += (x - width / 2) * zoom;
        bottom -= (y - width / 2) * zoom;
    }
    glutPostRedisplay();
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(width, height);
    glutCreateWindow("Draw Points by Mouse Input");
    glutDisplayFunc(RenderScene);
    glutMouseFunc(mouse1);
    glutMainLoop();
}

