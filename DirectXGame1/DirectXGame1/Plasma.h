
#pragma once

#include "Entity.h"
#include "BoundingSphere.h"
#include "VSD3DStarter.h"
#include "Animation\Animation.h"

using namespace VSD3DStarter;
using namespace DirectX;

// Class that represents a 3D entity
class Plasma : public Entity
{
	std::wstring m_modelFilename = L"laser.cmo";

	const float BOUNDING_SPHERE_RADIUS = 2.0f;
	const float LASER_VELOCITY = 1000.f;
	const float plasmaLifetime = .09f;

public:

		Plasma(VSD3DStarter::Graphics& m_graphics);
		Plasma(std::vector<VSD3DStarter::Mesh*>&	m_model);

		void Update();
		void Update(double timeDelta);
		void SetFireSetting(bool);
		void Damage(int);
		void SetDirectionAndOrientation(XMFLOAT4&, XMVECTOR&);
		BoundingSphere* GetBoundingSphere();
		void UpdateBoundingSphere();
		int GetType();		//type = 2
		std::vector<VSD3DStarter::Mesh*>& GetModel();

		// Operator overload to align 16 on heap
		void* operator new (size_t size)
		{
			void * p = _aligned_malloc(size, 16);
			return p;
		}
		void operator delete (void *p)
		{
			Plasma* pc = static_cast<Plasma*>(p);
			_aligned_free(p);
		}

	private:
		std::wstring GetModelFilename();

	private:
		bool			m_allowFire;
		float			m_velocity;
		BoundingSphere* m_BoundingSphere;
		double lifetime;

	public:
		float initialOffsetX, initialOffsetY, initialOffsetZ;
};