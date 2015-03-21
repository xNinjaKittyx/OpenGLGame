#ifndef bullet
#define bullet

#include <GLFW\glfw3.h>

class Bullet {
public:
	Bullet(GLfloat x, GLfloat y, GLfloat z, GLfloat xrot, GLfloat yrot, GLfloat zrot);
	~Bullet();
	void colorBullet(GLfloat red, GLfloat green, GLfloat blue);
	bool inBoundCheck(float t, float dt);
	void drawBullet();

	struct State {

		float pos[3];

		float v[3];

	};

	State currentState;

	struct Derivative {
		float dx[3]; // dx/dt = velocity

		float dv[3]; // dv/dt = acceleration
	};


	bool collision(GLfloat camXPos, GLfloat camYPos, GLfloat camZPos, GLfloat r);

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

	float speed;

	float gravity;

	void integrate(float t, float dt);
	float acceleration(const State &state, int x, float t);
	Derivative evaluate(const State &initial, float t);
	Derivative evaluate(const State &initial, float t, float dt, const Derivative &d);

};

#endif