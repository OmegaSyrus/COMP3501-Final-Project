#include "pch.h"
#include "Asteroid.h"

//static members
static std::vector<VSD3DStarter::Mesh*>	m_asteroidModel;

Asteroid::Asteroid(VSD3DStarter::Graphics& m_graphics) : Entity(m_graphics)
{
	if (m_asteroidModel.empty())
		InitializeModelFromFile(m_graphics);

	radius = BOUNDING_SPHERE_RADIUS;

	m_BoundingSphere = new BoundingSphere(BOUNDING_SPHERE_RADIUS, transform.pos);
}

Asteroid::Asteroid(std::vector<VSD3DStarter::Mesh*>&	m_model) : Entity(m_model) {

}

std::vector<VSD3DStarter::Mesh*>& Asteroid::GetModel() { return m_asteroidModel; }
std::wstring Asteroid::GetModelFilename(){	return m_modelFilename;}
BoundingSphere* Asteroid::GetBoundingSphere(){	return m_BoundingSphere;}

int Asteroid::GetType() { return 3; }

void Asteroid::Update()
{
	Rotate(0, 2, 0);

	Entity::Update();

	UpdateBoundingSphere();
}

void Asteroid::UpdateBoundingSphere()
{
	m_BoundingSphere->UpdateSphere(transform.pos);
}

bool Asteroid::CheckBoundingSphereCollision(BoundingSphere* collidingSphere)
{
	//Declare local variables
	XMVECTOR world_1;
	XMVECTOR world_2;
	float objectsDistance = 0.0f;

	// Initialize both sphere's world space position vectors
	world_1 = collidingSphere->GetSpherePosition();
	world_2 = m_BoundingSphere->GetSpherePosition();

	// Get the distance between the two objects from the world space position vectors
	objectsDistance = XMVectorGetX(XMVector3Length(world_1 - world_2));

	// If the distance between the two objects is less than the sum of their bounding spheres...
	if (objectsDistance <= (collidingSphere->GetRadius() + m_BoundingSphere->GetRadius()))
		//Return true
		return true;

	// If the bounding spheres are not colliding, return false
	return false;
}