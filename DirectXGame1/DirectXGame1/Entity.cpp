#include "pch.h"
#include "Entity.h"

Entity::Entity() {}

Entity::Entity(VSD3DStarter::Graphics& m_graphics)
{
	// Initialize the Entity's transform
	state.pos = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	state.dstate.SetPos(state.pos.x, state.pos.y, state.pos.z);
	state.ori = XMQuaternionRotationRollPitchYaw(0, 0, 0);

	// Initialize animation clip name
	m_animationClipName = L"placeholder";

	m_allowRender = true;

	radius = 0;
	health = 1;
	lives = 1;
}


Entity::Entity(std::vector<VSD3DStarter::Mesh*>&	m_model) {
	this->m_model = m_model;
}

Entity::~Entity()
{
	for (Mesh* m : m_model)
	{
		if (m != nullptr)
		{
			AnimationState* animState = (AnimationState*)m->Tag;
			if (animState != nullptr)
			{
				m->Tag = nullptr;
				delete animState;
			}
		}
		delete m;
	}
	m_model.clear();
}

std::wstring Entity::GetModelFilename()
{
	return m_modelFilename;
}

bool Entity::GetRenderSetting()
{
	return m_allowRender;
}

XMVECTOR Entity::GetPos()			{ return XMLoadFloat4(&(state.pos)); }
XMVECTOR Entity::GetOri()			{ return state.ori; }
XMVECTOR Entity::GetForward()		{ return XMLoadFloat4(&(state.forward)); }
XMVECTOR Entity::GetLeft()			{ return XMLoadFloat4(&(state.left)); }
XMVECTOR Entity::GetUp()			{ return XMLoadFloat4(&(state.up)); }
PhysicalState& Entity::GetState()	{ return state; }
PhysicalState& Entity::GetFutureState()	{ return newstate; }
XMFLOAT4 Entity::GetFUp()			{ return state.up; }
XMFLOAT4 Entity::GetFLeft()			{ return state.left; }
XMFLOAT4 Entity::GetFForward()		{ return state.forward; }
XMFLOAT4 Entity::GetFPos()			{ return state.pos; }
int Entity::GetType() { return 0; }//Base entity
std::vector<VSD3DStarter::Mesh*>& Entity::GetModel() { return m_model; }

void Entity::SetState(PhysicalState const &newState) {
	state = newState;
}

void Entity::ReversePosition() {
	state = newstate;
}

void Entity::InitializeModelFromFile(VSD3DStarter::Graphics& m_graphics)
{
	auto loadMeshTask = Mesh::LoadFromFileAsync(
		m_graphics,
		GetModelFilename(),
		L"",
		L"",
		GetModel())
		.then([this]()
	{
		// Initialize animated models.
		for (Mesh* m : GetModel())
		{
			if (m->BoneInfoCollection().empty() == false)
			{
				auto animState = new AnimationState();
				animState->m_boneWorldTransforms.resize(m->BoneInfoCollection().size());

				m->Tag = animState;
			}
		}
	}).wait();
}

void Entity::SetRenderSetting(bool s)
{
	m_allowRender = s;
}

void Entity::SetPosition(float x, float y, float z)
{
	state.pos = XMFLOAT4(x, y, z, 0);
	state.dstate.SetPos(x, y, z);
}

void Entity::Damage(int damage) {
	health -= damage;

	if (health <= 0) {
		lives--;
	}

	if (lives <= 0) {
		SetRenderSetting(false);
	}
}

void Entity::Rotate(float roll, float pitch, float yaw)
{
	// apply transform-relative orientation changes
	//grab initial frame of reference
	state.forward = base_forward;
	state.up = base_up;
	state.left = base_left;

	//rotate the frame by the quaternion
	XMStoreFloat4(&(state.forward),
		XMVector4Transform(XMLoadFloat4(&(state.forward)), XMMatrixRotationQuaternion(state.ori)));
	XMStoreFloat4(&(state.up),
		XMVector4Transform(XMLoadFloat4(&(state.up)), XMMatrixRotationQuaternion(state.ori)));
	XMStoreFloat4(&(state.left),
		XMVector3Cross(XMLoadFloat4(&(state.forward)), XMLoadFloat4(&(state.up))));


	//Roll:
	state.ori = XMQuaternionMultiply(state.ori, XMQuaternionRotationAxis(XMLoadFloat4(&(state.forward)), roll*0.01f));
	//Pitch:
	state.ori = XMQuaternionMultiply(state.ori, XMQuaternionRotationAxis(XMLoadFloat4(&(state.left)), pitch*0.01f));
	//Yaw:
	state.ori = XMQuaternionMultiply(state.ori, XMQuaternionRotationAxis(XMLoadFloat4(&(state.up)), yaw*0.01f));
}

void Entity::LoadIntoEngine(PhysicsEngine* phys) {
	physEng = phys;
}



void Entity::Render(VSD3DStarter::Graphics& m_graphics)
{
	if (!m_allowRender)
		return;

	auto temp = GetModel();

	for (UINT i = 0; i < temp.size(); i++)
	{
		temp[i]->Render(m_graphics, XMLoadFloat4x4(&m_transform));
	}
}

//update states
void Entity::Update()
{

	state.pos.x = (float)state.dstate.x;
	state.pos.y = (float)state.dstate.y;
	state.pos.z = (float)state.dstate.z;

	DirectX::XMStoreFloat4x4(&m_transform,
		XMMatrixRotationQuaternion(state.ori) * XMMatrixTranslation(state.pos.x, state.pos.y, state.pos.z));
}

void Entity::Update(double timeDelta) {
	PhysicsEngine::UpdateState(this, timeDelta, LINEAR);
	//PhysicsEngine::UpdateState(this, timeDelta, LINEAR);

	Update();
}


void Entity::UpdateEntityTransform()
{
	//create transform
	DirectX::XMStoreFloat4x4(&m_transform,
		XMMatrixRotationQuaternion(state.ori) * XMMatrixTranslation(state.pos.x, state.pos.y, state.pos.z));

	int test = 3;
}