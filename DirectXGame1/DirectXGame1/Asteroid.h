#pragma once

#include "Entity.h"
#include "BoundingSphere.h"
#include "VSD3DStarter.h"
#include "Animation\Animation.h"

using namespace VSD3DStarter;
using namespace DirectX;

// Class that represents a 3D entity
class Asteroid : public Entity
{
	std::wstring m_modelFilename = L"asteroid.cmo";

	const float BOUNDING_SPHERE_RADIUS = 3.0f;

	public:
		Asteroid(VSD3DStarter::Graphics& m_graphics);
		Asteroid(std::vector<VSD3DStarter::Mesh*>&	m_model);

		void Update();
		BoundingSphere* GetBoundingSphere();
		void UpdateBoundingSphere();
		bool CheckBoundingSphereCollision(BoundingSphere*);
		int GetType();	//type = 3
		std::vector<VSD3DStarter::Mesh*>& GetModel();

		// Operator overload to align 16 on heap
		void* operator new (size_t size)
		{
			void * p = _aligned_malloc(size, 16);
			return p;
		}
		void operator delete (void *p)
		{
			Asteroid* pc = static_cast<Asteroid*>(p);
			_aligned_free(p);
		}

	private:
		std::wstring GetModelFilename();
		BoundingSphere* m_BoundingSphere;
};