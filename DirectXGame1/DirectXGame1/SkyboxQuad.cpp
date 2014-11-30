#include "pch.h"

#include "SkyboxQuad.h"

SkyboxQuad::SkyboxQuad(DX::DeviceResources* m_deviceResources, std::wstring textureFilename, float quadScale)
{
	// Create the quad geometry
	CreateQuad(m_deviceResources);

	// Initialize the quad's texture resources
	InitializeTextureView(m_deviceResources, textureFilename);

	// Initialize the quad's transform
	m_transform.pos		= XMVectorSet(0, 0, 0, 0);
	m_transform.scale	= XMVectorSet(quadScale, quadScale, quadScale, 1);
	m_transform.ori		= XMQuaternionRotationRollPitchYaw(0, 0, 0);

	// Initialize camera position
	m_cameraPosition = XMVECTOR();
}

// Creates the quad's vertices and indices buffers
void SkyboxQuad::CreateQuad(DX::DeviceResources* m_deviceResources)
{
	VertexPositionUv newVertex;

	// Top-left point
	newVertex =
	{
		XMFLOAT3(-1, 1, 0),	// position
		XMFLOAT2(0, 0)		// u,v coordinates
	};
	vertices.push_back(newVertex);

	// Top-right point
	newVertex =
	{
		XMFLOAT3(1, 1, 0),	// position
		XMFLOAT2(1, 0)		// u,v coordinates
	};
	vertices.push_back(newVertex);

	// Bottom-left point
	newVertex =
	{
		XMFLOAT3(-1, -1, 0),// position
		XMFLOAT2(0, 1)		// u,v coordinates
	};
	vertices.push_back(newVertex);

	// Bottom-right point
	newVertex =
	{
		XMFLOAT3(1, -1, 0),	// position
		XMFLOAT2(1, 1)		// u,v coordinates
	};
	vertices.push_back(newVertex);

	// Initializing quad indices
	indices.push_back(WORD(0));
	indices.push_back(WORD(1));
	indices.push_back(WORD(2));
	indices.push_back(WORD(1));
	indices.push_back(WORD(3));
	indices.push_back(WORD(2));

	// Create the vertex buffer with the data created in 'CreateSphere' function
	// by sending it in the 1st element of the dynamic list of 'sphereVertices'
	// and giving it the total size of the list
	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = &vertices[0];
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC vertexBufferDesc(vertices.size()*sizeof(VertexPositionUv), D3D11_BIND_VERTEX_BUFFER);
	DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateBuffer(
		&vertexBufferDesc,
		&vertexBufferData,
		&m_vertexBuffer
		)
		);

	// Setting the total number of indices from the 'sphereIndices' list's size
	m_indexCount = indices.size();

	// Create the indices buffer with the data created in 'CreateSphere' function
	// by sending it in the 1st element of the dynamic list of 'sphereIndices'
	// and giving it the total size of the list
	D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
	indexBufferData.pSysMem = &indices[0];
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC indexBufferDesc(indices.size()*sizeof(WORD), D3D11_BIND_INDEX_BUFFER);
	DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateBuffer(
		&indexBufferDesc,
		&indexBufferData,
		&m_indexBuffer
		)
		);
}

// Initialize texture resource view
void SkyboxQuad::InitializeTextureView(DX::DeviceResources* m_deviceResources, std::wstring textureFilename)
{
	CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), 
								textureFilename.c_str(),
								nullptr,
								m_resourceView.GetAddressOf());
}

// Sets the quad's world position
void SkyboxQuad::SetPosition(float x, float y, float z)
{
	m_transform.pos = XMVectorSet(x, y, z, 0);
}

// Sets the quad's rotation from input (input is in radians)
void SkyboxQuad::SetRotation(float pitch, float yaw, float roll)
{
	m_transform.ori = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
}

ID3D11Buffer** SkyboxQuad::GetVertexBufferAddress()
{
	return m_vertexBuffer.GetAddressOf();
}

ID3D11ShaderResourceView** SkyboxQuad::GetResourceViewAddress()
{
	return m_resourceView.GetAddressOf();
}

ID3D11Buffer* SkyboxQuad::GetIndexBuffer()
{
	return m_indexBuffer.Get();
}

uint32 SkyboxQuad::GetIndexCount()
{
	return m_indexCount;
}

// Compute the transform matrix from the quad's rotation and position
XMFLOAT4X4& SkyboxQuad::GetTransformMatrix()
{
	XMMATRIX newTransform	= XMMatrixIdentity();
	XMMATRIX newRotation	= XMMatrixIdentity();
	XMMATRIX newScale		= XMMatrixIdentity();

	newTransform = XMMatrixScalingFromVector(m_transform.scale) * XMMatrixRotationQuaternion(m_transform.ori) * XMMatrixTranslationFromVector(m_transform.pos) * XMMatrixTranslationFromVector(m_cameraPosition);

	DirectX::XMStoreFloat4x4(&m_transformMatrix, newTransform);

	return m_transformMatrix;
}

void SkyboxQuad::UpdatePosition(XMVECTOR cameraPosition)
{
	m_cameraPosition = cameraPosition;
}