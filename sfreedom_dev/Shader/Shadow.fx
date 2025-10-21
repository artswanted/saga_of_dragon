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

//--------------------------------------------------------------
// Shadow.fx 
// Shadow generation techniques that include gauss-blur downsize
// to create semi-soft edges.
//--------------------------------------------------------------

// Textures
texture BaseMap
<
    string NTM = "base";
>;

// Samplers
sampler BaseSampler = sampler_state
{
    Texture = (BaseMap);
    MINFILTER = LINEAR;
    MIPFILTER = LINEAR;
    MAGFILTER = LINEAR;
    ADDRESSU = WRAP;
    ADDRESSV = WRAP;
};

sampler BaseClampSampler = sampler_state
{
    Texture = (BaseMap);
    MINFILTER = LINEAR;
    MIPFILTER = LINEAR;
    MAGFILTER = LINEAR;
    ADDRESSU = CLAMP;
    ADDRESSV = CLAMP;
};

sampler2D BasePointClampSampler =
sampler_state
{
    Texture = (BaseMap);
    MinFilter = Point;
    MagFilter = Point;
    MipFilter = None;
    AddressU = Clamp;
    AddressV = Clamp;
};

static const int MAX_SAMPLES = 13;

float2 g_akGaussSampleOffsets[MAX_SAMPLES] : GLOBAL;
float4 g_akGaussSampleWeights[MAX_SAMPLES] : GLOBAL;

float4x4 View : VIEW;
float4x4 ViewProj : VIEWPROJ;
float4x4 Proj : PROJ;
float4x4 WorldViewProj : WORLDVIEWPROJECTION;
float4x4 inv_view_matrix : INVVIEW;
float4x4 world_matrix : WORLD;

static const int MAX_BONES = 29;
float4x3 SkinBone[MAX_BONES] : SKINBONEMATRIX3;

float4 g_afShadowColor : GLOBAL = float4(0.6f, 0.6f, 0.6f, 1.0f);
float g_fShadowThreshold : GLOBAL = 0.9f;

//--------------------------------------------------------------//

float4 CalculateSkinnedPosition(float4 inPos, float4 inBlendWeights,
    float4 inBlendIndices)
{
	// Compensate for lack of UBYTE4 on Geforce3
    int4 indices = D3DCOLORtoUBYTE4(inBlendIndices);

    // Calculate normalized fourth bone weight
    float weight4 = 1.0f - inBlendWeights[0] - inBlendWeights[1] - 
        inBlendWeights[2];

    // Calculate bone transform
    float4x3 SkinBoneTransform;
	SkinBoneTransform = inBlendWeights[0] * SkinBone[indices[0]];
	SkinBoneTransform += inBlendWeights[1] * SkinBone[indices[1]];
	SkinBoneTransform += inBlendWeights[2] * SkinBone[indices[2]];
	SkinBoneTransform += weight4 * SkinBone[indices[3]];
	
	float4 outPos = float4(mul(inPos, SkinBoneTransform), 1.0);
    float4 ProjectedPos = mul(outPos, ViewProj);
    
    return ProjectedPos;
}

//--------------------------------------------------------------//

struct VS_OUTPUT
{
    float4 Pos      : POSITION;
    float2 Tex0     : TEXCOORD0;
};

VS_OUTPUT VSMain(float4 inPos: POSITION, float2 inTex0: TEXCOORD0)
{
    VS_OUTPUT Out;
    Out.Pos = mul(inPos, WorldViewProj);
    Out.Tex0 = inTex0;
    return Out;
}

//-----------------------------------------------------------------------------
// Pixel Shader: GaussBlur5x5
// Desc: Simulate a 5x5 kernel gaussian blur by sampling the 12 points closest
//       to the center point.
//-----------------------------------------------------------------------------
float4 PSGaussBlur5x5(in float2 kScreenPosition : TEXCOORD0) : COLOR
{
    float4 kSample = 0.0f;

	for (int i = 0; i < MAX_SAMPLES; i++)
	{
		kSample += g_akGaussSampleWeights[i] * tex2D(BasePointClampSampler, 
            kScreenPosition + g_akGaussSampleOffsets[i] );
	}

	return kSample;
}


//--------------------------------------------------------------//
// Shadow Skinned: Position only
//--------------------------------------------------------------//

struct VS_OUTPUT_PosOnly 
{
    float4 Pos  :     POSITION;
};

VS_OUTPUT_PosOnly VSShadowSkinned(float4 inPos: POSITION,
    float4 inBlendWeights : BLENDWEIGHT, 
    float4 inBlendIndices : BLENDINDICES)
{
    VS_OUTPUT_PosOnly Out;

    Out.Pos = CalculateSkinnedPosition(inPos, inBlendWeights,
        inBlendIndices);

    return Out;
}

float4 PSShadowSkinned() : COLOR0
{

    return g_afShadowColor;
}

//--------------------------------------------------------------//
// Shadow NoAlpha: Texkill used to avoid any issues with the
// multiple render target shaders
//--------------------------------------------------------------//

struct VS_OUTPUT_PosTex 
{
    float4 Pos  :     POSITION;
    float2 Tex  :     TEXCOORD0;
};

VS_OUTPUT_PosTex VSShadowDefault(float4 inPos: POSITION,
     float2 inTex0: TEXCOORD0)
{
    VS_OUTPUT_PosTex Out;

    Out.Pos = mul(inPos, WorldViewProj);
    Out.Tex = inTex0;
    
    return Out;
}

float4 PSShadowDefault(float2 inTex0: TEXCOORD0) : COLOR0
{
    // does a texkill if the shadow is brighter that a given threshold
    // This ensures that distant pieces of the shadow geometry do not
    // overwrite any target buffers 
    float4 color = tex2D(BaseClampSampler, inTex0);
    if (color.r > g_fShadowThreshold)
        clip(-1);

    return color;
}
//--------------------------------------------------------------//
// Shadow NoAlpha: Position only
//--------------------------------------------------------------//

VS_OUTPUT_PosTex VSShadowNoAlpha(float4 inPos: POSITION,
     float2 inTex0: TEXCOORD0)
{
    VS_OUTPUT_PosTex Out;

    Out.Pos = mul(inPos, WorldViewProj);
    Out.Tex = inTex0;
    
    return Out;
}

float4 PSShadowNoAlpha(float2 inTex0: TEXCOORD0) : COLOR0
{
    // does a texkill if there is no shadow at the given point 
    float4 color = tex2D(BaseClampSampler, inTex0);
    if (color.r > g_afShadowColor.r)
        clip(-1);

    return color;
}
//--------------------------------------------------------------//
// Technique Section
//--------------------------------------------------------------//
technique ShadowSkinned
<
    string description = "Shader for shadow casters\n"; 
    bool UsesNiRenderState = true;
    bool UsesNiLightState = false;
	int BonesPerPartition = MAX_BONES;
>
{
    pass Single_Pass
    {
        VertexShader = compile vs_1_1 VSShadowSkinned();
        PixelShader = compile ps_2_0 PSShadowSkinned();
    }
}
//--------------------------------------------------------------//
technique ShadowNoAlpha
<
    string description = "Shader for shadow recipient\n"; 
    bool UsesNiRenderState = true;
    bool UsesNiLightState = false;
>
{
    pass Single_Pass
    {
        VertexShader = compile vs_1_1 VSShadowNoAlpha();
        PixelShader = compile ps_2_0 PSShadowNoAlpha();
    }
}
//--------------------------------------------------------------//
technique ShadowDefault
<
    string description = "Shader for shadow recipient\n"; 
    bool UsesNiRenderState = true;
    bool UsesNiLightState = false;
>
{
    pass Single_Pass
    {
        VertexShader = compile vs_1_1 VSShadowDefault();
        PixelShader = compile ps_2_0 PSShadowDefault();
    }
}
//--------------------------------------------------------------//
technique ShadowGaussBlur
<
    string description = "Shader for shadow blurring\n"; 
    bool UsesNiRenderState = true;
    bool UsesNiLightState = false;
>
{
    pass Single_Pass
    {
        VertexShader = compile vs_1_1 VSMain();
        PixelShader = compile ps_2_0 PSGaussBlur5x5();
    }
}
//--------------------------------------------------------------//
