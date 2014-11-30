#pragma once

#include <DirectXMath.h>

using namespace DirectX;

namespace Physics {
	typedef struct DOUBLESTATE {
		double x = 0;
		double y = 0;
		double z = 0;

		double vx = 0;
		double vy = 0;
		double vz = 0;

		double ax = 0;
		double ay = 0;
		double az = 0;

		void SetPos(float dx, float dy, float dz) {
			x = dx;
			y = dy;
			z = dz;
		}
		void SetVel(float dx, float dy, float dz) {
			vx = dx;
			vy = dy;
			vz = dz;
		}
		void SetAccel(float dx, float dy, float dz) {
			ax = dx;
			ay = dy;
			az = dz;
		}
	} DOUBLESTATE;
	//store the position and orientation of the entity
	typedef struct PhysicalState {
		//holds the more accurate information
		DOUBLESTATE dstate;

		//hold the useable information for buffers
		XMFLOAT4 pos;
		XMFLOAT4 vel;
		XMVECTOR ori;
		XMFLOAT4 forward;
		XMFLOAT4 up;
		XMFLOAT4 left;

		double roll = 0;
		double pitch = 0;
		double yaw = 0;

		PhysicalState() {

			//Initialize the Entity's physical state
			this->pos = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
			this->vel = XMFLOAT4(0.f, 0.f, 0.f, 1.f);
			this->ori = XMQuaternionRotationRollPitchYaw(0, 0, 0);
			this->left = XMFLOAT4(1.f, 0.f, 0.f, 0.f);
			this->up = XMFLOAT4(0.f, 1.f, 0.f, 0.f);
			this->forward = XMFLOAT4(0.f, 0.f, 1.0f, 1.f);
		}

		PhysicalState& operator= (const PhysicalState& other) {
			this->pos = other.pos;
			this->vel = other.vel;
			this->ori = other.ori;
			this->forward = other.forward;
			this->up = other.up;
			this->left = other.left;

			this->roll = other.roll;
			this->pitch = other.pitch;
			this->yaw = other.yaw;

			this->dstate = other.dstate;

			return *this;
		}
	} PhysicalState;
	typedef struct Derivative {
		double dx;
		double dy;
		double dz;

		double dvx;
		double dvy;
		double dvz;

		Derivative() {
			dx = 0;
			dy = 0;
			dz = 0;
			dvx = 0;
			dvy = 0;
			dvz = 0;
		}
	} Derivative;

	enum AccelType {
		LINEAR
	};

	static const XMFLOAT4 base_forward = XMFLOAT4(0.f, 0.f, 1.0f, 0.0f);
	static const XMFLOAT4 base_up = XMFLOAT4(0.f, 1.f, 0.0f, 0.0f);
	static const XMFLOAT4 base_left = XMFLOAT4(1.f, 0.f, 0.0f, 0.0f);
	static const XMFLOAT4X4 ident = XMFLOAT4X4( 1.f, 0.f, 0.f, 0.f,
												0.f, 1.f, 0.f, 0.f,
												0.f, 0.f, 1.f, 0.f,
												0.f, 0.f, 0.f, 1.f );

	static const float plasmaDelay = 0.09f;
	static const float homingDelay = 9.0f;
	static const float laserDelay = 15.0f;
}