#include "SceneDesignerFrameworkPCH.h"
#include "PgBrightBloom.h"

PgBrightBloom::PgBrightBloom(bool bActive)
{
}

void PgBrightBloom::CleanupEffect()
{
	m_spRenderTarget = 0;
	m_spRenderTargetGroup = 0;

	m_spRenderTargetGroup = 0;
	m_spRenderTarget = 0;

	m_spRTScreenElementBlurred = 0;
	
	m_spRTDownSampled = 0;
	m_spRTGDownSampled = 0;
	m_spRTSEDownSampled = 0;

	m_spRTXGuassian = 0;
	m_spRTGXGuassian = 0;
	m_spRTSEXGuassian = 0;

	m_spRTSEYGuassian = 0;

	m_spRTGaussBlur = 0;
	m_spRTGGuassBlur = 0;
	m_spSEGuassBlur = 0;
	m_spSEBloom = 0;

	m_spShadeMap = 0;
}
bool PgBrightBloom::CreateEffect()
{

	m_iBlurCount = 1;

	NiRenderer *pkRenderer = NiRenderer::GetRenderer();
	assert(pkRenderer);


	unsigned int uiWidth = pkRenderer->GetDefaultRenderTargetGroup()->GetWidth(0);
	unsigned int uiHeight = pkRenderer->GetDefaultRenderTargetGroup()->GetHeight(0);

	// Render Target

	float fMul =1.0f;

	m_spRenderTarget = NiRenderedTexture::Create(uiWidth, uiHeight, pkRenderer);
    m_spRenderTargetGroup = NiRenderTargetGroup::Create(m_spRenderTarget->GetBuffer(), pkRenderer, true, true);

	m_spRTDownSampled = NiRenderedTexture::Create((unsigned int)(uiWidth*fMul), (unsigned int)(uiHeight*fMul), pkRenderer);
	m_spRTGDownSampled = NiRenderTargetGroup::Create(m_spRTDownSampled->GetBuffer(), pkRenderer, true, true);;
	m_spRTXGuassian = NiRenderedTexture::Create((unsigned int)(uiWidth*fMul), (unsigned int)(uiHeight*fMul), pkRenderer);
	m_spRTGXGuassian = NiRenderTargetGroup::Create(m_spRTXGuassian->GetBuffer(), pkRenderer, true, true);;
	
	m_spRTGaussBlur = NiRenderedTexture::Create((unsigned int)(uiWidth*fMul), (unsigned int)(uiHeight*fMul), pkRenderer);
	m_spRTGGuassBlur = NiRenderTargetGroup::Create(m_spRTGaussBlur->GetBuffer(), pkRenderer, true, true);;

	NiSourceTexture::FormatPrefs fp;
	m_spShadeMap = NiSourceTexture::Create("BlackWhite.bmp",fp);

	//////
	//
	// DownSample
	//
	m_spRTSEDownSampled = NiNew NiScreenElements(NiNew NiScreenElementsData(false, false, 1));
	m_spRTSEDownSampled->Insert(4);
    m_spRTSEDownSampled->SetRectangle(0, 0.0f, 0.0f, 1.0f, 1.0f);
    m_spRTSEDownSampled->UpdateBound();
    m_spRTSEDownSampled->SetTextures(0, 0, 0.0f, 0.0f, 1.0f, 1.0f);
	
	NiShaderPtr spShaderDownSample4x = NiShaderFactory::GetInstance()->RetrieveShader("downsample4x", NiShader::DEFAULT_IMPLEMENTATION, true);
	NiSingleShaderMaterial* pkMaterial = NiSingleShaderMaterial::Create(spShaderDownSample4x);
	m_spRTSEDownSampled->ApplyAndSetActiveMaterial(pkMaterial);

	NiTexturingProperty *pkProperty = NiNew NiTexturingProperty;
	pkProperty->SetApplyMode(NiTexturingProperty::APPLY_REPLACE);
	pkProperty->SetBaseFilterMode(NiTexturingProperty::FILTER_NEAREST);
	pkProperty->SetShaderMap(0, NiNew NiTexturingProperty::ShaderMap(m_spRenderTarget, 0));
	pkProperty->SetShaderMap(1, NiNew NiTexturingProperty::ShaderMap(m_spShadeMap, 0));

	m_spRTSEDownSampled->AttachProperty(pkProperty);
    m_spRTSEDownSampled->UpdateProperties();
    m_spRTSEDownSampled->Update(0.0f);

	//////
	//
	// Gaussian Blur
	//
	m_spSEGuassBlur = NiNew NiScreenElements(NiNew NiScreenElementsData(false, false, 1));
	m_spSEGuassBlur->Insert(4);
    m_spSEGuassBlur->SetRectangle(0, 0.0f, 0.0f, 1.0f, 1.0f);
    m_spSEGuassBlur->UpdateBound();
    m_spSEGuassBlur->SetTextures(0, 0, 0.0f, 0.0f, 1.0f, 1.0f);
	
	NiShaderPtr spShaderGaussianBlur = NiShaderFactory::GetInstance()->RetrieveShader("GaussBlur", NiShader::DEFAULT_IMPLEMENTATION, true);
	pkMaterial = NiSingleShaderMaterial::Create(spShaderGaussianBlur);
	m_spSEGuassBlur->ApplyAndSetActiveMaterial(pkMaterial);

	pkProperty = NiNew NiTexturingProperty;
	pkProperty->SetApplyMode(NiTexturingProperty::APPLY_REPLACE);
	pkProperty->SetBaseFilterMode(NiTexturingProperty::FILTER_NEAREST);
	pkProperty->SetShaderMap(0, NiNew NiTexturingProperty::ShaderMap(m_spRTDownSampled, 0));

	m_spSEGuassBlur->AttachProperty(pkProperty);
    m_spSEGuassBlur->UpdateProperties();
    m_spSEGuassBlur->Update(0.0f);

	//////
	//
	// Gaussian Bloom
	//
	m_spSEBloom = NiNew NiScreenElements(NiNew NiScreenElementsData(false, false, 1));
	m_spSEBloom->Insert(4);
    m_spSEBloom->SetRectangle(0, 0.0f, 0.0f, 1.0f, 1.0f);
    m_spSEBloom->UpdateBound();
    m_spSEBloom->SetTextures(0, 0, 0.0f, 0.0f, 1.0f, 1.0f);
	
	NiShaderPtr spShaderGaussianBloom = NiShaderFactory::GetInstance()->RetrieveShader("GaussBloom", NiShader::DEFAULT_IMPLEMENTATION, true);
	pkMaterial = NiSingleShaderMaterial::Create(spShaderGaussianBloom);
	m_spSEBloom->ApplyAndSetActiveMaterial(pkMaterial);

	pkProperty = NiNew NiTexturingProperty;
	pkProperty->SetApplyMode(NiTexturingProperty::APPLY_REPLACE);
	pkProperty->SetBaseFilterMode(NiTexturingProperty::FILTER_NEAREST);
	pkProperty->SetShaderMap(0, NiNew NiTexturingProperty::ShaderMap(m_spRTDownSampled, 0));

	m_spSEBloom->AttachProperty(pkProperty);
    m_spSEBloom->UpdateProperties();
    m_spSEBloom->Update(0.0f);


	//////
	//
	// XGaussian
	//
	m_spRTSEXGuassian = NiNew NiScreenElements(NiNew NiScreenElementsData(false, false, 1));
	m_spRTSEXGuassian->Insert(4);
    m_spRTSEXGuassian->SetRectangle(0, 0.0f, 0.0f, 1.0f, 1.0f);
    m_spRTSEXGuassian->UpdateBound();
    m_spRTSEXGuassian->SetTextures(0, 0, 0.0f, 0.0f, 1.0f, 1.0f);
	
	NiShaderPtr spShaderXGaussian = NiShaderFactory::GetInstance()->RetrieveShader("x_gaussian", NiShader::DEFAULT_IMPLEMENTATION, true);
	pkMaterial = NiSingleShaderMaterial::Create(spShaderXGaussian);
	m_spRTSEXGuassian->ApplyAndSetActiveMaterial(pkMaterial);

	pkProperty = NiNew NiTexturingProperty;
	pkProperty->SetApplyMode(NiTexturingProperty::APPLY_REPLACE);
	pkProperty->SetBaseFilterMode(NiTexturingProperty::FILTER_NEAREST);
	pkProperty->SetShaderMap(0, NiNew NiTexturingProperty::ShaderMap(m_spRTDownSampled, 0));

	m_spRTSEXGuassian->AttachProperty(pkProperty);
    m_spRTSEXGuassian->UpdateProperties();
    m_spRTSEXGuassian->Update(0.0f);

	float dx = 1.0f / uiWidth ;
	float dy = 1.0f / uiHeight;

	D3DXVECTOR2 v[7];
	v[0] = D3DXVECTOR2(0.0,0.0);
	v[1] = D3DXVECTOR2(1.3366f * dx, 0);
	v[2] = D3DXVECTOR2(3.4295f * dx, 0);
	v[3] = D3DXVECTOR2(5.4264f * dx, 0);
	v[4] = D3DXVECTOR2(7.4359f * dx, 0);
	v[5] = D3DXVECTOR2(9.4436f * dx, 0);
	v[6] = D3DXVECTOR2(11.4401f * dx, 0);

	NiShaderFactory::UpdateGlobalShaderConstant("horzTapOffs",sizeof(v),&v[0]);

	//////
	//
	// YGaussian
	//
	m_spRTSEYGuassian = NiNew NiScreenElements(NiNew NiScreenElementsData(false, false, 1));
	m_spRTSEYGuassian->Insert(4);
    m_spRTSEYGuassian->SetRectangle(0, 0.0f, 0.0f, 1.0f, 1.0f);
    m_spRTSEYGuassian->UpdateBound();
    m_spRTSEYGuassian->SetTextures(0, 0, 0.0f, 0.0f, 1.0f, 1.0f);
	
	NiShaderPtr spShaderYGaussian = NiShaderFactory::GetInstance()->RetrieveShader("y_gaussian", NiShader::DEFAULT_IMPLEMENTATION, true);
	pkMaterial = NiSingleShaderMaterial::Create(spShaderYGaussian);
	m_spRTSEYGuassian->ApplyAndSetActiveMaterial(pkMaterial);

	pkProperty = NiNew NiTexturingProperty;
	pkProperty->SetApplyMode(NiTexturingProperty::APPLY_REPLACE);
	pkProperty->SetBaseFilterMode(NiTexturingProperty::FILTER_NEAREST);
	pkProperty->SetShaderMap(0, NiNew NiTexturingProperty::ShaderMap(m_spRTXGuassian, 0));

	m_spRTSEYGuassian->AttachProperty(pkProperty);
    m_spRTSEYGuassian->UpdateProperties();
    m_spRTSEYGuassian->Update(0.0f);

	v[0] = D3DXVECTOR2(0.0,0.0);
	v[1] = D3DXVECTOR2(0,1.3366f * dy);
	v[2] = D3DXVECTOR2(0,3.4295f * dy);
	v[3] = D3DXVECTOR2(0,5.4264f * dy);
	v[4] = D3DXVECTOR2(0,7.4359f * dy);
	v[5] = D3DXVECTOR2(0,9.4436f * dy);
	v[6] = D3DXVECTOR2(0,11.4401f * dy);

	NiShaderFactory::UpdateGlobalShaderConstant("vertTapOffs",sizeof(v),&v[0]);

	
	//////
	//
	// Compose
	//
	m_spRTScreenElementBlurred = NiNew NiScreenElements(NiNew NiScreenElementsData(false, false, 1));
	m_spRTScreenElementBlurred->Insert(4);
    m_spRTScreenElementBlurred->SetRectangle(0, 0.0f, 0.0f, 1.0f, 1.0f);
    m_spRTScreenElementBlurred->UpdateBound();
    m_spRTScreenElementBlurred->SetTextures(0, 0, 0.0f, 0.0f, 1.0f, 1.00f);     

	NiShaderPtr spShaderComp = NiShaderFactory::GetInstance()->RetrieveShader("comp", NiShader::DEFAULT_IMPLEMENTATION, true);
	NiSingleShaderMaterial* pkMaterialComp = NiSingleShaderMaterial::Create(spShaderComp);
	m_spRTScreenElementBlurred->ApplyAndSetActiveMaterial(pkMaterialComp);

	NiTexturingProperty *pkPropertyBlurred = NiNew NiTexturingProperty;
	pkPropertyBlurred->SetApplyMode(NiTexturingProperty::APPLY_REPLACE);
    pkPropertyBlurred->SetBaseFilterMode(NiTexturingProperty::FILTER_NEAREST);
	pkPropertyBlurred->SetShaderMap(0, NiNew NiTexturingProperty::ShaderMap(m_spRenderTarget, 0));
//	pkPropertyBlurred->SetShaderMap(1, NiNew NiTexturingProperty::ShaderMap(m_spRTDownSampled, 0));
	pkPropertyBlurred->SetShaderMap(1, NiNew NiTexturingProperty::ShaderMap(m_spRTGaussBlur, 0));

	m_spRTScreenElementBlurred->AttachProperty(pkPropertyBlurred);
    m_spRTScreenElementBlurred->UpdateProperties();
    m_spRTScreenElementBlurred->Update(0.0f);

	// 초기화 부분은 XmlPropertiesPanel.cs 코드 안에 BloomOption 클래스에 있다.
	//m_fBlurWidth = 0.65f;
	//m_fBrightness = 8.0f;
	//m_fSceneIntensity = 0.7f;
	//m_fGlowIntensity = 0.6f;
	//m_fHighLightIntensity = -0.04f;

	NiShaderFactory::UpdateGlobalShaderConstant("g_fBloomScale", sizeof(float), &m_fBrightness);
	NiShaderFactory::UpdateGlobalShaderConstant("g_fBlur", sizeof(float), &m_fBlurWidth);
	NiShaderFactory::UpdateGlobalShaderConstant("g_fSceneIntensity", sizeof(float), &m_fSceneIntensity);
	NiShaderFactory::UpdateGlobalShaderConstant("g_fGlowIntensity", sizeof(float), &m_fGlowIntensity);
	NiShaderFactory::UpdateGlobalShaderConstant("g_fHighlightIntensity", sizeof(float), &m_fHighLightIntensity);

	return	true;
}
void PgBrightBloom::RenderFrame(float fTime, NiRenderer* pkRenderer, NiRenderTargetGroup* pkRenderTarget, NiRenderedTexture* pkTexture)
{
		NiPoint2 akSampleOffsets[16];
		float afSampleOffsets[16];
		Point4 akSampleWeights[16];

		pkRenderer->BeginUsingRenderTargetGroup(m_spRTGDownSampled, NiRenderer::CLEAR_ALL);
		pkRenderer->GetRenderer()->SetScreenSpaceCameraData();
		m_spRTSEDownSampled->Draw(pkRenderer);
		pkRenderer->EndUsingRenderTargetGroup();

		GetSampleOffsets_GaussBlur5x5(m_spRTDownSampled->GetWidth(), 
			m_spRTDownSampled->GetHeight(), akSampleOffsets, 
			akSampleWeights);

		UpdateSampleConstants(akSampleOffsets, akSampleWeights);

		pkRenderer->BeginUsingRenderTargetGroup(m_spRTGGuassBlur, NiRenderer::CLEAR_ALL);
		pkRenderer->GetRenderer()->SetScreenSpaceCameraData();
		m_spSEGuassBlur->Draw(pkRenderer);
		pkRenderer->EndUsingRenderTargetGroup();

		for(int j=0;j<m_iBlurCount;j++)
		{
		////////////////////////////
		// Bloom Pass for Horizontal
		GetSampleOffsets_Bloom(m_spRTDownSampled->GetWidth(), 
			afSampleOffsets, akSampleWeights, 3.0f, 1.0f );
		for (int i = 0; i < 16; i++ )
			akSampleOffsets[i] = NiPoint2(afSampleOffsets[i], 0.0f);

		UpdateSampleConstants(akSampleOffsets, akSampleWeights);

		NiTexturingProperty *pkTP = (NiTexturingProperty*)m_spSEBloom->GetProperty(NiProperty::TEXTURING);
		pkTP->SetShaderMap(0,NiNew NiTexturingProperty::ShaderMap(m_spRTGaussBlur,0));
		pkRenderer->BeginUsingRenderTargetGroup(m_spRTGDownSampled, NiRenderer::CLEAR_ALL);
		pkRenderer->GetRenderer()->SetScreenSpaceCameraData();
		m_spSEBloom->Draw(pkRenderer);
		pkRenderer->EndUsingRenderTargetGroup();


		////////////////////////////
		// Bloom Pass for Vertical

		GetSampleOffsets_Bloom(m_spRTDownSampled->GetHeight(), 
			afSampleOffsets, akSampleWeights, 3.0f, 1.0f);
		for (int i = 0; i < 16; i++ )
			akSampleOffsets[i] = NiPoint2(0.0f, afSampleOffsets[i]);

		UpdateSampleConstants(akSampleOffsets, akSampleWeights);

		pkTP->SetShaderMap(0,NiNew NiTexturingProperty::ShaderMap(m_spRTDownSampled,0));
		pkRenderer->BeginUsingRenderTargetGroup(m_spRTGGuassBlur, NiRenderer::CLEAR_ALL);
		pkRenderer->GetRenderer()->SetScreenSpaceCameraData();
		m_spSEBloom->Draw(pkRenderer);
		pkRenderer->EndUsingRenderTargetGroup();
		}

	/*
		pkRenderer->BeginUsingRenderTargetGroup(m_spRTGXGuassian, NiRenderer::CLEAR_ALL);
		pkRenderer->GetRenderer()->SetScreenSpaceCameraData();
		m_spRTSEXGuassian->Draw(pkRenderer->GetRenderer());
		pkRenderer->EndUsingRenderTargetGroup();

		pkRenderer->BeginUsingRenderTargetGroup(m_spRTGDownSampled, NiRenderer::CLEAR_ALL);
		pkRenderer->GetRenderer()->SetScreenSpaceCameraData();
		m_spRTSEYGuassian->Draw(pkRenderer->GetRenderer());
		pkRenderer->EndUsingRenderTargetGroup();
*/
		pkRenderer->BeginUsingRenderTargetGroup(pkRenderTarget, NiRenderer::CLEAR_ALL);
		pkRenderer->GetRenderer()->SetScreenSpaceCameraData();
		m_spRTScreenElementBlurred->Draw(pkRenderer);
		pkRenderer->EndUsingRenderTargetGroup();
}
void	PgBrightBloom::SetParam(const char *ParamName,float fValue)
{
 	if(_stricmp(ParamName,"Brightness")==0) m_fBrightness = fValue;
	if(_stricmp(ParamName,"BlurWidth")==0) m_fBlurWidth = fValue;
	if(_stricmp(ParamName,"SceneIntensity")==0) m_fSceneIntensity = fValue;
	if(_stricmp(ParamName,"GlowIntensity")==0) m_fGlowIntensity = fValue;
	if(_stricmp(ParamName,"HighlightIntensity")==0) m_fHighLightIntensity = fValue;
	if(_stricmp(ParamName,"BlurCount")==0) m_iBlurCount = (int)fValue;

	bool bCheck = false;
	bCheck = NiShaderFactory::UpdateGlobalShaderConstant("g_fBrightness", sizeof(float), &m_fBrightness);
	bCheck = NiShaderFactory::UpdateGlobalShaderConstant("g_fBlur", sizeof(float), &m_fBlurWidth);
	bCheck = NiShaderFactory::UpdateGlobalShaderConstant("g_fSceneIntensity", sizeof(float), &m_fSceneIntensity);
	bCheck = NiShaderFactory::UpdateGlobalShaderConstant("g_fGlowIntensity", sizeof(float), &m_fGlowIntensity);
	bCheck = NiShaderFactory::UpdateGlobalShaderConstant("g_fHighlightIntensity", sizeof(float), &m_fHighLightIntensity);

}
float PgBrightBloom::GetParam(const char *ParamName)
{
	if(_stricmp(ParamName,"Brightness")==0) return m_fBrightness;
	if(_stricmp(ParamName,"BlurWidth")==0) return m_fBlurWidth;
	if(_stricmp(ParamName,"SceneIntensity")==0) return m_fSceneIntensity;
	if(_stricmp(ParamName,"GlowIntensity")==0) return m_fGlowIntensity;
	if(_stricmp(ParamName,"HighlightIntensity")==0) return m_fHighLightIntensity;
	if(_stricmp(ParamName,"BlurCount")==0) return (float)m_iBlurCount;

	return 0;
}
void PgBrightBloom::UpdateSampleConstants(NiPoint2* pkOffsets, Point4* pkWeights)
{
    assert(pkOffsets);
    NiShaderFactory::UpdateGlobalShaderConstant("gakSampleOffsets_BB",
        sizeof(NiPoint2) * 16, pkOffsets);

    if (pkWeights)
    {
        NiShaderFactory::UpdateGlobalShaderConstant("gakSampleWeights_BB",
			sizeof(Point4) * 16, pkWeights);
    }
}
//---------------------------------------------------------------------------
void PgBrightBloom::GetSampleOffsets_GaussBlur5x5(unsigned int uiTexWidth,
    unsigned int uiTexHeight, NiPoint2 akTexCoordOffsets[],
    Point4 akSampleWeights[], float fMultiplier)
{
    // Get the texture coordinate offsets to be used inside 
    // the GaussBlur5x5 pixel shader.
    float tu = 1.0f / (float)uiTexWidth ;
    float tv = 1.0f / (float)uiTexHeight ;

    Point4 vWhite( 1.0f, 1.0f, 1.0f, 1.0f );
    
    float totalWeight = 0.0f;
    int index = 0;
    for (int x = -2; x <= 2; x++)
    {
        for (int y = -2; y <= 2; y++)
        {
            // Exclude pixels with a block distance greater than 2. This will
            // create a kernel which approximates a 5x5 kernel using only 13
            // sample points instead of 25; this is necessary since 2.0 
            // shaders only support 16 texture grabs.
            if(abs(x) + abs(y) > 2)
                continue;

            // Get the unscaled Gaussian intensity for this offset
            akTexCoordOffsets[index] = NiPoint2(x * tu, y * tv);
            float fGauss = GaussianDistribution((float)x, (float)y, 1.0f);
            akSampleWeights[index].x = vWhite.x * fGauss; 
            akSampleWeights[index].y = vWhite.y * fGauss; 
            akSampleWeights[index].z = vWhite.z * fGauss; 
            akSampleWeights[index].w = vWhite.w * fGauss; 

            totalWeight += akSampleWeights[index].x;

            index++;
        }
    }

    // Divide the current weight by the total weight of all the samples; 
    // Gaussian blur kernels add to 1.0f to ensure that the intensity of 
    // the image isn't changed when the blur occurs. An optional 
    // multiplier variable is used to add or remove image intensity 
    // during the blur.
    for (int i = 0; i < index; i++)
    {
        akSampleWeights[i].x /= totalWeight;
        akSampleWeights[i].y /= totalWeight;
        akSampleWeights[i].z /= totalWeight;
        akSampleWeights[i].w /= totalWeight;

        akSampleWeights[i].x *= fMultiplier;
        akSampleWeights[i].y *= fMultiplier;
        akSampleWeights[i].z *= fMultiplier;
        akSampleWeights[i].w *= fMultiplier;
    }
}
//---------------------------------------------------------------------------
void PgBrightBloom::GetSampleOffsets_Bloom(unsigned int uiTexSize,
    float afTexCoordOffset[], Point4 akColorWeight[],
    float fDeviation, float fMultiplier)
{
    // Get the texture coordinate offsets to be used inside the Bloom
    // pixel shader.
    float tu = 1.0f / (float)uiTexSize;

    // Fill the center texel
    float weight = fMultiplier * GaussianDistribution( 0, 0, fDeviation);
    akColorWeight[0].x = weight;
    akColorWeight[0].y = weight;
    akColorWeight[0].z = weight;
    akColorWeight[0].w = 1.0f;

    afTexCoordOffset[0] = 0.0f;
    
    // Fill the first half
    for (int i = 1; i < 8; i++)
    {
        // Get the Gaussian intensity for this offset
        weight = fMultiplier * 
            GaussianDistribution((float)i, 0, fDeviation);
        afTexCoordOffset[i] = i * tu;

        akColorWeight[i].x = weight; 
        akColorWeight[i].y = weight; 
        akColorWeight[i].z = weight; 
        akColorWeight[i].w = 1.0f; 
    }

    // Mirror to the second half
    for (int i = 8; i < 15; i++ )
    {
        akColorWeight[i].x = akColorWeight[i - 7].x;
        akColorWeight[i].y = akColorWeight[i - 7].y;
        akColorWeight[i].z = akColorWeight[i - 7].z;
        akColorWeight[i].w = akColorWeight[i - 7].w;
        afTexCoordOffset[i] = -afTexCoordOffset[i - 7];
    }
}
//---------------------------------------------------------------------------
float PgBrightBloom::GaussianDistribution(float x, float y, float rho)
{
    // Helper function for GetSampleOffsets function to compute the 2 
    // parameter Gaussian distrubution using the given standard deviation rho
    
    float g = 1.0f / sqrtf(2.0f * NI_PI * rho * rho);
    g *= expf(-(x * x + y * y) / (2 * rho * rho));

    return g;
}
