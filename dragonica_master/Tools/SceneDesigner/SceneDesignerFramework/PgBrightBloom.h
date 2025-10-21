#pragma once

#include <NiMain.H>

public class PgBrightBloom
{
	class Point4 : public NiMemObject
	{
	public:
		Point4(float X, float Y, float Z, float W) : 
		  x(X), y(Y), z(Z), w(W) { /* */ }
		Point4() { /**/ }
		float x, y, z, w;
	};

public:
	PgBrightBloom(bool bActive);

	virtual int GetType() const {	return	0;	}

	bool CreateEffect();
	void CleanupEffect();
	NiRenderTargetGroup* GetRenderTargetGroup()	{	return	m_spRenderTargetGroup;	}
	NiRenderedTexture* GetTargetTexture()	{	return	m_spRenderTarget;	}
	void RenderFrame(float fTime, NiRenderer* pkRenderer, NiRenderTargetGroup* pkRenderTarget, NiRenderedTexture* pkTexture);
	void SetParam(const char *ParamName,float fValue);
	float GetParam(const char *ParamName);
	void UpdateSampleConstants(NiPoint2* pkOffsets, Point4* pkWeights);

	void GetSampleOffsets_GaussBlur5x5(unsigned int uiTexWidth, unsigned int uiTexHeight, NiPoint2 akTexCoordOffsets[], Point4 akSampleWeights[], float fMultiplier=1.0f);
	void GetSampleOffsets_Bloom(unsigned int uiTexSize, float afTexCoordOffset[], Point4 akColorWeight[], float fDeviation, float fMultiplier);
	float GaussianDistribution(float x, float y, float rho);

private:

	NiSourceTexturePtr	m_spShadeMap;

	NiRenderedTexturePtr m_spRenderTarget;
	NiRenderTargetGroupPtr m_spRenderTargetGroup;

	NiScreenElementsPtr m_spRTScreenElementBlurred;

	NiRenderedTexturePtr m_spRTDownSampled;
	NiRenderTargetGroupPtr m_spRTGDownSampled;
	NiScreenElementsPtr m_spRTSEDownSampled;

	NiRenderedTexturePtr m_spRTXGuassian;
	NiRenderTargetGroupPtr m_spRTGXGuassian;
	NiScreenElementsPtr m_spRTSEXGuassian;

	NiScreenElementsPtr m_spRTSEYGuassian;

	NiScreenElementsPtr m_spSEGuassBlur;
	NiRenderTargetGroupPtr	m_spRTGGuassBlur;
	NiRenderedTexturePtr	m_spRTGaussBlur;

	NiScreenElementsPtr m_spSEBloom;

	float	m_fBlurWidth;
	float	m_fBrightness;
	float	m_fSceneIntensity;
	float	m_fGlowIntensity;
	float	m_fHighLightIntensity;

	int m_iBlurCount;


};
