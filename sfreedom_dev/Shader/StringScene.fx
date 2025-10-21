
float4x4 kWorldViewProj             : WORLDVIEWPROJECTION;
float4   kMatDiffuse            : MATERIALDIFFUSE;

texture  BaseTex
< 
    string NTM = "base";
>;

void	StringSceneVS(float4 iPos : POSITION,
						float2 iTex : TEXCOORD0,
						float4 kColor : COLOR0,
						out float4 oPos : POSITION,
						out float2 oTex : TEXCOORD0,
						out float4 oColor : COLOR0)
{
	oTex = iTex;
	oPos = mul( iPos, kWorldViewProj );
	oColor = kColor * kMatDiffuse;
}

technique StringScene
<
    bool UsesNiRenderState = true;
    bool UsesNiLightState = false;
>
{
    pass p0
    {
        VertexShader = compile vs_1_1 StringSceneVS();
        PixelShader  = NULL;
        
        Texture[0] = (BaseTex);
        ColorOp[0] = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
        AlphaOp[0] = Modulate;
        AlphaArg1[0] = Texture;
        AlphaArg2[0] = Diffuse;
        AddressU[0] = Wrap;
        AddressV[0] = Wrap;
        MipFilter[0] = LINEAR;
        MinFilter[0] = LINEAR;
        
    }
}
