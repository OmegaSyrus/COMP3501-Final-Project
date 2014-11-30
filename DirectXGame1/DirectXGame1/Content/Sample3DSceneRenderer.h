//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

#include "..\Helpers\DeviceResources.h"
#include "ShaderStructures.h"
#include "..\Helpers\StepTimer.h"

#include <memory>
#include <vector>
#include <random>

#include "VSD3DStarter.h"
#include "Animation\Animation.h"

#include "Asteroid.h"
#include "Player.h"
//#include "PlayerHitQuad.h"
#include "BoundingSphere.h"
#include "PhysicsEngine.h"

#include "Skybox.h"

using namespace Microsoft::WRL;
using namespace VSD3DStarter;

namespace DirectXGame1
{
	// This sample renderer instantiates a basic rendering pipeline.
	class Sample3DSceneRenderer
	{
		const float CAMERA_Z_OFFSET = -40.0f;
		const float CAMERA_X_OFFSET = 0.0f;
		const float CAMERA_Y_OFFSET = 10.0f;
		const float CAMERA_MOVEMENT_DAMPENING = 10.0f;

		const float HIT_EFFECT_TIMER = 0.8f;

	public:
		Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);
		void Render();
		void TransferMouseCoordinates(float, float);
		void FireWeapon();
		void AttachPhysics();

		// Operator overload to align 16 on heap
		void* operator new (size_t size)
		{
			void * p = _aligned_malloc(size, 16);
			return p;
		}
			void operator delete (void *p)
		{
			Sample3DSceneRenderer* pc = static_cast<Sample3DSceneRenderer*>(p);
			_aligned_free(p);
		}

	private:
		void InitializeAsteroidField();
		void InitializePlayer();
		void CreateCamera();
		void UpdatePlayerCamera(float);

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// Direct3D resources for cube geometry.
		Microsoft::WRL::ComPtr<ID3D11InputLayout>   m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>        m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>        m_indexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>  m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>   m_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>        m_constantBuffer;

		// System resources for cube geometry.
		ModelViewProjectionConstantBuffer    m_constantBufferData;
		uint32    m_indexCount;

		// Variables used with the rendering loop.
		bool    m_loadingComplete;
		float   m_degreesPerSecond;
		bool    m_tracking;

		// Variables for player camera
		typedef struct Camera
		{
			XMVECTOR pos;
			XMVECTOR ori;
			XMVECTOR forward;
			XMVECTOR up;
			XMVECTOR left; // player coordinate frame, rebuilt from orientation
		};
		Camera cam;

		// Used for visual effect when player collides with asteroid
		bool m_playerHit;
		//PlayerHitQuad* m_hitEffect;
		float m_hitEffectTimer;

		// Members used to keep track of the scene state.
		SkinnedMeshRenderer m_skinnedMeshRenderer;
		float m_rotation;
		double weaponRemainingTime;
		std::vector<Entity*> m_entities;
		std::vector<Asteroid*> m_asteroids;

		// Random number device
		std::random_device rd;

		// Player
		Player* player;

		// Members used to keep track of the graphics state.
		VSD3DStarter::Graphics m_graphics;
		VSD3DStarter::LightConstants m_lightConstants;
		VSD3DStarter::MiscConstants m_miscConstants;

		std::unique_ptr<Skybox> m_skybox;

		PhysicsEngine physEng;
	};
}

