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

float2 horzTapOffs[7]
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

VS_OUTPUT_TEX7 filter_gaussian_x_vs(VS_INPUT v)
{
    VS_OUTPUT_TEX7 OUT = (VS_OUTPUT_TEX7)0;
    
    OUT.vPos = mul(v.vPos, worldViewProjection);
    OUT.vTap0 = v.vTexCoord;
    OUT.vTap1 = v.vTexCoord + horzTapOffs[1];
    OUT.vTap2 = v.vTexCoord + horzTapOffs[2];
    OUT.vTap3 = v.vTexCoord + horzTapOffs[3];
    OUT.vTap1Neg = v.vTexCoord - horzTapOffs[1];
    OUT.vTap2Neg = v.vTexCoord - horzTapOffs[2];
    OUT.vTap3Neg = v.vTexCoord - horzTapOffs[3];
    
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
half4 filter_gaussian_x_ps(PS_INPUT_TEX7 v) : COLOR
{   
	float4 s0,s1,s2,s3,s4,s5,s6;
	float4 vWeights4;
	float3 vWeights3;
	float3 vColorSum;
	float fWeightSum;
	float fCenterLumi;
	
	s0 = tex2D(SourceSampler,v.vTap0);
	s1 = tex2D(SourceSampler,v.vTap1);
	s2 = tex2D(SourceSampler,v.vTap2);
	s3 = tex2D(SourceSampler,v.vTap3);
	s4 = tex2D(SourceSampler,v.vTap1Neg);
	s5 = tex2D(SourceSampler,v.vTap2Neg);
	s6 = tex2D(SourceSampler,v.vTap3Neg);
	
	fCenterLumi = 0.0;//lumi(s0.xyz);
	
	vWeights4.x = saturate(lumi(s0.xyz));
	vWeights4.y = saturate(lumi(s1.xyz) - fCenterLumi);
	vWeights4.z = saturate(lumi(s2.xyz) - fCenterLumi);
	vWeights4.w = saturate(lumi(s3.xyz) - fCenterLumi);
	
	vColorSum = s0 * vWeights4.x + s1 * vWeights4.y + s2 * vWeights4.z + s3 * vWeights4.w;
	fWeightSum = dot(vWeights4, 1);
	
	vWeights3.x = saturate(lumi(s4.xyz) - fCenterLumi);
	vWeights3.y = saturate(lumi(s5.xyz) - fCenterLumi);
	vWeights3.z = saturate(lumi(s6.xyz) - fCenterLumi);
	
	vColorSum += s4 * vWeights3.x + s5 * vWeights3.y + s6 * vWeights3.z;
	
	fWeightSum += dot(vWeights3,1);
	
	float2 vTap4 = v.vTap0 + horzTapOffs[4];
	float2 vTap5 = v.vTap0 + horzTapOffs[5];
	float2 vTap6 = v.vTap0 + horzTapOffs[6];
	float2 vTap4Neg = v.vTap0 - horzTapOffs[4];
	float2 vTap5Neg = v.vTap0 - horzTapOffs[5];
	float2 vTap6Neg = v.vTap0 - horzTapOffs[6];
	
	s0 = tex2D(SourceSampler,vTap4);
	s1 = tex2D(SourceSampler,vTap5);
	s2 = tex2D(SourceSampler,vTap6);
	s3 = tex2D(SourceSampler,vTap4Neg);
	s4 = tex2D(SourceSampler,vTap5Neg);
	s5 = tex2D(SourceSampler,vTap6Neg);
	
	vWeights3.x = saturate(lumi(s0.xyz) - fCenterLumi);
	vWeights3.y = saturate(lumi(s1.xyz) - fCenterLumi);
	vWeights3.z = saturate(lumi(s2.xyz) - fCenterLumi);
	
	vColorSum += s0*vWeights3.x + s1*vWeights3.y + s2*vWeights3.z;
	fWeightSum += dot(vWeights3, 1);
	
	vWeights3.x = saturate(lumi(s3.xyz) - fCenterLumi);
	vWeights3.y = saturate(lumi(s4.xyz) - fCenterLumi);
	vWeights3.z = saturate(lumi(s5.xyz) - fCenterLumi);
	
	vColorSum += s3*vWeights3.x + s4*vWeights3.y + s5*vWeights3.z;
	fWeightSum += dot(vWeights3, 1);
	
	vColorSum /= fWeightSum;

	return saturate(float4(vColorSum,fWeightSum));
	//return s0;
}  

////////////////////////////////////////////////////////////
technique x_gaussian
{
	pass One 
	{
		cullmode = none;
		ZEnable = false;
		ZWriteEnable = false;
		FogEnable =false; 
		VertexShader = compile vs_1_1 filter_gaussian_x_vs();
		PixelShader  = compile ps_2_0 filter_gaussian_x_ps();
	}
}

