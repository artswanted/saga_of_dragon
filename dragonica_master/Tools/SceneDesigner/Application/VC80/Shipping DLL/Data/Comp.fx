///////////////////////////////////////////////////////////
float g_fSceneIntensity <
   string VarType = "Global"; 
> = 0.5f;

float g_fGlowIntensity <
	string VarType = "Global";
> = 0.5f;


float4x4 worldViewProjection
<
	string VarType = "Predefined";
	string DefinedMapping = "WORLDVIEWPROJECTION";
>;

///////////////////////////////////////////////////////////
texture SceneMap : RENDERCOLORTARGET
< 
    float2 ViewportRatio = { 1.0, 1.0 };
    int MIPLEVELS = 1;
    string format = "A8R8G8B8";
>;

sampler SceneSampler = sampler_state 
{
    texture = <SceneMap>;
    AddressU  = CLAMP;        
    AddressV  = CLAMP;
    AddressW  = CLAMP;
    MIPFILTER = POINT;
    MINFILTER = POINT;
    MAGFILTER = POINT;
};

texture FinalBlurMap : RENDERCOLORTARGET
< 
    float2 ViewportRatio = { 1, 1 };
    int MIPLEVELS = 1;
    string format = "A8R8G8B8";
>;

sampler FinalBlurSampler = sampler_state 
{
    texture = <FinalBlurMap>;
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
    float2 TexCoord1  : TEXCOORD1;
};

VS_OUTPUT VS_Quad(float4 Position : POSITION, 
				  float2 TexCoord : TEXCOORD0)
{
    VS_OUTPUT OUT;
    
//	 OUT.Position = Position;

    OUT.Position = mul(Position, worldViewProjection);

    OUT.TexCoord0 = TexCoord;
    OUT.TexCoord1 = TexCoord;
    
	 return OUT;
}
float lumi(float3 color)
{
	return dot(color,float3(0.3,0.59,0.11));
}

half4 PS_Comp(VS_OUTPUT IN,
			  uniform sampler2D sceneSampler,
			  uniform sampler2D blurredSceneSampler) : COLOR
{   
	half4 orig = tex2D(sceneSampler, IN.TexCoord0);
	half4 blur = tex2D(blurredSceneSampler, IN.TexCoord1);
	return saturate(g_fSceneIntensity*orig + blur*g_fGlowIntensity);

}  

////////////////////////////////////////////////////////////
technique comp
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
		VertexShader = compile vs_1_1 VS_Quad();
		PixelShader  = compile ps_2_0 PS_Comp(SceneSampler, FinalBlurSampler);
	}
}

