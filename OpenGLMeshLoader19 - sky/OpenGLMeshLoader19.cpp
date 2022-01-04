#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <string>  
#include <glut.h>
#include <windows.h>
#include <mmsystem.h>
#include <tchar.h>

GLuint denim1;
GLuint denim2;
GLuint snowball;
GLuint goomba;
GLuint flag;
GLuint pole;

int timer = 100;
bool youWin = false;

int lives = 3;
int score = 0;
bool heartTaken = false;
bool gameOver = false;
bool start = false;
double stepR=0;
double stepG=0;
double stepB=0;

// Model Variables
Model_3DS model_snowman;
Model_3DS model_tree;
Model_3DS model_heart;
Model_3DS model_coin;

// Textures
GLTexture tex_ground;
GLTexture tex_ground2;
using namespace std;


#define GLUT_KEY_ESCAPE 27
#define DEG2RAD(a) (a * 0.0174532925)

class Vector3f {
public:
	float x, y, z;

	Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3f operator+(Vector3f& v) {
		return Vector3f(x + v.x, y + v.y, z + v.z);
	}

	Vector3f operator-(Vector3f& v) {
		return Vector3f(x - v.x, y - v.y, z - v.z);
	}

	Vector3f operator*(float n) {
		return Vector3f(x * n, y * n, z * n);
	}

	Vector3f operator/(float n) {
		return Vector3f(x / n, y / n, z / n);
	}

	Vector3f unit() {
		return *this / sqrt(x * x + y * y + z * z);
	}

	Vector3f cross(Vector3f v) {
		return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
};

class Camera {
public:
	Vector3f eye, center, up;

	Camera(float eyeX = 1.0f, float eyeY = 1.0f, float eyeZ = 1.0f, float centerX = 0.0f, float centerY = 0.0f, float centerZ = 0.0f, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f) {
		eye = Vector3f(eyeX, eyeY, eyeZ);
		center = Vector3f(centerX, centerY, centerZ);
		up = Vector3f(upX, upY, upZ);
	}

	void moveX(float d) {
		Vector3f right = up.cross(center - eye).unit();
		eye = eye + right * d;
		center = center + right * d;
	}

	void moveY(float d) {
		eye = eye + up.unit() * d;
		center = center + up.unit() * d;
	}

	void moveZ(float d) {
		Vector3f view = (center - eye).unit();
		eye = eye + view * d;
		center = center + view * d;
	}

	void rotateX(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
		up = view.cross(right);
		center = eye + view;
	}

	void rotateY(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
		right = view.cross(up);
		center = eye + view;
	}

	void look() {
		gluLookAt(
			eye.x, eye.y, eye.z,
			center.x, center.y, center.z,
			up.x, up.y, up.z
		);
	}
};

void mouse(int button, int state, int mousex, int mousey) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		start = true;
	}
}
//void starttext() {
//	if (!start) {
//
//	}
//}

void drawScoreAndLives() {
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D); //added this
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glColor3d(1, 1, 1);
	glLoadIdentity();
	gluOrtho2D(0, 800, 0, 600);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glRasterPos2i(470, 550);
	string livestxt = "Lives: ";
	string livesStr = to_string(lives);
	string scoretxt = "   Score: ";
	string scoreStr = to_string(score);
	string timelefttxt = "   Time Left: ";
	string timeleft = to_string(timer);
	void* font = GLUT_BITMAP_9_BY_15;
	for (string::iterator i = livestxt.begin(); i != livestxt.end(); ++i)
	{
		char c = *i;
		glColor3d(0.0, 0.0, 0.0);
		glutBitmapCharacter(font, c);
	}
	for (string::iterator i = livesStr.begin(); i != livesStr.end(); ++i)
	{
		char c = *i;
		glColor3d(0.0, 0.0, 0.0);
		glutBitmapCharacter(font, c);
	}
	for (string::iterator i = scoretxt.begin(); i != scoretxt.end(); ++i)
	{
		char c = *i;
		glColor3d(0.0, 0.0, 0.0);
		glutBitmapCharacter(font, c);
	}
	for (string::iterator i = scoreStr.begin(); i != scoreStr.end(); ++i)
	{
		char c = *i;
		glColor3d(0.0, 0.0, 0.0);
		glutBitmapCharacter(font, c);
	}
	for (string::iterator i = timelefttxt.begin(); i != timelefttxt.end(); ++i)
	{
		char c = *i;
		glColor3d(0.0, 0.0, 0.0);
		glutBitmapCharacter(font, c);
	}
	for (string::iterator i = timeleft.begin(); i != timeleft.end(); ++i)
	{
		char c = *i;
		glColor3d(0.0, 0.0, 0.0);
		glutBitmapCharacter(font, c);
	}
	glMatrixMode(GL_PROJECTION); //swapped this with...
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW); //...this
	glPopMatrix();
	//added this
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}

void drawWinLose() {
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D); //added this
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glColor3d(1, 1, 1);
	glLoadIdentity();
	gluOrtho2D(0, 800, 0, 600);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glRasterPos2i(350, 300);
	string livestxt = "You Lose";
	if (youWin) {
		livestxt = "You Win";
	}
	
	void* font = GLUT_BITMAP_9_BY_15;
	for (string::iterator i = livestxt.begin(); i != livestxt.end(); ++i)
	{
		char c = *i;
		glColor3d(0.0, 0.0, 0.0);
		glutBitmapCharacter(font, c);
	}
	
	glMatrixMode(GL_PROJECTION); //swapped this with...
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW); //...this
	glPopMatrix();
	//added this
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}

void drawStart() {
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D); //added this
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glColor3d(1, 1, 1);
	glLoadIdentity();
	gluOrtho2D(0, 800, 0, 600);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glRasterPos2i(250, 300);
	string livestxt = "Click the left mouse button to start";
	

	void* font = GLUT_BITMAP_9_BY_15;
	for (string::iterator i = livestxt.begin(); i != livestxt.end(); ++i)
	{
		char c = *i;
		glColor3d(0.0, 0.0, 0.0);
		glutBitmapCharacter(font, c);
	}

	glMatrixMode(GL_PROJECTION); //swapped this with...
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW); //...this
	glPopMatrix();
	//added this
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}

void LoadAssets()
{
	// Loading Model files
	model_snowman.Load("Models/snowman/Snowman N291214.3DS");
	model_tree.Load("Models/tree/Tree1.3ds");
	model_heart.Load("Models/heart/Love.3DS");
	//model_coin.Load("Models/coin/Coin2.3ds");
	model_coin.Load("Models/coin1/coin.3ds");

	// Loading texture files
	tex_ground2.Load("Textures/snow.bmp");
	tex_ground.Load("Textures/ground.bmp");
	loadBMP(&denim1, "Textures/denim1.bmp", true);
	loadBMP(&denim2, "Textures/denim.bmp", true);
	loadBMP(&snowball, "Textures/snow.bmp", true);
	loadBMP(&goomba, "Textures/goomba.bmp", true);
	loadBMP(&flag, "Textures/flag.bmp", true);
	loadBMP(&pole, "Textures/pole.bmp", true);

}

int level = 1;
bool levelCompleted = false;
Camera camera;

void RenderGround()
{
	if (level == 1) {
		//glDisable(GL_LIGHTING);	// Disable lighting 
		glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit
		glEnable(GL_TEXTURE_2D);	// Enable 2D texturing
		glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);	// Bind the ground texture

		glPushMatrix();
		glTranslated(0.5, 0.025, 0.5);
		glScaled(0.1, 1, 0.1);
		glBegin(GL_QUADS);
		glNormal3f(0, 1, 0);	// Set quad normal direction.
		glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
		glVertex3f(-20, 0, -20);
		glTexCoord2f(5, 0);
		glVertex3f(20, 0, -20);
		glTexCoord2f(5, 5);
		glVertex3f(20, 0, 20);
		glTexCoord2f(0, 5);
		glVertex3f(-20, 0, 20);
		glEnd();
		glPopMatrix();
	}
	else {
		glDisable(GL_LIGHTING);	// Disable lighting 
		glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit
		glEnable(GL_TEXTURE_2D);	// Enable 2D texturing
		glBindTexture(GL_TEXTURE_2D, tex_ground2.texture[0]);	// Bind the ground texture

		glPushMatrix();
		glTranslated(0.5, 0.025, 0.5);
		glScaled(0.1, 0.1, 0.1);
		glBegin(GL_QUADS);
		glNormal3f(0, 1, 0);	// Set quad normal direction.
		glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
		glVertex3f(-20, 0, -20);
		glTexCoord2f(5, 0);
		glVertex3f(20, 0, -20);
		glTexCoord2f(5, 5);
		glVertex3f(20, 0, 20);
		glTexCoord2f(0, 5);
		glVertex3f(-20, 0, 20);
		glEnd();
		glPopMatrix();
		glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	}
	


	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}
void drawGround(double thickness) {
	
	glPushMatrix();

	if (level == 1)
		glColor3d(0, 1, 0);
	else
		glColor3d(1, 1, 1);
	glTranslated(0.5, 0.5 * thickness, 0.5);
	glScaled(4.0, thickness, 4.0);
	glutSolidCube(1);
	glPopMatrix();
}

double sphereRad = 20;
float lightPosY = 5.0f;
float gb = 0.5;
void setupLights() {
	GLfloat ambient[] = { 0.7f, 0.7f, 0.7, 1.0f };
	GLfloat diffuse[] = { 0.6f, 0.6f, 0.6, 1.0f };
	GLfloat specular[] = { 1.0f, 1.0f, 1.0, 1.0f }; //hena
	GLfloat shininess[] = { 255 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
	//glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	GLfloat lightIntensitymain[] = { 0.7f, 1.0f, 0.7f, 1.0f };
	GLfloat lightIntensity[] = { 0.2f, 0.5f, 0.2f, 1.0f };
	GLfloat lightPosition[] = { -2.0f, lightPosY, -1.0f, 0.0f };
	GLfloat lightPosition2[] = { 0.0f, -10.0f, 0.0f, 0.0f };
	GLfloat lightPosition3[] = { 0.0f, 10.0f, 0.0f, 0.0f };
	GLfloat lightPosition4[] = { 0.0f, 0.0f, 10.0f, 0.0f };
	GLfloat lightPosition5[] = { 10.0f, 0.0f, 0.0f, 0.0f };

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensitymain);

	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightIntensity);

	glLightfv(GL_LIGHT2, GL_POSITION, lightPosition3);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, lightIntensity);

	glLightfv(GL_LIGHT3, GL_POSITION, lightPosition4);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, lightIntensity);

	glLightfv(GL_LIGHT4, GL_POSITION, lightPosition5);
	glLightfv(GL_LIGHT4, GL_DIFFUSE, lightIntensity);
}

void setupCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 640 / 480, 0.001, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.look();
}


void rgb(double r, double g, double b) {
	r = (double)r / 255;
	g = (double)g / 255;
	b = (double)b / 255;
	glColor3d(r, g, b);
}

void drawHeart(double x, double y, double z) {
	glPushMatrix();

	glTranslated(x, y, z);
	glScaled(0.002, 0.002, 0.002);
	model_heart.Draw();
	glPopMatrix();
}

void drawTree(double x, double y, double z) {
	glPushMatrix();

	glTranslated( x,  y,  z);
	glScaled(0.2, 0.2, 0.2);
	model_tree.Draw();
	glPopMatrix();
}

void drawCoin(double x, double y, double z) {
	glPushMatrix();
	rgb(244, 244, 0);
	glColor3d(1, 1, 0);
	glTranslated(x, y, z);
	glRotated(-90, 1, 0, 0);
	glScaled(0.05, 0.05, 0.05);
	model_coin.Draw();
	glPopMatrix();
}

void drawNose() {
	glPushMatrix();
	rgb(254, 193, 121);
	glTranslated(0, 0.2, 0.05);
	GLUquadric* quad;
	quad = gluNewQuadric();
	gluSphere(quad, 0.017, 32, 32);
	glPopMatrix();
}

void drawHead() {
	glPushMatrix();
	glTranslated(0, 0.1, 0);
	glPushMatrix();
	rgb(227, 172, 108);
	glTranslated(0, 0.2, 0);
	GLUquadric* quad;
	quad = gluNewQuadric();
	gluSphere(quad, 0.05, 32, 32);
	glPopMatrix();

	drawNose();

	glPopMatrix();
}




void drawTorso() {
	glPushMatrix();
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glBindTexture(GL_TEXTURE_2D, denim1);
	rgb(0, 0, 255);
	glScaled(0.4, 0.7, 0.3);
	glTranslated(0, 0.27, 0);
	glutSolidCube(0.2);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glPopMatrix();
}

void changeColor(double r1, double g1, double b1, double r2, double g2, double b2) {
	stepR = (r1 - r2) / 100;
	stepG = (g1 - g2) / 100;
	stepB = (b1 - b2) / 100;
		
	
}

bool leftLegForward = false;
bool leftLegBackward = false;

bool rightLegForward = false;
bool rightLegBackward = false;

void drawRightLeg() {
	glPushMatrix();
	if (rightLegForward) {
		glTranslated(0, 0.01, 0.07);
		glRotated(-30, 1, 0, 0);
	}
	if (rightLegBackward) {
		glTranslated(0, 0.01, -0.07);
		glRotated(30, 1, 0, 0);
	}

	//leg
	glPushMatrix();
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glBindTexture(GL_TEXTURE_2D, denim2);
	rgb(0, 0, 152);
	glScaled(0.15, 0.7, 0.3);
	glTranslated(-0.15, 0.1, 0);
	glutSolidCube(0.2);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glPopMatrix();

	//shoe
	glPushMatrix();
	rgb(160, 39, 19);
	glScaled(0.16, 0.2, 0.35);
	glTranslated(-0.14, 0.1, 0);
	glutSolidCube(0.2);
	glPopMatrix();

	glPopMatrix();

}


void drawLeftLeg() {
	glPushMatrix();
	if (leftLegForward) {
		glTranslated(0, 0.01, 0.07);
		glRotated(-30, 1, 0, 0);
	}
	if (leftLegBackward) {
		glTranslated(0, 0.01, -0.07);
		glRotated(30, 1, 0, 0);
	}


	glPushMatrix();
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glBindTexture(GL_TEXTURE_2D, denim2);
	rgb(0, 0, 152);
	glScaled(0.15, 0.7, 0.3);
	glTranslated(0.15, 0.1, 0);
	glutSolidCube(0.2);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);

	glPopMatrix();

	glPushMatrix();
	rgb(160, 39, 19);
	glScaled(0.16, 0.2, 0.35);
	glTranslated(0.14, 0.1, 0);
	glutSolidCube(0.2);
	glPopMatrix();

	glPopMatrix();
}

int armAngle = 0;
void drawLeftArm() {
	glPushMatrix();

	if (armAngle == 180) {
		glTranslated(0, 0.47, 0);
		glRotated(armAngle, 1, 0, 0);
	}


	glTranslated(0.11, -0.01, 0);
	glRotated(30, 0, 0, 1);


	glPushMatrix();
	rgb(255, 0, 0);
	glScaled(0.05, 0.4, 0.07);
	glTranslated(1.2, 0.4, 0);
	glutSolidCube(0.5);
	glPopMatrix();

	glPushMatrix();
	rgb(255, 255, 255);
	glTranslated(0.06, 0.07, 0);
	GLUquadric* quad;
	quad = gluNewQuadric();
	gluSphere(quad, 0.03, 32, 32);
	glPopMatrix();

	glPopMatrix();
}

void drawRightArm() {
	glPushMatrix();

	glTranslated(-0.21, 0.058, 0);
	glRotated(-30, 0, 0, 1);

	glPushMatrix();
	rgb(255, 0, 0);
	glScaled(0.05, 0.4, 0.07);
	glTranslated(1.2, 0.4, 0);
	glutSolidCube(0.5);
	glPopMatrix();

	glPushMatrix();
	rgb(255, 255, 255);
	glTranslated(0.06, 0.07, 0);
	GLUquadric* quad;
	quad = gluNewQuadric();
	gluSphere(quad, 0.03, 32, 32);
	glPopMatrix();

	glPopMatrix();
}

void drawHat() {
	glPushMatrix();

	glPushMatrix();

	glTranslated(0, 0.33, 0);
	glRotated(-100, 1, 0, 0);

	glPushMatrix();
	rgb(255, 0, 0);
	GLUquadric* quad;
	quad = gluNewQuadric();
	gluCylinder(quad, 0.04, 0.05, 0.05, 32, 32);
	glPopMatrix();

	glPushMatrix();
	rgb(255, 0, 0);
	GLUquadric* quad2;
	quad2 = gluNewQuadric();
	glTranslated(0, 0, 0.051);
	gluDisk(quad2, 0, 0.05, 32, 32);
	glPopMatrix();

	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 0.29, 0.065);
	glRotated(-100, 1, 0, 0);
	rgb(255, 0, 0);
	GLUquadric* quad3;
	quad3 = gluNewQuadric();
	glTranslated(0, 0, 0.051);
	gluDisk(quad3, 0, 0.045, 32, 32);
	glPopMatrix();

	glPopMatrix();
}

double marioX = 0;
double marioY = 0.021;
double marioZ = 0;
int marioAngle = 0;

void drawMario() {
	glPushMatrix();
	glTranslated(marioX, marioY, marioZ);
	glRotated(marioAngle, 0, 1, 0);
	drawHead();
	
	drawRightLeg();
	drawLeftLeg();
	drawLeftArm();
	drawRightArm();
	drawHat();
	drawTorso();
	glPopMatrix();
}


void drawGoomba(double x, double y, double z, int angle) {
	glPushMatrix();
	glTranslated(x, y, z);
	glRotated(angle, 0, 1, 0);
	glRotated(-90, 1, 0, 0);


	//headMain
	rgb(164, 74, 1);
	glPushMatrix();
	GLUquadric* quad;
	quad = gluNewQuadric();
	glBindTexture(GL_TEXTURE_2D, goomba);
	gluQuadricTexture(quad, true);
	gluQuadricNormals(quad, GL_SMOOTH);
	gluCylinder(quad, 0.1, 0.05, 0.1, 32, 32);
	gluDeleteQuadric(quad);

	glPopMatrix();

	//headTop
	glPushMatrix();
	glTranslated(0, 0, 0.085);
	GLUquadric* quad2;
	quad2 = gluNewQuadric();
	glBindTexture(GL_TEXTURE_2D, goomba);
	gluQuadricTexture(quad2, true);
	gluQuadricNormals(quad2, GL_SMOOTH);
	gluSphere(quad2, 0.051, 32, 32);
	gluDeleteQuadric(quad2);
	glPopMatrix();

	//body
	glPushMatrix();
	rgb(255, 255, 0);
	glTranslated(0, 0, -0.01);
	glScaled(1, 1, 2);
	GLUquadric* quad3;
	quad3 = gluNewQuadric();
	gluSphere(quad3, 0.05, 32, 32);
	glPopMatrix();

	//leg1
	glPushMatrix();
	rgb(0, 0, 0);
	glTranslated(0.04, 0, -0.1);
	glScaled(0.5, 1, 0.5);
	GLUquadric* quad4;
	quad4 = gluNewQuadric();
	gluSphere(quad4, 0.05, 32, 32);
	glPopMatrix();

	//leg2
	glPushMatrix();
	rgb(0, 0, 0);
	glTranslated(-0.04, 0, -0.1);
	glScaled(0.5, 1, 0.5);
	GLUquadric* quad5;
	quad5 = gluNewQuadric();
	gluSphere(quad5, 0.05, 32, 32);
	glPopMatrix();


	glPopMatrix();
}

void platform(double x, double y, double z, double l, double h, double w) {
	glPushMatrix();
	glTranslated(x, y + (h / 2), z);

	glPushMatrix();

	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glBindTexture(GL_TEXTURE_2D, goomba);
	rgb(242, 111, 62);
	glScaled(l, h, w);

	glutSolidCube(1);

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glPopMatrix();

	glPushMatrix();
	if (level == 1) {
		rgb(0, 233, 0);
	}
	else {
		rgb(244, 244, 244);
	}

	glTranslated(0, h / 2, 0);
	glScaled(l, 0.001, w);
	glutSolidCube(1);
	//from
	glTranslated(-0.125, 0.65, -0.125);// x,y helw
	glScaled(l/2.4, 1, w/3.2);//x helw
	RenderGround();
	// to
	glPopMatrix();

	glPopMatrix();
}

void drawFlag(double x, double y, double z) {
	glPushMatrix();

	rgb(255, 0, 0);
	glTranslated(x, y, z);
	glRotated(90, 0, 1, 0);
	GLUquadric* quad;
	quad = gluNewQuadric();
	glBindTexture(GL_TEXTURE_2D, flag);
	gluQuadricTexture(quad, true);
	gluQuadricNormals(quad, GL_SMOOTH);
	gluCylinder(quad, 0.1,0,0.3,32,32);
	//glutSolidCone(0.1, 0.3, 32, 32);
	gluDeleteQuadric(quad);

	glPopMatrix();
}
double flagY = 1.9;
void drawPole(double x, double y, double z) {
	glPushMatrix();

	glPushMatrix();
	rgb(244, 244, 244);
	glRotated(-90, 1, 0, 0);
	glTranslated(x, y, z);
	GLUquadric* quad;
	quad = gluNewQuadric();
	glBindTexture(GL_TEXTURE_2D, pole);
	gluQuadricTexture(quad, true);
	gluQuadricNormals(quad, GL_SMOOTH);
	gluCylinder(quad, 0.03, 0.03, 2, 32, 32);
	gluDeleteQuadric(quad);
	glPopMatrix();

	drawFlag(x, flagY, -y);

	glPopMatrix();
}

void drawHat2() {
	glPushMatrix();

	glPushMatrix();

	glTranslated(0, 0.33, 0);
	glRotated(-100, 1, 0, 0);

	glPushMatrix();
	rgb(0, 0, 0);
	GLUquadric* quad;
	quad = gluNewQuadric();
	gluCylinder(quad, 0.04, 0.05, 0.05, 32, 32);
	glPopMatrix();

	glPushMatrix();
	rgb(0, 0, 0);
	GLUquadric* quad2;
	quad2 = gluNewQuadric();
	glTranslated(0, 0, 0.051);
	gluDisk(quad2, 0, 0.05, 32, 32);
	glPopMatrix();

	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 0.29, 0.065);
	glRotated(-100, 1, 0, 0);
	rgb(0, 0, 0);
	GLUquadric* quad3;
	quad3 = gluNewQuadric();
	glTranslated(0, 0, 0.051);
	gluDisk(quad3, 0, 0.045, 32, 32);
	glPopMatrix();

	glPopMatrix();
}


double snowman_scale = 3;

void drawSnowMan(double x, double y, double z, int angle) {
	glPushMatrix();
	glTranslated(x, y, z);
	glScaled(snowman_scale, snowman_scale, snowman_scale);
	glRotated(angle, 0, 1, 0);

	glPushMatrix();
	glTranslated(0, -0.05, 0);
	drawHat2();

	glPopMatrix();

	glPushMatrix();
	glColor3d(1, 1, 1);

	GLUquadricObj* qobj;
	qobj = gluNewQuadric();
	glBindTexture(GL_TEXTURE_2D, snowball);
	gluQuadricTexture(qobj, true);
	gluQuadricNormals(qobj, GL_SMOOTH);
	gluSphere(qobj, 0.1, 25, 25);
	gluDeleteQuadric(qobj);

	glTranslated(0, 0.15, 0);
	GLUquadricObj* qobj1;
	qobj1 = gluNewQuadric();
	glBindTexture(GL_TEXTURE_2D, snowball);
	gluQuadricTexture(qobj1, true);
	gluQuadricNormals(qobj1, GL_SMOOTH);
	gluSphere(qobj1, 0.075, 25, 25);
	gluDeleteQuadric(qobj1);

	glTranslated(0, 0.1, 0);
	GLUquadricObj* qobj2;
	qobj2 = gluNewQuadric();
	glBindTexture(GL_TEXTURE_2D, snowball);
	gluQuadricTexture(qobj2, true);
	gluQuadricNormals(qobj2, GL_SMOOTH);
	gluSphere(qobj2, 0.05, 25, 25);
	gluDeleteQuadric(qobj2);
	glPopMatrix();

	glPushMatrix();
	rgb(237, 145, 33);
	glTranslated(0, 0.25, 0.05);
	glutSolidCone(0.01, 0.05, 32, 32);
	glPopMatrix();

	glPopMatrix();


}

//void drawSnowMan(double x, double y, double z) {
void drawSnowman(double x, double y, double z, int angle) {
	glPushMatrix();

	glTranslated(x, y-0.05, z);
	glRotated(angle, 0, 1, 0);
	glScaled(8, 8, 8);
	//body
	glPushMatrix();
	glColor3f(1, 1, 1);
	glTranslated(0, 0.046, 0);
	glScaled(0.3, 0.4, 0.3);
	GLUquadricObj* qobj;
	qobj = gluNewQuadric();
	glBindTexture(GL_TEXTURE_2D, snowball);
	gluQuadricTexture(qobj, true);
	gluQuadricNormals(qobj, GL_SMOOTH);
	gluSphere(qobj,0.1, 25, 25);
	gluDeleteQuadric(qobj);
	glPopMatrix();

	//head
	glPushMatrix();
	glColor3f(1, 1, 1);
	glTranslated(0, 0.1, 0);
	glScaled(0.3, 0.4, 0.3);
	GLUquadricObj* qobj1;
	qobj1 = gluNewQuadric();
	glBindTexture(GL_TEXTURE_2D, snowball);
	gluQuadricTexture(qobj1, true);
	gluQuadricNormals(qobj1, GL_SMOOTH);
	gluSphere(qobj1 ,0.06, 25, 25);
	gluDeleteQuadric(qobj1);
	
	glPopMatrix();

	//buttons
	glPushMatrix();
	glColor3f(0, 0, 0);
	glTranslated(0, 0.066, 0.025);
	glutSolidSphere(0.005, 25, 25);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0, 0, 0);
	glTranslated(0, 0.046, 0.03);
	glutSolidSphere(0.005, 25, 25);
	glPopMatrix();

	//eyes
	glPushMatrix();
	glColor3f(0, 0, 0);
	glTranslated(-0.006, 0.11, 0.016);
	glutSolidSphere(0.003, 25, 25);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0, 0, 0);
	glTranslated(0.006, 0.11, 0.016);
	glutSolidSphere(0.003, 25, 25);
	glPopMatrix();

	//nose
	glPushMatrix();
	glColor3f(0.8, 0.2, 0);
	glTranslated(0, 0.1, 0.01);
	glutSolidCone(0.004, 0.02, 25, 25);
	glPopMatrix();

	//arms
	glPushMatrix();
	glColor3f(0.4, 0.1, 0);
	glTranslated(0.026, 0.06, 0);
	glRotated(90, 0, 1, 0);
	glRotated(45, 1, 0, 0);
	glScaled(0.005, 0.05, 0.005);
	glutSolidCube(1);
	glPopMatrix();
	glPushMatrix();
	glColor3f(0.4, 0.1, 0);
	glTranslated(0.036, 0.076, 0);
	glRotated(45, 0, 0, 1);
	glRotated(90, 0, 1, 0);
	glRotated(45, 1, 0, 0);
	glScaled(0.005, 0.01, 0.005);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.4, 0.1, 0);
	glTranslated(-0.026, 0.06, 0);
	glRotated(-90, 0, 1, 0);
	glRotated(45, 1, 0, 0);
	glScaled(0.005, 0.05, 0.005);
	glutSolidCube(1);
	glPopMatrix();
	glPushMatrix();
	glColor3f(0.4, 0.1, 0);
	glTranslated(-0.036, 0.076, 0);
	glRotated(45, 0, 0, 1);
	glRotated(90, 0, 1, 0);
	glRotated(45, 1, 0, 0);
	glScaled(0.005, 0.01, 0.005);
	glutSolidCube(1);
	glPopMatrix();

	glPopMatrix();

}
//}
double backR = 255;
double backG = 200;
double backB = 124;

void drawBackground() {
	backR = backR - stepR;
	backG = backG - stepG;
	backB = backB - stepB;
	glPushMatrix();
	GLUquadric* quad3;
	quad3 = gluNewQuadric();
	rgb(backR + stepR, backG + stepG, backB + stepB);
	glutSolidSphere( sphereRad, 32, 32);
	glPopMatrix();
}


double goomba1x = 1;
double goomba1y = 0.145;
double goomba1z = 0.1;
bool goomba1MovesRight = true;
bool goomba1Alive = true;

double goomba2x = -0.5;
double goomba2y = 0.145;
double goomba2z = -1;
bool goomba2MovesRight = false;
bool goomba2Alive = true;


double platform1x = 1;
double platform1y = 0.021;
double platform1z = 1;
double platform1l = 0.6;
double platform1w = 0.8;
double platform1h = 0.15;

double platform2x = 1.5;
double platform2y = 0.3;
double platform2z = 1.5;
double platform2l = 0.6;
double platform2w = 0.8;
double platform2h = 0.15;

double platform3x = -1;
double platform3y = 0.021;
double platform3z = 1.7;
double platform3l = 0.3;
double platform3w = 0.3;
double platform3h = 0.15;

double heartx = 1.5;
double hearty = 0.6;
double heartz = 1.5;

double coin1x = 1;
double coin1y = 0.4;
double coin1z = 1;
bool coin1Taken = false;

double coin2x = -1;
double coin2y = 0.1;
double coin2z = -1;
bool coin2Taken = false;

double coin3x = 1.5;
double coin3y = 0.1;
double coin3z = -1.2;
bool coin3Taken = false;

double coin4x = 1.5;
double coin4y = 0.3;
double coin4z = -1.2;
bool coin4Taken = false;

double coin5x = 1.5;
double coin5y = 0.3;
double coin5z = -1.2;
bool coin5Taken = false;

double coin6x = 1.5;
double coin6y = 0.3; //zawedt y 4,5,6 from 0.1... dol malhomsh lazma ta2rebn
double coin6z = -1.2;
bool coin6Taken = false;




void Display() {
	setupCamera();
	setupLights();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawBackground();
	drawGround(0.02);
	RenderGround();
	drawMario();
	platform(platform1x, platform1y, platform1z, platform1l, platform1h, platform1w);
	platform(platform2x, platform2y, platform2z, platform2l, platform2h, platform2w);
	platform(platform3x, platform3y, platform3z, platform3l, platform3h, platform3w);
	drawPole(-1, -1.7, 0);
	if(!heartTaken) drawHeart(heartx, hearty, heartz);
	drawScoreAndLives();
	if (level == 1) {
		if (!coin1Taken) drawCoin(coin1x, coin1y, coin1z);
		if (!coin2Taken) drawCoin(coin2x, coin2y, coin2z);
		if (!coin3Taken) drawCoin(coin3x, coin3y, coin3z);
		drawTree(2, 0.021, 2);
		drawTree(1.1, 0.015, 2.2);
		drawTree(0, 0.021, 2.2);
		drawTree(2.2, 0.015, 1);
		drawTree(2.2, 0.015, 0);
		drawTree(2.2, 0.015, -1);
		//drawSnowMan(2, 0.1, 2);
		
	}
	else {
		if (!coin4Taken) drawCoin(coin4x, coin4y, coin4z);
		if (!coin5Taken) drawCoin(coin5x, coin5y, coin5z);
		if (!coin6Taken) drawCoin(coin6x, coin6y, coin6z);
		drawSnowMan(2.2, 0.25, 2.2,180);
		drawSnowman(1.1, 0.015, 2.2,180);
		drawSnowMan(0, 0.25, 2.2,180);
		drawSnowman(2.2, 0.015, 1, -90);
		drawSnowMan(2.2, 0.25, 0, -90);
		drawSnowman(2.2, 0.015, -1, -90);
	
	}
	if (lives <= 0 || gameOver) {
		drawWinLose();
	}
	if (!start) {
		drawStart();
	}

	if (goomba1Alive) drawGoomba(goomba1x, goomba1y, goomba1z, -90);
	if (goomba2Alive){
		glPushMatrix();
		drawGoomba(goomba2x, goomba2y, goomba2z, 0);
		glPopMatrix();
	} 


	
	glFlush();
}





int front = 0;
int top = 0;
int side = 0;

bool moving = false;
double xCam = 0;
double zCam = -1.5;
double leftBound = 1.98;
double rightBound = -1.41;
double nearBound = -1.41;
double farBound = 1.98;
int cameraView = 3;

void thirdPersonCamera() {
	camera.eye.x = xCam + marioX;
	camera.eye.y = 0.5 + marioY;
	camera.eye.z = zCam + marioZ;
	camera.center.x = 0 + marioX;
	camera.center.y = 0.5 + marioY;
	camera.center.z = 0.5;

}
int cameraAngle = 0;

void rotateCameraToAngle(int newAngle) {
	if (newAngle != cameraAngle) {
		if (newAngle == 0) {
			camera.center.x = marioX;
			camera.center.y = marioY;
			camera.center.z = farBound + 10;
		}
		else if (newAngle == 180) {
			cout << "in here";
			camera.center.x = marioX;
			camera.center.y = marioY;
			camera.center.z = nearBound - 10;
			camera.eye.z = marioZ - 0.1;
		}

	}
}

void firstPersonCamera() {
	camera.eye.x = marioX;
	camera.eye.y = 0.2 + marioY;
	camera.eye.z = 0.1 + marioZ;
	camera.center.x = marioX;
	camera.center.y =  marioY;
	camera.center.z = farBound+ 10;
	rotateCameraToAngle(marioAngle);
}

void updateCamera() {
	if (cameraView == 3)
		thirdPersonCamera();
	else if (cameraView == 1)
		firstPersonCamera();
}

bool marioHeartHit(double x, double y, double z) {
	if (marioX < x + 0.15 && marioX > x - 0.15 && marioZ < z +0.05 && marioZ > z - 0.05) {
		lives++;
		heartTaken = true;
		PlaySound(TEXT("sounds/lifeup.wav"), NULL, SND_FILENAME | SND_ASYNC);
		return true;
	}
	return false;
}



bool marioPlatform1Hit() {
	bool hit = false;

	double xEdge1 = platform1x - platform1l / 2;
	double xEdge2 = platform1x + platform1l / 2;
	double zEdge1 = platform1z - platform1w / 2;
	double zEdge2 = platform1z + platform1w / 2;



	if (marioX > xEdge1 && marioX < xEdge2 && marioZ > zEdge1 && marioZ < zEdge2) {
		if (marioY < platform1y + platform1h) {
			hit = true;
			cout << "Mario hit platform \n ";
		}

	}
	return hit;
}
bool marioAbovePlat = false;

bool marioPlatformXHit(double x, double y, double z, double l, double w, double h) {
	bool hit = false;

	double xEdge1 = x - l / 2;
	double xEdge2 = x + l / 2;
	double zEdge1 = z - w / 2;
	double zEdge2 = z + w / 2;


	if (marioX > xEdge1 && marioX < xEdge2 && marioZ > zEdge1 && marioZ < zEdge2) {
		marioAbovePlat = true;
		if (marioY < y + h) {
			hit = true;
			cout << "Mario hit platform \n ";
		}

	}
	return hit;
}
double baseHeight = 0.021;
bool marioAbovePlatformX(double x, double y, double z, double l, double w, double h) {
	bool hit = false;

	double xEdge1 = x - l / 2;
	double xEdge2 = x + l / 2;
	double zEdge1 = z - w / 2;
	double zEdge2 = z + w / 2;


	if (marioX > xEdge1 && marioX < xEdge2 && marioZ > zEdge1 && marioZ < zEdge2 && marioY >0.021) {
		baseHeight = y + h + 0.001;
		hit = true;
	}
	return hit;
}

bool marioAboveAnyPlatform() {
	if (marioAbovePlatformX(platform1x, platform1y, platform1z, platform1l, platform1w, platform1h)
		|| marioAbovePlatformX(platform2x, platform2y, platform2z, platform2l, platform2w, platform2h)
		|| marioAbovePlatformX(platform3x, platform3y, platform3z, platform3l, platform3w, platform3h)
		) {
		return true;
	}
	baseHeight = 0.021;
	return false;
}

bool marioAnyPlaformHit() {
	if (marioPlatformXHit(platform1x, platform1y, platform1z, platform1l, platform1w, platform1h)
		|| marioPlatformXHit(platform2x, platform2y, platform2z, platform2l, platform2w, platform2h)
		|| marioPlatformXHit(platform3x, platform3y, platform3z, platform3l, platform3w, platform3h)
		) {
		//cout << "plat hit";
		return true;
	}
	return false;
}



bool boundsHit() {
	if (marioX > leftBound || marioX < rightBound || marioZ > farBound || marioZ < nearBound) {
		return true;
	}
	return false;
}

void move(int direction) {
	/*cout << "\nX: ";
	cout << marioX;
	cout << "\nY: ";
	cout << marioY;
	cout << "\nZ: ";
	cout << marioZ;*/
	if (direction == 1) {
		marioZ += 0.03;
		if (marioAnyPlaformHit() || boundsHit()) {
			marioZ -= 0.03;
		}
	}
	else if (direction == -1) {
		marioZ -= 0.03;
		if (marioAnyPlaformHit() || boundsHit()) {
			marioZ += 0.03;
		}
	}
	else if (direction == 2) {
		marioX -= 0.03;
		if (marioAnyPlaformHit() || boundsHit()) {
			marioX += 0.03;
		}
	}
	else if (direction == -2) {
		marioX += 0.03;
		if (marioAnyPlaformHit() || boundsHit()) {
			marioX -= 0.03;
		}
	}

	moving = true;
	if (!rightLegForward) {
		rightLegBackward = false;
		rightLegForward = true;
		leftLegBackward = true;
		leftLegForward = false;
	}
	else {
		rightLegBackward = true;
		rightLegForward = false;
		leftLegBackward = false;
		leftLegForward = true;
	}
	updateCamera();

}

void setLegsOff() {
	rightLegBackward = false;
	rightLegForward = false;
	leftLegBackward = false;
	leftLegForward = false;
}




int jumpsAvailable = 1;



void Keyboard(unsigned char key, int x, int y) {
	float d = 0.04;
		
	if (start) {

		switch (key) {
		case 'w':
			marioAngle = 0;
			move(1);
			if (cameraView == 1) {


			}
			break;
		case 'a':
			marioAngle = 90;
			move(-2);
			if (cameraView == 1) {
			}

			break;
		case 's':
			marioAngle = 180;
			move(-1);

			break;
		case 'd':
			marioAngle = 270;
			move(2);
			if (cameraView == 1) {

			}

			break;
		case 'i':
			camera.moveY(d);
			break;
		case 'k':
			camera.moveY(-d);
			break;
		case 'j':
			camera.moveX(d);
			break;
		case 'l':
			camera.moveX(-d);
			break;
		case 'q':
			camera.moveZ(d);
			break;
		case 'e':
			camera.moveZ(-d);
			break;
		case 'c':
			if (front == 0) {
				top = 0;
				side = 0;
				camera.eye.x = 0.5;
				camera.eye.y = 0.5;
				camera.eye.z = -2;
				camera.center.x = 0.5 + marioX;
				camera.center.y = 0.5 + marioY;
				camera.center.z = 0.5 + marioZ;
				front = 1;
			}
			else {
				camera.eye.x = 1;
				camera.eye.y = 1;
				camera.eye.z = 1;
				camera.center.x = 0;
				camera.center.y = 0;
				camera.center.z = 0;
				top = 0;
				front = 0;
				side = 0;
			}
			break;

		case 'v':
			if (top == 0) {
				front = 0;
				side = 0;
				camera.eye.x = 0.5;
				camera.eye.y = 4;
				camera.eye.z = 0.51;
				camera.center.x = 0.5;
				camera.center.y = 0.5;
				camera.center.z = 0.5;
				top = 1;
			}
			else {
				camera.eye.x = 1;
				camera.eye.y = 1;
				camera.eye.z = 1;
				camera.center.x = 0;
				camera.center.y = 0;
				camera.center.z = 0;
				top = 0;
				front = 0;
				side = 0;
			}
			break;

		case 'b':
			if (side == 0) {
				front = 0;
				top = 0;
				camera.eye.x = 2;
				camera.eye.y = 0.5;
				camera.eye.z = 0.5;
				camera.center.x = 0.5;
				camera.center.y = 0.5;
				camera.center.z = 0.5;
				side = 1;
			}
			else {
				camera.eye.x = 1;
				camera.eye.y = 1;
				camera.eye.z = 1;
				camera.center.x = 0;
				camera.center.y = 0;
				camera.center.z = 0;
				top = 0;
				front = 0;
				side = 0;
			}
			break;
		case 'm':
			if (cameraView == 1) {
				cameraView = 3;
			}
			else {
				cameraView = 1;
			}
			updateCamera();
			break;

		case 32:
			if (jumpsAvailable > 0) {
				marioY += 0.5;
				jumpsAvailable--;
				updateCamera();
				/*camera.rotateY(marioAngle);*/
				armAngle = 180;
				PlaySound(TEXT("sounds/yahoo.wav"), NULL, SND_FILENAME | SND_ASYNC);
			}
			break;

		case GLUT_KEY_ESCAPE:
			exit(EXIT_SUCCESS);
		}
	}
	if (lives > 0 && !gameOver ) {//&&start
		/*if(cameraView == 1)
			camera.rotateY(marioAngle);*/
		glutPostRedisplay();
	}
}
void Special(int key, int x, int y) {
	float a = 1.0;
	if (start) {
		switch (key) {
		case GLUT_KEY_UP:
			camera.rotateX(a);
			break;
		case GLUT_KEY_DOWN:
			camera.rotateX(-a);
			break;
		case GLUT_KEY_LEFT:
			camera.rotateY(a);
			break;
		case GLUT_KEY_RIGHT:
			camera.rotateY(-a);
			break;
		}
	}
	if (lives > 0 && !gameOver ) {//&& start
		glutPostRedisplay();
	}
}

void goomba1Motion() {
	if (goomba1MovesRight) {
		if (goomba1x < 1.97)
			goomba1x += 0.03;
		else {
			goomba1MovesRight = false;
		}

	}
	else {
		if (goomba1x > -1.3)
			goomba1x -= 0.03;
		else {
			goomba1MovesRight = true;
		}
	}
}

void goomba2Motion() {
	if (goomba2MovesRight) {
		if (goomba2z < 1.97)
			goomba2z += 0.03;
		else {
			goomba2MovesRight = false;
		}

	}
	else {
		if (goomba2z > -1.3)
			goomba2z -= 0.03;
		else {
			goomba2MovesRight = true;
		}
	}
}

void marioCoinHit(double x, double y, double z, int coinId) {
	if (marioX < x + 0.1 && marioX > x - 0.1 && marioZ < z + 0.1 && marioZ > z - 0.1 && marioY < y + 0.1 && marioY+0.2 > y - 0.1) {// from 0.05 to 0.1 hmm  z kant 0.06
		score += 10;
		PlaySound(TEXT("sounds/coin.wav"), NULL, SND_FILENAME | SND_ASYNC);
		switch (coinId) {
		case 1:
			coin1Taken = true;
			break;
		case 2: 
			coin2Taken = true;
			break;
		case 3:
			coin3Taken = true;
			break;
		case 4:
			coin4Taken = true;
			break;
		case 5:
			coin5Taken = true;
			break;
		case 6:
			coin6Taken = true;
			break;
		}
	}
}

void marioGoombaXHit(double x, double z, int id) {
	if (marioX < x + 0.14 && marioX> x - 0.14 && marioZ < z + 0.14 && marioZ> z - 0.14) {

		if (marioY > 0.121 && marioY < 0.3) {
			cout << "Mario hit goomba\n";
			PlaySound(TEXT("sounds/goomba_dead.wav"), NULL, SND_FILENAME | SND_ASYNC);
			switch (id) {
			case 1:
				goomba1Alive = false;
				break;
			case 2:
				goomba2Alive = false;
				break;
			}
			
		}
		else if (marioY < 0.121) {
			cout << "goomba hit Mario\n";
			if (lives > 1) {
				marioY += 0.5;
				PlaySound(TEXT("sounds/mariodamage.wav"), NULL, SND_FILENAME | SND_ASYNC);
				lives--;
				
			}
			else {
				marioY += 0.5;
				lives--;
				PlaySound(TEXT("sounds/mariodie.wav"), NULL, SND_FILENAME | SND_ASYNC);
				glutPostRedisplay();
			}
			
		}
	}
}

int legsTimeout = 0;
int jumpTimeout = 0;


void baseHeightChange() {
	if (marioAbovePlatformX(platform1x, platform1y, platform1z, platform1l, platform1w, platform1h) && marioY <= platform1h + platform1y && baseHeight != platform1h + platform1y) {
		marioY = platform1h + platform1y;
		baseHeight = platform1h + platform1y;
	}
	if (marioAbovePlatformX(platform2x, platform2y, platform2z, platform2l, platform2w, platform2h) && marioY <= platform2h + platform2y && baseHeight != platform2h + platform2y) {
		marioY = platform2h + platform2y;
		baseHeight = platform2h + platform2y;
	}
	if (marioAbovePlatformX(platform3x, platform3y, platform3z, platform3l, platform3w, platform3h) && marioY <= platform3h + platform3y && baseHeight != platform3h + platform3y) {
		marioY = platform3h + platform3y;
		baseHeight = platform3h + platform3y;
	}
	/*cout << "Base Height";
	cout << baseHeight;
	cout << "\n";
	cout << "MarioY";
	cout << marioY;*/

}

void marioFalls() {
	cout << "falling\n";
	marioY -= 0.02;
	updateCamera();
	//camera.rotateY(marioAngle);
	baseHeightChange();
	armAngle = 0;
	if (marioY <= baseHeight && jumpsAvailable < 1) {
		jumpsAvailable++;
	}
}
bool flag_falling = false;

void changeLevel() {
	level++;
	levelCompleted = false;
	lightPosY = -5.0f;
	marioX = 0;
	marioY = 0.0215;
	marioZ = 0;

	platform1x = -0.5;
	platform1y = 0.021;
	platform1z = 1;

	platform2z = -platform2z + 1;
	platform2y = platform2y - 0.1;
	platform2x = platform2x - 0.1;

	coin4x = platform1x;
	coin4y = platform1y + 0.3; // +0.1
	coin4z = platform1z;

	coin5x = platform2x;
	coin5y = platform2y + 0.3;//+0.1
	coin5z = platform2z;

	coin6x = heartx;
	coin6y = 0.2; //+0.1
	coin6z = heartz;

	heartx = 0;
	hearty = 0.2;
	heartz = -1;
	heartTaken = false;

	goomba1Alive = true;
	goomba2Alive = true;

	goomba1x = -1;
	goomba1z = -1;

	goomba2x = 0.7;
	goomba2z = 1.2;

	flag_falling = false;
	flagY = 1.9;
	lightPosY = -5.0f;
	updateCamera();
}




int colorTimeout = 0;

int timerTimeout = 0;

void Timer(int value) {
	if (start) {// hmm
		cout << "\nTime Left";
		cout << timer;
		
		if (timerTimeout < 1000) {
			timerTimeout += 100;
		}
		else {
			timerTimeout = 0;
			timer -= 1;
			if (timer <= 0) {
				gameOver = true;
				glutPostRedisplay();
			}
		}

		baseHeightChange();
		if (score == 30 && level == 1) {
			levelCompleted = true;
		}
		if (score == 60 && level == 2) {
			youWin = true;
			levelCompleted = true;
		}

		if (lightPosY < 0.0f) {
			backR = 75;
			backG = 61;
			backB = 96;
		}
		else if (lightPosY < 1.5f) {
			backR = 253;
			backG = 94;
			backB = 83;
		}
		else if (lightPosY < 3.0f) {
			backR = 252;
			backG = 156;
			backB = 84;
		}

		if (level == 1) {
			//cout << lightPosY;
			//cout << "\n";
			lightPosY -= 0.05;


			/*if (colorTimeout == 0) {
				changeColor(backR, backB, backB, 252, 156, 84);
			}
			if (colorTimeout == 100) {
				changeColor(backR, backB, backB, 253, 94, 83);
			}
			if (colorTimeout == 200) {
				changeColor(backR, backB, backB, 75, 61, 96);
			}
			if (colorTimeout == 300) {
				stepB = stepG = stepR = 0;
			}
			colorTimeout += 1;*/
		}
		else {
			lightPosY += 0.05;
		}





		if (flag_falling || flagY > 0.1 && marioAbovePlatformX(platform3x, platform3y, platform3z, platform3l, platform3w, platform3h) && levelCompleted) {
			flagY -= 0.1;
			PlaySound(TEXT("sounds/clearstage.wav"), NULL, SND_FILENAME | SND_ASYNC);
			flag_falling = true;
		}
		if (flagY <= 0.1) {
			if (level == 2) {
				gameOver = true;
				glutPostRedisplay();
				PlaySound(TEXT("sounds/win.wav"), NULL, SND_FILENAME | SND_ASYNC);
			}
			else {
				changeLevel();
			}
		}

		if (goomba1Alive) {
			goomba1Motion();
			marioGoombaXHit(goomba1x, goomba1z, 1);
		}

		if (goomba2Alive) {
			goomba2Motion();
			marioGoombaXHit(goomba2x, goomba2z, 2);
		}

		if (moving) {
			legsTimeout += 100;
		}
		if (legsTimeout >= 500) {
			moving = false;
			setLegsOff();
		}

		if (level == 1) {
			if (!coin1Taken) {
				marioCoinHit(coin1x, coin1y, coin1z, 1);
			}

			if (!coin2Taken) {
				marioCoinHit(coin2x, coin2y, coin2z, 2);
			}

			if (!coin3Taken) {
				marioCoinHit(coin3x, coin3y, coin3z, 3);
			}
		}
		if (level == 2) {
			if (!coin4Taken) {
				marioCoinHit(coin4x, coin4y, coin4z, 4);
			}

			if (!coin5Taken) {
				marioCoinHit(coin5x, coin5y, coin5z, 5);
			}

			if (!coin6Taken) {
				marioCoinHit(coin6x, coin6y, coin6z, 6);
			}
		}


		if (marioY > baseHeight && jumpTimeout == 0) {
			jumpTimeout += 100;
		}
		else if (jumpTimeout >= 100 && marioY > baseHeight) {
			marioFalls();
		}

		if (!marioAboveAnyPlatform() && baseHeight != 0.021) {
			cout << "reseting base height\n";
			baseHeight = 0.021;
		}
		if (!heartTaken) {
			marioHeartHit(heartx, hearty, heartz);
		}


	}

	if (lives > 0 && !gameOver ) { //&&start
		glutPostRedisplay();
		glutTimerFunc(100, Timer, 0);
	}

	

}


void main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(50, 50);

	glutCreateWindow("Mario 64");
	glutDisplayFunc(Display);
	PlaySound(TEXT("sounds/we_go.wav"), NULL, SND_FILENAME | SND_ASYNC);
	
	glutMouseFunc(mouse);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(Special);
	glutTimerFunc(0, Timer, 0);
	LoadAssets();

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	glEnable(GL_LIGHT3);
	glEnable(GL_LIGHT4);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);

	glutMainLoop();
}
