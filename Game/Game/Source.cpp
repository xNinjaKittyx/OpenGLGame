#define GLFW_INCLUDE_GLU


#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <iostream>
#include <vector>
#include "Sphere.h"
#include "Bullet.h"
/*

======= CONSTANTS =======

*/
const double PI = 3.141592653; 
const float TO_RADS = 3.141592654f / 180.0f;


/*

======= Window Size =======

*/
GLint windowWidth = 800;
GLint windowHeight = 600;
GLint midWindowX = windowWidth / 2;
GLint midWindowY = windowHeight / 2;


/*

======= Perspective Variables =======

*/
GLfloat fieldOfView = 45.0f;                
GLfloat near = 1.0f;                  
GLfloat far = 1500.0f;       

/*

======= Camera Variables =======

*/
GLfloat camXRot = 0.0f;
GLfloat camYRot = 180.0f;
GLfloat camZRot = 0.0f;

GLfloat camXPos = 300.0f;
GLfloat camYPos = 0.0f;
GLfloat camZPos = 50.0f;

GLfloat camXSpeed = 0.0f;
GLfloat camYSpeed = 0.0f;
GLfloat camZSpeed = 0.0f;


/*

======= Player Speed =======

*/
GLfloat movementSpeedFactor = 0.1f;


/*

======= All Dem Booleans =======

*/
bool holdingForward = false;
bool holdingBackward = false;
bool holdingLeftStrafe = false;
bool holdingRightStrafe = false;
bool noClip = false;

bool xAllowed = true;
bool yAllowed = true;
bool zAllowed = true;

/*

======= Random counting variables =======

*/
int frameCount = 0;
int catches = 0;
double lastBulletShot = 0;

/*

======= I be using dem radians =======

*/
float toRads(const float &degrees)
{
	return degrees * TO_RADS;
}

/*

======= My structure for a ball =======

*/

Sphere redBall;

// Vector that stores bullets
std::vector<Bullet*> bullets;



/*

======= glfw error checker =======

*/
void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

//Mouse, Camera, Input handling Credits to r3dux.org


/*

======= Camera Functions =======
All these camera functions use the keyinput and mouse to navigate around the world.

*/
void moveCamera()
{
	
	camXPos += camXSpeed;
	camYPos += camYSpeed;
	camZPos += camZSpeed;
}

void handleMouseMove(GLFWwindow* window, double mouseX, double mouseY)
{
	GLfloat vertMouseSensitivity = 10.0f;
	GLfloat horizMouseSensitivity = 10.0f;

	//cout << "Mouse cursor is at position (" << mouseX << ", " << mouseY << endl;

	int horizMovement = mouseX - midWindowX;
	int vertMovement = mouseY - midWindowY;



	camXRot += vertMovement / vertMouseSensitivity;
	camYRot += horizMovement / horizMouseSensitivity;


	// Control looking up and down with the mouse forward/back movement
	// Limit loking up to vertically up
	if (camXRot < -90.0f)
	{
		camXRot = -90.0f;
	}

	// Limit looking down to vertically down
	if (camXRot > 90.0f)
	{
		camXRot = 90.0f;
	}

	// Looking left and right. Keep the angles in the range -180.0f (anticlockwise turn looking behind) to 180.0f (clockwise turn looking behind)
	if (camYRot < -180.0f)
	{
		camYRot += 360.0f;
	}

	if (camYRot > 180.0f)
	{
		camYRot -= 360.0f;
	}

	glfwSetCursorPos(window, midWindowX, midWindowY);
}

void handleMouseButton(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		Bullet *a = new Bullet(camXPos, camYPos, camZPos, camXRot, camYRot, camZRot);
		bullets.push_back(a);
		std::cout << "Bullet Created" << std::endl;
	}
		
}
	
void calculateCameraMovement()
{
	// Break up our movement into components along the X, Y and Z axis
	float camMovementXComponent = 0.0f;
	float camMovementYComponent = 0.0f;
	float camMovementZComponent = 0.0f;

	if (holdingForward == true)
	{
		// Control X-Axis movement
		float pitchFactor = cos(toRads(camXRot));
		if (xAllowed == true)
			camMovementXComponent += (movementSpeedFactor * float(sin(toRads(camYRot)))) * pitchFactor;

		// Control Y-Axis movement
		if (noClip == true)
			camMovementYComponent += movementSpeedFactor * float(sin(toRads(camXRot))) * -1.0f;

		// Control Z-Axis movement
		float yawFactor = float(cos(toRads(camXRot)));
		if (zAllowed == true)
			camMovementZComponent += (movementSpeedFactor * float(cos(toRads(camYRot))) * -1.0f) * yawFactor;
	}

	if (holdingBackward == true)
	{
		// Control X-Axis movement
		float pitchFactor = cos(toRads(camXRot));
		if (xAllowed == true)
			camMovementXComponent += (movementSpeedFactor * float(sin(toRads(camYRot))) * -1.0f) * pitchFactor;

		// Control Y-Axis movement
		if (noClip == true)
			camMovementYComponent += movementSpeedFactor * float(sin(toRads(camXRot)));

		// Control Z-Axis movement
		float yawFactor = float(cos(toRads(camXRot)));
		if (zAllowed == true)
			camMovementZComponent += (movementSpeedFactor * float(cos(toRads(camYRot)))) * yawFactor;
	}

	if (holdingLeftStrafe == true)
	{
		// Calculate our Y-Axis rotation in radians once here because we use it twice
		float yRotRad = toRads(camYRot);

		if (xAllowed == true)
			camMovementXComponent += -movementSpeedFactor * float(cos(yRotRad));
		if (zAllowed == true)
			camMovementZComponent += -movementSpeedFactor * float(sin(yRotRad));
	}

	if (holdingRightStrafe == true)
	{
		// Calculate our Y-Axis rotation in radians once here because we use it twice
		float yRotRad = toRads(camYRot);

		if (xAllowed == true)
			camMovementXComponent += movementSpeedFactor * float(cos(yRotRad));
		if (zAllowed == true)
			camMovementZComponent += movementSpeedFactor * float(sin(yRotRad));
	}

	// After combining our movements for any & all keys pressed, assign them to our camera speed along the given axis
	camXSpeed = camMovementXComponent;
	camYSpeed = camMovementYComponent;
	camZSpeed = camMovementZComponent;

	// Cap the speeds to our movementSpeedFactor (otherwise going forward and strafing at an angle is twice as fast as just going forward!)
	// X Speed cap
	if (camXSpeed > movementSpeedFactor)
	{
		//cout << "high capping X speed to: " << movementSpeedFactor << endl;
		camXSpeed = movementSpeedFactor;
	}
	if (camXSpeed < -movementSpeedFactor)
	{
		//cout << "low capping X speed to: " << movementSpeedFactor << endl;
		camXSpeed = -movementSpeedFactor;
	}

	// Y Speed cap
	if (camYSpeed > movementSpeedFactor)
	{
		//cout << "low capping Y speed to: " << movementSpeedFactor << endl;
		camYSpeed = movementSpeedFactor;
	}
	if (camYSpeed < -movementSpeedFactor)
	{
		//cout << "high capping Y speed to: " << movementSpeedFactor << endl;
		camYSpeed = -movementSpeedFactor;
	}

	// Z Speed cap
	if (camZSpeed > movementSpeedFactor)
	{
		//cout << "high capping Z speed to: " << movementSpeedFactor << endl;
		camZSpeed = movementSpeedFactor;
	}
	if (camZSpeed < -movementSpeedFactor)
	{
		//cout << "low capping Z speed to: " << movementSpeedFactor << endl;
		camZSpeed = -movementSpeedFactor;
	}
}

/*

======= Can't be Passing dose boundaries =======

*/
void inboundsCheck() {
	if (noClip == false) {
		//Section 1 check
		if (camZPos <= 200) {
			if (camXPos < 210) {
				xAllowed = false;
				camXPos = 210;
			}
			else if (camXPos > 390) {
				xAllowed = false;
				camXPos = 390;
			}

			if (camZPos < 10) {
				zAllowed = false;
				camZPos = 10;
			}
		}
		//Section 2 Check
		else if (camZPos > 200 && camZPos <= 800) {
			if (camXPos < 200) {
				//Check for Section 2, Bottom Left
				if (camZPos < 210) {
					zAllowed = false;
					camZPos = 210;
				}
				//Check for Section 2, Left Wall
				if (camXPos < 10) {
					xAllowed = false;
					camXPos = 10;
				}
			}
			else if (camXPos > 400) {
				//Check for Section 2, Bottom Right
				if (camZPos < 210) {
					zAllowed = false;
					camZPos = 210;
				}
				//Check for Section 2, Right Wall
				if (camXPos > 590) {
					xAllowed = false;
					camXPos = 590;
				}
			}
			//Section 2 Check Cylinder in Middle

			//Section 2 Check Back wall.
			if (camZPos > 790 && (camXPos > 350 || camXPos < 250)) {
				zAllowed = false;
				camZPos = 790;
			}
		}
	}
}


void handleKeypress(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// If a key is pressed, toggle the relevant key-press flag
	if (action == GLFW_REPEAT || action == GLFW_PRESS)
	{

		switch (key)
		{
		case 'W':
			holdingForward = true;
			break;

		case 'S':
			holdingBackward = true;
			break;

		case 'A':
			holdingLeftStrafe = true;
			break;

		case 'D':
			holdingRightStrafe = true;
			break;

		case 'X':
			if (noClip == true) {
				noClip = false;
				camYPos = 0;
			} else
				noClip = true;

		case GLFW_KEY_LEFT_SHIFT:
			//Speed Multiplier
			movementSpeedFactor = 0.15f;
			break;
			
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;

		default:
			// Do nothing...
			break;
		}
	}

	else // If a key is released, toggle the relevant key-release flag
	{
		switch (key)
		{
		case 'W':
			holdingForward = false;
			break;

		case 'S':
			holdingBackward = false;
			break;

		case 'A':
			holdingLeftStrafe = false;
			break;

		case 'D':
			holdingRightStrafe = false;
			break;

		case GLFW_KEY_LEFT_SHIFT:
			//Speed Multiplier
			movementSpeedFactor = 0.1f;
			break;

		default:
			// Do nothing...
			break;
		}
	}
}
//Up to here is where most of the code came from r3dux.org. Open-Sourced, Legal to use. Edited by me to fit my conditions.



/*

======= Draw a Cylinder =======

*/
void drawCylinder(GLfloat radius, GLfloat height, GLfloat red = 1.0f, GLfloat green = 0.0f, GLfloat blue = 0.0f) {
	double x, y, z;

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(red, green, blue);
	glBegin(GL_QUAD_STRIP);
	for (double thet = -180.0; thet <= 180.0; thet += 10.0)
	{
		y = height;
		x = radius * sin(toRads(thet));
		z = radius * cos(toRads(thet));
		glVertex3d(x, y, z);
		y = 0;
		glVertex3d(x, y, z);
	}
	glEnd();


}

/*

======= Draw a box. =======

*/
void cube(int size) {
	double x = size / 2;

	float ver[8][3] =
	{
		{ -x, -x, x },
		{ -x, x, x },
		{ x, x, x },
		{ x, -x, x },

		{ -x, x, -x },
		{ x, x, -x },
		{ x, -x, -x},
		{ -x, -x, -x}

	};
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_QUADS);

	glColor3f(0.75, 0.75, 0.0);
	for (int i = 0; i < 4; i++) {
		glVertex3fv(ver[i]);
	}

	glColor3f(0.75, 0.0, 0.75);
	for (int i = 4; i < 8; i++) {
		glVertex3fv(ver[i]);
	}
	
	glColor3f(0.75, 0.0, 0.0);
	//TOP FACE
	glVertex3fv(ver[1]);
	glVertex3fv(ver[2]);
	glVertex3fv(ver[5]);
	glVertex3fv(ver[4]);

	glColor3f(0.0, 0.75, 0.75);
	//BOTTOM FACE
	glVertex3fv(ver[0]);
	glVertex3fv(ver[3]);
	glVertex3fv(ver[6]);
	glVertex3fv(ver[7]);

	glColor3f(0.0, 0.0, 0.75);
	//LEFT FACE
	glVertex3fv(ver[0]);
	glVertex3fv(ver[1]);
	glVertex3fv(ver[4]);
	glVertex3fv(ver[7]);

	glColor3f(0.0, 0.75, 0.0);
	//RIGHT FACE
	glVertex3fv(ver[2]);
	glVertex3fv(ver[3]);
	glVertex3fv(ver[6]);
	glVertex3fv(ver[5]);

	glEnd();
	
}


/*

======= Draw the Ground =======

*/
void drawGround(GLfloat extent = 600.0f)
{
	//GLfloat extent = 2000.0f; // How far on the Z-Axis and X-Axis the ground extends
	GLfloat stepSize = 20.0f;  // The size of the separation between points
	GLfloat groundLevel = -50.0f;   // Where on the Y-Axis the ground is drawn

	// Set colour to white
	glColor3ub(169, 236, 255);

	// Draw our ground grid
	glBegin(GL_LINES);
	for (GLfloat loop = -extent; loop < extent; loop += stepSize)
	{
		// Draw lines along Z-Axis
		glVertex3f(loop, groundLevel, extent);
		glVertex3f(loop, groundLevel, -extent);

		// Draw lines across X-Axis
		glVertex3f(-extent, groundLevel, loop);
		glVertex3f(extent, groundLevel, loop);
	}
	glEnd();

}

void drawWall(GLfloat extent = 200.0f){
	GLfloat groundLevel = -50;
	GLfloat stepSize = 20.0f;

	glColor3ub(255, 169, 169);

	glBegin(GL_LINES);
	for (GLfloat loop = -50.0f; loop < 150.0f; loop += stepSize) {
		// Draw lines along Z-Axis
		glVertex3f(0, loop, extent);
		glVertex3f(0, loop, -extent);
	}
	for (GLfloat loop = -extent; loop < extent; loop += stepSize) {
		// Draw lines across X-Axis
		glVertex3f(0, 150.0f, loop);
		glVertex3f(0, -50.0f, loop);
	}
	glEnd();

}


// Function to draw our spheres and position the light source
void drawScene(GLFWwindow *window)
{
	// Clear the screen and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// Reset the matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fieldOfView, 4.0f / 3.0f, near, far);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//If you catch the ball (or hit by it depending how you see it) reset the player's position.
	if (redBall.collision(camXPos, camYPos, camZPos)) {
		camXPos = 300;
		camYPos = 0;
		camZPos = 100;
		camXRot = 0;
		camYRot = 180;
		camZRot = 0;
		catches++;
		std::cout << "You caught the ball!" << std::endl << "You have caught " << catches << " balls" << std::endl;
	}


	glRotatef(camXRot, 1.0f, 0.0f, 0.0f);        // Rotate our camera on the x-axis (looking up and down)
	glRotatef(camYRot, 0.0f, 1.0f, 0.0f);        // Rotate our camera on the  y-axis (looking left and right)
	glTranslatef(-camXPos, -camYPos, -camZPos);    // Translate the modelviewm matrix to the position of our camera
	
	// Draw Cylinder
	glPushMatrix();
	glTranslatef(300.0, -50.0, 500.0);
	drawCylinder(100, 200);
	glPopMatrix();
	
	// Draw the Gun
	glPushMatrix();
	glRotatef(-camXRot, 1.0f, 0.0f, 0.0f);
	glRotatef(-camYRot, 0.0f, 1.0f, 0.0f);
	glTranslatef(camXPos, camYPos - 5, camZPos + 5);
	//DrawGun
	glPopMatrix();


	// Draw the Floor

	//Section 1
	glPushMatrix();
	glTranslatef(300.0f, 0.0f, 100.0f);
	drawGround(100);
	glPopMatrix();

	//Section 2
	glPushMatrix();
	glTranslatef(300.0f, 0.0f, 500.0f);
	drawGround(300);
	glPopMatrix();

	//Section 3
	glPushMatrix();
	glTranslatef(300.0f, 0.0f, 900.0f);
	glScalef(.5, 1, 1);
	drawGround(100);
	glPopMatrix();

	//Section 4
	glPushMatrix();
	glTranslatef(300.0f, 0.0f, 1300.0f);
	drawGround(300);
	glPopMatrix();

	// Draw the Roof

	glPushMatrix();

	glTranslatef(0.0f, 200.0f, 0.0f);

	//Section 1
	glPushMatrix();
	glTranslatef(300.0f, 0.0f, 100.0f);
	drawGround(100);
	glPopMatrix();

	//Section 2
	glPushMatrix();
	glTranslatef(300.0f, 0.0f, 500.0f);
	drawGround(300);
	glPopMatrix();

	//Section 3
	glPushMatrix();
	glTranslatef(300.0f, 0.0f, 900.0f);
	glScalef(.5, 1, 1);
	drawGround(100);
	glPopMatrix();

	//Section 4
	glPushMatrix();
	glTranslatef(300.0f, 0.0f, 1300.0f);
	drawGround(300);
	glPopMatrix();

	glPopMatrix();

	// Draw the Walls
	//Section 1
	glPushMatrix();
	glTranslatef(200.0f, 0.0f, 100.0f);
	drawWall(100);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(400.0f, 0.0f, 100.0f);
	drawWall(100);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(300.0f, 0.0f, 0.0f);
	glRotatef(90, 0.0, 1.0, 0.0);
	drawWall(100);
	glPopMatrix();

	//Section 2
	glPushMatrix();	
	glTranslatef(100.0f, 0.0f, 200.0f);
	glRotatef(90, 0.0, 1.0, 0.0);
	drawWall(100);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(500.0f, 0.0f, 200.0f);
	glRotatef(90, 0.0, 1.0, 0.0);
	drawWall(100);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 500.0f);
	drawWall(300);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(600.0f, 0.0f, 500.0f);
	drawWall(300);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(125.0f, 0.0f, 800.0f);
	glRotatef(90, 0.0, 1.0, 0.0);
	drawWall(125);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(475.0f, 0.0f, 800.0f);
	glRotatef(90, 0.0, 1.0, 0.0);
	drawWall(125);
	glPopMatrix();

	for (int i = 0; i < bullets.size(); i++) {
		Bullet &b = *bullets[i];
		b.drawBullet();

	}

	redBall.drawSphere();

	// Draw the ball


	// ----- Stop Drawing Stuff! ------
}

void init(void) {

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_LIGHTING);

	glViewport(0, 0, windowWidth, windowHeight);
	GLfloat light_pos[] = { 300, -30, 500, 0.0 };
	GLfloat light_pos2[] = { -1, 1, 1, 0.0 };
	GLfloat light_pos3[] = { 1, 1, -1, 0.0 };
	GLfloat light_pos4[] = { -1, 1, -1, 0.0 };
	GLfloat light_Ka[] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat light_Kd[] = { 0.7, 0.7, 0.7, 1.0 };
	GLfloat light_Ks[] = { 1.0, 1.0, 1.0, 1.0 };
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);

	GLint specularMagnitude = 64;
	
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);/*
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_Ka);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_Kd);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_Ks);*/
	//glEnable(GL_LIGHT0);

	glEnable(GL_COLOR_MATERIAL);

	// Define the shininess of the material we'll use to draw things
	GLfloat materialSpecularReflectance[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	// Set Material properties to follow glColor values
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Use our shiny material and magnitude
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecularReflectance);
	glMateriali(GL_FRONT, GL_SHININESS, specularMagnitude);


	redBall.colorSphere(.75f, 0.0f, 0.0f);

}


int main(void)
{

	glfwSetErrorCallback(error_callback);

	GLFWwindow* window;

	/* Initialize the library */

	if (!glfwInit())
		exit(EXIT_FAILURE);

	std::cout << "glfw Initalized" << std::endl;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(windowWidth, windowHeight, "Dan's Puzzle", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	std::cout << "glfw Window Created" << std::endl;

	glfwSetWindowPos(window, 100, 100);

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	//Best to leave this at 1. Having this higher can lead to screen tearing and input latency.
	glfwSwapInterval(1);

	init();

	//Disable Cursor, Enable Event handlers
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, handleKeypress);
	glfwSetCursorPosCallback(window, handleMouseMove);
	glfwSetMouseButtonCallback(window, handleMouseButton);

	double t = 0.0;
	double dt = 1.0 / 60.0;

	std::cout << "Game Loop Initialized" << std::endl;
	while (!glfwWindowShouldClose(window))
	{
		/* Comput all the inputs */
		for (int i = 0; i < bullets.size(); i++) {
			Bullet &b = *bullets[i];
			b.integrate(t, dt);
			if (b.inBoundCheck() == false)
				bullets.erase(bullets.begin() + i);
		}
		redBall.inBoundCheck();
		redBall.integrate(t, dt);

		drawScene(window);

		inboundsCheck();


		calculateCameraMovement();

		moveCamera();

		frameCount++;

		xAllowed = true;
		zAllowed = true;

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

		t += dt;

	}

	glfwDestroyWindow(window);

	glfwTerminate();
	exit(EXIT_FAILURE);

}