#ifndef sphere
#define sphere

#include <GLFW\glfw3.h>


class Sphere 
{


public:
	Sphere();
	Sphere(GLfloat x, GLfloat y, GLfloat z, GLfloat radius);
	~Sphere();
	void colorSphere(GLfloat r, GLfloat g, GLfloat b);
	void inBoundCheck();
	void drawSphere();

	struct State;

	struct Derivative;

	void integrate(float t, float dt);
	
	bool collision(GLfloat camXPos, GLfloat camYPos, GLfloat camZPos);


	GLfloat getXPos();

	GLfloat getYPos();

	GLfloat getZPos();

	GLfloat getRadius() {
		return radius;
	}


private:

	const float TO_RADS = 3.141592654f / 180.0f;

	float toRads(const float &degrees)
	{
		return degrees * TO_RADS;
	}


	GLfloat red;
	GLfloat blue;
	GLfloat green;

	GLfloat radius;

	float acceleration(const State &state, int x, float t);
	Derivative evaluate(const State &initial, float t);
	Derivative evaluate(const State &initial, float t, float dt, const Derivative &d);

};

#endif