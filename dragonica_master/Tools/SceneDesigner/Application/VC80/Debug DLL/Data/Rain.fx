///////////////////////////////////////////////////////////
float4x4 WorldViewProj : WORLDVIEWPROJECTION;

texture RainTex1
< 
    string NTM = "shader";
    int NTMIndex = 0;
>;

sampler RainTex1Sampler = sampler_state 
{
    texture = <RainTex1>;
    AddressU  = WRAP;        
    AddressV  = WRAP;
    AddressW  = WRAP;
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};
texture RainTex2
< 
    string NTM = "shader";
    int NTMIndex = 0;
>;

sampler RainTex2Sampler = sampler_state 
{
    texture = <RainTex2>;
    AddressU  = WRAP;        
    AddressV  = WRAP;
    AddressW  = WRAP;
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};
float	g_fRain_FrameTime<	string VarType = "Global";> = 0;
float3	g_fRain_CamVel<	string VarType = "Global";> = 0;
float3	g_fRain_Gravity<	string VarType = "Global";> = 0;

///////////////////////////////////////////////////////////
struct VS_OUTPUT
{
    float4 Position   : POSITION;
    float2 TexCoord0  : TEXCOORD0;
    float2 TexCoord1  : TEXCOORD1;
    float2 TexCoord2  : TEXCOORD2;
    float2 TexCoord3  : TEXCOORD3;
};


VS_OUTPUT VS_Rain(float4 Position : POSITION, 
				  float2 TexCoord : TEXCOORD0)
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;
	float	g_fRain_ContFrameTime = 0.033;
	float	g_fRain_Streak  = 0.01;
   
   float2	fDprecip=-float2(0,g_fRain_Streak * g_fRain_FrameTime / g_fRain_ContFrameTime);
   float	fE = 0.2;
   
    OUT.Position = mul(Position, WorldViewProj);


    OUT.TexCoord0 = TexCoord*float2(4,fE*0.5)+fDprecip;
    OUT.TexCoord1 = TexCoord*float2(8,fE*1)+fDprecip*0.6+float2(0.2,0); 
    OUT.TexCoord2 = TexCoord*float2(12,fE*2)+fDprecip*0.3+float2(0.4,0); 
    OUT.TexCoord3 = TexCoord*float2(14,fE*4)+fDprecip*0.1+float2(0.6,0); 
    
    return OUT;
}
half4 PS_Rain(VS_OUTPUT vIn) : COLOR
{   

	float	fColor = max(tex2D(RainTex1Sampler,vIn.TexCoord0).x,tex2D(RainTex1Sampler,vIn.TexCoord1).y);
	fColor = max(fColor,tex2D(RainTex1Sampler,vIn.TexCoord2).z);
	fColor = max(fColor,tex2D(RainTex1Sampler,vIn.TexCoord3).a);
	
	float fAlpha = fColor;
	fColor = saturate(fColor+0.3);

	return float4(fColor,fColor,fColor,fAlpha);
}  

////////////////////////////////////////////////////////////
technique RainShader
<
    string shadername = "RainShader";
    bool UsesNiRenderState = true;
>
{

	pass One 
	{
		cullmode = none;
		VertexShader = compile vs_1_1 VS_Rain();
		PixelShader  = compile ps_1_4 PS_Rain();
	}
}

