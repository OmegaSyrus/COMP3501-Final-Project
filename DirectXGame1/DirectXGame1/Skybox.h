#pragma once

#include <vector>
#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl.h>

#include "Content\ShaderStructures.h"
#include "Helpers\DirectXHelper.h"
#include "Helpers\DeviceResources.h"
#include "VSD3DStarter.h"

#include "SkyboxQuad.h"

using namespace std;
using namespace DirectXGame1;
using namespace DirectX;
using namespace DX;

class Skybox
{
	const float SKYBOX_SCALE = 550.0f;

	public:
		Skybox(DX::DeviceResources*);

		void Render(ID3D11DeviceContext2*, VSD3DStarter::Camera*);
		void Update(XMVECTOR&);

	private:
		void InitializeShaders(DX::DeviceResources*);

	private:
		// Skybox's 6 quads
		std::unique_ptr<SkyboxQuad> m_quadUp;
		std::unique_ptr<SkyboxQuad> m_quadDown;
		std::unique_ptr<SkyboxQuad> m_quadLeft;
		std::unique_ptr<SkyboxQuad> m_quadRight;
		std::unique_ptr<SkyboxQuad> m_quadBack;
		std::unique_ptr<SkyboxQuad> m_quadFront;

		vector<SkyboxQuad*> m_quads;

		// Variables used to render
		Microsoft::WRL::ComPtr<ID3D11InputLayout>   m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>  m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>   m_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>        m_constantBuffer;
		Microsoft::WRL::ComPtr<ID3D11SamplerState>  m_samplerState;

		ModelViewProjectionConstantBuffer m_constantBufferData;

		bool m_loading;
};