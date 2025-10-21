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

///////////////////////////////////////////////////////////
struct VS_OUTPUT
{
    float4 Position   : POSITION;
    float2 TexCoord0  : TEXCOORD0;
};


VS_OUTPUT VS_RainDrop(float4 Position : POSITION, 
				  float2 TexCoord : TEXCOORD0)
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;
    OUT.Position = mul(Position, WorldViewProj);
    OUT.TexCoord0 = TexCoord;
    
    return OUT;
}
half4 PS_RainDrop(VS_OUTPUT vIn) : COLOR
{   
	return	tex2D(RainTex1Sampler,vIn.TexCoord0);
}  

////////////////////////////////////////////////////////////
technique RainDropShader
<
    string shadername = "RainDropShader";
    bool UsesNiRenderState = true;
>
{

	pass One 
	{
		cullmode = none;
		VertexShader = compile vs_1_1 VS_RainDrop();
		PixelShader  = compile ps_1_4 PS_RainDrop();
	}
}

