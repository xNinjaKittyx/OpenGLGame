#define GLFW_INCLUDE_GLU


#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <ctime>
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

======= PlayerState =======

*/
struct State {

	float pos[3];

	float v[3];

} currentState;

struct Derivative {
	float dx[3]; // dx/dt = velocity

	float dv[3]; // dv/dt = acceleration
};
/*

======= Camera Variables =======

*/
GLfloat camXRot = 0.0f;
GLfloat camYRot = 180.0f;
GLfloat camZRot = 0.0f;



/*

======= Player Speed =======

*/
GLfloat movementSpeedFactor = 100.0f;
GLfloat originalSpeedFactor = movementSpeedFactor;


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

unsigned int deaths = 0;
unsigned int hits = 0;
unsigned int highScore = 0;

void showInfo() {
	std::cout
		<< "================================" << std::endl
		<< "High Score : " << highScore << std::endl
		<< "Deaths : " << deaths << std::endl
		<< "Hits : " << hits << std::endl
		<< "================================" << std::endl;
}
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
std::vector<Bullet *> bullets;



/*

======= glfw error checker =======

*/
void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

//Mouse, Camera, Input handling Credits to r3dux.org

/*
	Integrate Physics
*/

float acceleration(const State &state, int x, float t) {
	if (x >= 3)
		return 0;
	const float k = 0;
	const float b = 0;
	return -k * state.pos[x] - b * state.v[x];
}

Derivative evaluate(const State &initial, float t) {
	/*
	Evaluate the Derivative at the very beginning.
	*/

	Derivative output;
	for (int i = 0; i < 3; i++) {
		output.dx[i] = initial.v[i];
		output.dv[i] = acceleration(initial, i, t);
	}
	return output;
}
Derivative evaluate(const State &initial, float t, float dt, const Derivative &d) {
	State state;
	for (int i = 0; i < 3; i++) {
		state.pos[i] = initial.pos[i] + d.dx[i] * dt;
		state.v[i] = initial.v[i] + d.dv[i] * dt;
	}

	Derivative output;
	for (int i = 0; i < 3; i++) {
		output.dx[i] = state.v[i];
		output.dv[i] = acceleration(state, i, t + dt);
	}
	return output;
}

void integrate(float t, float dt) {

	Derivative a, b, c, d;

	a = evaluate(currentState, t, 0.0f, Derivative());
	b = evaluate(currentState, t, dt * 0.5f, a);
	c = evaluate(currentState, t, dt * 0.5f, b);
	d = evaluate(currentState, t, dt, c);

	float dxdt[3];
	float dvdt[3];

	for (int i = 0; i < 3; i++) {

		dxdt[i] = 1.0f / 6.0f *
			(a.dx[i] + 2.0f*(b.dx[i] + c.dx[i]) + d.dx[i]);

		dvdt[i] = 1.0f / 6.0f *
			(a.dv[i] + 2.0f * (b.dv[i] + c.dv[i]) + d.dv[i]);

		currentState.pos[i] += dxdt[i] * dt;
		currentState.v[i] += dvdt[i] * dt;
	}


}



/*

======= Camera Functions =======
All these camera functions use the keyinput and mouse to navigate around the world.

*/
void moveCamera()
{
	
	currentState.pos[0] += currentState.v[0];
	currentState.pos[1] += currentState.v[1];
	currentState.pos[2] += currentState.v[2];
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
		if (bullets.size() < 5) {
			Bullet *a = new Bullet(currentState.pos[0], currentState.pos[1], currentState.pos[2], camXRot, camYRot, camZRot);
			bullets.push_back(a);
		}
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
	currentState.v[0] = camMovementXComponent;
	currentState.v[1] = camMovementYComponent;
	currentState.v[2] = camMovementZComponent;

	// Cap the speeds to our movementSpeedFactor (otherwise going forward and strafing at an angle is twice as fast as just going forward!)
	// X Speed cap
	if (currentState.v[0] > movementSpeedFactor)
	{
		//cout << "high capping X speed to: " << movementSpeedFactor << endl;
		currentState.v[0] = movementSpeedFactor;
	}
	if (currentState.v[0] < -movementSpeedFactor)
	{
		//cout << "low capping X speed to: " << movementSpeedFactor << endl;
		currentState.v[0] = -movementSpeedFactor;
	}

	// Y Speed cap
	if (currentState.v[1] > movementSpeedFactor)
	{
		//cout << "low capping Y speed to: " << movementSpeedFactor << endl;
		currentState.v[1] = movementSpeedFactor;
	}
	if (currentState.v[1] < -movementSpeedFactor)
	{
		//cout << "high capping Y speed to: " << movementSpeedFactor << endl;
		currentState.v[1] = -movementSpeedFactor;
	}

	// Z Speed cap
	if (currentState.v[2] > movementSpeedFactor)
	{
		//cout << "high capping Z speed to: " << movementSpeedFactor << endl;
		currentState.v[2] = movementSpeedFactor;
	}
	if (currentState.v[2] < -movementSpeedFactor)
	{
		//cout << "low capping Z speed to: " << movementSpeedFactor << endl;
		currentState.v[2] = -movementSpeedFactor;
	}
}

/*

======= Can't be Passing dose boundaries =======

*/
void inboundsCheck() {
	if (noClip == false) {
		//Section 1 check
		if (currentState.pos[2] <= 200) {
			if (currentState.pos[0] < 210) {
				xAllowed = false;
				currentState.pos[0] = 210;
			}
			else if (currentState.pos[0] > 390) {
				xAllowed = false;
				currentState.pos[0] = 390;
			}

			if (currentState.pos[2] < 10) {
				zAllowed = false;
				currentState.pos[2] = 10;
			}
		}
		//Section 2 Check
		else if (currentState.pos[2] > 200 && currentState.pos[2] <= 800) {
			if (currentState.pos[0] < 200) {
				//Check for Section 2, Bottom Left
				if (currentState.pos[2] < 210) {
					zAllowed = false;
					currentState.pos[2] = 210;
				}
				//Check for Section 2, Left Wall
				if (currentState.pos[0] < 10) {
					xAllowed = false;
					currentState.pos[0] = 10;
				}
			}
			else if (currentState.pos[0] > 400) {
				//Check for Section 2, Bottom Right
				if (currentState.pos[2] < 210) {
					zAllowed = false;
					currentState.pos[2] = 210;
				}
				//Check for Section 2, Right Wall
				if (currentState.pos[0] > 590) {
					xAllowed = false;
					currentState.pos[0] = 590;
				}
			}
			//Section 2 Check Cylinder in Middle

			//Section 2 Check Back wall.
			if (currentState.pos[2] > 790 && (currentState.pos[0] > 350 || currentState.pos[0] < 250)) {
				zAllowed = false;
				currentState.pos[2] = 790;
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
				currentState.pos[1] = 0;
			} else
				noClip = true;

		case GLFW_KEY_LEFT_SHIFT:
			//Speed Multiplier
			movementSpeedFactor = originalSpeedFactor * 2;
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
			movementSpeedFactor = originalSpeedFactor;
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
	if (redBall.collision(currentState.pos[0], currentState.pos[1], currentState.pos[2])) {
		currentState.pos[0] = 300;
		currentState.pos[1] = 0;
		currentState.pos[2] = 100;
		camXRot = 0;
		camYRot = 180;
		camZRot = 0;
		deaths++;
		if (hits > highScore)
			highScore = hits;
		hits = 0;
		std::cout << "You Have Died" << std::endl;
		showInfo();
	}

	for (int i = 0; i < bullets.size(); i++) {
		Bullet &b = *bullets[i];
		if (b.collision(redBall.getXPos(), redBall.getYPos(), redBall.getZPos(), redBall.getRadius())) {
			GLfloat r = rand() % 8 + 1;
			GLfloat g = rand() % 16 + 1;
			GLfloat b = rand() % 32 + 1;
			redBall.colorSphere(r / 8, g / 16, b / 32);
			bullets.erase(bullets.begin() + i);
			hits++;

			std::cout << "You Hit the Ball!" << std::endl;
			showInfo();
		}

	}


	glRotatef(camXRot, 1.0f, 0.0f, 0.0f);        // Rotate our camera on the x-axis (looking up and down)
	glRotatef(camYRot, 0.0f, 1.0f, 0.0f);        // Rotate our camera on the  y-axis (looking left and right)
	glTranslatef(-currentState.pos[0], -currentState.pos[1], -currentState.pos[2]);    // Translate the modelviewm matrix to the position of our camera
	
	// Draw Cylinder
	glPushMatrix();
	glTranslatef(300.0, -50.0, 500.0);
	drawCylinder(100, 200);
	glPopMatrix();
	
	// Draw the Gun
	glPushMatrix();
	glRotatef(-camXRot, 1.0f, 0.0f, 0.0f);
	glRotatef(-camYRot, 0.0f, 1.0f, 0.0f);
	glTranslatef(currentState.pos[0], currentState.pos[1] - 5, currentState.pos[2] + 5);
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
	std::srand(time(NULL));

	currentState.pos[0] = 300.0f;
	currentState.pos[1] = 0.0f;
	currentState.pos[2] = 50.0f;

	currentState.v[0] = 0.0f;
	currentState.v[1] = 0.0f;
	currentState.v[2] = 0.0f;


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
	double dt = 0.01;

	double currentTime = glfwGetTime();
	double accumulator = 0.0;

	std::cout << "Game Loop Initialized" << std::endl;
	while (!glfwWindowShouldClose(window))
	{

		double newTime = glfwGetTime();
		double frameTime = newTime - currentTime;
		currentTime = newTime;
		accumulator += frameTime;
		/* Comput all the inputs */

		while (accumulator >= dt) {
			//Integrate
			for (int i = 0; i < bullets.size(); i++) {
				Bullet &b = *bullets[i];
				if (b.inBoundCheck(t, dt) == false)
					bullets.erase(bullets.begin() + i);
			}

			redBall.inBoundCheck();
			redBall.integrate(t, dt);

			inboundsCheck();
			calculateCameraMovement();
			integrate(t, dt);

			accumulator -= dt;
			t += dt;
		}

		drawScene(window);


		xAllowed = true;
		zAllowed = true;

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();


	}

	glfwDestroyWindow(window);

	glfwTerminate();
	exit(EXIT_FAILURE);

}