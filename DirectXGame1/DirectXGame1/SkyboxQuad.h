#pragma once

typedef unsigned short WORD;

#include <vector>
#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl.h>

#include "DirectXTex\DDSTextureLoader.h"
#include "Content\ShaderStructures.h"
#include "Helpers\DirectXHelper.h"
#include "Helpers\DeviceResources.h"

using namespace std;
using namespace DirectXGame1;
using namespace DirectX2;
using namespace DirectX;
using namespace DX;

class SkyboxQuad
{
	public:
		SkyboxQuad(DX::DeviceResources*, std::wstring, float);

	public:
		// Setters
		void SetPosition(float, float, float);
		void SetRotation(float, float, float);

		// Getters
		ID3D11Buffer**				GetVertexBufferAddress();
		ID3D11Buffer*				GetIndexBuffer();
		ID3D11ShaderResourceView**	GetResourceViewAddress();
		XMFLOAT4X4&					GetTransformMatrix();
		uint32						GetIndexCount();

		void UpdatePosition(XMVECTOR);

		//align
		void* operator new (size_t size)
		{
			void * p = _aligned_malloc(size, 16);
			return p;
		}
			void operator delete (void *p)
		{
			SkyboxQuad* pc = static_cast<SkyboxQuad*>(p);
			_aligned_free(p);
		}

	private:
		void CreateQuad(DX::DeviceResources*);
		void InitializeTextureView(DX::DeviceResources*, std::wstring);

	private:
		// Vertex and Indices data
		vector<VertexPositionUv>	vertices;
		vector<WORD>				indices;

		Microsoft::WRL::ComPtr<ID3D11Buffer>    m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>    m_indexBuffer;

		uint32    m_indexCount;

		// Quad's transform matrix
		typedef struct Transform
		{
			XMVECTOR pos;
			XMVECTOR scale;
			XMVECTOR ori;
		};
		Transform m_transform;

		XMFLOAT4X4	m_transformMatrix;

		XMVECTOR m_cameraPosition;

		// Quad texture resources
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_resourceView;
};