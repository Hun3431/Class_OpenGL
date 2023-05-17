#include <GLUT/GLUT.h>
#include <math.h>

void draw_figure(int x) {
	float radius = 0.5;
    float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < x; ++i) {
		angle = 2.0f * M_PI * i / x;
		float x = radius * cos(angle);
		float y = radius * sin(angle);
		glVertex2f(x, y);
	}
	glEnd();
	glFinish();
}

void display(void) {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.1f, 0.3f, 0.1f);

    int x = 17;
    draw_figure(x);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Figure");
	glutDisplayFunc(display);
	glutMainLoop();
}
