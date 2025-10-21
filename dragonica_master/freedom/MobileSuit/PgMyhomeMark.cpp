#include "stdafx.h"

#include <NiNode.H>
#include "XUI/XUI_Font.h"
#include "PgRenderer.h"
#include "PgActor.h"
#include "PgNifMan.h"
#include "PgPilot.h"
#include "PgMath.h"
#include "Pg2DString.h"
#include "FreedomPool.H"
#include "PgWorld.H"
#include "PgCameraMan.H"
#include "PgOption.h"
#include "PgUIScene.H"
#include "PgInput.h"

#include "PgMyhomeMark.h"

#include "NewWare/Renderer/DrawUtils.h"
#include "NewWare/Scene/ApplyTraversal.h"


NiImplementRTTI(PgMyhomeMark, NiGeometry);

NiMaterialPropertyPtr		PgMyhomeMark::m_spMaterialProperty = NULL;
NiAlphaPropertyPtr			PgMyhomeMark::m_spAlphaProperty = NULL;
NiZBufferPropertyPtr		PgMyhomeMark::m_spZBufferProperty = NULL;
NiVertexColorPropertyPtr	PgMyhomeMark::m_spVertexColorProperty = NULL;
std::string const STR_DEFAULT_MYHOME_MARK_IMG = "../Data/6_UI/icon/icon_home.tga";
extern unsigned int MakeTextureScreenRect(NiScreenTexturePtr& rkTexture, NiPoint2 kTargetPos, NiPoint2 kSize, NiPoint2 const kTexPos, NiPoint2 const kLeftTop, NiPoint2 const kRightBottom);

PgMyhomeMark::PgMyhomeMark()
	: NiGeometry(NiNew NiTriShapeData(0,0,0,0,0,0,NiGeometryData::NBT_METHOD_NONE,0,0))
	, m_spScreenTexture(NULL)
{
	Init();
}

PgMyhomeMark::~PgMyhomeMark()
{
	Destroy();
}

void PgMyhomeMark::UpdateWorldBound()
{
	m_kWorldBound.SetCenterAndRadius(GetWorldTranslate(),200);
}

void PgMyhomeMark::Init()
{
	NiAlphaPropertyPtr spAlphaProperty = NiNew NiAlphaProperty();
	spAlphaProperty->SetAlphaBlending(true);
	spAlphaProperty->SetAlphaGroup(AG_EFFECT);

	AttachProperty(spAlphaProperty);

	UpdateProperties();
	UpdateEffects();

	m_kIsDraw = true;
	m_kNameWidth = 0.0f;
	m_fQuadScale = 1.0f;

	if( !m_spScreenTexture )
	{
		NiSourceTexturePtr pTexture = g_kNifMan.GetTexture(STR_DEFAULT_MYHOME_MARK_IMG);
		if( pTexture )
		{
			m_spScreenTexture = NiNew NiScreenTexture(pTexture);
		}
	}
}

void PgMyhomeMark::Destroy()
{
	m_spScreenTexture = NULL;
	m_spQuad = NULL;
}

void PgMyhomeMark::DestoryStatic()
{
	m_spMaterialProperty = NULL;
	m_spAlphaProperty = NULL;
	m_spZBufferProperty = NULL;
	m_spVertexColorProperty = NULL;
}

void PgMyhomeMark::RenderImmediate(NiRenderer* pkRenderer)
{
	PgRenderer *pkPgRenderer = PgRenderer::GetPgRenderer();
	if( !pkPgRenderer )
	{
		return;
	}

	NiCamera *pkCamera = pkPgRenderer->GetCameraData();
	if( !pkCamera )
	{
		return;
	}

	NiPoint3 const kPos = GetWorldTranslate();

	Draw3DPos(pkPgRenderer, pkCamera, kPos);
}

void PgMyhomeMark::ReCreateQuad()
{
	if ( m_spScreenTexture )
	{
		m_spQuad = 0;
		m_spQuad = CreateQuad( m_spScreenTexture->GetTexture(), 0, 0, true );
	}
}

void PgMyhomeMark::Draw3DPos(PgRenderer* pkRenderer, NiCamera* pkCamera, NiPoint3 const &rkPos)
{
	if( !m_kIsDraw )			{ return; }
	if( !pkRenderer )			{ return; }
	if( !pkCamera )				{ return; }

	if( !m_spQuad )
	{
		if ( m_spScreenTexture )
		{
			m_spQuad = CreateQuad( m_spScreenTexture->GetTexture(), 0, 0, true );
		}
	}

	if ( m_spQuad )
	{
		if( 0 != m_spQuad->GetArrayCount() )
		{
			m_spZBufferProperty->SetZBufferWrite(true);

			m_spQuad->SetColorLocal( NiColorA::WHITE );
			m_spQuad->SetTranslate(rkPos);
			m_spQuad->Update(0);

			NiVisibleArray	kVisibleArray;
			pkRenderer->CullingProcess_Deprecated( pkCamera, m_spQuad, &kVisibleArray );
            NewWare::Renderer::DrawUtils::DrawImmediate( pkRenderer->GetRenderer(), m_spQuad );
		}
	}
}

NiBillboardNodePtr PgMyhomeMark::CreateQuad( NiTexture* pkTexture, float const fRowIcon, float const fCollIcon, bool const bLeft )
{
	if( pkTexture )
	{
		NiBillboardNodePtr spQuad = NiNew NiBillboardNode();
		if ( spQuad )
		{
			spQuad->SetMode(NiBillboardNode::RIGID_FACE_CAMERA);

			float const fScale = m_fQuadScale * 0.2f * 2.f * 0.8f;

			NiPoint2 const kIconSize(24.0f, 24.0f);
			float const fBoundingWidth = kIconSize.x * fScale;
			float const fBoundingHeight = kIconSize.y * fScale;
			NiPoint3 const vCamRight = NiPoint3::UNIT_X;
			NiPoint3 const vCamUp = NiPoint3::UNIT_Y;
			float const fOrgScale = m_fQuadScale * 0.24f;

			NiPoint3 vQuadPos;
			if ( true == bLeft )
			{		
				vQuadPos = -( vCamRight * fBoundingWidth ) - ( vCamRight*m_kNameWidth*fOrgScale/2.0f) + (vCamUp*fBoundingHeight/2.0f);
			}
			else
			{
				vQuadPos = ( vCamRight*m_kNameWidth*fOrgScale/2.0f) + (vCamUp*fBoundingHeight/2.0f);
			}

			NiPoint2 const kTexPos( kIconSize.x * fRowIcon, kIconSize.y * fCollIcon );
			float const fQuadWidth = kIconSize.x * fScale, fQuadHeight = kIconSize.y * fScale;

			size_t const iVertexCount = 4;
			NiPoint3	*pkVert = NiNew NiPoint3[iVertexCount];
			if ( pkVert )
			{
				pkVert[0] = vQuadPos+vCamRight*fQuadWidth-vCamUp*fQuadHeight;
				pkVert[1] = vQuadPos+vCamRight*fQuadWidth;
				pkVert[2] = vQuadPos-vCamUp*fQuadHeight;
				pkVert[3] = vQuadPos;
			}

			NiPoint2	*pkTextureVert = NiNew NiPoint2[iVertexCount];
			if ( pkTextureVert )
			{
				float const fWidth = static_cast<float>(pkTexture->GetWidth());
				float const fHeight = static_cast<float>(pkTexture->GetHeight());
				pkTextureVert[0] = NiPoint2((kTexPos.x+kIconSize.x)/fWidth, (kTexPos.y+kIconSize.y)/fHeight);
				pkTextureVert[1] = NiPoint2((kTexPos.x+kIconSize.x)/fWidth, kTexPos.y/fHeight);
				pkTextureVert[2] = NiPoint2(kTexPos.x/fWidth, (kTexPos.y+kIconSize.y)/fHeight);
				pkTextureVert[3] = NiPoint2(kTexPos.x/fWidth, kTexPos.y/fHeight);
			}

			NiColorA	*pkColors = NiNew NiColorA[iVertexCount];
			if ( pkColors )
			{
				pkColors[0] = NiColorA::WHITE;
				pkColors[1] = NiColorA::WHITE;
				pkColors[2] = NiColorA::WHITE;
				pkColors[3] = NiColorA::WHITE;
			}

			NiTriShape *pkTriShape = CreateShape( pkVert, pkColors, pkTextureVert, pkTexture );
			if ( pkTriShape )
			{
				spQuad->AttachChild( pkTriShape, true);
			}
		}
		return spQuad;
	}
	return NULL;
}

NiTriShape*	PgMyhomeMark::CreateShape(NiPoint3 *pkVerts,NiColorA *pkColors,NiPoint2 *pkUVs,NiTexture *pkBaseMap)
{
	int	nTriangles = 2;
	int	iVertexCount = 4;

	unsigned	short	*pusTriangleList = NiAlloc(unsigned short, nTriangles * 3);

	*(pusTriangleList+0) = 0;	
	*(pusTriangleList+1) = 1;	
	*(pusTriangleList+2) = 3;	

	*(pusTriangleList+3) = 3;	
	*(pusTriangleList+4) = 2;	
	*(pusTriangleList+5) = 0;	

	NiTriShape *pkShape = NiNew NiTriShape(iVertexCount,pkVerts,NULL,pkColors,pkUVs,1,NiGeometryData::NBT_METHOD_NONE, nTriangles,pusTriangleList);
	pkShape->SetConsistency(NiGeometryData::STATIC);

	if( !m_spMaterialProperty )
	{
		m_spMaterialProperty = NiNew NiMaterialProperty();
		m_spMaterialProperty->SetAlpha(1.0f);
		m_spMaterialProperty->SetAmbientColor(NiColor::WHITE);
		m_spMaterialProperty->SetDiffuseColor(NiColor::WHITE);
		m_spMaterialProperty->SetEmittance(NiColor::WHITE);
		m_spMaterialProperty->SetSpecularColor(NiColor::BLACK);
	}

	if( !m_spAlphaProperty )
	{
		m_spAlphaProperty	=	NiNew	NiAlphaProperty();
		m_spAlphaProperty->SetAlphaBlending(true);
		m_spAlphaProperty->SetSrcBlendMode(NiAlphaProperty::ALPHA_SRCALPHA);
		m_spAlphaProperty->SetDestBlendMode(NiAlphaProperty::ALPHA_INVSRCALPHA);
		m_spAlphaProperty->SetAlphaTesting(true);
		m_spAlphaProperty->SetTestMode(NiAlphaProperty::TEST_GREATEREQUAL);
		m_spAlphaProperty->SetTestRef(10);
	}

	if( !m_spZBufferProperty )
	{
		m_spZBufferProperty	=	NiNew	NiZBufferProperty();
		m_spZBufferProperty->SetZBufferWrite(false);
		m_spZBufferProperty->SetZBufferTest(true);
	}

	NiTexturingPropertyPtr	spTexturing = NiNew NiTexturingProperty();
	spTexturing->SetApplyMode(NiTexturingProperty::APPLY_MODULATE);
	spTexturing->SetBaseTexture(pkBaseMap);

	if( !m_spVertexColorProperty )
	{
		m_spVertexColorProperty = NiNew NiVertexColorProperty();
		m_spVertexColorProperty->SetLightingMode(NiVertexColorProperty::LIGHTING_E);
		m_spVertexColorProperty->SetSourceMode(NiVertexColorProperty::SOURCE_AMB_DIFF);
	}
	
	pkShape->AttachProperty(m_spAlphaProperty);
	pkShape->AttachProperty(m_spMaterialProperty);
	pkShape->AttachProperty(m_spVertexColorProperty);
	pkShape->AttachProperty(spTexturing);
	pkShape->AttachProperty(m_spZBufferProperty);

	pkShape->UpdateProperties();
	pkShape->UpdateNodeBound();
	pkShape->Update(0);

	pkShape->SetActiveTriangleCount(0);
    NewWare::Scene::ApplyTraversal::Geometry::SetDefaultMaterialNeedsUpdateFlag( pkShape, false );

	return	pkShape;
}

void PgMyhomeMark::SetNameLength(float fLen)
{
	m_kNameWidth = fLen;
}
