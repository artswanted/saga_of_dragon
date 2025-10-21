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
static const float2 QuadScreenSize = { 1024.0, 768.0 };
static const float2 BlurStart = {1,1};

float2 g_fRadialBlurCenter : GLOBAL= { 0.5, 0.6 };
float2 g_fRadialBlurWidth : GLOBAL= { 40.0/1024.0, 40.0/768.0 };

///////////////////////////////////////////////////////////
struct VS_OUTPUT
{
    float4 Position   : POSITION;
    float2 TexCoord0  : TEXCOORD0;
};

VS_OUTPUT VS_RADIAL_BLUR(float4 Position : POSITION, 
				  float2 TexCoord : TEXCOORD0)
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;
    
    OUT.Position = mul(Position, worldViewProjection);
    
 	float2 texelSize = {1.0 / QuadScreenSize.x,1.0 / QuadScreenSize.y};
 	OUT.TexCoord0 = TexCoord  - g_fRadialBlurCenter;
    
    return OUT;
}
float lumi(float3 color)
{
	return dot(color,float3(0.3,0.59,0.11));
}
half4 PS_RADIAL_BLUR(float2 Tex: TEXCOORD0,uniform int nsamples) : COLOR
{   
    half4 c = 0;
    // this loop will be unrolled by compiler and the constants precalculated:
    for(int i=0; i<nsamples; i++) {
    	float2 scale = BlurStart + g_fRadialBlurWidth*(i/(float) (nsamples-1));
    	c += tex2D(SourceMapSampler, Tex*scale + g_fRadialBlurCenter );
   	}
   	c /= nsamples;
    return c;
}  

////////////////////////////////////////////////////////////
technique RADIAL_BLUR_2_0
<
	    bool UsesNiRenderState = true;
	    string shadername = "RADIAL_BLUR";
	    int Implementation = 0;
>
{
	pass One 
	{
		cullmode = none;
		ZEnable = false;
		ZWriteEnable = false;
		FogEnable = false;
		VertexShader = compile vs_2_0 VS_RADIAL_BLUR();
		PixelShader  = compile ps_2_0 PS_RADIAL_BLUR(16);
	}
}
