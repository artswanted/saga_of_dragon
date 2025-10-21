///////////////////////////////////////////////////////////
float4x4 WorldViewProj : WORLDVIEWPROJECTION;

texture SourceMap 
< 
    string NTM = "shader";
    int NTMIndex = 0;
>;
texture LuminianceMap
< 
    string NTM = "shader";
    int NTMIndex = 1;
>;
sampler SourceMapSampler = sampler_state 
{
    texture = <SourceMap>;
    AddressU  = CLAMP;        
    AddressV  = CLAMP;
    AddressW  = CLAMP;
};
sampler LuminianceMapSampler = sampler_state 
{
    texture = <LuminianceMap>;
    AddressU  = CLAMP;        
    AddressV  = CLAMP;
    AddressW  = CLAMP;
};

///////////////////////////////////////////////////////////
struct VS_OUTPUT
{
    float4 Position   : POSITION;
    float2 TexCoord0  : TEXCOORD0;
};

VS_OUTPUT VS_DownSample4x(float4 Position : POSITION, 
				  float2 TexCoord : TEXCOORD0)
{
    VS_OUTPUT OUT;
    OUT.Position = mul(Position, WorldViewProj);
    OUT.TexCoord0 = TexCoord;
	 return OUT;
}
float lumi(float3 color)
{
	return dot(color,float3(0.3,0.59,0.11));
}
half4 PS_DownSample4x(float2 Tex: TEXCOORD0) : COLOR
{   
	float4 Color_frame = tex2D(SourceMapSampler,Tex);
	
	Color_frame.a = 1;
	float flum1 = lumi(Color_frame.rgb);
	float flum2 = tex2D(LuminianceMapSampler,float2(flum1,0.5)).r;
	
	return Color_frame*flum2*flum1;
}  

////////////////////////////////////////////////////////////
technique downsample4x
<
	    bool UsesNiRenderState = true;
	    bool UsesNiLightState = false;
>
{
	pass One 
	{
		cullmode = none;
		ZEnable = false;
		ZWriteEnable = false;
		FogEnable = false;
		VertexShader = compile vs_1_1 VS_DownSample4x();
		PixelShader  = compile ps_2_0 PS_DownSample4x();
	}
}

