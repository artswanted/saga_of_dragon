///////////////////////////////////////////////////////////
float4x4 worldViewProjection
<
	string VarType = "Predefined";
	string DefinedMapping = "WORLDVIEWPROJECTION";
>;

texture SourceMap
< 
    string NTM = "shader";
    int NTMIndex = 0;
>;
sampler SourceMapSampler = sampler_state 
{
    texture = <SourceMap>;
    AddressU  = CLAMP;        
    AddressV  = CLAMP;
    AddressW  = CLAMP;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};

///////////////////////////////////////////////////////////
struct VS_OUTPUT
{
    float4 Position   : POSITION;
    float2 TexCoord0  : TEXCOORD0;
};

VS_OUTPUT VS_FSAA2(float4 Position : POSITION, 
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
half4 PS_FSAA2(float2 Tex: TEXCOORD0) : COLOR
{   
	return	tex2D(SourceMapSampler,Tex);
}  

////////////////////////////////////////////////////////////
technique FSAA2_2_0
<
	    bool UsesNiRenderState = true;
	    string shadername = "FSAA2";
	    int Implementation = 0;
>
{
	pass One 
	{
		cullmode = none;
		ZEnable = false;
		ZWriteEnable = false;
		FogEnable = false;
		VertexShader = compile vs_1_1 VS_FSAA2();
		PixelShader  = compile ps_2_0 PS_FSAA2();
	}
}
technique FSAA2_1_4
<
	    bool UsesNiRenderState = true;
	    string shadername = "FSAA2";
	    int Implementation = 1;
>
{
	pass One 
	{
		cullmode = none;
		ZEnable = false;
		ZWriteEnable = false;
		FogEnable = false;
		VertexShader = compile vs_1_1 VS_FSAA2();
		PixelShader  = compile ps_2_0 PS_FSAA2();
	}
}
technique FSAA2_NO_PS
<
	    bool UsesNiRenderState = true;
	    string shadername = "FSAA2";
	    int Implementation = 2;
>
{
	pass One 
	{
		cullmode = none;
		ZEnable = false;
		ZWriteEnable = false;
		FogEnable = false;
		VertexShader = compile vs_1_1 VS_FSAA2();
		PixelShader  = NULL;
		
        Texture[0] = (SourceMap);
        ColorOp[0] = SelectArg1;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
        AlphaOp[0] = SelectArg1;
        AlphaArg1[0] = Texture;
        AlphaArg2[0] = Diffuse;
        AddressU[0] = Wrap;
        AddressV[0] = Wrap;
        MipFilter[0] = LINEAR;
        MinFilter[0] = LINEAR;		
	}
}

