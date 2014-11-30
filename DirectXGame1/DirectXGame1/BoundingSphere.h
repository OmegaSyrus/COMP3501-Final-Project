
#pragma once

#include "pch.h"

using namespace DirectX;

// Class that represents a 3D entity
class BoundingSphere
{
	public:
		BoundingSphere(float, XMVECTOR&);

		float		GetRadius();
		XMVECTOR&	GetSpherePosition();

		void UpdateSphere(XMVECTOR&);

		// Operator overload to align 16 on heap
		void* operator new (size_t size)
		{
			void * p = _aligned_malloc(size, 16);
			return p;
		}
		void operator delete (void *p)
		{
			BoundingSphere* pc = static_cast<BoundingSphere*>(p);
			_aligned_free(p);
		}

	private:
		float		m_radius;
		XMVECTOR	m_spherePosition;
};