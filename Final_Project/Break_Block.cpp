#include <GLUT/GLUT.h>
#include <iostream>
#include <cmath>
#include <random>
#include <ctime>
#include <string>

using namespace std;

#define    PI               M_PI
#define    WIDTH            1000
#define    HEIGHT           1000

#define    STATE_BREAK      0
#define    STATE_ONE        1
#define    STATE_TWO        2
#define    STATE_THREE      3

#define    MODE_DEFAULT     0
#define    MODE_SIZEUP      1
#define    MODE_SIZEDOWN    2
#define    MODE_SCORE       3
#define    MODE_COPY        4

#define    RECTANGLE_BLOCK_NUM 19
#define    WALL_NUM         9
#define    FIRENUM          1000

#define    GAMEREADY        0
#define    GAMERUN          1
#define    GAMEOVER         2
#define    GAMECLEAR        3
#define    GAMEHELP         4
#define    GAMERANKING      5

bool debug = false;

/*
 *  구조체 선언부
 */
/// 좌표들의 정보를 나타내는 구조체
typedef struct _Point {
    float    x = 0.0;
    float    y = 0.0;
} Point;

/// 색상(RGBC)의 정보를 나타내는 구조체
typedef struct _Color {
    float   red;
    float   green;
    float   blue;
    float   clamp = 0.0;
} Color;

Color ColorList[] = {
    { 0.99, 0.4, 0.4 },
    { 0.94, 0.6, 0.45 },
    { 0.9, 0.8, 0.5 },
    { 0.5, 0.8, 0.7 },
    { 0.5, 0.8, 0.9 },
    { 0.3, 0.4, 0.9 },
    { 0.6, 0.4, 0.95 },
    { 0.9, 0.9, 0.8 },
    { 0.4, 0.4, 0.4 }
};

/// 벽돌의 정보를 나타내는 구조체
typedef struct _Block {
    Point leftTop = { 0, 0 };
    Point leftBottom = { 0, 0 };
    Point rightTop = { 0, 0 };
    Point rightBottom = { 0, 0 };
    int mode = MODE_DEFAULT;
    int state = STATE_TWO;
    bool modeState = true;
    float x = 0;
    float y = 0;
    
    void Event() {
        if(y == 0 && modeState) {
            x = (rightBottom.x - leftBottom.x) / 2 + leftBottom.x;
            y = rightBottom.y;
        }
        if(modeState && !state && mode) {
            switch (mode) {
                case MODE_COPY:
                    glColor3f(ColorList[1].red, ColorList[1].green, ColorList[1].blue);
                    break;
                case MODE_SCORE:
                    glColor3f(ColorList[5].red, ColorList[5].green, ColorList[5].blue);
                    break;
                case MODE_SIZEUP:
                    glColor3f(ColorList[7].red, ColorList[7].green, ColorList[7].blue);
                    break;
                case MODE_SIZEDOWN:
                    glColor3f(ColorList[8].red, ColorList[8].green, ColorList[8].blue);
                    break;
                default:
                    break;
            }
            Show();
            y -= 0.5;
            if(y < 1) modeState = false;
        }
    }
    void Show() {
        int num = 32;
        float radius = 5;
        float delta = 2 * PI / num;
        glBegin(GL_POLYGON);
        for(int i = 0; i < num; i ++) {
            glVertex2f(x + radius * cos(delta * i), y + + radius * sin(delta * i));
        }
        glEnd();
    }
} Block;

/// 하단의 슬라이딩 바의 정보를 나타내는 구조체
typedef struct _Bar {
    Point center;
    int len;
    int weight;
} Bar;

/// 벡터를 나타내는 구조체
struct Vector {
    float x;
    float y;

    Vector(float _x = 0.0f, float _y = 0.0f) {
        x = _x;
        y = _y;
    }

    // 벡터 정규화
    void normalize() {
        float length = sqrt(x * x + y * y);
        x /= length;
        y /= length;
    }

    // 벡터 내적
    float dot(const Vector& other) const {
        return x * other.x + y * other.y;
    }
    
    // 벡터의 반사
    Vector reflect(const Vector& normal) const {
        float dotProduct = dot(normal);
        Vector reflection = *this - (normal * (2.0f * dotProduct));
        return reflection;
    }
    
    // 벡터 덧셈
    Vector operator+(const Vector& other) const {
        return Vector(x + other.x, y + other.y);
    }
    
    // 벡터 뺄셈
    Vector operator-(const Vector& other) const {
        return Vector(x - other.x, y - other.y);
    }

    // 스칼라 곱
    Vector operator*(float scalar) const {
        return Vector(x * scalar, y * scalar);
    }
};

struct Space {
    float x;
    float y;
    Color c;
};

class FireCracker {
private:
    int delay;
    int time = 0;
    int num;
    int len;
    float delta;
    float radius = 0;
    float x;    // x좌표(고정)
    float y;    // y좌표(0 ~ 최대)
    float maxY; // y좌표(최대)
    float maxRadius;
    float changeY;
    Color color;
    
public:
    /// FireCracker 생성자
    /// 고정 x좌표
    /// 최대 y좌표
    /// y좌표 변화량
    /// 폭죽 반지름
    /// 색상
    FireCracker(float _x, float _maxY, float _changeY, float _radius, Color _color, int _num, int _delay, int _len) {
        x = _x;
        maxY = _maxY;
        changeY = _changeY;
        maxRadius = _radius;
        color = _color;
        num = _num;
        delay = _delay;
        len = _len;
    }
    
    void Fire() {
        if(time > delay) {
            glColor3f(color.red, color.green, color.blue);
            glPointSize(2);
            
            if(maxY > y) {
                y += changeY;
                glBegin(GL_POINTS);
                for(int i = 0; i < len; i ++) {
                    glVertex2f(x, y - i * 10);
                }
                glEnd();
            }
            if(maxY <= y) {
                if(maxRadius > radius){
                    glBegin(GL_POINTS);
                    glVertex2f(x, y);
                    glEnd();
                    for(int i = 0; i < radius; i += 10) {
                        delta = 2 * PI / num;
                        glBegin(GL_POINTS);
                        for(int j = 0; j < num; j ++) {
                            glVertex2f(x + (i + 10) * cos(delta * j), y + (i + 10) * sin(delta * j));
                        }
                        glEnd();
                    }
                    radius += 0.5;
                }
            }
        }
        time ++;
    }
};

class Ball {
public:
    bool state = false;
    float ballRadius = 10.0;
    float speedSum;
    int beforeTouch = -1;
    Point ballPosition;
    Vector ballSpeed;
    Color color = { 0.6, 0.6, 0.6 };
    
    Ball(Point _ballPosition, Vector _ballSpeed) {
        ballPosition = _ballPosition;
        ballSpeed = _ballSpeed;
        speedSum = sqrt(ballSpeed.x * ballSpeed.x + ballSpeed.y * ballSpeed.y);
        state = true;
    }
    
    void Show() {
        if (state) {
            int num = 36;
            float delta = 2 * PI / num;
            glColor3f(color.red, color.green, color.blue);
            glBegin(GL_POLYGON);
            for(int i = 0; i < num; i ++) {
                glVertex2f(ballPosition.x + ballRadius * cos(delta * i), ballPosition.y + ballRadius * sin(delta * i));
            }
            glEnd();
        }
    }
    
    void ChangePosition() {
        ballPosition.x += ballSpeed.x;
        ballPosition.y += ballSpeed.y;
    }
    
};

typedef struct _RanKing {
    string name = "unk";
    int score = 1234;
    int time = 12345;
} RanKing;

RanKing ranking[5];

/*
 *  변수 선언부
 */
/// 가상 공간의 그리기 영역 좌측과 하단을 나타내는 변수
int _left = 0;
int bottom = 0;

/// 하단의 슬라이딩 바 선언 및 초기화
int slidingBarLen = 200;
int slidingBarWeight = 20;
int slidingBarSpeed = 10;
Bar slidingBar = { WIDTH / 2, 0, slidingBarLen, slidingBarWeight };

/// Sliding Bar Power Hit Mode 변수 선언 및 초기화
bool powerHitCheck = false;
float powerHitMax = 50;
float powerHitGauge = powerHitMax;
float powerHitVariation;
bool powerShut = false;
bool powerMode = false;

/// 공 선언 및 초기화
float ballRadius = 10.0;
Point ballPosition = { WIDTH / 2, slidingBarWeight + ballRadius };
float speedX = 1.0;
float speedY = 6.0;
float speedSum = sqrt(speedX * speedX + speedY * speedY);
Vector ballSpeed = { speedX, speedY };
int beforeTouch = -1;
float startX = 90;
float startY = 50;

/// 내부 벽 선언 및 초기화
Point Wall[WALL_NUM] = {
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

/// 내부 벽 법선벡터
Vector nomalWall[WALL_NUM - 1];

/// 직사각형 벽돌 선언 및 초기화
Block rectangleBlock[RECTANGLE_BLOCK_NUM];
int rectangleBlockLen = 100;
int rectangleBlockWeight = 50;

bool pause = true;
int  mode  = GAMERANKING;
bool start = false;

const int star_num = 100;
Space star[star_num];

int arrownum = 0;

int gameoverColor = 0;

FireCracker * fire[FIRENUM];
Ball * copyball[100];
int copycount = 0;
int life = 3;
int runtime = 0;
int score = 0;

/*
 *  Shape Color
 */
Color backGroundColor = { 0.1, 0.1, 0.1 };
Color softYellow = { 0.9, 0.8, 0.5 };
Color softWhite = { 0.97, 0.95, 0.99 };
Color softRed = { 0.99, 0.4, 0.4 };
Color softGreen = { 0.5, 0.8, 0.7 };
Color softBlue = { 0.6, 0.8, 0.95 };

/*
 *  Bitmap Setting
 */
bool ALPHA[][5][5] = {
    /// A
    {
        { 0, 1, 1, 1, 0 },
        { 1, 0, 0, 0, 1 },
        { 1, 0, 1, 1, 1 },
        { 1, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 1 }
    },
    /// B
    {
        { 1, 1, 1, 1, 0 },
        { 1, 0, 0, 0, 1 },
        { 1, 0, 1, 1, 0 },
        { 1, 0, 0, 0, 1 },
        { 1, 1, 1, 1, 0 }
    },
    /// C
    {
        { 0, 1, 1, 1, 1 },
        { 1, 0, 0, 0, 0 },
        { 1, 0, 0, 0, 0 },
        { 1, 0, 0, 0, 0 },
        { 0, 1, 1, 1, 1 }
    },
    /// D
    {
        { 1, 1, 1, 1, 0 },
        { 1, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 1 },
        { 1, 1, 1, 1, 0 }
    },
    /// E
    {
        { 0, 1, 1, 1, 1 },
        { 1, 0, 0, 0, 0 },
        { 1, 0, 1, 1, 1 },
        { 1, 0, 0, 0, 0 },
        { 0, 1, 1, 1, 1 }
    },
    /// F
    {
        { 0, 1, 1, 1, 1 },
        { 1, 0, 0, 0, 0 },
        { 1, 0, 1, 1, 1 },
        { 1, 0, 0, 0, 0 },
        { 1, 0, 0, 0, 0 }
    },
    /// G
    {
        { 0, 1, 1, 1, 0 },
        { 1, 0, 0, 0, 0 },
        { 1, 0, 1, 1, 0 },
        { 1, 0, 0, 0, 1 },
        { 0, 1, 1, 1, 0 }
    },
    /// H
    {
        { 1, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 1 },
        { 1, 1, 1, 1, 1 },
        { 1, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 1 }
    },
    /// I
    {
        { 0, 1, 1, 1, 0 },
        { 0, 0, 1, 0, 0 },
        { 0, 0, 1, 0, 0 },
        { 0, 0, 1, 0, 0 },
        { 0, 1, 1, 1, 0 }
    },
    /// J
    {
        { 0, 0, 0, 1, 0 },
        { 0, 0, 0, 1, 0 },
        { 0, 0, 0, 1, 0 },
        { 0, 0, 0, 1, 0 },
        { 0, 1, 1, 0, 0 }
    },
    /// K
    {
        { 1, 0, 0, 1, 1 },
        { 1, 0, 1, 0, 0 },
        { 1, 1, 1, 1, 0 },
        { 1, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 1 }
    },
    /// L
    {
        { 1, 0, 0, 0, 0 },
        { 1, 0, 0, 0, 0 },
        { 1, 0, 0, 0, 0 },
        { 1, 0, 0, 0, 0 },
        { 0, 1, 1, 1, 1 }
    },
    /// M
    {
        { 0, 1, 0, 1, 0 },
        { 1, 0, 1, 0, 1 },
        { 1, 0, 1, 0, 1 },
        { 1, 0, 1, 0, 1 },
        { 1, 0, 0, 0, 1 }
    },
    /// N
    {
        { 0, 1, 0, 0, 1 },
        { 1, 0, 1, 0, 1 },
        { 1, 0, 1, 0, 1 },
        { 1, 0, 1, 0, 1 },
        { 1, 0, 0, 1, 0 }
    },
    /// O
    {
        { 0, 1, 1, 1, 0 },
        { 1, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 1 },
        { 0, 1, 1, 1, 0 }
    },
    /// P
    {
        { 0, 1, 1, 1, 0 },
        { 1, 0, 0, 0, 1 },
        { 1, 0, 1, 1, 0 },
        { 1, 0, 0, 0, 0 },
        { 1, 0, 0, 0, 0 }
    },
    /// Q
    {
        { 0, 1, 1, 1, 0 },
        { 1, 0, 0, 0, 1 },
        { 1, 0, 1, 0, 1 },
        { 1, 0, 0, 1, 0 },
        { 0, 1, 1, 0, 1 }
    },
    /// R
    {
        { 1, 1, 1, 1, 0 },
        { 1, 0, 0, 0, 1 },
        { 1, 0, 1, 1, 0 },
        { 1, 0, 1, 0, 0 },
        { 1, 0, 0, 1, 1 }
    },
    /// S
    {
        { 0, 1, 1, 1, 1 },
        { 1, 0, 0, 0, 0 },
        { 0, 1, 1, 1, 0 },
        { 0, 0, 0, 0, 1 },
        { 1, 1, 1, 1, 0 }
    },
    /// T
    {
        { 1, 1, 1, 1, 1 },
        { 0, 0, 1, 0, 0 },
        { 0, 0, 1, 0, 0 },
        { 0, 0, 1, 0, 0 },
        { 0, 0, 1, 0, 0 }
    },
    /// U
    {
        { 1, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 1 },
        { 0, 1, 1, 1, 0 }
    },
    /// V
    {
        { 1, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 1 },
        { 0, 1, 0, 1, 0 },
        { 0, 1, 0, 1, 0 },
        { 0, 0, 1, 0, 0 }
    },
    /// W
    {
        { 1, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 1 },
        { 1, 0, 1, 0, 1 },
        { 1, 0, 1, 0, 1 },
        { 0, 1, 0, 1, 0 }
    },
    /// X
    {
        { 1, 0, 0, 0, 1 },
        { 0, 1, 0, 1, 0 },
        { 0, 0, 1, 0, 0 },
        { 0, 1, 0, 1, 0 },
        { 1, 0, 0, 0, 1 }
    },
    /// Y
    {
        { 1, 0, 0, 0, 1 },
        { 0, 1, 0, 1, 0 },
        { 0, 0, 1, 0, 0 },
        { 0, 0, 1, 0, 0 },
        { 0, 0, 1, 0, 0 }
    },
    /// Z
    {
        { 1, 1, 1, 1, 1 },
        { 0, 0, 0, 1, 0 },
        { 0, 0, 1, 0, 0 },
        { 0, 1, 0, 0, 0 },
        { 1, 1, 1, 1, 1 }
    }
};

/// 목록을 가리키는 화살표
bool ARROW[5][5] = {
    { 1, 0, 0, 0, 0 },
    { 0, 1, 0, 0, 0 },
    { 0, 0, 1, 0, 0 },
    { 0, 1, 0, 0, 0 },
    { 1, 0, 0, 0, 0 }
};

/// Bitmap Number Array
bool NUMBER[10][5][5] = {
    /// 0
    {
        { 0, 1, 1, 1, 0 },
        { 1, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 1 },
        { 0, 1, 1, 1, 0 }
    },
    /// 1
    {
        { 0, 1, 1, 0, 0 },
        { 0, 0, 1, 0, 0 },
        { 0, 0, 1, 0, 0 },
        { 0, 0, 1, 0, 0 },
        { 0, 1, 1, 1, 0 }
    },
    /// 2
    {
        { 0, 1, 1, 1, 0 },
        { 0, 0, 0, 0, 1 },
        { 0, 1, 1, 1, 0 },
        { 1, 0, 0, 0, 0 },
        { 0, 1, 1, 1, 1 }
    },
    /// 3
    {
        { 1, 1, 1, 1, 0 },
        { 0, 0, 0, 0, 1 },
        { 0, 0, 1, 1, 0 },
        { 0, 0, 0, 0, 1 },
        { 1, 1, 1, 1, 0 }
    },
    /// 4
    {
        { 1, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 1 },
        { 0, 1, 1, 1, 1 },
        { 0, 0, 0, 0, 1 },
        { 0, 0, 0, 0, 1 }
    },
    /// 5
    {
        { 0, 1, 1, 1, 1 },
        { 1, 0, 0, 0, 0 },
        { 1, 1, 1, 1, 0 },
        { 0, 0, 0, 0, 1 },
        { 0, 1, 1, 1, 0 }
    },
    /// 6
    {
        { 0, 1, 1, 1, 1 },
        { 1, 0, 0, 0, 0 },
        { 1, 1, 1, 1, 0 },
        { 1, 0, 0, 0, 1 },
        { 0, 1, 1, 1, 0 }
    },
    /// 7
    {
        { 1, 1, 1, 1, 0 },
        { 0, 0, 0, 0, 1 },
        { 0, 0, 1, 1, 1 },
        { 0, 0, 0, 0, 1 },
        { 0, 0, 0, 0, 1 }
    },
    /// 8
    {
        { 0, 1, 1, 1, 0 },
        { 1, 0, 0, 0, 1 },
        { 0, 1, 1, 1, 0 },
        { 1, 0, 0, 0, 1 },
        { 0, 1, 1, 1, 0 }
    },
    /// 9
    {
        { 0, 1, 1, 1, 0 },
        { 1, 0, 0, 0, 1 },
        { 0, 1, 1, 1, 1 },
        { 0, 0, 0, 0, 1 },
        { 1, 1, 1, 1, 0 }
    },
};


/*
 *  Initial Function
 */
///
void InitWall() {
    for(int i = 0; i < WALL_NUM - 1; i ++) {
        Vector v = { Wall[i + 1].x - Wall[i].x, Wall[i + 1].y - Wall[i].y };
        nomalWall[i] = { v.y, -v.x };
    }
}

void InitSpace() {
    srand(time(NULL));
    for(int i = 0; i < star_num; i ++) {
        Color color = {
            red:static_cast <float> (rand()) / static_cast <float> (RAND_MAX),
            green:static_cast <float> (rand()) / static_cast <float> (RAND_MAX),
            blue:static_cast <float> (rand()) / static_cast <float> (RAND_MAX)
        };
        star[i].c = color;
        star[i].x = rand() % WIDTH;
        star[i].y = rand() % HEIGHT;
    }
}

void InitClear() {
    for(int i = 0; i < FIRENUM; i ++) {
        float x = rand() % 1000;
        float y = rand() % 500 + 400;
        float change = (rand() % 100) / 10.0 + 0.5;
        float radius = rand() % 50 + 100;
        float red = (rand() % 50 + 50) / 100.0;
        float green = (rand() % 50 + 50) / 100.0;
        float blue = (rand() % 50 + 50) / 100.0;
        Color color = { red, green, blue };
        int num = rand() % 30 + 20;
        int d = rand() % 10000;
        int len = rand() % 5 + 2;
        
        fire[i] = new FireCracker(x, y, change, radius, color, num, d, len);
    }
}

/*
 *  Create Function
 */
/// 직사각형 벽돌을 생성해주는 함수
void CreateRectangleBlock() {
    int startX = 0;
    int xVariation = 0;
    int startY = 0;
    for(int i = 0; i < RECTANGLE_BLOCK_NUM; i ++) {
        if(i < 3) {
            startX = 350;
            startY = 650;
            xVariation = i;
        }
        else if(i < 7) {
            startX = 300;
            startY = 600;
            xVariation = i % 4;
        }
        else if(i < 12) {
            startX = 250;
            startY = 550;
            xVariation = i % 7;
        }
        else if(i < 16) {
            startX = 300;
            startY = 500;
            xVariation = i % 12;
        }
        else if(i < 19) {
            startX = 350;
            startY = 450;
            xVariation = i % 16;
        }

        rectangleBlock[i].leftTop.x = startX + xVariation * rectangleBlockLen;
        rectangleBlock[i].leftTop.y = startY;

        rectangleBlock[i].leftBottom.x = startX + xVariation * rectangleBlockLen;
        rectangleBlock[i].leftBottom.y = startY - rectangleBlockWeight;

        rectangleBlock[i].rightTop.x = startX + xVariation * rectangleBlockLen + rectangleBlockLen;
        rectangleBlock[i].rightTop.y = startY;

        rectangleBlock[i].rightBottom.x = startX + xVariation * rectangleBlockLen + rectangleBlockLen;
        rectangleBlock[i].rightBottom.y = startY - rectangleBlockWeight;
        
//        int mode = rand() % 19 - 15;
//        rectangleBlock[i].mode = mode < 1 ? MODE_DEFAULT : mode;
        rectangleBlock[i].mode = rand() % 4 + 1;
        
        rectangleBlock[i].state = rand() % 3 + 1;
    }
}

void CreateCopyBall() {
    float px = rand() % 600 + 200;
    float py = rand() % 400 + 50;
    Point position = { px, py };
    float sx = (rand() % 10 - 5) / 2.0;
    float sy = (rand() % 10 ) / 2.0;
    Vector speed = { sx, sy };
    
    copyball[copycount ++] = new Ball(position, speed);
}


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
    float m = inclination(a1, a2);
    if (a2.x - a1.x != 0) {
        return m * x - m * a1.x + a1.y;
    } else {
        // x 값이 두 점의 x 좌표와 동일한 경우, 해당 x 값에 대응하는 y 값을 직접 반환
        return a1.y;
    }
}

/// 벽돌의 갯수를 반환하는 함수
int CountBlock() {
    int count = 0;
    for(int i = 0; i < RECTANGLE_BLOCK_NUM; i ++) {
        count += rectangleBlock[i].state;
    }
    return count;
}

/// 공의 속도를 변환하는 함수
void ChangeSpeed(float change) {
    speedX *= change;
    speedY *= change;
    speedSum = sqrt(speedX * speedX + speedY * speedY);
}

/// 두 선분의 교점을 구하는 함수
Point MeetPoint(Point a1, Point a2, Point b1, Point b2) {
    float x = ((a1.x * a2.y - a1.y * a2.x) * (b1.x - b2.x) - (a1.x - a2.x) * (b1.x * b2.y - b1.y * b2.x)) / ((a1.x - a2.x) * (b1.y - b2.y) - (a1.y - a2.y) * (b1.x - b2.x));
    float y = ((a1.x * a2.y - a1.y * a2.x) * (b1.y - b2.y) - (a1.y - a2.y) * (b1.x * b2.y - b1.y * b2.x)) / ((a1.x - a2.x) * (b1.y - b2.y) - (a1.y - a2.y) * (b1.x - b2.x));
    Point p = { x, y };
    
    return p;
}


/*
 *  SpecialMode Function
 */
/// Sliding Bar Power Hit Mode
void PowerHit() {
    //cout << powerHitGauge << endl;
    if(powerHitCheck) {
        if(powerHitGauge > slidingBar.center.y) {
            slidingBar.center.y += powerHitVariation;
        }
        else {
            powerShut = false;
            speedY *= speedY < 0 ? -1 : 1;
            powerHitCheck = false;
            powerHitGauge = 0;
        }
    }
    // Power Hit가 끝나고 게이지가 채워지는 과정
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
void CollisionDetectionToWindow(Point& ball = ballPosition, Vector& speed = ballSpeed, float* radius = &ballRadius, int* touch = &beforeTouch, bool* state = nullptr) {
    speed.x *= ball.x + *radius >= WIDTH ? -1 : 1;
    speed.x *= ball.x - *radius <= 0 ? -1 : 1;
    speed.y *= ball.y + *radius >= HEIGHT ? -1 : 1;
    if(ball.y - *radius <= 0) {
        if(state != nullptr) {
            *state = false;
        }
        else {
            life --;
            speed.y *= -1;
        }
    }
}



/// 점과 직선 사이의 거리를 구하는 함수
/// d = |Ax + By + C| / sqrt(A^2 + B^2)
float PointToLineDistance(Point p, Point w1, Point w2) {
    float A = inclination(w1, w2); // 기울기
    float B = -1.0f;
    float C = return_Y(0, w1, w2); // Y 절편
    
    return abs(A * p.x + B * p.y + C) / sqrt(A * A + B * B);
}

/// 점과 점 사이의 거리를 구하는 함수
float PointToPointDistance(Point a, Point b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}


/// 내부 벽과 충돌을 확인하는 함수(방법 번경)
void CollisionDetectionToWall(Point& ball = ballPosition, Vector& speed = ballSpeed, float* radius = &ballRadius, int* touch = &beforeTouch) {
    
    for(int i = 0; i < WALL_NUM - 1; i ++) {
        // 벽 y 좌표상의 위아래를 나눠주는 조건문
        float top, bottom;
        if(Wall[i].y > Wall[i + 1].y) {
            top = Wall[i].y;
            bottom = Wall[i + 1].y;
        }
        else {
            top = Wall[i + 1].y;
            bottom = Wall[i].y;
        }
        
        float distance;
        bool check = false;
        
        // 벽의 y가 같은 경우 예외처리 x축에 평행한 벽
        if(i == 2 || i == 5) {
            distance = abs(Wall[i].y - ball.y);
            float l = (Wall[i].x < Wall[i + 1].x) ? Wall[i].x : Wall[i + 1].x;
            float r = (Wall[i].x < Wall[i + 1].x) ? Wall[i + 1].x : Wall[i].x;
            
            if(l <= ball.x && r >= ball.x) {
                check = true;
            }
        }
        
        // 공이 벽 내부의 y값이 넘어가는지 확인하는 조건문
        if ((bottom <= ball.y && top >= ball.y ) || check) {
            if(!check) {
                // 벽의 x가 같은 경우 예외처리 y축에 평행한 벽
                if(i == 0 || i == 7) {
                    distance = abs(Wall[i].x - ball.x);
                }
                // 대각선 부분 처리
                else {
                    distance = PointToLineDistance(ball, Wall[i], Wall[i + 1]);
                }
            }
            if (distance <= *radius && *touch != i){
                // 공이 충돌해서 벽을 넘어간 경우 벽에 넘어가기 직전으로(다여있는 곳으로) 이동
                Point before = { ball.x - speed.x, ball.y - speed.y };
                Point meet = MeetPoint(Wall[i], Wall[i + 1], ball, before);
                float len = PointToPointDistance(ball, before);
                float ratio = len / PointToPointDistance(ball, meet);
                float x = (ball.x - before.x) * ratio;
                float y = (ball.y - before.y) * ratio;
                
                ball.x = before.x + x;
                ball.y = before.y + y;
                
                // 공의 방향 변경
                // 벡터의 정규화
                speed.normalize();
                nomalWall[i].normalize();

                // 반사 벡터 계산
                Vector v = speed.reflect(nomalWall[i]);
                float vSum = sqrt(v.x * v.x + v.y * v.y);
                                    
                speed.x = v.x * (speedSum / vSum / 2);
                speed.y = v.y * (speedSum / vSum / 2);
                
                cout << i << "벽충돌\n";
                
                *touch = i;
                
                return;
            }
        }
    }
}

/// 벽면 모서리와의 충돌을 확인하는 함수
void CollisionDetectionToCorner(Point& ball = ballPosition, Vector& speed = ballSpeed, float* radius = &ballRadius, int* touch = &beforeTouch) {
    for(int i = 1; i < WALL_NUM - 1; i ++) {
        float distance = (ball.x - Wall[i].x) * (ball.x - Wall[i].x) + (ball.y - Wall[i].y) * (ball.y - Wall[i].y);
        if(distance <= pow(*radius, 2)) {
            Vector v = nomalWall[i - 1] + nomalWall[i];
            float vSum = sqrt(v.x * v.x + v.y * v.y);
                                
            speed.x = v.x * (speedSum / vSum / 2);
            speed.y = v.y * (speedSum / vSum / 2);
        }
    }
}


/// 슬라이딩바와 충돌을 확인하는 함수
void CollisionDetectionToSlidingBar(Point& ball = ballPosition, Vector& speed = ballSpeed, float* radius = &ballRadius, int* touch = &beforeTouch) {
    if (*touch != RECTANGLE_BLOCK_NUM + WALL_NUM + 1) {
        float distance = pow(ball.x - (slidingBar.center.x - slidingBarLen / 2), 2) + pow(ball.y - (slidingBar.center.y + slidingBarWeight), 2);
        if(distance <= pow(*radius, 2)) {
            cout << "슬라이딩 바 좌측 상단 충돌" << endl;
            Vector v = { -1.0, 1.0 };
            float vSum = sqrt(v.x * v.x + v.y * v.y);
            
            speed.x = v.x * (speedSum / vSum / 2);
            speed.y = v.y * (speedSum / vSum / 2);
            
            *touch = RECTANGLE_BLOCK_NUM + WALL_NUM + 1;
            
            return;
        }
        
        distance = pow(ball.x - (slidingBar.center.x + slidingBarLen / 2), 2) + pow(ball.y - (slidingBar.center.y + slidingBarWeight), 2);
        if(distance <= pow(*radius, 2)) {
            cout << "슬라이딩 바 우측 상단 충돌" << endl;
            Vector v = { 1.0, 1.0 };
            float vSum = sqrt(v.x * v.x + v.y * v.y);
            
            speed.x = v.x * (speedSum / vSum / 2);
            speed.y = v.y * (speedSum / vSum / 2);
            
            *touch = RECTANGLE_BLOCK_NUM + WALL_NUM + 1;
            
            return;
        }
        
        if(ball.y - *radius < slidingBar.center.y + slidingBar.weight) {
            if(ball.x < slidingBar.center.x + slidingBarLen / 2 && ball.x > slidingBar.center.x - slidingBarLen / 2){
                if(powerHitCheck) {
                    powerShut = true;
                    powerMode = true;
                }
                speed.y *= -1;
                *touch = RECTANGLE_BLOCK_NUM + WALL_NUM + 1;
            }
        }
    }
}


/// 직사각형 벽돌과의 충돌을 확인하는 함수
void CollisionDetectionToRectangleBlock() {
    for(int i = 0; i < RECTANGLE_BLOCK_NUM; i ++) {
        if(rectangleBlock[i].state && i + WALL_NUM != beforeTouch) {
            // 벽돌의 모서리와의 충돌을 확인
            // 좌측 하단
            float distance = pow(ballPosition.x - rectangleBlock[i].leftBottom.x, 2) + pow(ballPosition.y - rectangleBlock[i].leftBottom.y, 2);
            if(distance <= ballRadius * ballRadius) {
                cout << "벽돌 좌측 하단 충돌" << endl;
                Vector v = { -1.0, -1.0 };
                float vSum = sqrt(v.x * v.x + v.y * v.y);
                                    
                ballSpeed.x = v.x * (speedSum / vSum / 2);
                ballSpeed.y = v.y * (speedSum / vSum / 2);
                
                if (powerMode) {
                    rectangleBlock[i].state = 0;
                    powerMode = false;
                }
                else rectangleBlock[i].state --;
                
                if(!rectangleBlock[i].state) score += 50;
                
                beforeTouch = i + WALL_NUM;
                
                return;
            }
            
            // 우측 하단
            distance = pow(ballPosition.x - rectangleBlock[i].rightBottom.x, 2) + pow(ballPosition.y - rectangleBlock[i].rightBottom.y, 2);
            if(distance <= ballRadius * ballRadius) {
                cout << "벽돌 우측 하단 충돌" << endl;
                Vector v = { 1.0, -1.0 };
                float vSum = sqrt(v.x * v.x + v.y * v.y);
                                    
                ballSpeed.x = v.x * (speedSum / vSum / 2);
                ballSpeed.y = v.y * (speedSum / vSum / 2);
                
                if (powerMode) {
                    rectangleBlock[i].state = 0;
                    powerMode = false;
                }
                else rectangleBlock[i].state --;
                
                if(!rectangleBlock[i].state) score += 50;
                
                beforeTouch = i + WALL_NUM;
                
                return;
            }
            
            // 우측 상단
            distance = pow(ballPosition.x - rectangleBlock[i].rightTop.x, 2) + pow(ballPosition.y - rectangleBlock[i].rightTop.y, 2);
            if(distance <= ballRadius * ballRadius) {
                cout << "벽돌 우측 상단 충돌" << endl;
                Vector v = { 1.0, 1.0 };
                float vSum = sqrt(v.x * v.x + v.y * v.y);
                                    
                ballSpeed.x = v.x * (speedSum / vSum / 2);
                ballSpeed.y = v.y * (speedSum / vSum / 2);
                
                if (powerMode) {
                    rectangleBlock[i].state = 0;
                    powerMode = false;
                }
                else rectangleBlock[i].state --;
                
                if(!rectangleBlock[i].state) score += 50;
                
                beforeTouch = i + WALL_NUM;
                
                return;
            }
            
            // 좌측 상단
            distance = pow(ballPosition.x - rectangleBlock[i].leftTop.x, 2) + pow(ballPosition.y - rectangleBlock[i].leftTop.y, 2);
            if(distance <= ballRadius * ballRadius) {
                cout << "벽돌 좌측 상단 충돌" << endl;
                Vector v = { -1.0, 1.0 };
                float vSum = sqrt(v.x * v.x + v.y * v.y);
                                    
                ballSpeed.x = v.x * (speedSum / vSum / 2);
                ballSpeed.y = v.y * (speedSum / vSum / 2);
                
                
                if (powerMode) {
                    rectangleBlock[i].state = 0;
                    powerMode = false;
                }
                else rectangleBlock[i].state --;
                
                if(!rectangleBlock[i].state) score += 50;
                
                beforeTouch = i + WALL_NUM;
                
                return;
            }
            
            // 공이 벽돌과 충돌을 확인
            if(ballPosition.y + ballRadius >= rectangleBlock[i].leftBottom.y && ballPosition.y - ballRadius <= rectangleBlock[i].leftTop.y) {
                if(ballPosition.x + ballRadius >= rectangleBlock[i].leftBottom.x && ballPosition.x - ballRadius <= rectangleBlock[i].rightBottom.x){
                    
                    if (powerMode) {
                        rectangleBlock[i].state = 0;
                        powerMode = false;
                    }
                    else rectangleBlock[i].state --;
                    
                    if(!rectangleBlock[i].state) score += 50;
                    
                    Point _block;
                    _block.x = rectangleBlock[i].leftBottom.x + rectangleBlockLen / 2;
                    _block.y = rectangleBlock[i].leftBottom.y + rectangleBlockWeight / 2;
                    float inc = inclination(ballPosition, _block);
                    
                    if(inc < 0.5 && inc > -0.5) {
                        ballSpeed.x *= -1;
                    }
                    else {
                        ballSpeed.y *= -1;
                    }
                    
                    beforeTouch = i + WALL_NUM;
                    
                    return;
                }
            }
        }
    }
}

/// 직사각형 벽돌과 복사된 공의 충돌을 확인하는 함수
void CollisionDetectionToCopyBall() {
    for(int i = 0; i < RECTANGLE_BLOCK_NUM; i ++) {
        for(int j = 0; j < copycount; j ++) {

            if(rectangleBlock[i].state && i + WALL_NUM != copyball[j]->beforeTouch) {
                // 벽돌의 모서리와의 충돌을 확인
                // 좌측 하단
                float distance = pow(copyball[j]->ballPosition.x - rectangleBlock[i].leftBottom.x, 2) + pow(copyball[j]->ballPosition.y - rectangleBlock[i].leftBottom.y, 2);
                if(distance <= copyball[j]->ballRadius * copyball[j]->ballRadius) {
                    cout << "벽돌 좌측 하단 충돌" << endl;
                    Vector v = { -1.0, -1.0 };
                    float vSum = sqrt(v.x * v.x + v.y * v.y);
                                        
                    copyball[j]->ballSpeed.x = v.x * (speedSum / vSum / 2);
                    copyball[j]->ballSpeed.y = v.y * (speedSum / vSum / 2);
                    
                    rectangleBlock[i].state --;
                    
                    if(!rectangleBlock[i].state) score += 50;
                    
                    copyball[j]->beforeTouch = i + WALL_NUM;
                    
                    return;
                }
                
                // 우측 하단
                distance = pow(copyball[j]->ballPosition.x - rectangleBlock[i].rightBottom.x, 2) + pow(copyball[j]->ballPosition.y - rectangleBlock[i].rightBottom.y, 2);
                if(distance <= copyball[j]->ballRadius * copyball[j]->ballRadius) {
                    cout << "벽돌 우측 하단 충돌" << endl;
                    Vector v = { 1.0, -1.0 };
                    float vSum = sqrt(v.x * v.x + v.y * v.y);
                                        
                    copyball[j]->ballSpeed.x = v.x * (speedSum / vSum / 2);
                    copyball[j]->ballSpeed.y = v.y * (speedSum / vSum / 2);
                    
                    rectangleBlock[i].state --;
                    if(!rectangleBlock[i].state) score += 50;
                    
                    copyball[j]->beforeTouch = i + WALL_NUM;
                    
                    return;
                }
                
                // 우측 상단
                distance = pow(copyball[j]->ballPosition.x - rectangleBlock[i].rightTop.x, 2) + pow(copyball[j]->ballPosition.y - rectangleBlock[i].rightTop.y, 2);
                if(distance <= copyball[j]->ballRadius * copyball[j]->ballRadius) {
                    cout << "벽돌 우측 상단 충돌" << endl;
                    Vector v = { 1.0, 1.0 };
                    float vSum = sqrt(v.x * v.x + v.y * v.y);
                                        
                    copyball[j]->ballSpeed.x = v.x * (speedSum / vSum / 2);
                    copyball[j]->ballSpeed.y = v.y * (speedSum / vSum / 2);
                    
                    rectangleBlock[i].state --;
                    if(!rectangleBlock[i].state) score += 50;
                    
                    copyball[j]->beforeTouch = i + WALL_NUM;
                    
                    return;
                }
                
                // 좌측 상단
                distance = pow(copyball[j]->ballPosition.x - rectangleBlock[i].leftTop.x, 2) + pow(copyball[j]->ballPosition.y - rectangleBlock[i].leftTop.y, 2);
                if(distance <= copyball[j]->ballRadius * copyball[j]->ballRadius) {
                    cout << "벽돌 좌측 상단 충돌" << endl;
                    Vector v = { -1.0, 1.0 };
                    float vSum = sqrt(v.x * v.x + v.y * v.y);
                                        
                    copyball[j]->ballSpeed.x = v.x * (speedSum / vSum / 2);
                    copyball[j]->ballSpeed.y = v.y * (speedSum / vSum / 2);
                    
                    rectangleBlock[i].state --;
                    if(!rectangleBlock[i].state) score += 50;
                    
                    copyball[j]->beforeTouch = i + WALL_NUM;
                    
                    return;
                }
                
                // 공이 벽돌과 충돌을 확인
                if(copyball[j]->ballPosition.y + copyball[j]->ballRadius >= rectangleBlock[i].leftBottom.y && copyball[j]->ballPosition.y - copyball[j]->ballRadius <= rectangleBlock[i].leftTop.y) {
                    if(copyball[j]->ballPosition.x + copyball[j]->ballRadius >= rectangleBlock[i].leftBottom.x && copyball[j]->ballPosition.x - copyball[j]->ballRadius <= rectangleBlock[i].rightBottom.x){
                        
                        rectangleBlock[i].state --;
                        if(!rectangleBlock[i].state) score += 50;
                        
                        Point _block;
                        _block.x = rectangleBlock[i].leftBottom.x + rectangleBlockLen / 2;
                        _block.y = rectangleBlock[i].leftBottom.y + rectangleBlockWeight / 2;
                        float inc = inclination(copyball[j]->ballPosition, _block);
                        
                        if(inc < 0.5 && inc > -0.5) {
                            copyball[j]->ballSpeed.x *= -1;
                        }
                        else {
                            copyball[j]->ballSpeed.y *= -1;
                        }

                        copyball[j]->beforeTouch = i + WALL_NUM;
                        
                        return;
                    }
                }
            }
        }
    }
}


/// 슬라이딩바와 충돌을 확인하는 함수
void CollisionDetectionToItem() {
    for(int i = 0; i < RECTANGLE_BLOCK_NUM; i ++) {
        if(!rectangleBlock[i].state && rectangleBlock[i].modeState){
            if(rectangleBlock[i].y < slidingBar.center.y + slidingBar.weight) {
                if(rectangleBlock[i].x < slidingBar.center.x + slidingBarLen / 2 && rectangleBlock[i].x > slidingBar.center.x - slidingBarLen / 2){
                    rectangleBlock[i].modeState = false;
                    switch (rectangleBlock[i].mode) {
                        case MODE_COPY:
                            cout << "MODE_COPY" << endl;
                            CreateCopyBall();
                            break;
                        case MODE_SCORE:
                            cout << "MODE_SCORE" << endl;
                            score += 100;
                            break;
                        case MODE_SIZEUP:
                            cout << "MODE_SIZEUP" << endl;
                            ballRadius += ballRadius < 50 ? 4 : 0;
                            break;
                        case MODE_SIZEDOWN:
                            cout << "MODE_SIZEDOWN" << endl;
                            ballRadius -= ballRadius > 5 ? 2 : 0 ;
                            break;
                            
                        default:
                            break;
                    }
                }
            }
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
    ballPosition.y = powerShut ? slidingBar.center.y + slidingBarWeight + ballRadius + 3 : ballPosition.y;
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
    for(int i = 0; i < WALL_NUM; i ++) {
        glVertex2f(Wall[i].x, Wall[i].y);
    }
    glEnd();
}

/// 직사각형 벽돌을 출력해주는 함수
void ShowRectangleBlock() {
    for(int i = 0; i < RECTANGLE_BLOCK_NUM; i ++) {
        if(rectangleBlock[i].state) {
            switch (rectangleBlock[i].state) {
                case STATE_ONE:
                    glColor3f(softRed.red, softRed.green, softRed.blue);
                    break;
                case STATE_TWO:
                    glColor3f(softBlue.red, softBlue.green, softBlue.blue);
                    break;
                case STATE_THREE:
                    glColor3f(softGreen.red, softGreen.green, softGreen.blue);
                default:
                    break;
            }
            glBegin(GL_POLYGON);
            glVertex2f(rectangleBlock[i].leftTop.x, rectangleBlock[i].leftTop.y);
            glVertex2f(rectangleBlock[i].leftBottom.x, rectangleBlock[i].leftBottom.y);
            glVertex2f(rectangleBlock[i].rightBottom.x, rectangleBlock[i].rightBottom.y);
            glVertex2f(rectangleBlock[i].rightTop.x, rectangleBlock[i].rightTop.y);
            glEnd();

            glColor3f(0, 0, 0);
            glBegin(GL_LINE_LOOP);
            glVertex2f(rectangleBlock[i].leftTop.x, rectangleBlock[i].leftTop.y);
            glVertex2f(rectangleBlock[i].leftBottom.x, rectangleBlock[i].leftBottom.y);
            glVertex2f(rectangleBlock[i].rightBottom.x, rectangleBlock[i].rightBottom.y);
            glVertex2f(rectangleBlock[i].rightTop.x, rectangleBlock[i].rightTop.y);
            glEnd();
        }
        else {
            if(pause) rectangleBlock[i].Event();
            else {
                switch (rectangleBlock[i].mode) {
                case MODE_COPY:
                    glColor3f(ColorList[1].red, ColorList[1].green, ColorList[1].blue);
                    break;
                case MODE_SCORE:
                    glColor3f(ColorList[5].red, ColorList[5].green, ColorList[5].blue);
                    break;
                case MODE_SIZEUP:
                    glColor3f(ColorList[7].red, ColorList[7].green, ColorList[7].blue);
                    break;
                case MODE_SIZEDOWN:
                    glColor3f(ColorList[8].red, ColorList[8].green, ColorList[8].blue);
                    break;
                default:
                    break;
                }
                rectangleBlock[i].Show();
            }
        }
    }
}

void ShowArrow() {
    float delta = 2 * PI / 360;
    float x = slidingBar.center.x + startY * cos(delta * startX);
    float y = slidingBarWeight + startY * sin(delta * startX);
    glColor3f(softRed.red, softRed.green, softRed.blue);
    glLineWidth(5);
    glBegin(GL_LINES);
    glVertex2f(slidingBar.center.x, 0);
    glVertex2f(x, y);
    glEnd();
}


/*
 *  내부 화면들
 */
/// 원을 그려주는 함수
void DrawCircle(float x, float y, float c_radius) {
   float    delta;
   int      num = 36;

   delta = 2 * PI / num;
   glBegin(GL_POLYGON);
   for (int i = 0; i < num; i++)
      glVertex2f(x + c_radius * cos(delta * i), y + c_radius * sin(delta * i));
   glEnd();
}

/// 우주 배경을 출력하는 함수
void DrawSpace() {
    glBegin(GL_POLYGON);
    for(int i = 0; i < star_num; i ++) {
        glColor3f(star[i].c.red, star[i].c.green, star[i].c.blue);
        DrawCircle(star[i].x, star[i].y, rand() % 3);
    }
}

/// 게이지 바를 출력하는 함수
void GaugeBar() {
    /// 게이지바 정도
    for(int i = 1; i <= (int)powerHitGauge / 7; i ++) {
        glBegin(GL_POLYGON);
        glColor3f(ColorList[7 - i].red, ColorList[7 - i].green, ColorList[7 - i].blue);
        glVertex2f(50.0, 50.0 + (i - 1) * 28.5);
        glVertex2f(100.0, 50.0 + (i - 1) * 28.5);
        glVertex2f(100.0, 50.0 + i * 28.5);
        glVertex2f(50.0, 50.0 + i * 28.5);
        glEnd();
    }
    
    /// 게이지 바 테두리
    glColor3f(softWhite.red, softWhite.green, softWhite.blue);
    glBegin(GL_LINE_LOOP);
    glVertex2f(50.0, 50.0);
    glVertex2f(100.0, 50.0);
    glVertex2f(100.0, 50.0 + powerHitMax * 4);
    glVertex2f(50.0, 50.0 + powerHitMax * 4);
    glEnd();
    
    /// 최소 게이지
    glBegin(GL_LINES);
    glColor3f(softRed.red, softRed.green, softRed.blue);
    glVertex2f(45, 164);
    glVertex2f(105, 164);
    glEnd();
}

/// 하트를 출력하는 함수
void Heart() {
    glColor3f(softRed.red, softRed.green, softRed.blue);
    glBegin(GL_POLYGON);
    double r = 0, t = 0;
    double x = 0, y = 0;
    
    while(t < 6.28)
    {
        if(t > 3.14) r = sqrt(9.0 / (17.0 + 8 * sin(2.0 * t)));
        else   r = sqrt(9.0 / (17.0 - 8 * sin(2.0 * t)));
        
        x = r * sin(t);
        y = r * cos(t);
        
        glVertex2f((x * 50) + 500, (y * 50) + 200);
        
        t += 0.1;
    }
    glEnd();
}

/// 남은 목숨을 출력하는 함수
void ShowLife() {
    for(int i = 0; i < life; i ++) {
        glPushMatrix();
        glTranslatef(425, -125 + 100 * i, 0);
        Heart();
        glPopMatrix();
    }
}


/*
 *  Draw Bitmap
 */
void DrawRectangle(int x, int y, int w) {
    glBegin(GL_POLYGON);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + w);
    glVertex2f(x, y + w);
    glEnd();
}

/// 입력 받은 알파벳을 출력하는 함수
void DrawALPHA(float startX, float startY, int size, char c) {
    if(c > 'Z') c ^= ' ';

    for(int y = 0; y < 5; y ++) {
        for(int x = 0; x < 5; x ++) {
            if(c != ' ' && ALPHA[c - 'A'][y][x]) {
                DrawRectangle(x * size + startX , (4 - y) * size + startY, size);
            }
        }
    }
}


/// GAME Bitmap 출력 함수
void DrawGAME() {
    int size = 32;
    string str = "GAME";
    for(int index = 0; index < 4; index ++) {
        if(mode == GAMEOVER){
            glColor3f(ColorList[(index + gameoverColor / 10) % 8].red, ColorList[(index + gameoverColor / 10) % 8].green, ColorList[(index + gameoverColor / 10) % 8].blue);
        }
        else if(mode == GAMECLEAR) {
            glColor3f(ColorList[index].red, ColorList[index].green, ColorList[index].blue);
        }
        DrawALPHA(132 + index * (size * 6), 600, size, str.at(index));
    }
}

/// START Bitmap 출력 함수
void DrawSTART() {
    int size = 10;
    string str = "START";
    for(int index = 0; index < 5; index ++) {
        DrawALPHA(400 + index * (size * 6), 500, size, str.at(index));
    }
}

void DrawHELP() {
    int size = 10;
    string str = "HELP";
    for(int index = 0; index < 4; index ++) {
        DrawALPHA(400 + index * (size * 6), 400, size, str.at(index));
    }
}

void DrawRANK() {
    int size = 10;
    string str = "RANK";
    for(int index = 0; index < 4; index ++) {
        DrawALPHA(400 + index * (size * 6), 300, size, str.at(index));
    }
}

/// EXIT Bitmap 출력 함수
void DrawEXIT() {
    int size = 10;
    string str = "EXIT";
    for(int index = 0; index < 4; index ++) {
        DrawALPHA(400 + index * (size * 6), 200, size, str.at(index));
    }
}

/// ARROW Bitmap 출력 함수
void DrawARROW() {
    int size = 10;
    for(int y = 0; y < 5; y ++) {
        for(int x = 0; x < 5; x ++) {
            if(ARROW[y][x]) {
                DrawRectangle(x * size + 300, (4 - y) * size + 500 - (100 * arrownum), size);
            }
        }
    }
}

/// PAUSE Bitmap 출력 함수
void DrawPAUSE() {
    int size = 20;
    string str = "PAUSE";
    for(int index = 0; index < 5; index ++) {
        DrawALPHA(210 + index * (size * 6), 400, size, str.at(index));
    }
}

/// OVER Bitmap 출력 함수
void DrawOVER() {
    int size = 32;
    string str = "OVER";
    for(int index = 0; index < 4; index ++) {
        if(mode == GAMEOVER){
            glColor3f(ColorList[(index + 4 + gameoverColor / 10) % 8].red, ColorList[(index + 4 + gameoverColor / 10) % 8].green, ColorList[(index + 4 + gameoverColor / 10) % 8].blue);
        }
        DrawALPHA(132 + index * (size * 6), 300, size, str.at(index));
    }

}

/// CLEAR Bitmap 출력 함수
void DrawCLEAR() {
    int size = 25;
    string str = "CLEAR";
    for(int index = 0; index < 5; index ++) {
        glColor3f(ColorList[index + 4].red, ColorList[index + 4].green, ColorList[index + 4].blue);
        DrawALPHA(137 + index * (size * 6), 400, size, str.at(index));
    }
}

/// 소요 시간을 알려주는 함수
void DrawTimer() {
    glColor3f(softYellow.red, softYellow.green, softYellow.blue);
    int size = 10;
    int time = runtime;
    int timer[5] = { 0 };
    
    timer[0] = time / 10000;
    time %= 10000;
    timer[1] = time / 1000;
    time %= 1000;
    timer[2] = time / 100;
    time %= 100;
    timer[3] = time / 10;
    timer[4] = time % 10;
    for(int index = 0; index < 5; index ++) {
        for(int y = 0; y < 5; y ++) {
            for(int x = 0; x < 5; x ++) {
                if(NUMBER[timer[index]][y][x]) {
                    DrawRectangle(x * size + 650 + index * (size * 6), (4 - y) * size + 900, size);
                }
            }
        }
    }
}

void DrawScore() {
    glColor3f(softYellow.red, softYellow.green, softYellow.blue);
    int size = 10;
    int yscore = score;
    int num[4] = { 0 };
    
    num[0] = yscore / 1000;
    yscore %= 1000;
    num[1] = yscore / 100;
    yscore %= 100;
    num[2] = yscore / 10;
    num[3] = yscore % 10;
    
    for(int index = 0; index < 4; index ++) {
        for(int y = 0; y < 5; y ++) {
            for(int x = 0; x < 5; x ++) {
                if(NUMBER[num[index]][y][x]) {
                    DrawRectangle(x * size + 50 + index * (size * 6), (4 - y) * size + 900, size);
                }
            }
        }
    }
}

void DrawNumber() {
    int size = 5;
    int num[] = { 2, 0, 1, 9, 3, 2, 1, 0 };
    
    for(int index = 0; index < 8; index ++) {
        glColor3f(ColorList[index].red, ColorList[index].green, ColorList[index].blue);
        
        for(int y = 0; y < 5; y ++) {
            for(int x = 0; x < 5; x ++) {
                if(NUMBER[num[index]][y][x]) {
                    DrawRectangle(x * size + 50 + index * (size * 6), (4 - y) * size + 50, size);
                }
            }
        }
    }
}

void DrawNUM(float startX, float startY, int size, int num) {
    for(int y = 0; y < 5; y ++) {
        for(int x = 0; x < 5; x ++) {
            if(NUMBER[num][y][x]) {
                DrawRectangle(x * size + startX, (4 - y) * size + startY, size);
            }
        }
    }
}

void DrawHelp() {
    int size = 20;
    string str = "HELP";
    for(int index = 0; index < 4; index ++) {
        glColor3f(softYellow.red, softYellow.green, softYellow.blue);
        DrawALPHA(270 + index * (size * 6), 800, size, str.at(index));
    }
}

void DrawRank() {
    int size = 20;
    string str = "RANK";
    for(int index = 0; index < 4; index ++) {
        glColor3f(softYellow.red, softYellow.green, softYellow.blue);
        DrawALPHA(270 + index * (size * 6), 800, size, str.at(index));
    }
}

void DrawMEMO(float startX, float startY, int size, string str) {
    for(int index = 0; index < str.length(); index ++) {
        DrawALPHA(startX + index * (size * 6), startY, size, str.at(index));
    }
}


/*
 *  GAMEMODE PAGE
 */
void ShowREADY(){
    DrawSpace();
    glColor3f(softYellow.red,softYellow.green,softYellow.blue);
    DrawGAME();
    if(arrownum != 0) glColor3f(softWhite.red, softWhite.green, softWhite.blue);
    else glColor3f(softRed.red, softRed.green, softRed.blue);
    DrawSTART();
    if(arrownum != 1) glColor3f(softWhite.red, softWhite.green, softWhite.blue);
    else glColor3f(softRed.red, softRed.green, softRed.blue);
    DrawHELP();
    if(arrownum != 2) glColor3f(softWhite.red, softWhite.green, softWhite.blue);
    else glColor3f(softRed.red, softRed.green, softRed.blue);
    DrawRANK();
    if(arrownum != 3) glColor3f(softWhite.red, softWhite.green, softWhite.blue);
    else glColor3f(softRed.red, softRed.green, softRed.blue);
    DrawEXIT();
    glColor3f(softRed.red, softRed.green, softRed.blue);
    DrawARROW();
}

void ShowCLEAR() {
    for(int i = 0; i < FIRENUM; i ++) {
        fire[i] -> Fire();
    }
    DrawGAME();
    DrawCLEAR();
    DrawTimer();
    ShowLife();
}

void ShowHELP() {
    DrawHelp();
    
    float startX = 80;
    float startY = 700;
    float size = 4;
    
    glColor3f(softWhite.red, softWhite.green, softWhite.blue);
    DrawNUM(startX - 40, startY, size, 1);
    DrawMEMO(startX, startY, size, "Control is possible through the rudder ");
    DrawMEMO(startX, startY - 40, size, "key and space bar");
    
    startY -= 100;
    DrawNUM(startX - 40, startY, size, 2);
    DrawMEMO(startX, startY, size, "Initially you can use the rudder to ");
    DrawMEMO(startX, startY - 40, size, "specify the direction and speed of");
    DrawMEMO(startX, startY - 80, size, "the ball");
    
    startY -= 140;
    
    DrawNUM(startX - 40, startY, size, 3);
    DrawMEMO(startX, startY, size, "You can use the ESC key to stop the");
    DrawMEMO(startX, startY - 40, size, "game");
    
    startY -= 100;
    DrawNUM(startX - 40, startY, size, 4);
    DrawMEMO(startX, startY, size, "Press the space bar to hit the ball ");
    DrawMEMO(startX, startY - 40, size, "and it will be in power mode");
    DrawMEMO(startX, startY - 80, size, "It is three times as fast as it flies");
    DrawMEMO(startX, startY - 120, size, "and it destroys all the block at once");
    
    startY -= 180;
    DrawNUM(startX - 40, startY, size, 5);
    DrawMEMO(startX, startY, size, "If you destroy a block you will get");
    DrawMEMO(startX, startY - 40, size, "a special item");

    
    size = 3;
    startX = 650;
    startY = 50;
    glColor3f(softRed.red, softRed.green, softRed.blue);
    DrawMEMO(startX, startY, size, "Push esc to back");
}

void ReverseNum(int num, int* arr, int len) {
    for(int i = 0; i < len; i ++) {
        arr[len - 1 - i] = num % 10;
        num /= 10;
    }
}

void ShowRANK() {
    DrawRank();
    
    glColor3f(softWhite.red, softWhite.green, softWhite.blue);
    
    float startX = 40;
    float startY = 700;
    float size = 6;
    
    DrawMEMO(startX, startY, size, "NO");
    DrawMEMO(startX + 125, startY, size, "user");
    DrawMEMO(startX + 360, startY, size, "score");
    DrawMEMO(startX + 680, startY, size, "time");
    
    startX = 150;
    startY = 600;
    size = 10;
    
    for(int i = 0; i < 5; i ++) {
        glColor3f(ColorList[i * 2].red, ColorList[i * 2].green, ColorList[i * 2].blue);
        DrawNUM(startX - 100, startY, size, i + 1);
        DrawMEMO(startX, startY, size, ranking[i].name);
        
        int score[4];
    
        ReverseNum(ranking[i].score, score, 4);
        for(int j = 0; j < 4; j ++) {
            DrawNUM(startX + 220 + j * 60, startY, size, score[j]);
        }
        
        int timer[5];
        
        ReverseNum(ranking[i].time, timer, 5);
        for(int j = 0; j < 5; j ++) {
            DrawNUM(startX + 500 + j * 60, startY, size, timer[j]);
        }
        
        startY -= 110;
    }
    
    size = 3;
    startX = 650;
    startY = 50;
    glColor3f(softRed.red, softRed.green, softRed.blue);
    DrawMEMO(startX, startY, size, "Push esc to back");
}

/*
 *  Event Callback Function
 */
/// 스페셜 키가 입력되면 실행되는 콜백함수
void MySpecialKey(int key, int x, int y) {
    if (debug) {
        switch (key) {
            case GLUT_KEY_UP:
                ballPosition.y += 5;
                break;
            case GLUT_KEY_DOWN:
                ballPosition.y -= 5;
                break;
            case GLUT_KEY_LEFT:
                ballPosition.x -= 5;
                break;
            case GLUT_KEY_RIGHT:
                ballPosition.x += 5;
                break;
            case GLUT_KEY_PAGE_UP:
                debug = !debug;
                break;
            default:
                break;
        }
    }
    else {
        switch (key) {
            case GLUT_KEY_LEFT:
                if(!start) {
                    startX += startX < 180 ? + 1 : 0;
                }
                else if(pause) {
                    slidingBar.center.x -= slidingBar.center.x - slidingBar.len / 2 > Wall[0].x ? slidingBarSpeed : 0;
                }
                break;
            case GLUT_KEY_RIGHT:
                if(!start) {
                    startX += startX > 0 ? - 1 : 0;
                }
                else if(pause) {
                    slidingBar.center.x += slidingBar.center.x + slidingBar.len / 2 < Wall[8].x ? slidingBarSpeed : 0;
                }
                break;
                // 스페이스바
            case 32:
                switch (mode) {
                    case GAMEREADY:
                        switch (arrownum) {
                            case 0:
                                mode = GAMERUN;
                                break;
                            case 1:
                                mode = GAMEHELP;
                                break;
                            case 2:
                                mode = GAMERANKING;
                                break;
                            case 3:
                                exit(0);
                                break;
                                
                            default:
                                break;
                        }
                        break;
                    case GAMERUN:
                        if(!start && startX != 90) {
                            float delta = 2 * PI / 360;
                            start = !start;
                            ballSpeed.x = (startY / 20) * cos(delta * startX);
                            
                            ballSpeed.y = (startY / 20) * sin(delta * startX);
                        }
                        else if (start && powerHitGauge > 30){
                            powerHitCheck = true;
                            powerHitVariation = powerHitGauge / 20;
                        }
                        break;
                    default:
                        break;
                }
                break;
                // 게임 일시정지
            case 27:
                switch (mode) {
                    case GAMERUN:
                        pause = pause ? false : true;
                        break;
                    case GAMECLEAR:
                        exit(0);
                        break;
                    case GAMEOVER:
                        exit(0);
                        break;
                    case GAMEHELP:
                        mode = GAMEREADY;
                        break;
                    case GAMERANKING:
                        mode = GAMEREADY;
                        break;
                    default:
                        break;
                }
                break;
            case GLUT_KEY_UP:
                switch (mode) {
                    case GAMEREADY:
                        arrownum += arrownum - 1 < 0 ? 3 : - 1;
                        break;
                    case GAMERUN:
                        startY += startY < 100 ? + 1 : 0;
                        break;
                }
                break;
            case GLUT_KEY_DOWN:
                switch (mode) {
                    case GAMEREADY:
                        arrownum = ++arrownum % 4;
                        break;
                    case GAMERUN:
                        startY += startY > 20 ? - 1 : 0;
                        break;
                }
                break;
            case GLUT_KEY_PAGE_UP:
                debug = !debug;
            default:
                break;
        }
    }
    glutPostRedisplay();
}

/// Window 화면의 크기가 변경되면 실행되는 콜백함수
void MyReshape(int w, int h) {
    glViewport((w - WIDTH) / 2, (h - HEIGHT) / 2, WIDTH, HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(_left, _left + WIDTH, bottom, bottom + HEIGHT);
}

/// Window 화면을 출력할 때 실행되는 콜백함수
void RenderScene(void) {
    glClearColor(backGroundColor.red, backGroundColor.green, backGroundColor.blue, backGroundColor.clamp);
    glClear(GL_COLOR_BUFFER_BIT);
    
    if(mode == GAMEREADY) {
        ShowREADY();
        DrawNumber();
    }
    else if(mode == GAMERUN){
        //int yscore = score + (RECTANGLE_BLOCK_NUM - CountBlock() * 50);

        // 충돌 검증
        CollisionDetectionToWindow();
        CollisionDetectionToCorner();
        CollisionDetectionToWall();
        CollisionDetectionToSlidingBar();
        CollisionDetectionToRectangleBlock();
        CollisionDetectionToItem();
        CollisionDetectionToCopyBall();
        
        // 복사공 충돌 검증
        for (int i = 0; i < copycount; i ++) {
            if (copyball[i]->state) {
                CollisionDetectionToWindow(copyball[i]->ballPosition, copyball[i]->ballSpeed, &copyball[i]->ballRadius, &copyball[i]->beforeTouch, &copyball[i]->state);
                CollisionDetectionToCorner(copyball[i]->ballPosition, copyball[i]->ballSpeed, &copyball[i]->ballRadius, &copyball[i]->beforeTouch);
                CollisionDetectionToWall(copyball[i]->ballPosition, copyball[i]->ballSpeed, &copyball[i]->ballRadius, &copyball[i]->beforeTouch);
                CollisionDetectionToSlidingBar(copyball[i]->ballPosition, copyball[i]->ballSpeed, &copyball[i]->ballRadius, &copyball[i]->beforeTouch);
            }
        }
        
        if(start) {
            for(int i = 0; i < copycount; i ++) {
                copyball[i]->Show();
            }
        }
        
        // 요소 출력
        if(powerMode) glColor3f(softRed.red, softRed.green, softRed.blue);
        else glColor3f(softWhite.red, softWhite.green, softWhite.blue);
        
        if(start) {
            ShowBall();
        }
        
        if(!start) ShowArrow();
        
        glColor3f(softBlue.red, softBlue.green, softBlue.blue);
        ShowSlidingBar();

        glColor3f(softYellow.red, softYellow.green, softYellow.blue);
        ShowWall();

        ShowRectangleBlock();

        GaugeBar();
        ShowLife();
        DrawTimer();
        DrawScore();
        
        if(debug){
            
        }
        else {
            if(CountBlock() && pause && start) {
                // 공의 위치 결정
                ChangeSpeed(1);
                if(powerMode) {
                    ballPosition.x += powerShut ? 0.0 : ballSpeed.x;
                    ballPosition.y += ballSpeed.y * 3;
                }
                else {
                    ballPosition.x += powerShut ? 0.0 : ballSpeed.x;
                    ballPosition.y += ballSpeed.y;
                }
                for(int i = 0; i < copycount; i ++) {
                    copyball[i]->ChangePosition();
                }
                runtime++;
            }
        }
        
        if(!life) {
            mode = GAMEOVER;
        }
        
        if(!CountBlock()) {
            mode = GAMECLEAR;
        }
        
        if(!pause) {
            glColor3f(1.0f, 1.0f, 1.0f);
            DrawPAUSE();
        }
    }
    else if(mode == GAMEOVER) {
        DrawGAME();
        DrawOVER();
        gameoverColor = gameoverColor == 0 ? 80 : gameoverColor - 1;
        DrawTimer();
        DrawNumber();
        DrawScore();
    }
    else if(mode == GAMECLEAR) {
        ShowCLEAR();
        DrawNumber();
        DrawScore();
    }
    else if(mode == GAMEHELP) {
        ShowHELP();
        
    }
    else if(mode == GAMERANKING) {
        ShowRANK();
    }
    glutSwapBuffers();
    glFlush();
}


int main(int argc, char** argv) {
    InitSpace();
    InitWall();
    InitClear();
    glutInit(&argc, argv);
    CreateRectangleBlock();
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



