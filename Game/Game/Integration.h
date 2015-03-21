#ifndef integration
#define integration

class Integration
{
public:
	struct State;

	struct Derivative;

	void integrate(State &state, float t, float dt);

private:

	float acceleration(const State &state, float t);
	Derivative evaluate(const State &initial, float t);
	Derivative evaluate(const State &initial, float t, float dt, const Derivative &d);
};

#endif
