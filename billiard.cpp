#include "main.h"

#define DRAG_SENSITIVITY 30.f
#define Pai 3.14;


void Circle2DFill(float radius, int x, int y)
{
	glColor3f(0.f, 0.f, 0.f);
	for (float th1 = 0.0; th1 <= 360.0; th1 = th1 + 1.0)
	{
		float th2 = th1 + 10.0;
		float th1_rad = th1 / 180.0 * Pai;
		float th2_rad = th2 / 180.0 * Pai;

		float x1 = radius * cos(th1_rad);
		float y1 = radius * sin(th1_rad);
		float x2 = radius * cos(th2_rad);
		float y2 = radius * sin(th2_rad);

		glBegin(GL_TRIANGLES);
		glVertex2f(x, y);
		glVertex2f(x1 + x, y1 + y);
		glVertex2f(x2 + x, y2 + y);
		glEnd();
	}
}

void drawTable(void) {
	Circle2DFill(0.6f, -4.f, -4.f);
	Circle2DFill(0.6f, 4.f, -4.f);
	Circle2DFill(0.6f, 4.f, 4.f);
	Circle2DFill(0.6f, -4.f, 4.f);

	textureID = loadTexture("wood.bmp");

	glBindTexture(GL_TEXTURE_2D, textureID);
	glEnable(GL_TEXTURE_2D);

	glNormal3f(0,1,0);
	glColor3f(1.f, 1.f, 1.f);

	glBegin(GL_POLYGON);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-5.f, -5.f, 0.f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-5.f, 5.f, 0.f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(5.f, 5.f, 0.f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(5.f, -5.f, 0.f);
	glEnd();

	glDisable(GL_TEXTURE_2D);

}

void mouseFunction(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		float normX = (2.f * x) / WINDOW_WIDTH - 1.f;
		float normY = 1.f - (2.f * y) / WINDOW_HEIGHT;
		isDragging = true;
		initialMouseX = normX;
		initialMouseY = normY;
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		isDragging = false;
	}
}

void motionFunction(int x, int y) {
	if (isDragging && selectedBall >= 0 && selectedBall < numSpheres) {
		float normX = (2.f * x) / WINDOW_WIDTH - 1.f;
		float normY = 1.f - (2.f * y) / WINDOW_HEIGHT;

		float diffX = normX - initialMouseX;
		float diffY = normY - initialMouseY;

		vx[selectedBall] = DRAG_SENSITIVITY * diffX;
		vy[selectedBall] = DRAG_SENSITIVITY * diffY;
	}
}

void printCharacter(const char* buffer, const int width, const int height)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)WINDOW_WIDTH, 0.0, (GLdouble)WINDOW_HEIGHT);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColor3f(1.0f, 1.0f, 1.0f);
	glRasterPos2i(width, WINDOW_HEIGHT - height);
	for (unsigned i = 0; i < strlen(buffer); i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, buffer[i]);
	}

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

void printScore(void)
{
	char buffer[256];
	sprintf_s(buffer, sizeof(buffer), "Score :  %3d ", score);
	printCharacter(buffer, 10, 30);
}

void setCamera(void) {
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.f, (GLdouble)WINDOW_WIDTH / (GLdouble)WINDOW_HEIGHT, 0.1f, 100.f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.f, 0.f, 20.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f);
}

void setLight(void)
{
	GLfloat light_position[4] = { 5.f, 5.f, 5.f, 1.f };
	GLfloat light_ambient[4] = { 0.3f, 0.3f, 0.3f, 0.f };
	GLfloat light_diffuse[4] = { 1.f, 1.f, 1.f, 1.f };
	GLfloat light_specular[4] = { 1.f, 1.f, 1.f, 1.f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
}

void displaySphere(float radius, float dr, float dg, float db, float sr, float sg, float sb, float shininess)
{
	float diffuse[4] = { dr, dg, db, 1.f };
	float specular[4] = { sr, sg, sb, 1.f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
	glutSolidSphere(radius, 100, 100);
}

void displayAxis(float length)
{
	glBegin(GL_LINES);
	glColor3f(1.f, 0.f, 0.f);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(length, 0.f, 0.f);

	glColor3f(0.f, 1.f, 0.f);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(0.f, length, 0.f);

	glColor3f(0.f, 0.f, 1.f);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(0.f, 0.f, length);
	glEnd();
}

void displayWalls(float length)
{
	length = length / 2.1f;

	glColor3f(1.f, 1.f, 1.f);
	glBegin(GL_LINE_LOOP);
	glVertex3f(length, length, 0.f);
	glVertex3f(-length, length, 0.f);
	glVertex3f(-length, -length, 0.f);
	glVertex3f(length, -length, 0.f);
	glEnd();
}

void initSphere(void)
{
	vx[0] = 0.f;
	vy[0] = 0.f;
	vx[1] = 0.f;
	vy[1] = 0.f;
	vx[2] = 0.f;
	vy[2] = 0.f;
	px[0] = (rnd() % 50) / 10.0f;
	py[0] = (rnd() % 50) / 10.0f;
	px[1] = (rnd() % 50) / 10.0f;
	py[1] = (rnd() % 50) / 10.0f;
	px[2] = (rnd() % 50) / 10.0f;
	py[2] = (rnd() % 50) / 10.0f;
	radius[0] = 0.5f;
	radius[1] = 0.5f;
	radius[2] = 0.5f;
	mass[0] = 1.f;
	mass[1] = 1.f;
	mass[2] = 1.f;
	ecoef = 1.f;
	ecoef_wall = 0.9f;
}

unsigned char* loadBMPFile(const char* filename, int* width, int* height)
{
	const int HEADERSIZE = 54;
	int i, j;
	int real_width;
	unsigned int color;
	FILE* fp;
	unsigned char header_buf[HEADERSIZE];
	unsigned char* tmp;
	unsigned char* pixel;
	errno_t err = fopen_s(&fp, filename, "rb");
	if (err != 0) {
		fprintf(stderr, "Cannot open file [ %s ].\n", filename);
		exit(-1);
	}

	fread(header_buf, sizeof(unsigned char), HEADERSIZE, fp);

	if (strncmp((char*)header_buf, "BM", 2)) {
		fprintf(stderr, "Error : [ %s ] is not bitmap file.\n", filename);
		exit(-1);
	}
	memcpy(width, header_buf + 18, sizeof(int));
	memcpy(height, header_buf + 22, sizeof(int));
	memcpy(&color, header_buf + 28, sizeof(unsigned int));

	if (color != 24) {
		fprintf(stderr, "Error : [ %s ] is not 24bit color image.\n", filename);
		exit(-1);
	}

	if (*height < 0) {
		*height *= -1;
		real_width = *width * 3 + *width % 4;
		tmp = new unsigned char[real_width];
		pixel = new unsigned char[3 * (*width) * (*height)];

		for (i = 0; i < *height; i++) {
			fread(tmp, 1, real_width, fp);
			for (j = 0; j < *width; j++) {
				pixel[3 * ((*height - i - 1) * (*width) + j)] = tmp[3 * j + 2];
				pixel[3 * ((*height - i - 1) * (*width) + j) + 1] = tmp[3 * j + 1];
				pixel[3 * ((*height - i - 1) * (*width) + j) + 2] = tmp[3 * j];
			}
		}
	}
	else {
		real_width = *width * 3 + *width % 4;
		tmp = new unsigned char[real_width];
		pixel = new unsigned char[3 * (*width) * (*height)];

		for (i = 0; i < *height; i++) {
			fread(tmp, 1, real_width, fp);
			for (j = 0; j < *width; j++) {
				pixel[3 * (i * (*width) + j)] = tmp[3 * j + 2];
				pixel[3 * (i * (*width) + j) + 1] = tmp[3 * j + 1];
				pixel[3 * (i * (*width) + j) + 2] = tmp[3 * j];
			}
		}
	}
	delete[] tmp;
	return pixel;
}

GLuint loadTexture(const char* filename)
{
	GLuint texID;
	GLubyte* texture_image;
	int texture_width, texture_height;
	texture_image = loadBMPFile(filename, &texture_width, &texture_height);
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_image);
	delete[] texture_image;
	return texID;
}

void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	setCamera();
	setLight();

	displayWalls(10.f);
	drawTable();

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	
	for (int i = 0; i < numSpheres; i++) {
		glPushMatrix();
		if (i == selectedBall) {
			glTranslatef(px[i], py[i], 0.f);
			displaySphere(radius[i], 0.5f, 0.7f, 0.f, 0.3f, 0.3f, 0.3f, 30.f);
		}
		else {
			glTranslatef(px[i], py[i], 0.f);
			displaySphere(radius[i], 0.f, 0.3f, 1.f, 0.3f, 0.3f, 0.3f, 30.f);
		}
		glPopMatrix();
	}

	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);

	printScore();

	glutSwapBuffers();
}

void update(void)
{
	bool allStopped = false;
	if (!isDragging) {
		bool allStopped = true;

		for (int i = 0; i < numSpheres; i++) {
			if (fabs(vx[i]) > 0.01f || fabs(vy[i]) > 0.01f) {
				allStopped = false;
			}
		}
		if (allStopped) {
			if (!isDragging) {
				allStopped = false; 
			}
		}

		for (int i = 0; i < numSpheres; i++) {
			px[i] += vx[i] * dt;
			py[i] += vy[i] * dt;
			vx[i] /= 1.01;
			vy[i] /= 1.01;
		}

		for (int i = 0; i < numSpheres; i++) {
			int j = (i + 1) % numSpheres;
			if (collisionDetection(i, j)) {
				calculateCollideVelocity(i, j);
				score += 100;
			}
		}

		for (int i = 0; i < numSpheres; i++) {
			if (collisionDetection_Wall_x(i, 10.f)) {
				calculateCollideVelocity_Wall_x(i, 10.f);
			}
		}
		for (int i = 0; i < numSpheres; i++) {
			if (collisionDetection_Wall_y(i, 10.f)) {
				calculateCollideVelocity_Wall_y(i, 10.f);
			}
		}

		for (int i = 0; i < numSpheres; i++) {
			if (abs(px[i]) > 4 && abs(py[i]) > 4 && abs(px[i])* abs(py[i]) < 100) {
				vx[i] = 0;
				vy[i] = 0;
				px[i] = (i+1) * 1000;
				py[i] = (i+1) * 1000;
				score = score + 1000;
			}
		}
	}

	for (int i = 0; i < numSpheres; i++) {
		if (abs(vx[i]) < 0.1 && abs(vy[i] < 0.1) ){
			vx[i] = 0;
			vy[i] = 0;
		}
	}

	glutPostRedisplay();
}

bool collisionDetection(const int id0, const int id1)
{
	float d;
	d = (px[id0] - px[id1]) * (px[id0] - px[id1]) + (py[id0] - py[id1]) * (py[id0] - py[id1]);
	if (d <= (radius[id0] + radius[id1]) * (radius[id0] + radius[id1])) {
		return true;
	}
	else {
		return false;
	}
}

bool collisionDetection_Wall_x(const int id, float length)
{
	float dx, dy;
	length = length / 2.1f;
	dx = abs(length) - abs(px[id]);
	dy = abs(length) - abs(py[id]);
	if (dx <= radius[id]) {
		return true;
	}
	else {
		return false;
	}
}

bool collisionDetection_Wall_y(const int id, float length)
{
	float dx, dy;
	length = length / 2.1f;
	dx = abs(length) - abs(px[id]);
	dy = abs(length) - abs(py[id]);
	if (dy <= radius[id]) {
		return true;
	}
	else {
		return false;
	}
}


void calculateCollideVelocity(const int id0, const int id1)
{
	float nx, ny, l, vn[2], vnn[2];
	float vtx[2], vty[2];

	nx = px[id1] - px[id0];
	ny = py[id1] - py[id0];
	l = 1.f / (float)sqrt(nx * nx + ny * ny);
	nx *= l;
	ny *= l;

	vn[0] = vx[id0] * nx + vy[id0] * ny;
	vn[1] = vx[id1] * nx + vy[id1] * ny;

	vtx[0] = vx[id0] - vn[0] * nx;
	vty[0] = vy[id0] - vn[0] * ny;

	vtx[1] = vx[id1] - vn[1] * nx;
	vty[1] = vy[id1] - vn[1] * ny;

	vnn[0] = 1.f / (mass[id0] + mass[id1]) * ((mass[id0] - ecoef * mass[id1]) * vn[0] + mass[id1] * (1.f + ecoef) * vn[1]);
	vnn[1] = 1.f / (mass[id0] + mass[id1]) * (mass[id0] * (1.f + ecoef) * vn[0] + (mass[id1] - mass[id0] * ecoef) * vn[1]);

	vx[id0] = vnn[0] * nx + vtx[0];
	vy[id0] = vnn[0] * ny + vty[0];

	vx[id1] = vnn[1] * nx + vtx[1];
	vy[id1] = vnn[1] * ny + vty[1];
}

void calculateCollideVelocity_Wall_x(const int id, float length)
{
	vx[id] *= -ecoef_wall;
}

void calculateCollideVelocity_Wall_y(const int id, float length)
{
	vy[id] *= -ecoef_wall;
}

void keyboardFunction(unsigned char key, int x, int y)
{
	switch (key) {
	case '0':
		selectedBall = 0;
		break;
	case '1':
		selectedBall = 1;
		break;
	case '2':
		selectedBall = 2;
		break;
	case '3':
		selectedBall = 3;
		break;
	case 'q':
		exit(0);
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("ビリヤード");
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	initSphere();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboardFunction);
	glutMouseFunc(mouseFunction);
	glutMotionFunc(motionFunction);
	glutIdleFunc(update);
	glutMainLoop();

	return 0;

}
