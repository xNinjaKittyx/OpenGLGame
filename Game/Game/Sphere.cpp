/*
Class created for creating and customizing Spheres
*/

#include <cmath>
#include "Sphere.h"


struct Sphere::State {

	float pos[3];

	float v[3];

} currentState;

struct Sphere::Derivative {
	float dx[3]; // dx/dt = velocity

	float dv[3]; // dv/dt = acceleration
};


Sphere::Sphere() {

	currentState.pos[0] = 200.0f;
	currentState.pos[1] = 50.0f;
	currentState.pos[2] = 400.0f;

	red = 0.2f;
	blue = 0.5f;
	green = 0.7f;

	radius = 20.0f;

	currentState.v[0] = 10.0f;
	currentState.v[1] = 5.0f;
	currentState.v[2] = 7.5f;
}

Sphere::Sphere(GLfloat x, GLfloat y, GLfloat z, GLfloat r) {
	currentState.pos[0] = x;
	currentState.pos[1] = y;
	currentState.pos[2] = z;

	red = 0.2f;
	blue = 0.5f;
	green = 0.7f;

	radius = r;

	currentState.v[0] = 10.0f;
	currentState.v[1] = 5.0f;
	currentState.v[2] = 7.5f;
}

Sphere::~Sphere() {
}

/*
		Customizing the Sphere's contents, Drawing it, and its boundaries
*/

void Sphere::colorSphere(GLfloat r, GLfloat g, GLfloat b) {
	red = r;
	green = g;
	blue = b;
}


void Sphere::inBoundCheck() {

	//Section 1 check
	if (currentState.pos[2] <= 200.0f) {
		if (currentState.pos[0] - radius < 200.0f)
			currentState.v[0] = (-currentState.v[0]);

		else if (currentState.pos[0] + radius > 400.0f)
			currentState.v[0] = (-currentState.v[0]);

		if (currentState.pos[2] - radius < 0.0f)
			currentState.v[2] = (-currentState.v[2]);

	}
	//Section 2 Check
	if (currentState.pos[2] > 200.0f && currentState.pos[2] <= 800.0f) {
		if (currentState.pos[0] - radius < 200.0f) {
			//Check for Section 2, Bottom Left
			if (currentState.pos[2] - radius < 200.0f)
				currentState.v[2] = (-currentState.v[2]);

			//Check for Section 2, Left Wall
			if (currentState.pos[0] - radius < 0.0f)
				currentState.v[0] = (-currentState.v[0]);
		}
		if (currentState.pos[0] + radius > 400.0f) {
			//Check for Section 2, Bottom Right
			if (currentState.pos[2] - radius < 200.0f)
				currentState.v[2] = (-currentState.v[2]);

			//Check for Section 2, Right Wall
			if (currentState.pos[0] + radius > 600.0f)
				currentState.v[0] = (-currentState.v[0]);
		}
		//Section 2 Check Cylinder in Middle
	}
	//Section 2 Check Back wall.
	if (currentState.pos[2] + radius > 800 /*&& (currentState.pos[0] + radius > 350 || currentState.pos[0] - radius < 250) This section checks back hole.*/)
		currentState.v[2] = (-currentState.v[2]);

	//Y-Axis Check
	if (currentState.pos[1] + radius > 150)
		currentState.v[1] = (-currentState.v[1]);
	else if (currentState.pos[1] - radius < -50)
		currentState.v[1] = (-currentState.v[1]);

}


void Sphere::drawSphere() {

	double x, y, z, thet, phi;


	glColor3f(red, blue, green);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//North pole area
	glBegin(GL_TRIANGLE_FAN);
	x = currentState.pos[0];
	y = currentState.pos[1];
	z = currentState.pos[2] + radius;
	glVertex3d(x, y, z);	//North pole

	//Triangle fans around the north pole
	z = radius * sin(toRads(80.0)) + currentState.pos[2];
	for (thet = -180.0; thet <= 180.0; thet += 10.0)
	{
		x = radius * sin(toRads(thet))*cos(toRads(80.0)) + currentState.pos[0];
		y = radius * cos(toRads(thet))*cos(toRads(80.0)) + currentState.pos[1];
		glVertex3d(x, y, z);
	}
	glEnd();


	//South pole area
	glBegin(GL_TRIANGLE_FAN);
	x = currentState.pos[0];
	y = currentState.pos[1];
	z = radius + currentState.pos[2];
	glVertex3d(x, y, z);	//South pole

	//Triangle fans around the south pole
	z = radius * -sin(toRads(80.0)) + currentState.pos[2];
	for (thet = -180.0; thet <= 180.0; thet += 10.0)
	{
		x = radius * sin(toRads(thet))*cos(toRads(80.0)) + currentState.pos[0];
		y = radius * cos(toRads(thet))*cos(toRads(80.0)) + currentState.pos[1];
		glVertex3d(x, y, z);
	}
	glEnd();


	//Other areas
	for (phi = -80.0; phi <= 80.0; phi += 5.0)
	{
		glBegin(GL_QUAD_STRIP);
		for (thet = -180.0; thet <= 180.0; thet += 10.0)
		{
			x = radius * sin(toRads(thet))*cos(toRads(phi)) + currentState.pos[0];
			y = radius * cos(toRads(thet))*cos(toRads(phi)) + currentState.pos[1];
			z = radius * sin(toRads(phi)) + currentState.pos[2];
			glVertex3d(x, y, z);
			x = radius * sin(toRads(thet))*cos(toRads((phi + 20.0))) + currentState.pos[0];
			y = radius * cos(toRads(thet))*cos(toRads((phi + 20.0))) + currentState.pos[1];
			z = radius * sin(toRads((phi + 20.0))) + currentState.pos[2];
			glVertex3d(x, y, z);
		}
		glEnd();
	}
}

bool Sphere::collision(GLfloat camXPos, GLfloat camYPos, GLfloat camZPos) {
	if (
		(camXPos >= (int)currentState.pos[0] - (int)radius) && (camXPos <= ((int)currentState.pos[0] + (int)radius)) &&
		(camYPos >= (int)currentState.pos[1] - (int)radius) && (camYPos <= ((int)currentState.pos[1] + (int)radius)) &&
		(camZPos >= (int)currentState.pos[2] - (int)radius) && (camZPos <= ((int)currentState.pos[2] + (int)radius))
		)
		return true;
	return false;

}
/*
	If for some reason, you need the x, y, z values outside this class
*/
GLfloat Sphere::getXPos() {
	return currentState.pos[0];
}

GLfloat Sphere::getYPos() {
	return currentState.pos[1];
}

GLfloat Sphere::getZPos() {
	return currentState.pos[2];
}


/*
		The Game Physics behind the Sphere
*/
Sphere::Derivative Sphere::evaluate(const State &initial, float t) {
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
Sphere::Derivative Sphere::evaluate(const State &initial, float t, float dt, const Derivative &d) {
	State state;
	for (int i = 0; i < 3; i++) {
		state.pos[i] = initial.pos[i] + d.dx[i] * dt;
		state.v[i] = initial.v[i] + d.dv[i] * dt;
	}

	Derivative output;
	for (int i = 0; i < 3; i++) {
		output.dx[i] = state.v[i];
		output.dv[i] = acceleration(state, i, t+dt);
	}
	return output;
}

float Sphere::acceleration(const State &state, int x, float t) {
	if (x >= 3)
		return 0;
	const float k = 0;
	const float b = 0;
	return -k * state.pos[x] - b * state.v[x] ;
}
void Sphere::integrate(float t, float dt) {

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