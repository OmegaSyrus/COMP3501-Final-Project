#include "pch.h"
#include "Player.h"

Player::Player(VSD3DStarter::Graphics& m_graphics) : Entity(m_graphics)
{
	InitializeModelFromFile(m_graphics);

	radius = BOUNDING_SPHERE_RADIUS;
	m_BoundingSphere = new BoundingSphere(BOUNDING_SPHERE_RADIUS, transform.pos);

	//create the weaponry
	////////////////////////////
	//PLASMA WEAPONS
	Plasma* rightPlasma;
	Plasma* leftPlasma;


	for (int i = 0; i < 20; i++) {

		leftPlasma = new Plasma(m_graphics);
		rightPlasma = new Plasma(m_graphics);

		leftPlasma->SetRenderSetting(false);
		rightPlasma->SetRenderSetting(false);

		//add to plasma collection
		plasma.push_back(leftPlasma);
		plasma.push_back(rightPlasma);

		//add to weapon collection
		weapons.push_back(leftPlasma);
		weapons.push_back(rightPlasma);
	}

	state.vel = XMFLOAT4(0.f, 0.f, PLAYER_SPEED, 0.f);
	state.dstate.vx = state.vel.x;
	state.dstate.vy = state.vel.y;
	state.dstate.vz = state.vel.z;

	//initialize mouse vector
	mouseVector = XMFLOAT4(base_forward);
	mouseForward = base_forward;

	//set weapon defaults
	activeWeapon = PLASMA;
	weaponDelay = 0.0f;
	fired = false;

	health = 5;
	lives = 3;


	DirectX::XMStoreFloat4x4(&m_view, DirectX::XMMatrixIdentity());
}
Player::~Player() {
	Entity::~Entity();

	plasma.empty();

}

std::wstring Player::GetModelFilename(){ return m_modelFilename; }
BoundingSphere* Player::GetBoundingSphere(){ return m_BoundingSphere; }
std::vector<Entity*>& Player::GetWeapons() { return weapons; }
int Player::GetType() { return 1; }

//set the projection matrix for mouse calculations
void Player::SetViewMatrix(DirectX::XMVECTOR& vPosition, DirectX::XMVECTOR& vLook, DirectX::XMVECTOR& vUp) {
	DirectX::XMStoreFloat4x4(&m_view, DirectX::XMMatrixLookToRH(vPosition, vLook, vUp));
}

//initialize plasma weapon bullet fire
void Player::InitializePlasmas(XMFLOAT4& forward, XMVECTOR& ori)
{
	//prepare initial firing position
	plasma.at(currentAmmo)->SetPosition(state.pos.x + state.forward.x + 0.7f * state.left.x * (currentAmmo % 2 == 0 ? -1 : 1),
										state.pos.y + state.forward.y + 0.7f * state.left.y * (currentAmmo % 2 == 0 ? -1 : 1),
										state.pos.z + state.forward.z + 0.7f * state.left.z * (currentAmmo % 2 == 0 ? -1 : 1));

	//set frame of reference for plasma bolt
	plasma.at(currentAmmo)->SetDirectionAndOrientation(forward, ori);

	//reset timer and set it to be drawn
	plasma.at(currentAmmo)->SetFireSetting(true);

}

void Player::FireWeapon()
{
	fired = true;
}


//send weapon into space to hit enemies
void Player::ActivateWeapon(double time) {
	weaponDelay += time;
	if (fired && weaponDelay > GetDelayLength()) {
		if (activeWeapon == PLASMA) {
			//iterate through available plasma bolts
			currentAmmo = (currentAmmo + 1) % plasma.size();

			//prepare bullet
			XMFLOAT4 trajectory = XMFLOAT4(-mouseVector.x, -mouseVector.y,- mouseVector.z, 1.f);
			XMFLOAT2 ang;
			XMStoreFloat2(&ang, XMVector4AngleBetweenVectors(XMLoadFloat4(&trajectory), XMLoadFloat4(&state.forward)));

			XMVECTOR cross = XMVector3Cross(XMLoadFloat4(&trajectory), XMLoadFloat4(&state.forward));

			XMStoreFloat4(&trajectory, XMVector4Normalize(XMLoadFloat4(&mouseVector)));

			XMVECTOR plasmaOrient = XMQuaternionMultiply(state.ori, XMQuaternionRotationAxis(cross, -ang.x));

			InitializePlasmas(trajectory, plasmaOrient);
		}
		//reset weapon delay
		weaponDelay = 0;
		fired = false;
	}
	else fired = false;
}

//determine which length of time to wait
float Player::GetDelayLength() {
	switch (activeWeapon) {
	case LASER:
		break;
	case PLASMA:
		return plasmaDelay;
		break;
	case HOMING_MISSILE:
		break;
	default:
		return plasmaDelay;
		break;
	}

	return plasmaDelay;
}

void Player::Update()
{
	//MoveForward(PLAYER_SPEED);

	state.pos.x = (float)state.dstate.x;
	state.pos.y = (float)state.dstate.y;
	state.pos.z = (float)state.dstate.z;

	DirectX::XMStoreFloat4x4(&m_transform,
		XMMatrixRotationQuaternion(state.ori) * XMMatrixTranslation(state.pos.x, state.pos.y, state.pos.z));
}

void Player::Update(double timeDelta)
{
	PhysicsEngine::UpdateState(this, timeDelta, LINEAR);

	Update();

	ActivateWeapon(timeDelta);
	UpdateBoundingSphere();
}

void Player::Damage(int damage) {
	Entity::Damage(damage);


}

void Player::Rotate(float roll, float pitch, float yaw)  {
	// apply transform-relative orientation changes
	//grab initial frame of reference
	//modify forward vector by mouse location

	//Roll:
	state.ori = XMQuaternionMultiply(state.ori, XMQuaternionRotationAxis(XMLoadFloat4(&(state.forward)), roll*0.01f));
	//Pitch:
	state.ori = XMQuaternionMultiply(state.ori, XMQuaternionRotationAxis(XMLoadFloat4(&(state.left)), pitch*0.01f));
	//Yaw:
	state.ori = XMQuaternionMultiply(state.ori, XMQuaternionRotationAxis(XMLoadFloat4(&(state.up)), yaw*0.01f));

	state.forward = base_forward;
	mouseForward = XMFLOAT4(mouseVector.x, mouseVector.y, 1.0f, 1.0f);
	XMFLOAT4 accel;
	state.up = base_up;
	state.left = base_left;	
	

	//rotate mouse vector
	XMStoreFloat4(&(mouseForward),
		XMVector4Transform(XMVector4NormalizeEst(XMLoadFloat4(&mouseForward)), XMMatrixRotationQuaternion(state.ori)));

	//rotate the framce by the quaternion
	XMStoreFloat4(&(state.forward),
		XMVector4Transform(XMVector4Normalize(XMLoadFloat4(&(state.forward))), XMMatrixRotationQuaternion(state.ori)));
	XMStoreFloat4(&(state.up),
		XMVector4Transform(XMLoadFloat4(&(state.up)), XMMatrixRotationQuaternion(state.ori)));
	XMStoreFloat4(&(state.left),
		XMVector3Cross(XMLoadFloat4(&(state.forward)), XMLoadFloat4(&(state.up))));

	//calculate sideways acceleration vector
	XMStoreFloat4(&accel,
		XMVector3NormalizeEst(XMVector3Cross(XMLoadFloat4(&(mouseForward)), XMLoadFloat4(&(state.up)))));

	state.dstate.ax = 70 * accel.x;
	state.dstate.ay = 70 * accel.y;
	state.dstate.az = 70 * accel.z;

	//get intended direction
	state.vel = state.forward;

	state.dstate.vx = PLAYER_SPEED * state.vel.x;
	state.dstate.vy = PLAYER_SPEED * state.vel.y;
	state.dstate.vz = PLAYER_SPEED * state.vel.z;
}

void Player::MoveWithMouse(float x, float y)
{
	float roll, pitch, yaw;
	
	//rotate ship
	roll = 0;
	yaw = XM_PIDIV2 * -x;
	pitch = XM_PIDIV2 * y;

	//https://www.mvps.org/directx/articles/rayproj.htm for help on mouse coordinates
	float dx = tan(70.0f * XM_PI / 180.0f / 2.0f) * -x;
	float dy = tan(70.0f * XM_PI / 180.0f / 2.0f) * -y;

	//near clip distance = 1.0f, far is 1000.f
	XMFLOAT3 endNear(dx * 1.0f, dy * 1.0f, 1.0f);
	XMFLOAT3  endFar  (dx * 1000.f, dy * 1000.f, 1000.f);

	//create inverse view matrix
	XMFLOAT4X4 inverseView;
	XMStoreFloat4x4(&inverseView, XMMatrixInverse(NULL, XMLoadFloat4x4(&m_view)));

	XMStoreFloat3(&endNear,
		XMVector3Transform(XMLoadFloat3(&endNear), XMLoadFloat4x4(&inverseView)));
	XMStoreFloat3(&endFar,
		XMVector3Transform(XMLoadFloat3(&endFar), XMLoadFloat4x4(&inverseView)));

	//prepare mouseVector for rotation
	mouseVector = XMFLOAT4(base_forward.x - x, base_forward.y - x, base_forward.z, 1.0f);
	mouseVector = XMFLOAT4(
		-endFar.x + endNear.x,
		-endFar.y + endNear.y,
		-endFar.z + endNear.z,
		1.0f);

	Rotate(roll, pitch, yaw);
}

void Player::UpdateBoundingSphere()
{
	m_BoundingSphere->UpdateSphere(transform.pos);
}

bool Player::CheckBoundingSphereCollision(BoundingSphere* collidingSphere)
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