
#include "Bullet.h"
#include <cmath>


Bullet::Bullet(GLfloat x, GLfloat y, GLfloat z, GLfloat xrot, GLfloat yrot, GLfloat zrot) {

	currentState.pos[0] = x;
	currentState.pos[1] = y + 5;
	currentState.pos[2] = z;

	red = 0.3f;
	blue = 0.3f;
	green = 0.8f;

	radius = 3.0f;

	//Speed of bullet
	speed = 600.0f;
	float xv = sin(toRads(yrot)) * cos(toRads(xrot)) * speed;
	float yv = sin(toRads(xrot)) * -1.0f * speed;
	float zv = cos(toRads(yrot)) * -1.0f * cos(toRads(xrot)) * speed;
	
	if (xv > speed)
		xv = speed;

	if (xv < -speed)
		xv = -speed;

	if (yv > speed)
		yv = speed;

	if (yv < -speed)
		yv = -speed;

	if (zv > speed)
		zv = speed;

	if (zv < -speed)
		zv = -speed;

	currentState.v[0] = xv;
	currentState.v[1] = yv;
	currentState.v[2] = zv;
}


Bullet::~Bullet() {

}


void Bullet::colorBullet(GLfloat r, GLfloat g, GLfloat b) {
	red = r;
	green = g;
	blue = b;
}


bool Bullet::inBoundCheck(float t, float dt) {
	//Section 1 check
	if (currentState.pos[2] <= 200.0f) {
		if (currentState.pos[0] - radius < 200.0f)
			return false;

		else if (currentState.pos[0] + radius > 400.0f)
			return false;

		if (currentState.pos[2] - radius < 0.0f)
			return false;

	}
	//Section 2 Check
	if (currentState.pos[2] > 200.0f && currentState.pos[2] <= 800.0f) {
		if (currentState.pos[0] - radius < 200.0f) {
			//Check for Section 2, Bottom Left
			if (currentState.pos[2] - radius < 200.0f)
				return false;

			//Check for Section 2, Left Wall
			if (currentState.pos[0] - radius < 0.0f)
				return false;
		}
		if (currentState.pos[0] + radius > 400.0f) {
			//Check for Section 2, Bottom Right
			if (currentState.pos[2] - radius < 200.0f)
				return false;

			//Check for Section 2, Right Wall
			if (currentState.pos[0] + radius > 600.0f)
				return false;
		}
		//Section 2 Check Cylinder in Middle
	}
	//Section 2 Check Back wall.
	if (currentState.pos[2] + radius > 800 /*&& (currentState.pos[0] + radius > 350 || currentState.pos[0] - radius < 250) This section checks back hole.*/)
		return false;

	//Y-Axis Check
	if (currentState.pos[1] + radius > 150)
		return false;
	else if (currentState.pos[1] - radius < -50)
		return false;

	integrate(t, dt);

	return true;
}


void Bullet::drawBullet() {

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


bool Bullet::collision(GLfloat camXPos, GLfloat camYPos, GLfloat camZPos, GLfloat r) {
	if (
		((camXPos + r) >= (int)currentState.pos[0] - (int)radius) && ((camXPos - r) <= ((int)currentState.pos[0] + (int)radius)) &&
		((camYPos + r) >= (int)currentState.pos[1] - (int)radius) && ((camYPos - r) <= ((int)currentState.pos[1] + (int)radius)) &&
		((camZPos + r) >= (int)currentState.pos[2] - (int)radius) && ((camZPos - r) <= ((int)currentState.pos[2] + (int)radius))
		)
		return true;
	return false;

}
/*
If for some reason, you need the x, y, z values outside this class
*/
GLfloat Bullet::getXPos() {
	return currentState.pos[0];
}

GLfloat Bullet::getYPos() {
	return currentState.pos[1];
}

GLfloat Bullet::getZPos() {
	return currentState.pos[2];
}


/*
The Game Physics behind the Bullet
*/
Bullet::Derivative Bullet::evaluate(const State &initial, float t) {
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
Bullet::Derivative Bullet::evaluate(const State &initial, float t, float dt, const Derivative &d) {
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

float Bullet::acceleration(const State &state, int x, float t) {
	if (x >= 3)
		return 0;
	const float k = 0;
	const float b = 0;
	return -k * state.pos[x] - b * state.v[x];
}
void Bullet::integrate(float t, float dt) {

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