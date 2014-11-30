#pragma once

//#include "pch.h"
#include <d3d11_2.h>
#include <DirectXMath.h>
#include "Entity.h"
#include "PhysicalStructs.h"

class Entity;

using namespace std;
using namespace DirectX;
using namespace Physics;

class PhysicsEngine {
	//RNK4 method with help from http://gafferongames.com/game-physics/integration-basics/
	friend Entity;

public:
	PhysicsEngine();
	~PhysicsEngine();

	/*
		Colliding: this is a facade pattern method that will determine colliding objects and the appropriate collision check
			input - first entity decides the style of detection, second entity is the subject
			output - collision boolean
	*/
	static bool Colliding(Entity&, Entity&);
	/*
		UpdateState  :  updates the physical state by RNK4
			input - Entity to be updated
					double time factor
					the type of motion updated
	*/
	static void UpdateState(Entity* entity, double dt, enum AccelType type);
	/*
		PredictState  :  predicts the next appropriate state to resolve collision
			input - Entity to be updated
					double time factor
					the type of motion updated
			output - resolution state
	*/
	static void PredictState(PhysicalState*, double, enum AccelType);


private:
	static void Integrate(PhysicalState& state, double dt, enum AccelType type);
	static Physics::Derivative Evaluate(Physics::PhysicalState* state, double dt, Physics::Derivative* deriv, enum AccelType type);
	static Physics::DOUBLESTATE* AccelerationLinear(PhysicalState* state, double dt);

	bool static SphereCollision(Entity&, Entity&);
	/*
	RayCastCollision : Predict if a projectile hits an enemy
	*/
	static bool RayCastCollision(Entity&, Entity&);
};