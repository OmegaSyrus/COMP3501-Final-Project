#include "pch.h"
#include "Plasma.h"

//static member
static std::vector<VSD3DStarter::Mesh*>	m_plasmaModel;

Plasma::Plasma(VSD3DStarter::Graphics& m_graphics)
{
	if (m_plasmaModel.empty())
		InitializeModelFromFile(m_graphics);

	m_allowFire = false;
	m_velocity	= LASER_VELOCITY;

	m_BoundingSphere = new BoundingSphere(BOUNDING_SPHERE_RADIUS, transform.pos);

	//set lifetime
	lifetime = (double)plasmaLifetime;
}

Plasma::Plasma(std::vector<VSD3DStarter::Mesh*>&	m_model) : Entity(m_model) {

}

/////////////////////////////////////////////////////////////////////////////////////
//GETTERS

std::wstring Plasma::GetModelFilename(){	return m_modelFilename;}
BoundingSphere* Plasma::GetBoundingSphere(){	return m_BoundingSphere;}
int Plasma::GetType() { return 2; }
std::vector<VSD3DStarter::Mesh*>& Plasma::GetModel() { return m_plasmaModel; }

void Plasma::Damage(int damage) {
	Entity::Damage(damage);

	//remove plasma shot from sight at 0 health
	if (health <= 0) {
		SetFireSetting(false);
	}
}

void Plasma::SetFireSetting(bool s) {	
	m_allowRender = s;
	if (s)
		lifetime = (double)plasmaLifetime;
	Update();
}
void Plasma::SetDirectionAndOrientation(XMFLOAT4& forward, XMVECTOR& ori)
{
	state.vel	= forward;
	//prepare background numbers
	state.dstate.SetVel(LASER_VELOCITY * state.vel.x,
		LASER_VELOCITY * state.vel.y,
		LASER_VELOCITY * state.vel.z);

	state.ori		= XMVECTOR(ori);
}

void Plasma::Update()
{
	state.pos.x = (float)state.dstate.x;
	state.pos.y = (float)state.dstate.y;
	state.pos.z = (float)state.dstate.z;

	DirectX::XMStoreFloat4x4(&m_transform,
		XMMatrixRotationQuaternion(state.ori) * XMMatrixTranslation(state.pos.x, state.pos.y, state.pos.z));

	UpdateBoundingSphere();
}

void Plasma::Update(double timeDelta) {
	lifetime -= timeDelta;
	if (m_allowRender) {
		if (lifetime > 0) {
			PhysicsEngine::UpdateState(this, timeDelta, LINEAR);

			Update();
		}
		else m_allowRender = false;
	}
}


void Plasma::UpdateBoundingSphere()
{
	m_BoundingSphere->UpdateSphere(transform.pos);
}