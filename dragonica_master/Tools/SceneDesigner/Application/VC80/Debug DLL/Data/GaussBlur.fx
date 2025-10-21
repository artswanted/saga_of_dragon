// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2006 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

//---------------------------------------------------------------------------
// HDRScene.fx
// Implements all the techniques necessary to perform "HDR Lighting".  
// 1) Average scene luminance calculation
// 2) Bright-pass filtering
// 3) "Bloom" operations (via downsizing/gauss blurring)
// 4) Final scene compositing
//
// Based directly on concepts from the Microsoft DirectX9 SDK "HDR Lighting"
// sample.
// 
//---------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// **GLOBALS AND CONSTANTS**
//-----------------------------------------------------------------------------

static const int MAX_SAMPLES = 16;


float2 gakSampleOffsets_BB[MAX_SAMPLES] : GLOBAL;
float gakSampleWeights_BB[MAX_SAMPLES] : GLOBAL;
///////////////////////////////////////////////////////////
float4x4 worldViewProjection
<
	string VarType = "Predefined";
	string DefinedMapping = "WORLDVIEWPROJECTION";
>;

texture TargetMap : RENDERCOLORTARGET
< 
    float2 ViewportRatio = { 1, 1 };
    int MIPLEVELS = 1;
    string format = "A8R8G8B8";
>;

sampler TargetSampler = sampler_state 
{
    texture = <TargetMap>;
    AddressU  = CLAMP;        
    AddressV  = CLAMP;
    AddressW  = CLAMP;
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};

///////////////////////////////////////////////////////////
struct VS_OUTPUT
{
    float4 Position   : POSITION;
    float2 TexCoord0  : TEXCOORD0;
};

VS_OUTPUT GaussBlurVS(float4 Position : POSITION, 
				  float2 TexCoord : TEXCOORD0)
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;
    
    OUT.Position = mul(Position, worldViewProjection);
    OUT.TexCoord0 = TexCoord;
    
    return OUT;
}
float lumi(float3 color)
{
	return dot(color,float3(0.3,0.59,0.11));
}
half4 GaussBlurPS(float2 Tex: TEXCOORD0) : COLOR
{   
    float4 kSample = 0.0f;

	for (int i = 0; i <= 12; i++)
	{
		kSample += gakSampleWeights_BB[i] * tex2D(TargetSampler, 
            Tex + gakSampleOffsets_BB[i] );
	}
	
	return kSample;
}  
//-----------------------------------------------------------------------------
// Pixel Shader: Bloom
// Desc: Blur the source image along one axis using a gaussian
//       distribution. Since gaussian blurs are separable, this shader is 
//       called twice; first along the horizontal axis, then along the 
//       vertical axis.
//-----------------------------------------------------------------------------
float4 GaussBloomPS(in float2 kScreenPosition : TEXCOORD0) : COLOR
{
    float4 kSample = 0.0f;
    float4 kColor = 0.0f;
        
    float2 kSamplePosition;
    
    // Perform a one-directional gaussian blur
    for (int iSample = 0; iSample < 15; iSample++)
    {
        kSamplePosition = kScreenPosition + gakSampleOffsets_BB[iSample];
        kColor = tex2D(TargetSampler, kSamplePosition);
        kSample += gakSampleWeights_BB[iSample] * kColor;
    }
    
    return kSample;
}

////////////////////////////////////////////////////////////
technique GaussBlur
<
	    bool UsesNiRenderState = true;
>
{
	pass One 
	{
		cullmode = none;
		ZEnable = false;
		ZWriteEnable = false;
		FogEnable = false;
		VertexShader = compile vs_1_1 GaussBlurVS();
		PixelShader  = compile ps_2_0 GaussBlurPS();
	}
}
technique GaussBloom
<
	    bool UsesNiRenderState = true;
>
{
	pass One 
	{
		cullmode = none;
		ZEnable = false;
		ZWriteEnable = false;
		FogEnable = false;
		VertexShader = compile vs_1_1 GaussBlurVS();
		PixelShader  = compile ps_2_0 GaussBloomPS();
	}
}
