///////////////////////////////////////////////////////////
float4x4 WorldViewProj : WORLDVIEWPROJECTION;
float4x4 WorldMat	:	WORLD;
float4x4 ProjMat	:	PROJECTION;
float4x4 ReflectTexTransform<string VarType = "Global";>;
texture UnderWater
< 
    string NTM = "shader";
    int NTMIndex = 0;
>;
texture UponWater
< 
    string NTM = "shader";
    int NTMIndex = 1;
>;
texture WaterBump1
< 
    string NTM = "shader";
    int NTMIndex = 2;
>;
texture WaterBump2
< 
    string NTM = "shader";
    int NTMIndex = 3;
>;
texture WaterDetail
< 
    string NTM = "shader";
    int NTMIndex = 4;
>;

float g_fWaterTime<	string VarType = "Global";> = 0;
float3 g_fCameraPos<	string VarType = "Global";> = 0;

//	물이 흐르는 속도 비율( 0 : 물이 흐르지 않음   0~:흐르는 속도 증가)
float	g_fWaterSpeed<	string VarType = "Global";> = 0.7f;
//	물결의 크기 비율
float	g_fWaterWaveSize<	string VarType = "Global";> = 0.05;
//	반사비율(1.0 : 완전반사, 0.0 : 반사 없음)
float	g_fWaterReflect<	string VarType = "Global";> = 0.3f;
//	물결 높이 비율(0: 물결 높이 없음 0~ : 물결 높이 증가)
float	g_fWaterWaveHeight<	string VarType = "Global";> = 0.35f;
//	해조 농도(0: 해조 없음 1:해조만 보임)
float	g_fWaterDetailDensity<	string VarType = "Global";> = 0.2f;

sampler sUnderWater = sampler_state 
{
    Texture = <UnderWater>;
    AddressU  = CLAMP;        
    AddressV  = CLAMP;
    AddressW  = CLAMP;
    MIPFILTER = LINEAR;
    MINFILTER = ANISOTROPIC;
    MAGFILTER = LINEAR;
};
sampler sUponWater = sampler_state 
{
    Texture = <UponWater>;
    AddressU  = MIRROR;        
    AddressV  = MIRROR;
    AddressW  = MIRROR;
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};
sampler sWaterBump1 = sampler_state 
{
    Texture = <WaterBump1>;
    AddressU  = WRAP;        
    AddressV  = WRAP;
    AddressW  = WRAP;
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};
sampler sWaterBump2 = sampler_state 
{
    Texture = <WaterBump2>;
    AddressU  = WRAP;        
    AddressV  = WRAP;
    AddressW  = WRAP;
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};
sampler sWaterDetail = sampler_state 
{
    Texture = <WaterDetail>;
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
    float4 TexCoord1  : TEXCOORD1;
    float4 TexCoord2  : TEXCOORD2;
    float4 TexCoord3  : TEXCOORD3;
    float4 TexCoord4  : TEXCOORD4;
    float4 TexCoord5  : TEXCOORD5;
    float4 TexCoord6  : TEXCOORD6;
    float4	Diffuse	:	Color;
};

VS_OUTPUT VS_WaterShader(float4 Position : POSITION)
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;
   
	float4 g_fBumpNormalMove1= float4(0.0f, 0.045f, 0.0f, 0.0f);
	float4 g_fBumpNormalScalar1= float4(0.15f, 0.15f, 0.0f, 0.0f);

	float4 g_fBumpNormalMove2= float4(0.0f, 0.015f, 0.0f, 0.0f);
	float4 g_fBumpNormalScalar2= float4(0.17f, 0.17f, 0.0f, 0.0f);

	float4 g_fBumpNormalMove3= float4(0.0f, 0.090f, 0.0f, 0.0f);
	float4 g_fBumpNormalScalar3= float4(0.50f, 0.50f, 0.0f, 0.0f);

	float4 g_fBumpNormalMove4= float4(0.0f, 0.030f, 0.0f, 0.0f);
	float4 g_fBumpNormalScalar4= float4(0.55f, 0.55f, 0.0f, 0.0f);    
    
    float2 Tex0=0,Tex1=0,Tex2=0,Tex3=0;
    
    OUT.Position = mul(Position, WorldViewProj);
    
    float4x4	trafo;
    
    trafo._m00 = 0.5;	trafo._m01 = 0;		trafo._m02 = 0; trafo._m03 = 0;
    trafo._m10 = 0;		trafo._m11 = -0.5;	trafo._m12 = 0; trafo._m13 = 0;
    trafo._m20 = 0;		trafo._m21 = 0;		trafo._m22 = 0; trafo._m23 = 0;
    trafo._m30 = 0.5;	trafo._m31 = 0.5;	trafo._m32 = 1; trafo._m33 = 1;    
    
    float4 r9 = 0.0;
   
    float4x4	tansform_matrix = transpose(mul(WorldMat,ReflectTexTransform));
    
    r9.x = dot(Position,float4(tansform_matrix._m00,tansform_matrix._m01,tansform_matrix._m02,tansform_matrix._m03));
    r9.y = dot(Position,float4(tansform_matrix._m10,tansform_matrix._m11,tansform_matrix._m12,tansform_matrix._m13));
    r9.zw = dot(Position,float4(tansform_matrix._m20,tansform_matrix._m21,tansform_matrix._m22,tansform_matrix._m23));
    OUT.TexCoord1 = r9;

	tansform_matrix = transpose(mul(WorldViewProj,trafo));
	
	r9.x = dot(Position,float4(tansform_matrix._m00,tansform_matrix._m01,tansform_matrix._m02,tansform_matrix._m03));
	r9.y = dot(Position,float4(tansform_matrix._m10,tansform_matrix._m11,tansform_matrix._m12,tansform_matrix._m13));
	r9.zw = dot(Position,float4(tansform_matrix._m20,tansform_matrix._m21,tansform_matrix._m22,tansform_matrix._m23));

	OUT.TexCoord2 = r9;
    OUT.TexCoord3.xy = Tex3;

	Tex0 = Position * g_fWaterWaveSize;
	Tex3 = Tex0 * 0.5;
	g_fWaterTime *= g_fWaterSpeed;
	
	float4 fFractional = g_fWaterTime*g_fBumpNormalMove1;
	fFractional.xy = frac(fFractional);
	OUT.TexCoord0.xy = (fFractional+Tex0*g_fBumpNormalScalar1);
	
	fFractional = g_fWaterTime*g_fBumpNormalMove2;
	fFractional.xy = frac(fFractional);
	OUT.TexCoord4.xy =(fFractional+Tex0*g_fBumpNormalScalar2);	
	
	fFractional = g_fWaterTime*g_fBumpNormalMove3;
	fFractional.xy = frac(fFractional);
	OUT.TexCoord5.xy = (fFractional+Tex0*g_fBumpNormalScalar3);
	
	fFractional = g_fWaterTime*g_fBumpNormalMove4;
	fFractional.xy = frac(fFractional);
	OUT.TexCoord6.xy = (fFractional+Tex0*g_fBumpNormalScalar4);
	
	float3	WorldPos = mul(Position,WorldMat);
	float3	vCamToWater = normalize(WorldPos -g_fCameraPos);
	
	OUT.Diffuse.xyz = vCamToWater*0.5+0.5;
		    
    return OUT;
}
float4 mad(float4 s1,float4 s2,float4 s3)
{
	return s1*s2+s3;
}
half4 PS_WaterShader(VS_OUTPUT In) : COLOR
{   

	float4 g_fBumpNormalScale1=float4(0.65f, 0.65f, 2.0f, 0.0f);
	float4 g_fBumpNormalScale2= float4(0.45f, 0.45f, 2.0f, 0.0f);
	float4 g_fBumpNormalScale3=float4(0.16f, 0.16f, 2.0f, 0.0f);
	float4 g_fBumpNormalScale4=float4(0.14f, 0.14f, 2.0f, 0.0f);
	float4 g_fWeightColorVectorShift= float4(-0.7f, -0.7f, -4.0f, 0.0f);
	float4 g_fColorVectorShift= float4(-0.5f, -0.5f, -0.5f, 0.0f);
	float4 g_fBumpReflectShift= float4(0.0f, -0.25f, 0.0f, 0.0f);
	float4 g_fReflectMapBumpScale= float4(-0.25f, -0.25f, 1.0f, 0.0f);
	float4 g_fRefractMapBumpScale=float4(0.10f, 0.10f, 1.0f, 0.0f);

	float4	c10 = float4(0.0204,0.9796f,0.3,1.0);
	float4	c14 = float4(0,0,1,0);
	float4	c15 = float4(0,1,0,0);
	
	float4	r0 = tex2D(sWaterBump1,In.TexCoord0);
	float4	r4 = tex2D(sWaterBump1,In.TexCoord4);
	float4	r5 = tex2D(sWaterBump2,In.TexCoord5);
	float4	r6 = tex2D(sWaterBump2,In.TexCoord6);
	
	r6.rgb = r6*g_fBumpNormalScale4;
	r5.rgb = r5*g_fBumpNormalScale3+r6;
	r4.rgb = r4*g_fBumpNormalScale2+r5;
	r5.rgb = r0*g_fBumpNormalScale1+r4;
	r5.rgb = r5 + g_fWeightColorVectorShift;
	
	float4 r7 = mad(r5+g_fBumpReflectShift,g_fReflectMapBumpScale*In.TexCoord1.w*g_fWaterWaveHeight,In.TexCoord1);
	float4 r8 = mad(r5,g_fRefractMapBumpScale*In.TexCoord2.w*g_fWaterWaveHeight,In.TexCoord2);
	
	float4 r1 = tex2Dproj(sUponWater,r7);
	float4 r2 = tex2Dproj(sUnderWater,r8);
	
	r2.a = (1-g_fWaterReflect);

	r6.rgb = normalize(r5);
	
	r5.r = r6.r;
	r5.g = dot(r6,c14);
	r5.b = dot(r6,c15);
	
	r0.rgb = r5 * c10.b;
	r0.rgb = r0 + In.TexCoord3;
	float4 r3 = tex2D(sWaterDetail,r0);
	
	r7.rgb = In.Diffuse + g_fColorVectorShift;
	r6.rgb = normalize(r7);
	
	r7.a = 1 - saturate(dot(r6,r5));
	
	r8.a = c10.a - r7.a;
	r6.a = r8.a * r8.a;
	r6.a = r6.a * r6.a;
	r6.a = r6.a * r8.a;
	r6.a = c10.g * r6.a+c10.r;
	
	r6.a = r6.a * r2.a;
	r7.rgb = saturate(lerp(r1,r2,r6.a));
	
	r8.a = (1-g_fWaterDetailDensity);
	r8.rgb = saturate(lerp(r3,r7,r8.a));
	
	return r8;
}  

////////////////////////////////////////////////////////////
technique WaterShader
<
    string shadername = "WaterShader";
    bool UsesNiRenderState = true;
>
{
	pass One 
	{
		cullmode = none;
		ZEnable = true;
		ZWriteEnable = true;
		VertexShader = compile vs_2_0 VS_WaterShader();
		PixelShader  = compile ps_2_0 PS_WaterShader();
	}
}

