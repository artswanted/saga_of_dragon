// 텍스처

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

sampler AnisotropicSampler = sampler_state
{
    Texture = <SceneMap>;
    MinFilter = Anisotropic;
    MagFilter = Linear;
    
    MaxAnisotropy = 4;
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
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};

float fass_y0,fass_y1,fass_y2,fass_y4,fass_y5,fass_y6;

// 픽셀쉐이더
float4 BX(    float2 Tex : TEXCOORD0   ) : COLOR
{
	//  오리지날 코드.
    //float BlurWeight[7] = { 0.1, 0.358, 0.773, 1.0, 0.773, 0.358, 0.1 };
    //float BlurN = 3;
    //float4 Color = 0;
    //Color += BlurWeight[0] * ( tex2D( TargetSampler, Tex + float2( -3.0 / MAP_CX, 0 ) ) );
    //Color += BlurWeight[1] * ( tex2D( TargetSampler, Tex + float2( -2.0 / MAP_CX, 0 ) ) );
    //Color += BlurWeight[2] * ( tex2D( TargetSampler, Tex + float2( -1.0 / MAP_CX, 0 ) ) );
    //Color += BlurWeight[3] * ( tex2D( TargetSampler, Tex + float2(  0.0 / MAP_CX, 0 ) ) );
    //Color += BlurWeight[4] * ( tex2D( TargetSampler, Tex + float2( +1.0 / MAP_CX, 0 ) ) );
    //Color += BlurWeight[5] * ( tex2D( TargetSampler, Tex + float2( +2.0 / MAP_CX, 0 ) ) );
    //Color += BlurWeight[6] * ( tex2D( TargetSampler, Tex + float2( +3.0 / MAP_CX, 0 ) ) );
    //return Color/BlurN;   
    
   float4 Color = 0;
   //Color +=  0.0333f * ( tex2D( TargetSampler, Tex + float2(  fass_y0,0) ) );
   Color +=  0.1193f * ( tex2D( TargetSampler, Tex + float2(  fass_y1,0 ) ) );
   Color +=  0.2576f * ( tex2D( TargetSampler, Tex + float2(  fass_y2,0) ) );
   Color +=  0.3333f * ( tex2D( TargetSampler, Tex + float2(  0 , 0) ) );
   Color +=  0.2576f * ( tex2D( TargetSampler, Tex + float2(  fass_y4 ,0) ) );
   Color +=  0.1193f * ( tex2D( TargetSampler, Tex + float2(  fass_y5 ,0) ) );
   //Color +=  0.0333f * ( tex2D( TargetSampler, Tex + float2(  fass_y6 ,0) ) );
   return Color;
}   

float4 BY(    float2 Tex : TEXCOORD0   ) : COLOR
{
   //  오리지날 코드.
   //float BlurWeight[7] = { 0.1, 0.358, 0.773, 1.0, 0.773, 0.358, 0.1 };
   //float BlurN = 3;
   //float4 Color = 0;
   //Color += BlurWeight[0] * ( tex2D( TargetSampler, Tex + float2( 0, -3.0 / MAP_CY ) ) );
   //Color += BlurWeight[1] * ( tex2D( TargetSampler, Tex + float2( 0, -2.0 / MAP_CY ) ) );
   //Color += BlurWeight[2] * ( tex2D( TargetSampler, Tex + float2( 0, -1.0 / MAP_CY ) ) );
   //Color += BlurWeight[3] * ( tex2D( TargetSampler, Tex + float2( 0,  0.0 / MAP_CY ) ) );
   //Color += BlurWeight[4] * ( tex2D( TargetSampler, Tex + float2( 0, +1.0 / MAP_CY ) ) );
   //Color += BlurWeight[5] * ( tex2D( TargetSampler, Tex + float2( 0, +2.0 / MAP_CY ) ) );
   //Color += BlurWeight[6] * ( tex2D( TargetSampler, Tex + float2( 0, +3.0 / MAP_CY ) ) );
   //return Color/BlurN;
   
   
   float4 Color = 0;
   //Color +=  0.0333f  * ( tex2D( TargetSampler, Tex + float2( 0, fass_y0 ) ) );
   Color +=  0.1193f  * ( tex2D( TargetSampler, Tex + float2( 0, fass_y1 ) ) );
   Color +=  0.2576f  * ( tex2D( TargetSampler, Tex + float2( 0, fass_y2 ) ) );
   Color +=  0.3333f  * ( tex2D( TargetSampler, Tex + float2( 0,  0 ) ) );
   Color +=  0.2576f  * ( tex2D( TargetSampler, Tex + float2( 0, fass_y4 ) ) );
   Color +=  0.1193f  * ( tex2D( TargetSampler, Tex + float2( 0, fass_y5 ) ) );
   //Color +=  0.0333f  * ( tex2D( TargetSampler, Tex + float2( 0, fass_y6 ) ) );
   return Color;
}

// 픽셀쉐이더
float4 PSMix(    float2 Tex0 : TEXCOORD0, float2 Tex1 : TEXCOORD1   ) : COLOR
{
   return tex2D(SceneSampler, Tex0) * 0.5f + tex2D(SceneSampler, Tex1) * 0.5f;
  // return tex2D(Sampler1, Tex0);
 
}   

float4 PSPower(    float2 Tex0 : TEXCOORD0   ) : COLOR
{
 	float4 Color;
    Color = tex2D( TargetSampler, Tex0 );
    Color = pow( Color, 32 );
    //Color = pow( Color, 2 );
    return Color;    
}

float4 TextureAnisotropicPS( float4 TexCoord : TEXCOORD0 ) : COLOR0
{
	float4 Color = 0;
	Color +=  0.08f  * ( tex2D( AnisotropicSampler, TexCoord + float4(-0.0015f, -0.0015f, 0.0f, 0.0f) ) );
	Color +=  0.08f  * ( tex2D( AnisotropicSampler, TexCoord + float4(-0.0015f, 0.0015f, 0.0f, 0.0f)) );
	Color +=  ( tex2D( AnisotropicSampler, TexCoord) ) * 0.68f;
	Color +=  0.08f  * ( tex2D( AnisotropicSampler, TexCoord + float4(0.0015f, -0.0015f, 0.0f, 0.0f)) );
	Color +=  0.08f  * ( tex2D( AnisotropicSampler, TexCoord + float4(0.0015f, 0.0015f, 0.0f, 0.0f)) );
	//Color = Color * 0.8f;
	Color = Color * 0.55f + tex2D( SceneSampler, TexCoord) * 0.47f;
    return Color;
}

// 테크닉 선언(정점 & 픽셀 쉐이더 사용)
technique Power
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
		VertexShader = NULL;
		PixelShader  = compile ps_1_4 PSPower();
	}
}

technique Blend
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
		VertexShader = NULL;
		PixelShader  = compile ps_1_4 PSMix();
	}
}

technique BlurY
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
		VertexShader = NULL;
		PixelShader  = compile ps_2_0 BY();
	}
}

technique BlurX
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
		VertexShader = NULL;
		PixelShader  = compile ps_2_0 BX();
	}
}

technique AnisotropicAA
<
	    bool UsesNiRenderState = true;
>
{
	pass P0
	{
		cullmode = none;
		ZEnable = false;
		ZWriteEnable = false;
		FogEnable = false;
		VertexShader = NULL;
		PixelShader  = compile ps_2_0 TextureAnisotropicPS();
	}
}
