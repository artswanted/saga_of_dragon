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

// Simple FX Skinning Shader

float4x4 SkinWorldViewProj      : SKINWORLDVIEWPROJ;
float4x4 SkinWorld : SKINWORLD;
float4x4 ViewProj : VIEWPROJ;
float4 MaterialSpecular	:	MATERIALSPECULAR;
float4 MaterialPower : MATERIALPOWER;
float4 MaterialEmissive : MATERIALEMISSIVE;
float4 MaterialDiffuse : MATERIALDIFFUSE;
float4 MaterialAmbient : MATERIALAMBIENT;
float4 AmbientLight : AMBIENTLIGHT;
float4x4	ProjectedLightMapWorldProjectionTransform :WORLDPROJECTIONTRANSFORM;

static const int MAX_BONES = 20;
float4x3 Bone[20] : BONEMATRIX3;

struct VS_INPUT 
{
    float4 Pos			: POSITION;
    float4 BlendWeights : BLENDWEIGHT;
    float4 BlendIndices : BLENDINDICES;
    float2 TexCoords    : TEXCOORD0;
    float4 Color	:	DIFFUSE;
};

struct VS_OUTPUT
{
    float4 Pos			: POSITION;
    float2 TexCoords    : TEXCOORD0;
    float4 Diffuse : COLOR0;
};
struct VS_OUTPUT_Projection
{
    float4 Pos			: POSITION;
    float2 TexCoords    : TEXCOORD0;
    float2 TexCoords2	: TEXCOORD1;
    float4 Diffuse : COLOR0;
};

texture BaseMap
<
    string NTM = "base";
>;
texture GlowMap
<
    string NTM = "glow";
>;
texture ProjectedLight
<
	string NTM ="ProjectedLight";
>;


void ComputeShadingCoefficients(float3 MatEmissive,
    float3 MatDiffuse,
    float3 MatAmbient,
    float3 MatSpecular,
    float3 LightAmbientAccum,
    out float3 Diffuse)
{

    Diffuse = MatEmissive + MatAmbient * LightAmbientAccum + 
        MatDiffuse;
}

VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;

	// Compensate for lack of UBYTE4 on Geforce3
    int4 indices = D3DCOLORtoUBYTE4(In.BlendIndices);

    // Calculate normalized fourth bone weight
    float weight4 = 1.0f - In.BlendWeights[0] - In.BlendWeights[1] - 
        In.BlendWeights[2];
    float4 weights = float4(In.BlendWeights[0], In.BlendWeights[1], 
        In.BlendWeights[2], weight4);

    // Calculate bone transform
    float4x3 BoneTransform;
	BoneTransform = weights[0] * Bone[indices[0]];
	BoneTransform += weights[1] * Bone[indices[1]];
	BoneTransform += weights[2] * Bone[indices[2]];
	BoneTransform += weights[3] * Bone[indices[3]];
	
	float3 BoneSpacePos = mul(In.Pos, BoneTransform);

	Out.Pos = mul(float4(BoneSpacePos, 1.0), SkinWorldViewProj);
	
	Out.TexCoords = In.TexCoords;
	
    ComputeShadingCoefficients(MaterialEmissive.rgb, In.Color, 
        MaterialAmbient.rgb, MaterialSpecular.rgb, AmbientLight.rgb,Out.Diffuse.rgb);

	Out.Diffuse.a = 1;
	
	return Out;
}
VS_OUTPUT_Projection VS_ProjectionLight(VS_INPUT In)
{
	VS_OUTPUT_Projection Out = (VS_OUTPUT_Projection)0;

	// Compensate for lack of UBYTE4 on Geforce3
    int4 indices = D3DCOLORtoUBYTE4(In.BlendIndices);

    // Calculate normalized fourth bone weight
    float weight4 = 1.0f - In.BlendWeights[0] - In.BlendWeights[1] - 
        In.BlendWeights[2];
    float4 weights = float4(In.BlendWeights[0], In.BlendWeights[1], 
        In.BlendWeights[2], weight4);

    // Calculate bone transform
    float4x3 BoneTransform;
	BoneTransform = weights[0] * Bone[indices[0]];
	BoneTransform += weights[1] * Bone[indices[1]];
	BoneTransform += weights[2] * Bone[indices[2]];
	BoneTransform += weights[3] * Bone[indices[3]];
	
	float3 BoneSpacePos = mul(In.Pos, BoneTransform);
	
	float4 SkinWorldPos = mul(float4(BoneSpacePos, 1.0), SkinWorld);
	float4 ProjectionPos = mul(SkinWorldPos, ProjectedLightMapWorldProjectionTransform);
	
	Out.TexCoords2 = ProjectionPos.xy;

	Out.Pos = mul(SkinWorldPos, ViewProj);
	
	Out.TexCoords = In.TexCoords;
	
    ComputeShadingCoefficients(MaterialEmissive.rgb, In.Color, 
        MaterialAmbient.rgb, MaterialSpecular.rgb, AmbientLight.rgb,Out.Diffuse.rgb);

	Out.Diffuse.a = 1;
	
	return Out;
}

technique FXSkinning
<
	string Description = "This shader performs skinning in the vertex shader "
    "using 20 bones. It uses HLSL shaders through an FX file. "
    "This shader uses BoneMatrix3, so it does not transform normals and "
    "cannot perform lighting calculations. "
    "This shader does apply a base map. ";

	int BonesPerPartition = 20;
	bool UsesNiRenderState = true;
>
{
	pass P0
	{
		VertexShader = compile vs_1_1 VS();
		PixelShader  = NULL;
        Texture[0] = (BaseMap);
        ColorOp[0] = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
        AlphaOp[0] = Modulate;
        AlphaArg1[0] = Texture;
        AlphaArg2[0] = Diffuse;
        AddressU[0] = Wrap;
        AddressV[0] = Wrap;
	}
}

technique FXSkinning_ProjectionLight
<
	string Description = "This shader performs skinning in the vertex shader "
    "using 20 bones. It uses HLSL shaders through an FX file. "
    "This shader uses BoneMatrix3, so it does not transform normals and "
    "cannot perform lighting calculations. "
    "This shader does apply a base map. ";

	int BonesPerPartition = 20;
	bool UsesNiRenderState = true;
>
{
	pass P0
	{
		VertexShader = compile vs_1_1 VS_ProjectionLight();
		PixelShader  = NULL;
        Texture[0] = (BaseMap);
        Texture[1] = (ProjectedLight);
        ColorOp[0] = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
        AlphaOp[0] = Modulate;
        AlphaArg1[0] = Texture;
        AlphaArg2[0] = Diffuse;
        AddressU[0] = Wrap;
        AddressV[0] = Wrap;
        
        ColorOp[1] = Modulate;
        ColorArg1[0] = Texture;
        ColorArg1[1] = Current;
        AlphaOp[1] = Modulate;
        AlphaArg1[0] = Texture;
        AlphaArg1[1] = Current;
        AddressU[1] = Wrap;
        AddressV[1] = Wrap;        
        
	}
}

technique FXSkinning_Glow
<
	string Description = "This shader performs skinning in the vertex shader "
    "using 20 bones. It uses HLSL shaders through an FX file. "
    "This shader uses BoneMatrix3, so it does not transform normals and "
    "cannot perform lighting calculations. "
    "This shader does apply a base map. ";

	int BonesPerPartition = 20;
	bool UsesNiRenderState = true;
>
{
	pass P0
	{
		VertexShader = compile vs_2_0 VS();
		PixelShader  = NULL;
        Texture[0] = (BaseMap);
        ColorOp[0] = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
        AlphaOp[0] = Modulate;
        AlphaArg1[0] = Texture;
        AlphaArg2[0] = Diffuse;
        AddressU[0] = Wrap;
        AddressV[0] = Wrap;
	}
}

technique FXSkinning_GlowProjectionLight
<
	string Description = "This shader performs skinning in the vertex shader "
    "using 20 bones. It uses HLSL shaders through an FX file. "
    "This shader uses BoneMatrix3, so it does not transform normals and "
    "cannot perform lighting calculations. "
    "This shader does apply a base map. ";

	int BonesPerPartition = 20;
	bool UsesNiRenderState = true;
>
{
	pass P0
	{
		VertexShader = compile vs_2_0 VS();
		PixelShader  = NULL;
        Texture[0] = (BaseMap);
        ColorOp[0] = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
        AlphaOp[0] = Modulate;
        AlphaArg1[0] = Texture;
        AlphaArg2[0] = Diffuse;
        AddressU[0] = Wrap;
        AddressV[0] = Wrap;
	}
}

