//---------------------------------------------------------------------------
// Constant variables:
//---------------------------------------------------------------------------
sampler2D Texture1 : register(s0);
sampler2D Texture2 : register(s1);
sampler2D Detail : register(s2);

//---------------------------------------------------------------------------
// Functions:
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Input:
//---------------------------------------------------------------------------
struct Input
{
    float4 DiffuseAccum : TEXCOORD0;
    float2 UVSet0 : TEXCOORD1;
    float2 UVSet1 : TEXCOORD2;
    float2 UVSet2 : TEXCOORD3;
};

//---------------------------------------------------------------------------
// Output:
//---------------------------------------------------------------------------
struct Output
{
    float4 Color : COLOR0;
};

//---------------------------------------------------------------------------
// Main():
//---------------------------------------------------------------------------
Output main(Input In)
{
    Output Out;

    float3 Blend0Color = tex2D(Texture1, In.UVSet0).rgb;
    float3 Blend1Color = tex2D(Texture2, In.UVSet1).rgb;;
    float3 BlendedColor = lerp(Blend1Color, Blend0Color, In.DiffuseAccum.a);
    float3 DetailColor = tex2D(Detail, In.UVSet2).rgb * float3(2.0f, 2.0f, 2.0f);
    float3 LitBlendedColor = In.DiffuseAccum.rgb * BlendedColor.rgb;
    LitBlendedColor = LitBlendedColor.rgb * DetailColor.rgb;
    Out.Color = float4(LitBlendedColor.r, LitBlendedColor.g,
        LitBlendedColor.b, 1.0f); 
    return Out;
}
