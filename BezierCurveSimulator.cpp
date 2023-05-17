#include <GLUT/GLUT.h>
#include <unistd.h>

// window 범위
#define    width             1000
#define    height            1000
// gluOrtho2D 범위
#define left    -500
#define right   500
#define bottom  -500
#define top     500

int j = 0;

typedef struct _Point {
    float x;
    float y;
} Point;

Point p[] = {
    { -400.0, -400.0 },
    { -300.0,  400.0 },
    {  300.0,  400.0 },
    {  400.0, -400.0 }
};

void Modeling_Axis(void) {
    glLineWidth(2.0);
    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 0.0);
    glVertex2i(0, bottom);
    glVertex2i(0, top);
    glVertex2i(left, 0);
    glVertex2i(right, 0);

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

void Modeling(){
    int split = 100;
    int size = sizeof(p) / sizeof(Point);
    float* delta_x = new float(size - 1);
    float* delta_y = new float(size - 1);
    
    for(int i = 0; i < size - 1; i ++) {
        delta_x[i] = (p[i + 1].x - p[i].x) / split;
        delta_y[i] = (p[i + 1].y - p[i].y) / split;
    }
    int i = 0;
    float bx0 = (p[i].x + delta_x[i] * j) + ((p[i + 1].x + delta_x[i + 1] * j) - (p[i].x + delta_x[i] * j)) / split * j;
    float by0 = (p[i].y + delta_y[i] * j) + ((p[i + 1].y + delta_y[i + 1] * j) - (p[i].y + delta_y[i] * j)) / split * j;
    float bx1 = (p[i + 1].x + delta_x[i + 1] * j) + ((p[i + 2].x + delta_x[i + 2] * j) - (p[i + 1].x + delta_x[i + 1] * j)) / split * j;
    float by1 = (p[i + 1].y + delta_y[i + 1] * j) + ((p[i + 2].y + delta_y[i + 2] * j) - (p[i + 1].y + delta_y[i + 1] * j)) / split * j;
    
    glLineWidth(3);
    // 점 P를 이어주는 선
    glBegin(GL_LINE_STRIP);
    glColor3f(0.0, 0.0, 0.0);
    for(int k = 0; k < 4; k ++)
        glVertex2f(p[k].x, p[k].y);
    glEnd();
    
    // 점 M을 이어주는 선
    glBegin(GL_LINES);
    glColor3f(0.6, 0.6, 0.6);
    glVertex2f(p[i].x + delta_x[i] * j, p[i].y + delta_y[i] * j);
    glVertex2f(p[i + 1].x + delta_x[i + 1] * j, p[i + 1].y + delta_y[i + 1] * j);
    
    glVertex2f(p[i + 1].x + delta_x[i + 1] * j, p[i + 1].y + delta_y[i + 1] * j);
    glVertex2f(p[i + 2].x + delta_x[i + 2] * j, p[i + 2].y + delta_y[i + 2] * j);
    glEnd();
    
    // 점 B를 이어주는 선
    glBegin(GL_LINES);
    glColor3f(0.0, 1.0, 0.0);
    glVertex2f(bx0, by0);
    glVertex2f(bx1, by1);
    glEnd();
    
    
    // 점 P
    glColor3f(0.5, 0.5, 0.5);
    glRasterPos2f(p[0].x - 12, p[0].y - 30);
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'P');
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, '0');

    glRasterPos2f(p[1].x - 12, p[1].y + 20);
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'P');
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, '1');

    glRasterPos2f(p[2].x - 12, p[2].y + 20);
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'P');
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, '2');

    glRasterPos2f(p[3].x - 12, p[3].y - 30);
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'P');
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, '3');

    // 점 M
    glColor3f(0.0, 0.0, 0.0);
    glRasterPos2f(p[i].x + delta_x[i] * j - 40, p[i].y + delta_y[i] * j - 10);
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'M');
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, '0');
    
    glRasterPos2f(p[i + 1].x + delta_x[i + 1] * j + - 15, p[i + 1].y + delta_y[i + 1] * j + 20);
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'M');
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, '1');
    
    glRasterPos2f(p[i + 2].x + delta_x[i + 2] * j + 10, p[i + 2].y + delta_y[i + 2] * j - 10);
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'M');
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, '2');

    // 점 B 0, 1
    glColor3f(0.0, 0.0, 0.0);
    glRasterPos2f(bx0 - 12, by0 - 30);
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'B');
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, '0');
    
    glRasterPos2f(bx1 - 12, by1 - 30);
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'B');
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, '1');
    
    // 점 B
    glColor3f(1.0, 0.0, 0.0);
    glRasterPos2f(bx0 + (bx1 - bx0) / split * j - 30, by0 + (by1 - by0) / split * j - 10);
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'B');
    
    
    // 점 P를 표시
    glPointSize(6);
    glBegin(GL_POINTS);
    glColor3f(0.0, 0.0, 0.0);
    for(int k = 0; k < sizeof(p) / sizeof(Point); k ++)
        glVertex2f(p[k].x, p[k].y);
    
    // P(i ~ i + 1) 선을 지나가는 점 M을 표시
    glVertex2f(p[i].x + delta_x[i] * j, p[i].y + delta_y[i] * j);
    glVertex2f(p[i + 1].x + delta_x[i + 1] * j, p[i + 1].y + delta_y[i + 1] * j);
    glVertex2f(p[i + 2].x + delta_x[i + 2] * j, p[i + 2].y + delta_y[i + 2] * j);
    glEnd();
    
    // M(i ~ i + 1) 선을 지나가는 점 B를 표시
    glBegin(GL_POINTS);
    glColor3f(0.0, 1.0, 0.0);
    glVertex2f(bx0, by0);
    glVertex2f(bx1, by1);
    glEnd();
    
    // 점 B를 지나가는 점 표시
    glPointSize(4);
    glBegin(GL_POINTS);
    glColor3f(1.0, 0.0, 0.0);
    for(int k = 0; k <= j; k ++) {
        float bx0 = (p[i].x + delta_x[i] * k) + ((p[i + 1].x + delta_x[i + 1] * k) - (p[i].x + delta_x[i] * k)) / split * k;
        float by0 = (p[i].y + delta_y[i] * k) + ((p[i + 1].y + delta_y[i + 1] * k) - (p[i].y + delta_y[i] * k)) / split * k;
        float bx1 = (p[i + 1].x + delta_x[i + 1] * k) + ((p[i + 2].x + delta_x[i + 2] * k) - (p[i + 1].x + delta_x[i + 1] * k)) / split * k;
        float by1 = (p[i + 1].y + delta_y[i + 1] * k) + ((p[i + 2].y + delta_y[i + 2] * k) - (p[i + 1].y + delta_y[i + 1] * k)) / split * k;

        glVertex2f(bx0 + (bx1 - bx0) / split * k , by0 + (by1 - by0) / split * k);
    }
    glEnd();
}

void RenderScene(void) {
    usleep(100000); // 0.1초 딜레이
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(left, right, bottom, top);
    
    Modeling_Axis();
    Modeling();
    glFlush();
    if(++ j > 100) {
        j = 0;
        sleep(1);
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(width, height);
    glutCreateWindow("BezierCurves Simulator");
    glutDisplayFunc(RenderScene);
    glutIdleFunc(RenderScene);
    glutMainLoop();
}
