
#pragma once

#include "VSD3DStarter.h"
#include "Animation\Animation.h"
#include "PhysicalStructs.h"
#include "PhysicsEngine.h"

using namespace VSD3DStarter;
using namespace DirectX;
using namespace Physics;

class PhysicsEngine;

// Class that represents a 3D entity
class Entity
{
	friend PhysicsEngine;

	std::wstring m_modelFilename = L"player-ship.cmo";

public:
	Entity(VSD3DStarter::Graphics&);
	Entity(std::vector<VSD3DStarter::Mesh*>&	m_model);
	Entity();
	~Entity();

	//get the XMVECTOR members
	XMVECTOR GetPos();
	XMVECTOR GetOri();
	XMVECTOR GetForward();
	XMVECTOR GetUp();
	XMVECTOR GetLeft();

	//get the float storage versions
	XMFLOAT4 GetFPos();
	XMFLOAT4 GetFForward();
	XMFLOAT4 GetFUp();
	XMFLOAT4 GetFLeft();
	virtual int GetType();

	//grab the whole state
	PhysicalState&  GetState();
	PhysicalState& GetFutureState();
	//grab the model
	virtual std::vector<VSD3DStarter::Mesh*>&	GetModel();

	void SetState(PhysicalState const &  newState);
	void ReversePosition();

	virtual void Update();
	virtual void Update(double timeDelta);
	virtual void Damage(int);
	virtual void Render(VSD3DStarter::Graphics&);
	virtual void Rotate(float, float, float);
	bool GetRenderSetting();
	void InitializeModelFromFile(VSD3DStarter::Graphics&);
	void SetPosition(float, float, float);
	void SetRenderSetting(bool s);
	void LoadIntoEngine(PhysicsEngine* phys);

protected:
	virtual std::wstring GetModelFilename();
	virtual void UpdateEntityTransform();

protected:
	DirectX::XMFLOAT4X4					m_transform;

	typedef struct Transform
	{
		XMVECTOR pos;
		XMVECTOR ori;
		XMVECTOR forward;
		XMVECTOR up;
		XMVECTOR left; // player coordinate frame, rebuilt from orientation
	};
	Transform transform;

	PhysicalState state;
	PhysicalState newstate;
	PhysicsEngine* physEng;
	int health;
	int lives;

	std::vector<VSD3DStarter::Mesh*>	m_model;
	std::wstring						m_animationClipName;
	bool								m_allowRender;

	float radius;

};