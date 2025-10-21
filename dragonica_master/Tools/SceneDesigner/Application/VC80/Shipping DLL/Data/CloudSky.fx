///////////////////////////////////////////////////////////
float4x4 WorldViewProj : WORLDVIEWPROJECTION;
float g_fCloud_Move_Factor <
	string VarType = "Global";
> = 0.5f;
float	g_fCloud_Move_Speed_Divider
<
	string VarType = "Global";
> = 10.0f;
float	g_fcloud_cover
<
	string VarType = "Global";
> = 0.8f;
float	g_fclouds_sharpness
<
	string VarType = "Global";
> = 0.97f;
float4	g_fCloud_Color
<
	string VarType = "Global";
> = float4(1,1,1,1);
float4	g_fSky_Color
<
	string VarType = "Global";
> = float4(0.5,0.5,1,1);

texture Octave1
< 
    string NTM = "shader";
    int NTMIndex = 0;
>;
/*texture Octave2
< 
    string NTM = "shader";
    int NTMIndex = 1;
>;
texture Octave3
< 
    string NTM = "shader";
    int NTMIndex = 2;
>;
texture Octave4
< 
    string NTM = "shader";
    int NTMIndex = 3;
>;
texture Octave5
< 
    string NTM = "shader";
    int NTMIndex = 4;
>;
texture Octave6
< 
    string NTM = "shader";
    int NTMIndex = 5;
>;
texture Octave7
< 
    string NTM = "shader";
    int NTMIndex = 6;
>;
texture Octave8
< 
    string NTM = "shader";
    int NTMIndex = 7;
>;*/

sampler Octave1Sampler = sampler_state 
{
    texture = <Octave1>;
    AddressU  = WRAP;        
    AddressV  = WRAP;
    AddressW  = WRAP;
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};
/*
sampler Octave2Sampler = sampler_state 
{
    texture = <Octave2>;
    AddressU  = WRAP;        
    AddressV  = WRAP;
    AddressW  = WRAP;
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};
sampler Octave3Sampler = sampler_state 
{
    texture = <Octave3>;
    AddressU  = WRAP;        
    AddressV  = WRAP;
    AddressW  = WRAP;
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};
sampler Octave4Sampler = sampler_state 
{
    texture = <Octave4>;
    AddressU  = WRAP;        
    AddressV  = WRAP;
    AddressW  = WRAP;
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};
sampler Octave5Sampler = sampler_state 
{
    texture = <Octave5>;
    AddressU  = WRAP;        
    AddressV  = WRAP;
    AddressW  = WRAP;
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};
sampler Octave6Sampler = sampler_state 
{
    texture = <Octave6>;
    AddressU  = WRAP;        
    AddressV  = WRAP;
    AddressW  = WRAP;
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};
sampler Octave7Sampler = sampler_state 
{
    texture = <Octave7>;
    AddressU  = WRAP;        
    AddressV  = WRAP;
    AddressW  = WRAP;
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};
sampler Octave8Sampler = sampler_state 
{
    texture = <Octave8>;
    AddressU  = WRAP;        
    AddressV  = WRAP;
    AddressW  = WRAP;
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};
*/
///////////////////////////////////////////////////////////
struct VS_OUTPUT
{
    float4 Position   : POSITION;
    float2 TexCoord0  : TEXCOORD0;
};


VS_OUTPUT VS_CloudSky(float4 Position : POSITION, 
				  float2 TexCoord : TEXCOORD0)
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;
    
    OUT.Position = mul(Position, WorldViewProj);
    OUT.TexCoord0 = TexCoord;
    
    return OUT;
}
half4 PS_CloudSky(float2 uv: TEXCOORD0) : COLOR
{   

	float	fMovement = g_fCloud_Move_Factor/g_fCloud_Move_Speed_Divider;
	
	float	ScaleControl = 1;

	float4	OctaveScales0 = float4(1*ScaleControl,2*ScaleControl,4*ScaleControl,8*ScaleControl);
	float4	OctaveScales1 = float4(16*ScaleControl,32*ScaleControl,64*ScaleControl,128*ScaleControl);

	float4	OctaveWeights0 = float4(1,0.5,0.25,0.125);
	float4	OctaveWeights1 = float4(0.0625,0.03125,0.015625,0.0078125);

	float	fMoveScale = 0.05;
	float4	MoveWeights0 = float4(1*fMoveScale,0.5*fMoveScale,0.25*fMoveScale,0.125*fMoveScale);
	float4	MoveWeights1 = float4(0.0625*fMoveScale,0.03125*fMoveScale,0.015625*fMoveScale,0.0078125*fMoveScale);

	float	cloud_cover = 0.6f;
	float	clouds_sharpness = 0.97f;
	float3	tex = tex2D(Octave1Sampler, uv*OctaveScales0.x+fMovement* MoveWeights0.x) * OctaveWeights0.x;
	tex += tex2D(Octave1Sampler, uv*OctaveScales0.y+fMovement* MoveWeights0.y) * OctaveWeights0.y;
	tex += tex2D(Octave1Sampler, uv*OctaveScales0.z+fMovement* MoveWeights0.z) * OctaveWeights0.z;
	tex += tex2D(Octave1Sampler, uv*OctaveScales0.w+fMovement* MoveWeights0.w) * OctaveWeights0.w;

	tex += tex2D(Octave1Sampler, uv*OctaveScales1.x+fMovement* MoveWeights1.x) * OctaveWeights1.x;
	tex += tex2D(Octave1Sampler, uv*OctaveScales1.y+fMovement* MoveWeights1.y) * OctaveWeights1.y;
	tex += tex2D(Octave1Sampler, uv*OctaveScales1.z+fMovement* MoveWeights1.z) * OctaveWeights1.z;
	tex += tex2D(Octave1Sampler, uv*OctaveScales1.w+fMovement* MoveWeights1.w) * OctaveWeights1.w;
	
	tex = max(tex - g_fcloud_cover,0.0f);
	tex = 1.0f - pow(g_fclouds_sharpness,tex*255.0f);
	
	float4	fFinal = lerp(g_fCloud_Color,g_fSky_Color,tex.r);

	return fFinal;
}  

////////////////////////////////////////////////////////////
technique CloudSky
<
    string shadername = "CloudSky";
    bool UsesNiRenderState = true;
>
{

	pass One 
	{
		cullmode = none;
		ZEnable = false;
		ZWriteEnable = false;
		VertexShader = compile vs_1_1 VS_CloudSky();
		PixelShader  = compile ps_2_0 PS_CloudSky();
	}
}

