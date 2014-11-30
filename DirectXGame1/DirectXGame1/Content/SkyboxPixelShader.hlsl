//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

// Texture resources
Texture2D skyboxTexture	: register(t0);

// Sampler
SamplerState mysampler	: register(s0);


cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
};

// Per-pixel data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float4 surfpos : POSITION0;
	float2 texcoord : TEXCOORD0;
};


float4 main(PixelShaderInput input) : SV_TARGET
{
	// use texture color from both textures
	// sample the pixel at texture coordinate from skybox texture
	float3 cr = skyboxTexture.Sample(mysampler, input.texcoord);

	return float4(cr, 1.0f);
}
