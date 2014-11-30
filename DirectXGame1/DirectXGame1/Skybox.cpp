#include "pch.h"

#include "Skybox.h"

Skybox::Skybox(DX::DeviceResources* m_deviceResources)
{
	m_loading = false;

	m_quadUp.reset(new SkyboxQuad(m_deviceResources, L"Assets/skybox_up.dds", SKYBOX_SCALE));
	m_quadUp->SetRotation(XM_PI / 2, XM_PI, 0);
	m_quadUp->SetPosition(0, SKYBOX_SCALE, 0);

	m_quadDown.reset(new SkyboxQuad(m_deviceResources, L"Assets/skybox_down.dds", SKYBOX_SCALE));
	m_quadDown->SetRotation(-XM_PI / 2, -XM_PI, 0);
	m_quadDown->SetPosition(0, -SKYBOX_SCALE, 0);

	m_quadLeft.reset(new SkyboxQuad(m_deviceResources, L"Assets/skybox_left.dds", SKYBOX_SCALE));
	m_quadLeft->SetRotation(0, XM_PI / 2, 0);
	m_quadLeft->SetPosition(-SKYBOX_SCALE, 0, 0);

	m_quadRight.reset(new SkyboxQuad(m_deviceResources, L"Assets/skybox_right.dds", SKYBOX_SCALE));
	m_quadRight->SetRotation(0, -XM_PI / 2, 0);
	m_quadRight->SetPosition(SKYBOX_SCALE, 0, 0);

	m_quadBack.reset(new SkyboxQuad(m_deviceResources, L"Assets/skybox_back.dds", SKYBOX_SCALE));
	m_quadBack->SetRotation(0, 0, 0);
	m_quadBack->SetPosition(0, 0, -SKYBOX_SCALE);

	m_quadFront.reset(new SkyboxQuad(m_deviceResources, L"Assets/skybox_front.dds", SKYBOX_SCALE));
	m_quadFront->SetRotation(0, XM_PI, 0);
	m_quadFront->SetPosition(0, 0, SKYBOX_SCALE);

	m_quads.push_back(m_quadUp.get());
	m_quads.push_back(m_quadDown.get());
	m_quads.push_back(m_quadLeft.get());
	m_quads.push_back(m_quadRight.get());
	m_quads.push_back(m_quadBack.get());
	m_quads.push_back(m_quadFront.get());

	InitializeShaders(m_deviceResources);
}

void Skybox::InitializeShaders(DX::DeviceResources* m_deviceResources)
{
	// Load shaders asynchronously.
	auto loadVSTask = DX::ReadDataAsync(L"SkyboxVertexShader.cso");
	auto loadPSTask = DX::ReadDataAsync(L"SkyboxPixelShader.cso");

	// After the vertex shader file is loaded, create the shader and input layout.
	auto createVSTask = loadVSTask.then([this, m_deviceResources](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
			&fileData[0],
			fileData.size(),
			nullptr,
			&m_vertexShader
			)
			);

		static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateInputLayout(
			vertexDesc,
			ARRAYSIZE(vertexDesc),
			&fileData[0],
			fileData.size(),
			&m_inputLayout
			)
			);
	});


	// After the pixel shader file is loaded, create the shader and constant buffer.
	auto createPSTask = loadPSTask.then([this, m_deviceResources](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
			&fileData[0],
			fileData.size(),
			nullptr,
			&m_pixelShader
			)
			);

		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
			&constantBufferDesc,
			nullptr,
			&m_constantBuffer
			)
			);
	});

	auto createSamplerTask = (createPSTask && createVSTask).then([this, m_deviceResources]() {
		// Create the sampler state
		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		m_deviceResources->GetD3DDevice()->CreateSamplerState(&sampDesc, m_samplerState.GetAddressOf());
	});

	createSamplerTask.then([this]() {
		m_loading = true;
	});
}

void Skybox::Update(XMVECTOR& cameraPosition)
{
	for each (SkyboxQuad* quad in m_quads)
		quad->UpdatePosition(cameraPosition);
}

void Skybox::Render(ID3D11DeviceContext2* context, VSD3DStarter::Camera* camera)
{
	if (m_loading == false)
		return;

	for each (SkyboxQuad* quad in m_quads)
	{
		XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMLoadFloat4x4(&quad->GetTransformMatrix())));
		XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMLoadFloat4x4(&camera->GetView())));
		XMStoreFloat4x4(&m_constantBufferData.projection, XMMatrixTranspose(XMLoadFloat4x4(&camera->GetProjection())));

		// Prepare the constant buffer to send it to the graphics device.
		context->UpdateSubresource(
			m_constantBuffer.Get(),
			0,
			NULL,
			&m_constantBufferData,
			0,
			0
			);

		// Each vertex is one instance of the VertexPositionColor struct.
		UINT stride = sizeof(VertexPositionUv);
		UINT offset = 0;
		context->IASetVertexBuffers(
			0,
			1,
			quad->GetVertexBufferAddress(),
			&stride,
			&offset
			);

		context->IASetIndexBuffer(
			quad->GetIndexBuffer(),
			DXGI_FORMAT_R16_UINT, // Each index is one 16-bit unsigned integer (short).
			0
			);

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->IASetInputLayout(m_inputLayout.Get());

		// Attach our vertex shader.
		context->VSSetShader(
			m_vertexShader.Get(),
			nullptr,
			0
			);

		// Send the constant buffer to the graphics device.
		context->VSSetConstantBuffers(
			0,
			1,
			m_constantBuffer.GetAddressOf()
			);

		// Attach our pixel shader.
		context->PSSetShader(
			m_pixelShader.Get(),
			nullptr,
			0
			);

		context->PSSetConstantBuffers(
			0,
			1,
			m_constantBuffer.GetAddressOf()
			);

		context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());
		context->PSSetShaderResources(0, 1, quad->GetResourceViewAddress());

		// Draw the objects.
		context->DrawIndexed(
			quad->GetIndexCount(),
			0,
			0
			);
	}
}