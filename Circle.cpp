#define _USE_MATH_DEFINES
#include <math.h>
#include <GLUT/GLUT.h>
#include <unistd.h>
#define PI M_PI

/// 윈도우의 사이즈를 결정하는 변수
int     Window_Width = 500;
int     Window_Height = 500;

/// 도형의 그리기 옵션을 나타내는 변수
int     model_type = GL_POLYGON;

/// 형태를 나타내는 변수들
int     num = 3;              // 도형의 꼭짓점 갯수
float   radius = 100;         // 도형의 반지름
float   start_angle = 0.0;    // 도형의 시작 각도

/// 도형의 위치를 나타내는 변수
float   move_x = 0.0;
float   move_y = 0.0;

/// 컬러를 나타내는 변수, rgb 값을 나타내는 float 배열
int     cnum = 0;
float   color[] = {0, 0, 0};


// 함수 선언부
void Modeling_Circle(void);
void Modeling_Axis(void);
void Modeling_Rectangle(void);
void RenderScene(void);
void MyReshape(int w, int h);
void SpecialKey(int key, int x, int y);


// 메인 함수
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(Window_Width, Window_Height);
    glutCreateWindow("N-Polygon & Keyboard Event");
    glutDisplayFunc(RenderScene);   // 윈도우 화면 크기가 변경되면 실행되는 이벤트
    glutReshapeFunc(MyReshape);     // 윈도우 화면이 출력될 때 실행되는 이벤트
    glutSpecialFunc(SpecialKey);    // 이벤트 등록 함수
    glutMainLoop();                 // 이벤트 대기모드로 변경(이벤트가 실행되면 해당하는 콜백함수 실행)
    return 0;
}


// 함수 구현부
/// n각형의 도형을 만들어주는 함수
void Modeling_Circle(void) {
    float    delta, theta;
    float    x, y;

    glColor3f(color[0], color[1], color[2]);
    glPointSize(3.0);
    delta = 2 * PI / num;

    glBegin(model_type);
    for (int i = 0; i < num; i++) {
        theta = start_angle + delta * i;
        x = radius * cos(theta);
        y = radius * sin(theta);
        glVertex2f(x, y);
    }
    glEnd();
}


/// 도형의 기준 축을 그려주는 함수
void Modeling_Axis(void) {
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex2f(-250.0, 0.0);
    glVertex2f(250.0, 0.0);

    glColor3f(0.0, 0.0, 1.0);
    glVertex2f(0, -250.0);
    glVertex2f(0, 250.0);
    glEnd();
}


/// 도형의 색을 결정하는 사각형을 그려주는 함수 
void Modeling_Rectangle(void) {
    glColor3f(cnum == 0,cnum == 1,cnum == 2);
    glBegin(GL_POLYGON);
    glVertex2i(100, 100);
    glVertex2i(150, 100);
    glVertex2i(150, 150);
    glVertex2i(100, 150);
    glEnd();
}


/// 윈도우 화면이 출력될 때 실행되는 콜백함수
void RenderScene(void) {
    glViewport(0 + move_x, 0 + move_y, Window_Width, Window_Height);
    glClearColor(0.5, 0.5, 0.5, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    Modeling_Rectangle();   // 사각형
    Modeling_Axis();        // 축
    Modeling_Circle();      // 도형

    glFlush();
}


/// 윈도우 화면의 사이즈가 변경되면 실행되는 콜백함수 
void MyReshape(int w, int h) {
    glViewport(0, 0, w, h);
    Window_Width = w;
    Window_Height = h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0 / 2.0 * w, 1.0 / 2.0 * w, -1.0 / 2.0 * h, 1.0 / 2.0 * h);
}


/// 키보드 입력을 처리하는 콜백함수
void SpecialKey(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_F1:    model_type = GL_POINTS;       break;       // 도형 그리기 옵션 : 점으로
        case GLUT_KEY_F2:    model_type = GL_POLYGON;      break;       // 도형 그리기 옵션 : 내부 채우기
        case GLUT_KEY_F3:    model_type = GL_LINE_LOOP;   break;       // 도형 그리기 옵션 : 선으로
        
        case GLUT_KEY_F4:    radius--;  break;      // 도형의 반지름 감소
        case GLUT_KEY_F5:    radius++;  break;      // 도형의 반지름 증가
        case GLUT_KEY_F6:    num--;     break;      // 도형의 꼭짓점 감소
        case GLUT_KEY_F7:    num++;     break;      // 도형의 꼭짓점 증가
        
        case GLUT_KEY_F8:    ++cnum %= 3;                            break;     // 변경할 컬러 선택
        case GLUT_KEY_F9:    if(color[cnum] > 0) color[cnum]-=0.1;   break;     // 해당 컬러 값 감소(min = 0.0)
        case GLUT_KEY_F10:   if(color[cnum] < 1) color[cnum]+=0.1;   break;     // 해당 컬러 값 증가(max = 1.0)
        
        case GLUT_KEY_LEFT:  move_x--;  break;
        case GLUT_KEY_RIGHT: move_x++;  break;
        case GLUT_KEY_UP:    move_y++;  break;
        case GLUT_KEY_DOWN:  move_y--;  break;
        
        default:    break;
    }
    glutPostRedisplay();
}