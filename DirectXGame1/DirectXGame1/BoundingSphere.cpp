#include "pch.h"
#include "BoundingSphere.h"

BoundingSphere::BoundingSphere(float r, XMVECTOR& pos)
{
	m_radius			= r;
	m_spherePosition	= pos;
}

float BoundingSphere::GetRadius()
{
	return m_radius;
}

XMVECTOR& BoundingSphere::GetSpherePosition()
{
	return m_spherePosition;
}

void BoundingSphere::UpdateSphere(XMVECTOR& pos)
{
	m_spherePosition = pos;
}