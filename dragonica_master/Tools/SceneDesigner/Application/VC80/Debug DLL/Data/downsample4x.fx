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
    MIPFILTER = POINT;
    MINFILTER = POINT;
    MAGFILTER = POINT;
};

texture ShadeMap : RENDERCOLORTARGET
< 
    float2 ViewportRatio = { 1, 1 };
    int MIPLEVELS = 1;
    string format = "A8R8G8B8";
>;

sampler ShadeMapSampler = sampler_state 
{
    texture = <ShadeMap>;
    AddressU  = CLAMP;        
    AddressV  = CLAMP;
    AddressW  = CLAMP;
    MIPFILTER = POINT;
    MINFILTER = POINT;
    MAGFILTER = POINT;
};

///////////////////////////////////////////////////////////
struct VS_OUTPUT
{
    float4 Position   : POSITION;
    float2 TexCoord0  : TEXCOORD0;
};
float g_fHighlightIntensity <
	string VarType = "Global";
> = 0.5f;

VS_OUTPUT VS_DownSample4x(float4 Position : POSITION, 
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
half4 PS_DownSample4x(float2 Tex: TEXCOORD0) : COLOR
{   
	float4 color = tex2D(TargetSampler,Tex);
	float fSrcLumi = exp(lumi(color.xyz));
	//float fLumi = tex2D(ShadeMapSampler,float2(fSrcLumi,0)).x;	//	High Pass Filtering
	return float4(color.xyz*(fSrcLumi),1);//+1+fLumi*g_fHighlightIntensity);	
}  

////////////////////////////////////////////////////////////
technique downsample4x
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
		VertexShader = compile vs_1_1 VS_DownSample4x();
		PixelShader  = compile ps_2_0 PS_DownSample4x();
	}
}

