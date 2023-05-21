#include <GLUT/GLUT.h>
#include <iostream>
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
int slidingBarSpeed = 10;
Bar slidingBar = { WIDTH / 2, 0, slidingBarLen, slidingBarWeight };

/// Sliding Bar Power Hit Mode 변수 초기값 및 선언
bool powerHitCheck = false;
float powerHitMax = 50;
float powerHitGauge = powerHitMax;
float powerHitVariation;

/// 공 초기값 및 선언
float ballRadius = 10.0;
Point ballPosition = { WIDTH / 2, slidingBarWeight + ballRadius};
Point ballSpeed = { 0.5, 2.0 };

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
 *  Math Function
 */
/// 기울기를 반환해주는 함수
float inclination(Point a1, Point a2) {
    return (a1.x == a2.x) ? 0 : (a2.y - a1.y) / (a2.x - a1.x);
}

/// 두 점과 y 값을 넣으면 x 값을 반환해주는 함수
float return_X(float y, Point a1, Point a2) {
    return (y - a1.y) / inclination(a1, a2) + a1.x;
}

/// 두 점과 x 값을 넣으면 y 값을 반환해주는 함수
float return_Y(float x, Point a1, Point a2) {
    return inclination(a1, a2) * x - a1.x + a1.y;
}


/*
 *  SpecialMode Function
 */
/// Sliding Bar Power Hit Mode
void PowerHit() {
    std::cout << powerHitGauge << std::endl;
    if(powerHitCheck) {
        if(powerHitGauge > slidingBar.center.y) {
            slidingBar.center.y += powerHitVariation;
        }
        else {
            powerHitCheck = false;
            powerHitGauge = 0;
        }
    }
    else {
        if(slidingBar.center.y > 0) {
            slidingBar.center.y -= powerHitVariation;
        }
        if(slidingBar.center.y < 0) {
            slidingBar.center.y = 0;
        }
        if(slidingBar.center.y == 0) {
            powerHitGauge += powerHitGauge < powerHitMax ? 0.1 : 0;
        }
    }
}


/*
 *  CollisionDetection Function
 */
/// 외부 벽과의 충돌을 확인하는 함수
void CollisionDetectionToWindow() {
    ballSpeed.x *= ballPosition.x + ballRadius >= WIDTH ? -1 : 1;
    ballSpeed.x *= ballPosition.x - ballRadius <= 0 ? -1 : 1;
    ballSpeed.y *= ballPosition.y + ballRadius >= HEIGHT ? -1 : 1;
    ballSpeed.y *= ballPosition.y - ballRadius <= 0 ? -1 : 1;
}

/// 내부 벽과의 충돌을 확인하는 함수
void CollisionDetectionToWall(void){
    for(int i = 0; i < 8; i ++) {
        /// 좌측 기울기가 - 인 대각선
        if(i == 1) {
            if (Wall[i].y <= ballPosition.y && Wall[i + 1].y >= ballPosition.y) {
                if (return_X(ballPosition.y, Wall[i], Wall[i+1]) >= ballPosition.x - ballRadius) {
                    ballSpeed.x *= -1;
                }
            }
        }
        /// 상단 기울기가 - 인 대각선
        else if(i == 4) {
            if (Wall[i + 1].y <= ballPosition.y && Wall[i].y >= ballPosition.y) {
                if (return_X(ballPosition.y, Wall[i], Wall[i+1]) <= ballPosition.x + ballRadius) {
                    ballSpeed.x *= -1;
                }
            }
        }
        /// 상단 기울기가 + 인 대각선
        else if(i == 3) {
            if (Wall[i].y <= ballPosition.y && Wall[i + 1].y >= ballPosition.y) {
                if (return_X(ballPosition.y, Wall[i], Wall[i+1]) >= ballPosition.x - ballRadius) {
                    ballSpeed.x *= -1;
                }
            }
        }
        /// 우측 기울기가 + 인 대각선
        else if(i == 6) {
            if (Wall[i + 1].y <= ballPosition.y && Wall[i].y >= ballPosition.y) {
                if (return_X(ballPosition.y, Wall[i], Wall[i+1]) <= ballPosition.x + ballRadius) {
                    ballSpeed.x *= -1;
                }
            }
        }
        /// 좌측 y 축에 평행한 직선
        else if(i == 0) {
            if(Wall[i].x >= ballPosition.x - ballRadius && Wall[i].y <= ballPosition.y && Wall[i + 1].y >= ballPosition.y) {
                ballSpeed.x *= -1;
            }
        }
        /// 우측 y 축에 평행한 직선
        else if(i == 7) {
            if(Wall[i].x <= ballPosition.x + ballRadius && Wall[i + 1].y <= ballPosition.y && Wall[i].y >= ballPosition.y) {
                ballSpeed.x *= -1;
            }
        }
        /// 좌측 x 축에 평행한 직선
        else if(i == 2) {
            if(Wall[i].x <= ballPosition.x + ballRadius && Wall[i + 1].x >= ballPosition.x - ballRadius) {
                if(Wall[i].y <= ballPosition.y + ballRadius) {
                    ballSpeed.y *= -1;
                }
            }
        }
        /// 우측 x 축에 평행한 직선
        else if(i == 5) {
            if(Wall[i].x <= ballPosition.x + ballRadius && Wall[i + 1].x >= ballPosition.x - ballRadius) {
                if(Wall[i].y <= ballPosition.y + ballRadius) {
                    ballSpeed.y *= -1;
                }
            }
        }
    }
}

/// 슬라이딩바와 충돌을 확인하는 함수
void CollisionDetectionToSlidingBar() {
    if(ballPosition.y - ballRadius < slidingBar.center.y + slidingBar.weight) {
        if(ballPosition.x < slidingBar.center.x + slidingBarLen / 2 && ballPosition.x > slidingBar.center.x - slidingBarLen / 2){
            ballSpeed.y *= -1;
            std::cout << "Detection" << std::endl;
        }
    }
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
    PowerHit();
    glBegin(GL_POLYGON);
    glVertex2i(slidingBar.center.x - slidingBar.len / 2, slidingBar.center.y);
    glVertex2i(slidingBar.center.x - slidingBar.len / 2, slidingBar.center.y + slidingBar.weight);
    glVertex2i(slidingBar.center.x + slidingBar.len / 2, slidingBar.center.y + slidingBar.weight);
    glVertex2i(slidingBar.center.x + slidingBar.len / 2, slidingBar.center.y);
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
            slidingBar.center.x -= slidingBar.center.x - slidingBar.len / 2 > Wall[0].x ? slidingBarSpeed : 0;
            break;
        case GLUT_KEY_RIGHT:
            slidingBar.center.x += slidingBar.center.x + slidingBar.len / 2 < Wall[8].x ? slidingBarSpeed : 0;
            break;
        case 32:
            powerHitCheck = true;
            powerHitVariation = powerHitGauge / 20;
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
    CollisionDetectionToWall();
    CollisionDetectionToSlidingBar();
    
    // 공의 위치 결정
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
