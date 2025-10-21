// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2006 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

//--------------------------------------------------------------
// ShaderBlack.fx 
// Implements a very simple shader that is used only for Z-pass
// operations -- simply dumps transformed position from VS
// and all black pixels from PS.
//--------------------------------------------------------------

float4x4 WorldViewProj : WORLDVIEWPROJECTION;

float4 VSMain(float4 inPos: POSITION) : POSITION
{
    return mul(inPos, WorldViewProj);
}

float4 PSMain() : COLOR0
{
    float4 color = 0.0f;
    
    return color;
}

//--------------------------------------------------------------//
// Technique Section
//--------------------------------------------------------------//
technique ShaderBlack
<
    bool UsesNiRenderState = false;
    bool UsesNiLightState = false;
>
{
    pass P0
    {   
        VertexShader = compile vs_1_1 VSMain();
        PixelShader = compile ps_2_0 PSMain();        
    }   
}
//--------------------------------------------------------------//
