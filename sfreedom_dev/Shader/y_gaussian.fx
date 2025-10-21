///////////////////////////////////////////////////////////
float4x4 worldViewProjection
<
	string VarType = "Predefined";
	string DefinedMapping = "WORLDVIEWPROJECTION";
>;

texture SourceMap : RENDERCOLORTARGET
< 
    float2 ViewportRatio = { 1, 1 };
    int MIPLEVELS = 1;
    string format = "A8R8G8B8";
>;

sampler SourceSampler = sampler_state 
{
    texture = <SourceMap>;
    AddressU  = CLAMP;        
    AddressV  = CLAMP;
    AddressW  = CLAMP;
    MIPFILTER = POINT;
    MINFILTER = POINT;
    MAGFILTER = POINT;
};

float2 vertTapOffs[7]
<
	string VarType = "Global";
>;

///////////////////////////////////////////////////////////
struct VS_OUTPUT_TEX7
{
    float4 vPos  : POSITION;
    float2 vTap0  : TEXCOORD0;
    float2 vTap1  : TEXCOORD1;
    float2 vTap2  : TEXCOORD2;
    float2 vTap3  : TEXCOORD3;
    float2 vTap1Neg  : TEXCOORD4;
    float2 vTap2Neg  : TEXCOORD5;
    float2 vTap3Neg  : TEXCOORD6;
};

struct VS_INPUT
{
	float4 vPos:	POSITION;
	float2 vTexCoord:	TEXCOORD;
};

VS_OUTPUT_TEX7 filter_gaussian_y_vs(VS_INPUT v)
{
    VS_OUTPUT_TEX7 OUT = (VS_OUTPUT_TEX7)0;
    
    OUT.vPos = mul(v.vPos, worldViewProjection);
    OUT.vTap0 = v.vTexCoord;
    OUT.vTap1 = v.vTexCoord + vertTapOffs[1];
    OUT.vTap2 = v.vTexCoord + vertTapOffs[2];
    OUT.vTap3 = v.vTexCoord + vertTapOffs[3];
    OUT.vTap1Neg = v.vTexCoord - vertTapOffs[1];
    OUT.vTap2Neg = v.vTexCoord - vertTapOffs[2];
    OUT.vTap3Neg = v.vTexCoord - vertTapOffs[3];
    
    return OUT;
}
float lumi(float3 color)
{
	return dot(color,float3(0.3,0.59,0.11));
}
struct PS_INPUT_TEX7
{
	float2 vTap0:	TEXCOORD0;
	float2 vTap1:	TEXCOORD1;
	float2 vTap2:	TEXCOORD2;
	float2 vTap3:	TEXCOORD3;
	float2 vTap1Neg:	TEXCOORD4;
	float2 vTap2Neg:	TEXCOORD5;
	float2 vTap3Neg:	TEXCOORD6;
};
half4 filter_gaussian_y_ps(PS_INPUT_TEX7 v) : COLOR
{   
	float4 s0,s1,s2,s3,s4,s5,s6;
	float4 vColorWeightSum;
	
	float4 vWeights0 = { 0.3, 0.2, 0.1, 0.4};
	float4 vWeights1 = { 0.3, 0.2, 0.1 , 0.1};
	
	s0 = tex2D(SourceSampler,v.vTap0);
	s1 = tex2D(SourceSampler,v.vTap1);
	s2 = tex2D(SourceSampler,v.vTap2);
	s3 = tex2D(SourceSampler,v.vTap3);
	s4 = tex2D(SourceSampler,v.vTap1Neg);
	s5 = tex2D(SourceSampler,v.vTap2Neg);
	s6 = tex2D(SourceSampler,v.vTap3Neg);

	s0.rgb = s0.rgb * s0.a;
	s1.rgb = s1.rgb * s1.a;
	s2.rgb = s2.rgb * s2.a;
	s3.rgb = s3.rgb * s3.a;
	s4.rgb = s4.rgb * s4.a;
	s5.rgb = s5.rgb * s5.a;
	s6.rgb = s6.rgb * s6.a;
	
	vColorWeightSum = s0 * vWeights0.w + (s1+s4) * vWeights0.x + (s2+s5) * vWeights0.y + (s3+s6) * vWeights0.z;
	
	float2 vTap4 = v.vTap0 + vertTapOffs[4];
	float2 vTap5 = v.vTap0 + vertTapOffs[5];
	float2 vTap6 = v.vTap0 + vertTapOffs[6];
	float2 vTap4Neg = v.vTap0 - vertTapOffs[4];
	float2 vTap5Neg = v.vTap0 - vertTapOffs[5];
	float2 vTap6Neg = v.vTap0 - vertTapOffs[6];
	
	s0 = tex2D(SourceSampler,vTap4);
	s1 = tex2D(SourceSampler,vTap5);
	s2 = tex2D(SourceSampler,vTap6);
	s3 = tex2D(SourceSampler,vTap4Neg);
	s4 = tex2D(SourceSampler,vTap5Neg);
	s5 = tex2D(SourceSampler,vTap6Neg);
	
	s0.rgb = s0.rgb * s0.a;
	s1.rgb = s1.rgb * s1.a;
	s2.rgb = s2.rgb * s2.a;
	s3.rgb = s3.rgb * s3.a;
	s4.rgb = s4.rgb * s4.a;
	s5.rgb = s5.rgb * s5.a;
	
	vColorWeightSum += (s0+s3)*vWeights1.x + (s1+s4)*vWeights1.y + (s2+s5)*vWeights1.z;

	vColorWeightSum.rgb /= vColorWeightSum.a;
	vColorWeightSum.a = 1;

	return vColorWeightSum;
	//return tex2D(SourceSampler,v.vTap0);
}  

////////////////////////////////////////////////////////////
technique y_gaussian
{
	pass One 
	{
		cullmode = none;
		ZEnable = false;
		ZWriteEnable = false;
		FogEnable = false;
		VertexShader = compile vs_1_1 filter_gaussian_y_vs();
		PixelShader  = compile ps_2_0 filter_gaussian_y_ps();
	}
}

