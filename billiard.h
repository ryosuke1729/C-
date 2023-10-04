#include <iostream>
#include <math.h>
#include <GL/glut.h>
#include <random>

const int WINDOW_WIDTH = 512;
const int WINDOW_HEIGHT = 512;
int cnt = 0;

//球の速度
float vx[3] ;
float vy[3] ;
float vx_wall;
float vy_wall;

//球の中心の座標
float px[3];
float py[3];

//float radius[3];
//float mass[3];
float radius[3] ;
float mass[3];
float ecoef;
float ecoef_wall;
int score = 0;

const float dt = 0.01f;

int numSpheres = 3;

bool isDragging = false;
int selectedBall = -1;
float initialMouseX, initialMouseY;

std::random_device rnd;

void setCamera(void);

void display(void);

void displayAxis(float length);

void displaySphere(float radius, float dr, float dg, float db, float sr, float sg, float sb, float shininess);

void keyboardFunction(unsigned char key, int x, int y);

void update(void);

bool collisionDetection(const int id0, const int id1);

bool collisionDetection_Wall_x(const int id, float length);

bool collisionDetection_Wall_y(const int id, float length);

void calculateCollideVelocity(const int id0, const int id1);

void calculateCollideVelocity_Wall_x(const int id, float length);

void calculateCollideVelocity_Wall_y(const int id, float length);

void stopSphere(int i);


void motionFunction(int x, int y);
void mouseFunction(int button, int state, int x, int y);
void drawTable(void);
void printScore(void);
void printCharacter(const char* buffer, const int width, const int height);

unsigned char* loadBMPFile(const char* filename, int* width, int* height);
GLuint loadTexture(const char* filename);
GLint textureID;
