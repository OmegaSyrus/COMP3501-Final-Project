//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"
#include "Sample3DSceneRenderer.h"

#include "..\Helpers\DirectXHelper.h"

using namespace DirectXGame1;

using namespace DirectX;
using namespace Windows::Foundation;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
Sample3DSceneRenderer::Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
m_loadingComplete(false),
m_degreesPerSecond(45),
m_indexCount(0),
m_tracking(false),
m_deviceResources(deviceResources)
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();

	CreateCamera();

	m_playerHit = false;
	m_hitEffectTimer = HIT_EFFECT_TIMER;

	//create the skybox
	m_skybox.reset(new Skybox(m_deviceResources.get()));

}

// Initialize the scene
void Sample3DSceneRenderer::CreateDeviceDependentResources()
{
	m_graphics.Initialize(m_deviceResources->GetD3DDevice(), m_deviceResources->GetD3DDeviceContext(), m_deviceResources->GetDeviceFeatureLevel());

	// Set DirectX to not cull any triangles so the entire mesh will always be shown.
	CD3D11_RASTERIZER_DESC d3dRas(D3D11_DEFAULT);
	d3dRas.CullMode = D3D11_CULL_NONE;
	d3dRas.MultisampleEnable = true;
	d3dRas.AntialiasedLineEnable = true;

	ComPtr<ID3D11RasterizerState> p3d3RasState;
	m_deviceResources->GetD3DDevice()->CreateRasterizerState(&d3dRas, &p3d3RasState);
	m_deviceResources->GetD3DDeviceContext()->RSSetState(p3d3RasState.Get());

	auto initializeSkinnedRendererTask = m_skinnedMeshRenderer.InitializeAsync(m_deviceResources->GetD3DDevice(), m_deviceResources->GetD3DDeviceContext());

	auto loadModelsTask = concurrency::create_task([this]
	{
		InitializeAsteroidField();
		InitializePlayer();
		//AttachPhysics();
	});



	(initializeSkinnedRendererTask && loadModelsTask).then([this]
	{
		m_loadingComplete = true;
	});
}

///
void Sample3DSceneRenderer::InitializeAsteroidField()
{
	// Engines 
	std::mt19937 e2(rd());

	// Distribtuions
	std::uniform_real_distribution<> dist(-40, 40);

	float randomX, randomY, randomZ;

	int ASTEROID_NUMBER = 40;

	Asteroid* newAsteroid;

	for (int i = 0; i < ASTEROID_NUMBER; i++)
	{
		newAsteroid = new Asteroid(m_graphics);

		randomX = (float)dist(e2);
		randomY = (float)dist(e2);
		randomZ = (float)dist(e2);

		newAsteroid->SetPosition(randomX, randomY, randomZ);

		newAsteroid->LoadIntoEngine(&physEng);

		m_entities.push_back(newAsteroid);
	}
}


void Sample3DSceneRenderer::InitializePlayer()
{
	player = new Player(m_graphics);
	player->LoadIntoEngine(&physEng);


	//m_hitEffect = new PlayerHitQuad(m_graphics);
	//m_hitEffect->SetRenderSetting(false);
	//m_entities.push_back((Entity*)m_hitEffect);


	m_entities.push_back((Entity*)player);

}

// Resets player's lasers to their initial positions
// and fires them again
// (this does not create a new set of lasers, it re-uses the player's lasers
// by just resetting their positions and orientation)
void Sample3DSceneRenderer::FireWeapon()
{
	if (player != nullptr)
		player->FireWeapon();
}

// Initializes view parameters when the window size changes.
void Sample3DSceneRenderer::CreateWindowSizeDependentResources()
{
	m_miscConstants.ViewportHeight = m_deviceResources->GetScreenViewport().Height;
	m_miscConstants.ViewportWidth = m_deviceResources->GetScreenViewport().Width;
	m_graphics.UpdateMiscConstants(m_miscConstants);

	m_graphics.GetCamera().SetOrientationMatrix(m_deviceResources->GetOrientationTransform3D());

	Size outputSize = m_deviceResources->GetOutputSize();

	// Setup the camera parameters for our scene.
	m_graphics.GetCamera().SetViewport((UINT)outputSize.Width, (UINT)outputSize.Height);
	m_graphics.GetCamera().SetPosition(XMFLOAT3(0.0f, 6.0f, -18.0f));
	m_graphics.GetCamera().SetLookAt(XMFLOAT3(0.0f, 0.0f, 0.0f));

	float aspectRatio = outputSize.Width / outputSize.Height;
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	if (aspectRatio < 1.0f)
	{
		// Portrait or snap view
		m_graphics.GetCamera().SetUpVector(XMFLOAT3(1.0f, 0.0f, 0.0f));
		fovAngleY = 120.0f * XM_PI / 180.0f;
	}
	else
	{
		// Landscape view.
		m_graphics.GetCamera().SetUpVector(XMFLOAT3(0.0f, 1.0f, 0.0f));
	}

	m_graphics.GetCamera().SetProjection(fovAngleY, aspectRatio, 1.0f, 1000.0f);
	//put this projection matrix into the player

	// Setup lighting for our scene.
	m_lightConstants.ActiveLights = 1;

	m_lightConstants.Ambient = XMFLOAT4(1.f, 1.f, 1.f, 1.0f);
	m_lightConstants.IsPointLight[0] = false;
	m_lightConstants.LightColor[0] = XMFLOAT4(1.f, 1.f, 1.f, 1.0f);
	static const XMVECTORF32 s_vPos = { 5.0f, 5.0f, -2.5f, 0.f };
	XMFLOAT4 dir;
	DirectX::XMStoreFloat4(&dir, XMVector3Normalize(s_vPos));
	m_lightConstants.LightDirection[0] = dir;
	m_lightConstants.LightSpecularIntensity[0].x = 2;

	m_graphics.UpdateLightConstants(m_lightConstants);
}

void Sample3DSceneRenderer::CreateCamera()
{
	cam.pos = XMVectorSet(0, 0, 0, 0);
	cam.ori = XMQuaternionRotationRollPitchYaw(0, 0, 0);
}

void Sample3DSceneRenderer::TransferMouseCoordinates(float x, float y)
{
	if (player == nullptr)
		return;

	Size outputSize = m_deviceResources->GetOutputSize();

	player->MoveWithMouse(2.0f * x / outputSize.Width - 1.0f, 1.0f - 2.0f * y / outputSize.Height);
}

void Sample3DSceneRenderer::AttachPhysics() {
	for (unsigned int i = 0; i < m_entities.size(); i++) {
		m_entities.at(i)->LoadIntoEngine(&physEng);
	}
}

void Sample3DSceneRenderer::UpdatePlayerCamera(float deltaTime)
{
	XMFLOAT4 playerForward = player->GetFForward();
	XMFLOAT4 playerLeft = player->GetFLeft();
	XMFLOAT4 playerUp = player->GetFUp();

	XMFLOAT4 offset = XMFLOAT4(CAMERA_X_OFFSET * playerLeft.x + CAMERA_Y_OFFSET * playerUp.x + CAMERA_Z_OFFSET * playerForward.x,
		CAMERA_X_OFFSET * playerLeft.y + CAMERA_Y_OFFSET * playerUp.y + CAMERA_Z_OFFSET * playerForward.y,
		CAMERA_X_OFFSET * playerLeft.z + CAMERA_Y_OFFSET * playerUp.z + CAMERA_Z_OFFSET * playerForward.z,
		0.0f);

	// Lerps the camera pos to the desired 'newPos'
	XMFLOAT4 newPos = player->GetFPos();
	XMFLOAT4 direction = player->GetFForward();
	newPos.x += offset.x;
	newPos.y += offset.y;
	newPos.z += offset.z;
	cam.pos = XMVectorLerp(cam.pos, XMLoadFloat4(&newPos), CAMERA_MOVEMENT_DAMPENING * deltaTime);

	// Sets camer's orientation to player orientation
	cam.ori = player->GetOri();

	// canonical coordinate frame
	//SPEEDUP
	cam.forward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	cam.up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	// rotate frame with current camera quaternion
	cam.forward = XMVector4Transform(cam.forward, XMMatrixRotationQuaternion(cam.ori));
	cam.up = XMVector4Transform(cam.up, XMMatrixRotationQuaternion(cam.ori));
	cam.left = XMVector3Cross(cam.forward, cam.up);

	player->SetViewMatrix(cam.pos, cam.forward, cam.up);
	m_graphics.GetCamera().UpdateCameraView(cam.pos, cam.forward, cam.up);
}

// Called once per frame, rotates the cube and calculates the model and view matrices.
void Sample3DSceneRenderer::Update(DX::StepTimer const& timer)
{
	//auto timeDelta = static_cast<float>(timer.GetElapsedSeconds());
	double timeDouble = timer.GetElapsedSeconds();

	// Update animated models.
	//m_skinnedMeshRenderer.UpdateAnimation(timeDelta, m_meshModels);

	// Rotate scene.
	//m_rotation = static_cast<float>(timer.GetTotalSeconds()) * 0.5f;

	if (!m_loadingComplete)
	{
		return;
	}
	//update the weaponry
	std::vector<Entity*> tempEntities = player->GetWeapons();
	weaponRemainingTime += timeDouble;
	double increment = timeDouble / 20;

	//weapon update and collision
	while (weaponRemainingTime >= 0) {
		for (UINT i = 0; i < tempEntities.size(); i++) {
			//auto tempEntity = tempEntities[i];
			tempEntities[i]->Update(increment);

			//check weapon entities
			/*if (tempEntity->GetRenderSetting()) {
				for (UINT j = 0; j < m_entities.size(); j++) {
					auto innerTempEntity = m_entities[j];
					if (PhysicsEngine::Colliding(*tempEntity, *innerTempEntity))
					{
						if (innerTempEntity->GetType() != 1) {
							tempEntity->Damage(1);
							innerTempEntity->Damage(1);
						}
					}
				}
			}*/
			weaponRemainingTime -= increment;
		}
	}
	//replace final step
	weaponRemainingTime+=increment;

	//handle collisions and motion
	for (UINT i = 0; i < m_entities.size(); i++)
	{
		auto tempEntity = m_entities[i];
		tempEntity->Update(timeDouble);
		
		//check all world entities
		for (UINT j = 0; j < m_entities.size(); j++) {
			auto innerTempEntity = m_entities[j];
			if (PhysicsEngine::Colliding(*player, *innerTempEntity))
			{
				if (innerTempEntity->GetType() == 3) {
					//asteroid collision damage
					//player->Damage(1);
//					innerTempEntity->Damage(1);


					//player->ReversePosition();
				}

			}
		}

	}

	if (player != nullptr)
		UpdatePlayerCamera((float)timeDouble);

	m_skybox->Update(cam.pos);
}

// Renders one frame using the vertex and pixel shaders.
void Sample3DSceneRenderer::Render()
{
	// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!m_loadingComplete)
	{
		return;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();

	// Set render targets to the screen.
	auto rtv = m_deviceResources->GetBackBufferRenderTargetView();
	auto dsv = m_deviceResources->GetDepthStencilView();
	ID3D11RenderTargetView *const targets[1] = { rtv };
	context->OMSetRenderTargets(1, targets, dsv);

	for (UINT i = 0; i < m_entities.size(); i++)
	{
		m_entities[i]->Render(m_graphics);

	}
	//draw the weaponry
	std::vector<Entity*> tempEntities = player->GetWeapons();

	for (UINT i = 0; i < tempEntities.size(); i++) {
		tempEntities[i]->Render(m_graphics);
	}

	// **************** SKYBOX
	m_skybox->Render(context, &m_graphics.GetCamera());
}

void Sample3DSceneRenderer::ReleaseDeviceDependentResources()
{
	m_loadingComplete = false;
}