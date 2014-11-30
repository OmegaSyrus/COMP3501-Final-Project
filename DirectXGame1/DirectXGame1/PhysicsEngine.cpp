#include "pch.h"
#include "PhysicsEngine.h"


//PUBLIC
PhysicsEngine::PhysicsEngine() {
}
PhysicsEngine::~PhysicsEngine() {}

//////////////////////////////////////////////////
//COLLISION ALGORITHMS

bool PhysicsEngine::Colliding(Entity& first, Entity& second) {

	//player collision with
	if (first.GetType() == 1) {
		//an asteroid
		if (second.GetType() == 3) {
			return SphereCollision(first, second);
		}
	}
	//laser collision
	if (first.GetType() == 2) {
		return RayCastCollision(first, second);
	}
	return false;
}
bool PhysicsEngine::RayCastCollision(Entity& first, Entity& second) {
	//get the right equation
	DOUBLESTATE diff;
	diff.x = first.state.dstate.x - second.state.dstate.x;
	diff.y = first.state.dstate.y - second.state.dstate.y;
	diff.z = first.state.dstate.z - second.state.dstate.z;

	double dot =	diff.x * first.state.forward.x +
					diff.y * first.state.forward.y +
					diff.z * first.state.forward.z;

	double magnitude =	diff.x * diff.x +
						diff.y * diff.y +
						diff.z * diff.z;

	//calculate discriminant
	double disc = (dot * dot) - (magnitude) + second.radius * second.radius;

	//was there an intersection?
	if (disc >= 0) {
		//calculate distance from missile origin
		return true;
		//return SphereCollision(first, second);
	}
	return false;
}
bool PhysicsEngine::SphereCollision(Entity& player, Entity& entity) {
	XMFLOAT3 temp;
	float dist;
	for (int i = -1; i < 2; i+=2) {
		for (int j = -1; j < 2; j+=2) {
			temp = XMFLOAT3(player.state.pos.x + i * player.state.forward.x + j * player.state.left.x,
				player.state.pos.x + i * player.state.forward.x + j * player.state.left.x,
				player.state.pos.x + i * player.state.forward.x + j * player.state.left.x);

			dist =	(temp.x - entity.state.pos.x) * (temp.x - entity.state.pos.x) + 
					(temp.y - entity.state.pos.y) * (temp.y - entity.state.pos.y) + 
					(temp.z - entity.state.pos.z) * (temp.z - entity.state.pos.z);

			if (dist <= (player.radius + entity.radius)*(player.radius + entity.radius))
				return true;
		}
	}

	return false;
}
void PhysicsEngine::UpdateState(Entity* entity, double dt, enum AccelType type) {
	//physType = type;

	//use PredictState to get the new state, and apply the result to the entity
	Integrate(entity->GetState(), dt, type);
}

void PhysicsEngine::PredictState(PhysicalState* state, double dt, enum AccelType type) {
	//physType = type;
	//return NULL;
	//return Integrate(&(entity->GetState()), dt, type);
}

//PRIVATE
void PhysicsEngine::Integrate(PhysicalState& state, double dt, enum AccelType type) {
	Derivative a, b, c, d;

	a = Evaluate(&state, 0, &(Derivative()), type);
	b = Evaluate(&state, dt*0.5, &a, type);
	c = Evaluate(&state, dt*0.5, &b, type);
	d = Evaluate(&state, dt, &c, type);

	//find approx position increment in each dimension
	double dxdt = 1.0f / 6.0f *
		(a.dx + 2.0f*(b.dx + c.dx) + d.dx);
	double dydt = 1.0f / 6.0f *
		(a.dy + 2.0f*(b.dy + c.dy) + d.dy);
	double dzdt = 1.0f / 6.0f *
		(a.dz + 2.0f*(b.dz + c.dz) + d.dz);

	//find approx velocity increment in each dimension
	double dvxdt = 1.0f / 6.0f *
		(a.dvx + 2.0f*(b.dvx + c.dvx) + d.dvx);
	double dvydt = 1.0f / 6.0f *
		(a.dvy + 2.0f*(b.dvy + c.dvy) + d.dvy);
	double dvzdt = 1.0f / 6.0f *
		(a.dvz + 2.0f*(b.dvz + c.dvz) + d.dvz);
	
	//determine predicted position
	state.dstate.x = state.dstate.x + dxdt * dt;
	state.dstate.y = state.dstate.y + dydt * dt;
	state.dstate.z = state.dstate.z + dzdt * dt;
	//determine predicted velocity
	state.dstate.vx = state.dstate.vx + dvxdt * dt;
	state.dstate.vy = state.dstate.vy + dvydt * dt;
	state.dstate.vz = state.dstate.vz + dvzdt * dt;
	//grab the acceleration state
	state.dstate.ax = state.dstate.ax;
	state.dstate.ay = state.dstate.ay;
	state.dstate.az = state.dstate.az;
}
Physics::Derivative PhysicsEngine::Evaluate(Physics::PhysicalState* state, double dt, Physics::Derivative* deriv, enum AccelType type){
	PhysicalState s;
	//find the position update in this evaluation
	s.dstate.x = state->dstate.x + deriv->dx*dt;
	s.dstate.y = state->dstate.y + deriv->dy*dt;
	s.dstate.z = state->dstate.z + deriv->dz*dt;

	//find the velocity update in this evaluation
	s.dstate.vx = state->dstate.vx + deriv->dvx*dt;
	s.dstate.vy = state->dstate.vy + deriv->dvy*dt;
	s.dstate.vz = state->dstate.vz + deriv->dvz*dt;

	//prepare derivative estimation at dt
	Derivative output;
	output.dx = s.dstate.vx;
	output.dy = s.dstate.vy;
	output.dz = s.dstate.vz;

	switch (type) {
	case LINEAR: 
		DOUBLESTATE dst = *(AccelerationLinear(state, dt));
		output.dvx = dst.ax;
		output.dvy = dst.ay;
		output.dvz = dst.az;
	}

	return output;
}
Physics::DOUBLESTATE* PhysicsEngine::AccelerationLinear(PhysicalState* state, double dt){
	DOUBLESTATE* retdst = new DOUBLESTATE();

	//get the acceleration from the state
	//ADD CODE HERE LIKE BOOST
	retdst->ax = state->dstate.ax;
	retdst->ay = state->dstate.ay;
	retdst->az = state->dstate.az;

	return retdst;
}