///////////////////////////////////////////////////////////

#define NUM_DOF_TAPS	12

float	maxCoC <	string VarType = "Global";> = 0;
float2	filterTaps[NUM_DOF_TAPS]<	string VarType = "Global";>;

struct	PS_INPUT
{
	float2	vTexCoord:	TEXCOORD;
};

float4x4 WorldViewProj : WORLDVIEWPROJECTION;
texture FrameBuffer
< 
    string NTM = "shader";
    int NTMIndex = 0;
>;
texture DOFBuffer
< 
    string NTM = "shader";
    int NTMIndex = 1;
>;
texture BluredBuffer
< 
    string NTM = "shader";
    int NTMIndex = 2;
>;
sampler sFrameBuffer = sampler_state 
{
    Texture = <FrameBuffer>;
    AddressU  = CLAMP;        
    AddressV  = CLAMP;
    AddressW  = CLAMP;
    MIPFILTER = POINT;
    MINFILTER = POINT;
    MAGFILTER = POINT;
};
sampler sDOFBuffer = sampler_state 
{
    Texture = <DOFBuffer>;
    AddressU  = CLAMP;        
    AddressV  = CLAMP;
    AddressW  = CLAMP;
    MIPFILTER = POINT;
    MINFILTER = POINT;
    MAGFILTER = POINT;
};
sampler sBluredBuffer = sampler_state 
{
    Texture = <BluredBuffer>;
    AddressU  = CLAMP;        
    AddressV  = CLAMP;
    AddressW  = CLAMP;
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = POINT;
};
///////////////////////////////////////////////////////////
struct VS_OUTPUT
{
    float4 Position   : POSITION;
    float2 TexCoord0  : TEXCOORD0;
};

VS_OUTPUT VS_DepthOfField(float4 Position : POSITION, 
				  float2 TexCoord : TEXCOORD0)
{
    VS_OUTPUT OUT;
    OUT.Position = mul(Position, WorldViewProj);
    OUT.TexCoord0 = TexCoord;
	 return OUT;
}

half4 PS_DepthOfField(VS_OUTPUT IN) : COLOR
{   

	float4	colorSumOriginal = tex2D(sFrameBuffer,IN.TexCoord0);
	float4	colorSumBlurred = tex2D(sBluredBuffer,IN.TexCoord0);
	float2	centerDepthBlur = tex2D(sDOFBuffer,IN.TexCoord0);
	float	sizeCoC = centerDepthBlur.y * maxCoC;
	float4	result = saturate(colorSumOriginal*(1-centerDepthBlur.y)+colorSumBlurred*centerDepthBlur.y);
	return	float4(result.xyz,1);

}  

////////////////////////////////////////////////////////////
technique DepthOfField
<
    string shadername = "DepthOfField";
    bool UsesNiRenderState = true;
    int Implementation = 0;
>
{
	pass One 
	{
		cullmode = none;
		ZEnable = false;
		ZWriteEnable = false;
		FogEnable = false;
		VertexShader = compile vs_1_1 VS_DepthOfField();
		PixelShader  = compile ps_2_0 PS_DepthOfField();
	}
}

