/* 
 * 조건 : 윈도우 상의 사이즈와 gluOrtho2D의 크기가 다를 경우에 윈도우 상의 좌표와 내부 그리기 공간 좌표의 관계를 나타냄
 */
#include <GLUT/GLUT.h>
#include <math.h>
#include <iostream>
using namespace std;

// window 범위
#define    width             600
#define    height            800

// gluOrtho2D 범위
#define left    -100
#define right   200
#define bottom  -200
#define top     600

// 마우스 입력 좌표
int p_x = 0;
int p_y = 0;

void Modeling_Axis(void) {
    glLineWidth(3.0);
    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 0.0);
    glVertex2i(0, -200);
    glVertex2i(0, 600);

    glVertex2f(-100.0, 0.0);
    glVertex2f(200.0, 0.0);

    for(int i = 0; i < (right - left)/50; i ++) {
        glVertex2i(left + i * 50, (i % 2)?5:10);
        glVertex2i(left + i * 50, (i % 2)?-5:-10);
    }
    for(int i = 0; i < (top - bottom)/50; i ++){
        glVertex2i((i % 2)?5:10, bottom + i * 50);
        glVertex2i((i % 2)?-5:-10, bottom + i * 50);
    }
    glEnd();
}

void Modeling_Circle(void) {
    glPointSize(15);
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_POINTS);
    glVertex2i(p_x, p_y);
    glEnd();
}


void RenderScene(void) {
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluOrtho2D(left, right, bottom, top);
    
    Modeling_Axis();
    Modeling_Circle();

    glFlush();
}


void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        cout << "click x : " << x << " | y : " << y << endl;

        y = height - y;
        p_x = x / 2 + left;
        p_y = y + bottom;
        cout << "point x : " << p_x << " | y : " << p_y << endl;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(width, height);
    glutCreateWindow("Draw Points by Mouse Input");
    glutDisplayFunc(RenderScene);
    glutMouseFunc(mouse);
    glutMainLoop();
}
