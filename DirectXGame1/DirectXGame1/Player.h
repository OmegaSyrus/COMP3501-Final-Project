
#pragma once

#include "Entity.h"
#include "Plasma.h"
#include "BoundingSphere.h"
#include "CollisionManager.h"

#include "VSD3DStarter.h"
#include "Animation\Animation.h"

using namespace VSD3DStarter;
using namespace DirectX;

// Class that represents a 3D entity
class Player : public Entity
{
	std::wstring m_modelFilename = L"player-ship.cmo";

	const float MOUSE_IN_CIRCLE_MOVEMENT_FACTOR = 0.5f;
	const float MOUSE_OUT_CIRCLE_MOVEMENT_FACTOR = 1.0F;

	const float BOUNDING_SPHERE_RADIUS = 1.0f;

	const float PLAYER_SPEED = 13.0f;

	public:
		Player(VSD3DStarter::Graphics& m_graphics);
		~Player();

		std::vector<Entity*>& GetWeapons();
		BoundingSphere* GetBoundingSphere();
		int GetType();

		void Update();
		void Update(double timeDelta);
		void UpdateBoundingSphere();

		void MoveWithMouse(float, float);
		XMFLOAT4& GetMouseRay();
		void Rotate(float, float, float);

		void SetViewMatrix(DirectX::XMVECTOR& vPosition, DirectX::XMVECTOR& vLook, DirectX::XMVECTOR& vUp);
		void Damage(int);

		void FireWeapon();
		void ActivateWeapon(double time);
		float GetDelayLength();
		bool CheckBoundingSphereCollision(BoundingSphere*);


		// Operator overload to align 16 on heap
		void* operator new (size_t size)
		{
			void * p = _aligned_malloc(size, 16);
			return p;
		}
		void operator delete (void *p)
		{
			Player* pc = static_cast<Player*>(p);
			_aligned_free(p);
		}

	private:
		std::wstring GetModelFilename();
		void InitializePlasmas(XMFLOAT4&, XMVECTOR&);
	
	private:
		std::vector<Plasma*> plasma;
		std::vector<Entity*> weapons;
		BoundingSphere* m_BoundingSphere;
		XMFLOAT4X4 m_view;
		XMFLOAT4 mouseVector;
		XMFLOAT4 mouseForward;


		double weaponDelay;
		int currentAmmo;

		bool fired;


		enum WeaponType { PLASMA, HOMING_MISSILE, LASER };
		WeaponType activeWeapon;
		
};