#include <GLUT/GLUT.h>
#include <cmath>
#include <iostream>
using namespace std;

/// 카메라 화면 변수 선언
float radius;
float theta, phi;
float delta_theta, delta_phi;

/// 도형 변수 선언
float rotation;
float speed = 1.0;
float up = 0.0;
bool state = false;

/// 도형 좌표값
GLfloat vertices[][3] = {
        { -1.0, -1.0,  1.0 },      // 0
        { -1.0,  1.0,  1.0 },      // 1
        {  1.0,  1.0,  1.0 },      // 2
        {  1.0, -1.0,  1.0 },      // 3
        { -1.0, -1.0, -1.0 },      // 4
        { -1.0,  1.0, -1.0 },      // 5
        {  1.0,  1.0, -1.0 },      // 6
        {  1.0, -1.0, -1.0 }       // 7
};

/// 도형 색상값
GLfloat colors[][3] = {
        { 1.0, 0.0, 0.0 },         // red
        { 0.0, 1.0, 0.0 },         // green
        { 1.0, 1.0, 0.0 },         // yellow
        { 1.0, 1.0, 1.0 },         // white
        { 0.0, 0.0, 1.0 },         // blue
        { 1.0, 0.0, 1.0 }          // magenta
};

/// 도형 이동 정도
GLfloat translateV[][3] = {
    {  0.0,  0.0,  0.0 },
    {  0.0,  1.0, -1.0 },
    {  1.0,  0.0, -1.0 },
    {  0.0, -1.0, -1.0 },
    {  0.0,  0.0,  0.0 },
    { -1.0,  0.0, -1.0 }
};

/// 도형 회전 정도
GLfloat rotateV[][3] = {
    {  0.0,  0.0,  0.0 },
    { -1.0,  0.0,  0.0 },
    {  0.0,  1.0,  0.0 },
    {  1.0,  0.0,  0.0 },
    {  0.0,  0.0,  0.0 },
    {  0.0, -1.0,  0.0 }
};

/// 함수 구현
void init();
void polygon(int a, int b, int c, int d);
void transform(int a, int b, int c, int d);
void cube(float x = 0, float y = 0, float z = 0);
void reshape(int w, int h);
void axis(void);
void frame_reset(void);
void camera(void);
void display(void);
void specialKey(int key, int x, int y);
void normalKey(unsigned char key, int x, int y);

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1000, 1000);
    glutCreateWindow("cube");
    init();
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(specialKey);
    glutKeyboardFunc(normalKey);
    
    glutMainLoop();
}


/// 변수 초기값 설정
void init(){
    // 카메라 화면
    radius = 15.0;
    theta = 7.0;
    phi = 0.5;
    
    // 카메라 변환 정도
    delta_theta = 0.1;
    delta_phi = 0.1;
    
    // 도형 옆면 각도
    rotation = 0.0;
    // 도형 옆면 각도 변화량
    speed = 1.0;
    // 도형 열기 / 닫기 모드
    state = false;
    // 도형 뚜껑 위치
    up = 0.0;
    

    glEnable(GL_DEPTH_TEST);
}


/// 3차원 공간에 사각형을 만들어주는 함수
void polygon(int a, int b, int c, int d) {
    glColor3fv(colors[a]);
    glBegin(GL_POLYGON);
    glVertex3fv(vertices[a]);
    glVertex3fv(vertices[b]);
    glVertex3fv(vertices[c]);
    glVertex3fv(vertices[d]);
    glEnd();
}

/// 3차원 공간에 있는 사각형의 움직임을 만들어주는 함수
void transform(int a, int b, int c, int d) {
    glPushMatrix();
    glTranslatef(translateV[a][0], translateV[a][1], translateV[a][2]);
    glRotatef(rotation, rotateV[a][0], rotateV[a][1], rotateV[a][2]);
    glTranslatef(-translateV[a][0], -translateV[a][1], -translateV[a][2]);
    polygon(a, b, c, d);
    glPopMatrix();
}

/// 정육면체를 만들어주는 함수
void cube(float x, float y, float z) {
    glPushMatrix();
    //glRotatef(rotation * 10, 0, 0, 1.0);
    glTranslated(x, y, z);
    polygon(4, 5, 6, 7);  // 바닥
    glPushMatrix();
    glTranslatef(0.0, 0.0, up);
    polygon(0, 3, 2, 1);  // 뚜껑
    glPopMatrix();
    transform(1, 2, 6, 5);
    transform(2, 3, 7, 6);
    transform(3, 0, 4, 7);
    transform(5, 4, 0, 1);
    glPopMatrix();
}

/// Window 화면이 변하는 이벤트가 들어왔을 때 실행하는 콜백함수
void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.0, 1.0, 20.0);
}

/// 기준 축을 만들어주는 함수
void axis(void) {
    glLineWidth(3);
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0); // x축
    for (int i = 0; i < 3; i ++) {
        glColor3f(i == 0 ? 1.0 : 0.0, i == 1 ? 1.0 : 0.0, i == 2 ? 1.0 : 0.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(i == 0 ? 10.0 : 0.0, i == 1 ? 10.0 : 0.0, i == 2 ? 10.0 : 0.0);
    }
    glEnd();
}

/// 새로운 영상을 출력하기 위해 컬러 버퍼를 초기화 해주는 함수
void frame_reset(void) {
    glClearColor(0.6, 0.6, 0.6, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/// 카메라의 위치를 설정해주는 함수
void camera(void) {
    float x, y, z;
    float r = 1.0;
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    x = radius * cos(phi) * cos(theta);
    y = radius * cos(phi) * sin(theta);
    z = radius * sin(phi);
    if(cos(phi) < 0) r *= -1;
    
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, r);
}

/// 출력 할 영상을 만들어 주는 함수
void display(void)
{
    frame_reset();
    camera();
    axis();
    
    float far = 3.0;
    cube();
    // cube( far,  0.0,  0.0);
    // cube(-far,  0.0,  0.0);
    // cube( 0.0,  far,  0.0);
    // cube( 0.0, -far,  0.0);
    // cube( 0.0,  0.0,  far);
    // cube( 0.0,  0.0, -far);

    if(state && rotation <= 90)
        rotation += speed;
    else if (!state && rotation > 0 && up < 0)
        rotation -= speed;
    
    if(state && rotation > 90 && up < 3)
        up += 0.03;
    else if (!state && up > 0)
        up -= 0.03;
    
    glFlush();
    glutSwapBuffers();
}

/// Key Event가 들어왔을 때 특별한 동작을 할 수 있도록 만들어준 이벤트 콜백함수
void specialKey(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT:  theta -= delta_theta;     break;     // Left  :  카메라 좌로 회전
        case GLUT_KEY_RIGHT: theta += delta_theta;     break;     // Right :  카메라 우로 회전
        case GLUT_KEY_UP:    phi += delta_phi;         break;     // Up    :  카메라 상단으로 회전
        case GLUT_KEY_DOWN:  phi -= delta_phi;         break;     // Down  :  카메라 하단으로 회전
        case GLUT_KEY_F1:    radius -= 0.5;            break;     // F1    :  카메라 확대
        case GLUT_KEY_F2:    radius += 0.5;            break;     // F2    :  카메라 축소
        case 32:             state = state?false:true; break;     // Space :  도형 OPEN/CLOSE 모드 설정
    }
    glutPostRedisplay();
}

void normalKey(unsigned char key, int x, int y) {
    switch (key) {
        case 't':            state = state?false:true; break;     // t     :  도형 OPEN/CLOSE 모드 설정
    }
    glutPostRedisplay();
}
