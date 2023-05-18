#include <GLUT/GLUT.h>
#include <cmath>
#define    PI               M_PI
#define    WIDTH            1000
#define    HEIGHT           1000
#define    STATE_BREAK      0
#define    STATE_ONE        1
#define    STATE_TWO        2
#define    MODE_DEFAULT     0

/// 좌표들의 정보를 나타내는 구조체
typedef struct _Point {
    float    x;
    float    y;
} Point;

/// 색상(RGBC)의 정보를 나타내는 구조체
typedef struct _Color {
    float   red;
    float   green;
    float   blue;
    float   clamp = 0.0;
} Color;

/// 벽돌의 정보를 나타내는 구조체
typedef struct _Block {
    Point leftTop;
    Point leftBottom;
    Point rightTop;
    Point rightBottom;
    int mode = MODE_DEFAULT;
    int state = STATE_ONE;
} Block;

/// 하단의 슬라이딩 바의 정보를 나타내는 구조체
typedef struct _Bar {
    Point center;
    int len;
    int weight;
} Bar;

int left = 0;
int bottom = 0;

/// 하단의 슬라이딩 바 초기값 및 선언
int slidingBarLen = 200;
int slidingBarWeight = 20;
int slidingBarPosition = 0;
int slidingBarSpeed = 10;
Bar slidingBar = { WIDTH / 2, 0, slidingBarLen, slidingBarWeight };

/// 공 초기값 및 선언
float ballRadius = 10.0;
Point ballPosition = { WIDTH / 2, slidingBarWeight + ballRadius };
Point ballSpeed = { 1.0, 1.0 };

Point Wall[] = {
    {  150,    0 },
    {  150,  350 },
    {    0,  700 },
    {  300,  700 },
    {  500, 1000 },
    {  700,  700 },
    { 1000,  700 },
    {  850,  350 },
    {  850,    0 }
};


/*
 *  Shape Color
 */
Color backGroundColor = { 0.1, 0.1, 0.1 };
Color wallColor = { 0.9, 0.8, 0.5 };
Color slidingBarColor = { 0.5, 0.8, 0.7 };
Color ballColor = { 0.97, 0.95, 0.99 };


/*
 *  CollisionDetection Function
 */
void CollisionDetectionToWindow() {
    ballSpeed.x *= ballPosition.x + ballRadius >= WIDTH ? -1 : 1;
    ballSpeed.x *= ballPosition.x - ballRadius <= 0 ? -1 : 1;
    ballSpeed.y *= ballPosition.y + ballRadius >= HEIGHT ? -1 : 1;
    ballSpeed.y *= ballPosition.y - ballRadius <= 0 ? -1 : 1;
}


/*
 *  Show Window Function
 */
/// 게임 플레이에 사용되는 공을 그려주는 함수
void ShowBall() {
    int num = 36;
    float delta = 2 * PI / num;
    glBegin(GL_POLYGON);
    for(int i = 0; i < num; i ++) {
        glVertex2f(ballPosition.x + ballRadius * cos(delta * i), ballPosition.y + ballRadius * sin(delta * i));
    }
    glEnd();
}

/// 하단의 슬라이딩 바를 그려주는 함수
void ShowSlidingBar() {
    glBegin(GL_POLYGON);
    glVertex2i(slidingBarPosition + slidingBar.center.x - slidingBar.len / 2, slidingBar.center.y);
    glVertex2i(slidingBarPosition + slidingBar.center.x - slidingBar.len / 2, slidingBar.center.y + slidingBar.weight);
    glVertex2i(slidingBarPosition + slidingBar.center.x + slidingBar.len / 2, slidingBar.center.y + slidingBar.weight);
    glVertex2i(slidingBarPosition + slidingBar.center.x + slidingBar.len / 2, slidingBar.center.y);
    glEnd();
}

/// Window 화면에 게임이 진행될 벽을 그려주는 함수
void ShowWall() {
    glLineWidth(3.0);
    glBegin(GL_LINE_STRIP);
    for(int i = 0; i < 9; i ++) {
        glVertex2f(Wall[i].x, Wall[i].y);
    }
    glEnd();
}


/*
 *  Event Callback Function
 */
/// 스페셜 키가 입력되면 실행되는 콜백함수
void MySpecialKey(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT:
            slidingBarPosition -= slidingBarPosition + slidingBar.center.x - slidingBar.len / 2 > Wall[0].x ? slidingBarSpeed : 0;
            break;
        case GLUT_KEY_RIGHT:
            slidingBarPosition += slidingBarPosition + slidingBar.center.x + slidingBar.len / 2 < Wall[8].x ? slidingBarSpeed : 0;
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

/// Window 화면의 크기가 변경되면 실행되는 콜백함수
void MyReshape(int w, int h) {
    glViewport((w - WIDTH) / 2, (h - HEIGHT) / 2, WIDTH, HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(left, left + WIDTH, bottom, bottom + HEIGHT);
}

/// Window 화면을 출력할 때 실행되는 콜백함수
void RenderScene(void) {
    glClearColor(backGroundColor.red, backGroundColor.green, backGroundColor.blue, backGroundColor.clamp);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // 요소 출력
    glColor3f(ballColor.red, ballColor.green, ballColor.blue);
    ShowBall();
    
    glColor3f(slidingBarColor.red, slidingBarColor.green, slidingBarColor.blue);
    ShowSlidingBar();
    
    glColor3f(wallColor.red, wallColor.green, wallColor.blue);
    ShowWall();
    
    
    // 충돌 검증
    CollisionDetectionToWindow();
    
    ballPosition.x += ballSpeed.x;
    ballPosition.y += ballSpeed.y;
    
    glutSwapBuffers();
    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowPosition(0, 0);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Break the Block!");
    glutReshapeFunc(MyReshape);
    glutDisplayFunc(RenderScene);
    glutSpecialFunc(MySpecialKey);
    glutIdleFunc(RenderScene);
    glutMainLoop();
}
