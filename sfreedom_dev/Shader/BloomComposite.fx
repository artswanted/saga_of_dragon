///////////////////////////////////////////////////////////

float4x4 WorldViewProj : WORLDVIEWPROJECTION;
texture FrameBuffer
< 
    string NTM = "shader";
    int NTMIndex = 0;
>;
texture BluredBuffer
< 
    string NTM = "shader";
    int NTMIndex = 1;
>;
sampler sFrameBuffer = sampler_state 
{
    Texture = <FrameBuffer>;
    AddressU  = CLAMP;        
    AddressV  = CLAMP;
    AddressW  = CLAMP;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};
sampler sBluredBuffer = sampler_state 
{
    Texture = <BluredBuffer>;
    AddressU  = CLAMP;        
    AddressV  = CLAMP;
    AddressW  = CLAMP;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};

float Bloom_SceneIntensity : GLOBAL = 0.5f;
float Bloom_BloomIntensity : GLOBAL = 0.5f;
float Bloom_HighlightIntensity : GLOBAL = 0.5f;

///////////////////////////////////////////////////////////
struct VS_OUTPUT
{
    float4 Position   : POSITION;
    float2 TexCoord0  : TEXCOORD0;
};

VS_OUTPUT VS_BloomComposite(float4 Position : POSITION, 
				  float2 TexCoord : TEXCOORD0)
{
    VS_OUTPUT OUT;
    OUT.Position = mul(Position, WorldViewProj);
    OUT.TexCoord0 = TexCoord;
	 return OUT;
}

half4 PS_BloomComposite(VS_OUTPUT IN) : COLOR
{   

	float4	colorSumOriginal = tex2D(sFrameBuffer,IN.TexCoord0);
	float4	colorSumBlurred = tex2D(sBluredBuffer,IN.TexCoord0);
	
	float	fAlpha = saturate(colorSumBlurred.a+0.001);
	
	colorSumBlurred.rgb = colorSumBlurred.rgb * (1/fAlpha);
	
	float	fbloominten = saturate(Bloom_BloomIntensity * colorSumBlurred.a);
	
	return saturate(colorSumOriginal*(1-fbloominten)+colorSumBlurred*fbloominten);
}  

////////////////////////////////////////////////////////////
technique BloomComposite
<
    string shadername = "BloomComposite";
    bool UsesNiRenderState = true;
    bool UsesNiLightState = false;
    int Implementation = 0;
>
{
	pass One 
	{
		cullmode = none;
		ZEnable = false;
		ZWriteEnable = false;
		FogEnable = false;
		VertexShader = compile vs_1_1 VS_BloomComposite();
		PixelShader  = compile ps_2_0 PS_BloomComposite();
	}
}

